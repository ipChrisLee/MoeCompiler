#ifndef IGNORE_WHEN_TESTING

#include "ASTVisitor.hpp"


namespace frontend {

ASTVisitor::ASTVisitor() : addrPool(), info(), retVal() {
	pScopeNow = addrPool.getRootScope();
}

std::any ASTVisitor::visitChildren(antlr4::tree::ParseTree * node) {
	for (auto son : node->children) {
		son->accept(this);
	}
	return nullptr;
}

std::any ASTVisitor::visitCompUnit(SysYParser::CompUnitContext * ctx) {
	return visitChildren(ctx);
}

std::any ASTVisitor::visitDecl(SysYParser::DeclContext * ctx) {
	//  Decl -> ConstDecl | VarDecl
	return visitChildren(ctx);
}

std::any ASTVisitor::visitConstDecl(SysYParser::ConstDeclContext * ctx) {
	//  ConstDecl -> "const" BType ConstDef {"," ConstDef};
	info.isConst = true;
	ctx->bType()->accept(this);
	for (auto son : ctx->constDef()) {
		son->accept(this);
	}
	info.isConst=false;
	return nullptr;
}

std::any ASTVisitor::visitConstDef(SysYParser::ConstDefContext * ctx) {
	//  ConstDef -> Ident {"[" ConstExp "]"}} "=" ConstInitVal
	std::string varname = ctx->Identifier()->getText();
	if (!ctx->constExp().empty()) {   //    Defining an array.
		std::vector<int> shape;
		for (auto son : ctx->constExp()) {
			son->accept(this);
			auto len = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
			shape.push_back(
				com::dynamic_cast_uPtr<ircode::IntStaticValue>(len)->value);
		}
		ctx->constInitVal()->accept(this);
		com::TODO("", CODEPOS);
	} else { // Defining a variable
		ctx->constInitVal()->accept(this);
		std::unique_ptr<ircode::TypeInfo> uType = bTypeToTypeInfoUPtr(info.btype);
		auto constVal = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ircode::Addr * pAddr = nullptr;
		if (info.inGlobal) {
			pAddr = addrPool.addAddrToScope(
				ircode::AddrGlobalVariable(*uType, varname, *constVal),
				pScopeNow, ircode::IdType::GlobalVarName, varname);
			instrPool.addInstrToPool(ircode::instr::DeclGlobal(
				dynamic_cast<ircode::AddrGlobalVariable *>(pAddr)));
		} else {
			pAddr = addrPool.addAddrToScope(
				ircode::AddrVariable(*uType, varname, *constVal),
				pScopeNow, ircode::IdType::GlobalVarName, varname);
			com::TODO("", CODEPOS);
		}
		return nullptr;
	}
}

std::any ASTVisitor::visitConstExp(SysYParser::ConstExpContext * ctx) {
	//  ConstExp -> AddExp
	info.visitingConst = true;
	ctx->addExp()->accept(this);
	auto ret = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
	info.visitingConst = false;
	retVal.save(std::move(ret));
	return nullptr;
}

std::any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext * ctx) {
	info.inGlobal = false;
	com::TODO("", CODEPOS);
	info.inGlobal = true;
	return nullptr;
}

std::any ASTVisitor::visitVarDecl(SysYParser::VarDeclContext * ctx) {
	info.isConst = false;
	com::TODO("", CODEPOS);
	return nullptr;
}

std::any ASTVisitor::visitBType(SysYParser::BTypeContext * ctx) {
	info.btype = strToBType(ctx->getText());
	return nullptr;
}

std::any ASTVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext * ctx) {
	// constInitVal -> constExp # scalarConstInitVal
	return visitChildren(ctx);
}

std::any ASTVisitor::visitListInitval(SysYParser::ListInitvalContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitFuncType(SysYParser::FuncTypeContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitInitVarDef(SysYParser::InitVarDefContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitFuncFParams(SysYParser::FuncFParamsContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any
ASTVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext * ctx) {
	// constInitVal -> '{' (constInitVal (',' constInitVal)* )? '}' # listConstInitVal
	com::TODO("", CODEPOS);
}

std::any
ASTVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext * ctx) {
	// constInitVal -> constExp # scalarConstInitVal
	return visitChildren(ctx);
}

std::any ASTVisitor::visitFuncFParam(SysYParser::FuncFParamContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBlock(SysYParser::BlockContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBlockItem(SysYParser::BlockItemContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitAssignment(SysYParser::AssignmentContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExpStmt(SysYParser::ExpStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBlockStmt(SysYParser::BlockStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitIfStmt1(SysYParser::IfStmt1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitIfStmt2(SysYParser::IfStmt2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitWhileStmt(SysYParser::WhileStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBreakStmt(SysYParser::BreakStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitContinueStmt(SysYParser::ContinueStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitReturnStmt(SysYParser::ReturnStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExp(SysYParser::ExpContext * ctx) {
	// exp -> addExp
	return visitChildren(ctx);
}

std::any ASTVisitor::visitCond(SysYParser::CondContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLVal(SysYParser::LValContext * ctx) {
	// lVal -> Identifier ('[' exp ']')*
	if (info.visitingConst) {
		com::TODO("To write after addr pool finished.", CODEPOS);
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context * ctx) {
	// primaryExp -> '(' exp ')' # primaryExp1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context * ctx) {
	// primaryExp -> lVal # primaryExp2
	return visitChildren(ctx);
}

std::any ASTVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context * ctx) {
	// primaryExp -> number # primaryExp3
	return visitChildren(ctx);
}

std::any ASTVisitor::visitNumber(SysYParser::NumberContext * ctx) {
	// number -> IntLiteral | FloatLiteral
	if (ctx->FloatLiteral()) {
		std::unique_ptr<ircode::StaticValue> v =
			std::make_unique<ircode::FloatStaticValue>(ctx->getText());
		retVal.save(std::move(v));
		return nullptr;
	} else if (ctx->IntLiteral()) {
		std::unique_ptr<ircode::StaticValue> v =
			std::make_unique<ircode::IntStaticValue>(ctx->getText());
		retVal.save(std::move(v));
		return nullptr;
	} else {
		com::Throw("ctx should be float/int literal.", CODEPOS);
	}
}

std::any ASTVisitor::visitUnary1(SysYParser::Unary1Context * ctx) {
	// unaryExp -> primaryExp # unary1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitUnary2(SysYParser::Unary2Context * ctx) {
	// unaryExp -> Identifier '(' (funcRParams)? ')' # unary2
	if (info.visitingConst) {
		com::Throw(
			com::concatToString(
				{
					"Const variable should be initialized with compile-time value, ",
					"but value calculated by function call is not compile time value ",
					"according to SysY semantic definition."
				}), CODEPOS
		);
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitUnary3(SysYParser::Unary3Context * ctx) {
	// unaryExp -> unaryOp unaryExp # unary3
	ctx->unaryOp()->accept(this);
	auto op = retVal.restore<std::string>();
	if (info.visitingConst) {
		ctx->unaryExp()->accept(this);
		auto p = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		retVal.save(p->calc(op));
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitUnaryOp(SysYParser::UnaryOpContext * ctx) {
	// unaryOp -> '+' | '-' | '!'
	if (auto p1 = ctx->Addition()) {
		retVal.save(p1->getText());
	} else if (auto p2 = ctx->Minus()) {
		retVal.save(p2->getText());
	} else if (auto p3 = ctx->Exclamation()) {
		retVal.save(p3->getText());
	} else {
		com::Throw("Unary op should be one of '+','-' or '!'.", CODEPOS);
	}
	return nullptr;
}

std::any ASTVisitor::visitFuncRParams(SysYParser::FuncRParamsContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExpAsRParam(SysYParser::ExpAsRParamContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitStringAsRParam(SysYParser::StringAsRParamContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitMul2(SysYParser::Mul2Context * ctx) {
	// mulExp -> mulExp ('*' | '/' | '%') unaryExp # mul2
	if (info.visitingConst) {
		ctx->mulExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ctx->unaryExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		std::string op;
		if (ctx->Division()) {
			op = "/";
		} else if (ctx->Modulo()) {
			op = "%";
		} else if (ctx->Multiplication()) {
			op = "*";
		} else {
			com::Throw("ctx should contain '/' or '%' or '*'.", CODEPOS);
		}
		retVal.save(resL->calc(*resR, op));
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitMul1(SysYParser::Mul1Context * ctx) {
	// mulExp -> unaryExp # mul1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitAdd2(SysYParser::Add2Context * ctx) {
	//  addExp -> addExp ('+'|'-') mulExp # add 2
	if (info.visitingConst) {
		ctx->addExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ctx->mulExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		if (ctx->Minus()) {
			retVal.save(resL->calc(*resR, "-"));
		} else if (ctx->Addition()) {
			retVal.save(resL->calc(*resR, "+"));
		} else {
			com::Throw("ctx should contain '+' or '-'.", CODEPOS);
		}
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitAdd1(SysYParser::Add1Context * ctx) {
	//  addExp -> mulExp # add1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitRel2(SysYParser::Rel2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitRel1(SysYParser::Rel1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitEq1(SysYParser::Eq1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitEq2(SysYParser::Eq2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLAnd2(SysYParser::LAnd2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLAnd1(SysYParser::LAnd1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLOr1(SysYParser::LOr1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLOr2(SysYParser::LOr2Context * ctx) {
	com::TODO("", CODEPOS);
}


}
#endif