#pragma once

#include <string>
#include <string_view>
#include <any>

#include <cprt.hpp>
#include <common.hpp>
#include <antlr4-runtime.h>

#include "mir/Addr.hpp"
#include "mir/Instr.hpp"
#include "mir/Module.hpp"
#include "frontend/SysAntlr/SysYParser.h"
#include "frontend/SysAntlr/SysYBaseVisitor.h"
#include "frontend/SysAntlr/SysYVisitor.h"
#include "mir/support/SupportFunc.hpp"
#include "mir/support/TypeInfo.hpp"
#include "mir/support/StaticValue.hpp"
#include "helper.hpp"


namespace frontend {
class ASTVisitor : public SysYBaseVisitor {
  protected:
	enum class BType {
		Float, Int, Error
	};

	enum class FuncType {
		Void, Float, Int, Error
	};

	static BType strToBType(const std::string & str);

	static FuncType strToFuncType(const std::string & str);

	static std::unique_ptr<sup::TypeInfo>
	bTypeToTypeInfoUPtr(BType btype, const std::vector<int> & shape = { });


	struct Info {
		struct Stat {
			/*  inGlobal: is visitor processing statements on global field.
			 *  isGlobal is true at first, and becomes false when enter `FuncDef`, and
			 *  becomes true after leaving `FuncDef`.
			 * */
			bool inGlobal = true;

			/*  calculatingStaticValue: is calculating static value from exp.
			 * */
			bool calculatingStaticValue = false;

			/*  visitingAssignmentLeft : for lVal
			 * */
			bool visitingAssignmentLeft = false;
		};

		struct Var {
			/*  btype: basic type of decl var.
			 *  Changed when visiting BType.
			 * */
			BType btype = BType::Error;

			/*  definingConstVar: is decl var constant.
			 *  Changed just after visiting ConstDecl/VarDecl.
			 * */
			bool definingConstVar = false;

			/*  shapeOfDefiningVar of array declaring.
			 *  The first index is the length of visiting.
			 *  You should back up shapeOfDefiningVar by yourself.
			 * */
			std::vector<int> shapeOfDefiningVar;
			IdxView idxView;    //  Used on defining array.
			int ndim = -1;           //  Used on defining array.
			std::vector<ArrayItem<std::unique_ptr<sup::StaticValue>>>
				staticArrayItems;
			std::vector<ArrayItem<mir::AddrOperand *>> localArrayItems;

			bool definingArray() const { return !shapeOfDefiningVar.empty(); }

			mir::AddrFunction * pUsingFunc = nullptr;    //  function using
		};


		struct Func {
			/*  address of retval of function.
			 * */
			mir::AddrVariable * pRetvalMem = nullptr;
			mir::AddrJumpLabel * pRetBlockLabel = nullptr;
			mir::FuncDef * pFuncDef = nullptr;
		};

		struct Cond {
			mir::AddrJumpLabel * jumpLabelTrue = nullptr;
			mir::AddrJumpLabel * jumpLabelFalse = nullptr;
			mir::AddrJumpLabel * whileCond = nullptr;
			mir::AddrJumpLabel * whileEnd = nullptr;
		};

		Stat stat;
		Var var;
		Func func;
		Cond cond;
	} info;

	com::UnaryVariant<
		std::unique_ptr<sup::StaticValue>,
		std::string,
		BType,
		FuncType,
		std::vector<mir::AddrPara *>,
		mir::AddrPara *,
		mir::AddrVariable *,
		std::vector<mir::AddrVariable *>,
		std::vector<mir::AddrOperand *>,
		mir::AddrOperand *
	> retVal;

	com::UnaryVariant<
		std::list<mir::Instr *>
	> retInstrs;

	frontend::SymbolTable symbolTable;
  public:
	mir::Module & ir;

	explicit ASTVisitor(mir::Module & irModule);

	antlrcpp::Any visitChildren(antlr4::tree::ParseTree * node) override;

	/**
	 * @return NOTHING
	 */
	antlrcpp::Any visitCompUnit(SysYParser::CompUnitContext * ctx) override;

	/**
	 * @return NOTHING
	 * @note all instructions will be added to function in this function.
	 */
	antlrcpp::Any visitFuncDef(SysYParser::FuncDefContext * ctx) override;

	/**
	 * @for @c visitDecl , @c visitConstDecl , @c visitConstDef , @c visitVarDecl
	 * 		@c visitUninitVarDef , @c visitInitVarDef
	 * @return_in_retInstrs NOTHING if @c info.stat.inGlobal
	 * @return_in_retInstrs Instrs to alloca and store in local memory otherwise.
	 */
	antlrcpp::Any visitDecl(SysYParser::DeclContext * ctx) override;

	antlrcpp::Any visitConstDecl(SysYParser::ConstDeclContext * ctx) override;

	antlrcpp::Any visitVarDecl(SysYParser::VarDeclContext * ctx) override;

	antlrcpp::Any visitConstDef(SysYParser::ConstDefContext * ctx) override;

	antlrcpp::Any visitUninitVarDef(SysYParser::UninitVarDefContext * ctx) override;

	antlrcpp::Any visitInitVarDef(SysYParser::InitVarDefContext * ctx) override;

	/**
	 * @note Every information is saved on @c info.var.staticArrayItems or @c info.var.localArrayItems
	 */
	antlrcpp::Any
	visitScalarInitVal(SysYParser::ScalarInitValContext * ctx) override;

	antlrcpp::Any visitScalarConstInitVal(
		SysYParser::ScalarConstInitValContext * ctx
	) override;

	antlrcpp::Any
	visitListConstInitVal(SysYParser::ListConstInitValContext * ctx) override;

	antlrcpp::Any visitListInitval(SysYParser::ListInitvalContext * ctx) override;

	/**
	 * @return_in_retVal vector\<AddrPara *\>
	 */
	antlrcpp::Any visitFuncFParams(SysYParser::FuncFParamsContext * ctx) override;

	antlrcpp::Any visitFuncFParam(SysYParser::FuncFParamContext * ctx) override;

	/**
	 * @for @c visitBlock , @c visitBlockItem , @c visitAssignment , @c visitExpStmt ,
	 * 		@c visitBlockStmt , @c visitIfStmt1 , @c visitIfStmt2 , @c visitWhileStmt ,
	 * 		@c visitBreakStmt , @c visitContinueStmt
	 * @return_in_retInstrs Instrs generated.
	 */
	antlrcpp::Any visitBlock(SysYParser::BlockContext * ctx) override;

	antlrcpp::Any visitBlockItem(SysYParser::BlockItemContext * ctx) override;

	antlrcpp::Any visitAssignment(SysYParser::AssignmentContext * ctx) override;

	antlrcpp::Any visitExpStmt(SysYParser::ExpStmtContext * ctx) override;

	antlrcpp::Any visitBlockStmt(SysYParser::BlockStmtContext * ctx) override;

	antlrcpp::Any visitIfStmt1(SysYParser::IfStmt1Context * ctx) override;

	antlrcpp::Any visitIfStmt2(SysYParser::IfStmt2Context * ctx) override;

	antlrcpp::Any visitWhileStmt(SysYParser::WhileStmtContext * ctx) override;

	antlrcpp::Any visitBreakStmt(SysYParser::BreakStmtContext * ctx) override;

	antlrcpp::Any visitContinueStmt(SysYParser::ContinueStmtContext * ctx) override;

	/**
	 * @return_in_retInstrs Instrs generated from return statement.
	 */
	antlrcpp::Any visitReturnStmt(SysYParser::ReturnStmtContext * ctx) override;

	/**
	 * @return_in_retInstrs Instrs generated to do conditional jump.
	 */
	antlrcpp::Any visitCond(SysYParser::CondContext * ctx) override;

	/**
	 * @return_in_retVal <tt>vector\<AddrOperand *\></tt> (Addrs of RParams)
	 * @return_in_retInstrs <tt>list\<Instr *\></tt> (Instrs generated for RParams)
	 */
	antlrcpp::Any visitFuncRParams(SysYParser::FuncRParamsContext * ctx) override;

	antlrcpp::Any visitExpAsRParam(SysYParser::ExpAsRParamContext * ctx) override;


	/**
	 * @return_in_retVal <tt>unique_ptr<StaticValue></tt> if @c info.stat.calculatingStaticValue
	 * @return_in_retVal <tt>AddrVariable *</tt> (Pointer of lVal) if @c info.stat.visitingAssignmentLeft
	 * @return_in_retVal <tt>AddrOperand *</tt> (Value of lVal) otherwise
	 * @return_in_retInstrs NOTHING if @c info.stat.calculatingStaticValue
	 * @return_in_retInstrs Instrs generated for getting this lVal.
	 */
	antlrcpp::Any visitLVal(SysYParser::LValContext * ctx) override;

	/**
	 * @For
	 * 		@c visitUnary1 , @c visitUnary2 , @c visitUnary3,
	 *      @c visitPrimaryExp1 , @c visitPrimaryExp2 , @c visitPrimaryExp3 ,
	 *      @c visitMul1 , @c visitMul2 , @c visitAdd1 , @c visitAdd2,
	 *      @c visitNumber , @c visitExp , @c visitConstExp
	 * @return_in_retVal <tt>unique_ptr\<StaticValue\></tt> (static value calculated) if @c info.stat.calculatingStaticValue
	 * @return_in_retVal <tt>AddrOperand *</tt> (var of result of this expression) otherwise
	 * @return_in_retInstrs NOTHING if @c info.stat.calculatingStaticValue
	 * @return_in_retInstrs Instrs generated otherwise
	 */
	antlrcpp::Any visitUnary1(SysYParser::Unary1Context * ctx) override;

	antlrcpp::Any visitUnary2(SysYParser::Unary2Context * ctx) override;

	antlrcpp::Any visitUnary3(SysYParser::Unary3Context * ctx) override;

	antlrcpp::Any visitPrimaryExp1(SysYParser::PrimaryExp1Context * ctx) override;

	antlrcpp::Any visitPrimaryExp2(SysYParser::PrimaryExp2Context * ctx) override;

	antlrcpp::Any visitPrimaryExp3(SysYParser::PrimaryExp3Context * ctx) override;

	antlrcpp::Any visitMul1(SysYParser::Mul1Context * ctx) override;

	antlrcpp::Any visitMul2(SysYParser::Mul2Context * ctx) override;

	antlrcpp::Any visitAdd1(SysYParser::Add1Context * ctx) override;

	antlrcpp::Any visitAdd2(SysYParser::Add2Context * ctx) override;

	antlrcpp::Any visitNumber(SysYParser::NumberContext * ctx) override;

	antlrcpp::Any visitExp(SysYParser::ExpContext * ctx) override;

	antlrcpp::Any visitConstExp(SysYParser::ConstExpContext * ctx) override;

	/**
	 * @return_in_retVal <tt>AddrOperand *</tt> (Type of this operand can be int/float/bool !)
	 * @return_in_retInstrs Instrs generated.
	 */
	antlrcpp::Any visitRel1(SysYParser::Rel1Context * ctx) override;

	antlrcpp::Any visitRel2(SysYParser::Rel2Context * ctx) override;

	antlrcpp::Any visitEq1(SysYParser::Eq1Context * ctx) override;

	antlrcpp::Any visitEq2(SysYParser::Eq2Context * ctx) override;

	/**
	 * @return_in_retInstrs Instructions generated.
	 * @note NOTICE: conversion from int/float to bool happened here!
	 */
	antlrcpp::Any visitLAnd1(SysYParser::LAnd1Context * ctx) override;

	antlrcpp::Any visitLAnd2(SysYParser::LAnd2Context * ctx) override;

	antlrcpp::Any visitLOr1(SysYParser::LOr1Context * ctx) override;

	antlrcpp::Any visitLOr2(SysYParser::LOr2Context * ctx) override;

	/**
	 * @brief @b Unused
	 */
	antlrcpp::Any
	visitStringAsRParam(SysYParser::StringAsRParamContext * ctx) override;

	/**
	 * @return_in_retVal <tt>string</tt> (context according to ctx)
	 */
	antlrcpp::Any visitUnaryOp(SysYParser::UnaryOpContext * ctx) override;

	/**
	 * @return_in_retVal @c FuncType
	 */
	antlrcpp::Any visitFuncType(SysYParser::FuncTypeContext * ctx) override;

	/**
	 * @return_in_retVal @c BType
	 */
	antlrcpp::Any visitBType(SysYParser::BTypeContext * ctx) override;
};

}
