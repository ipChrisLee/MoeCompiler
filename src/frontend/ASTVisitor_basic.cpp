#include "ASTVisitor.hpp"


using namespace sup;
namespace frontend {

ASTVisitor::ASTVisitor(ircode::IRModule & irModule)
	: info(), retVal(), symbolTable(), ir(irModule) {
}

antlrcpp::Any ASTVisitor::visitChildren(antlr4::tree::ParseTree * node) {
	for (auto son: node->children) {
		son->accept(this);
	}
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitCompUnit(SysYParser::CompUnitContext * ctx) {
	// compUnit -> (decl | funcDef)* EOF
	return visitChildren(ctx);
}

antlrcpp::Any ASTVisitor::visitDecl(SysYParser::DeclContext * ctx) {
	//  Decl -> ConstDecl | VarDecl
	if (ctx->constDecl()) {
		ctx->constDecl()->accept(this);
	} else if (ctx->varDecl()) {
		ctx->varDecl()->accept(this);
	}
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitVarDecl(SysYParser::VarDeclContext * ctx) {
	// varDecl -> bType varDef (',' varDef)* ';'
	setWithAutoRestorer(info.var.definingConstVar, false);
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.var.btype, retVal.restore<BType>());

	if (info.stat.inGlobal) {
		for (auto son: ctx->varDef()) {
			son->accept(this);
		}
		return nullptr;
	} else {
		auto instrsRes = std::list<ircode::IRInstr *>();
		for (auto son: ctx->varDef()) {
			son->accept(this);
			instrsRes.splice(
				instrsRes.end(), retInstrs.restore<std::list<ircode::IRInstr *>>()
			);
		}
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitBlock(SysYParser::BlockContext * ctx) {
	// block -> '{' (blockItem)* '}'
	auto instrsRes = std::list<ircode::IRInstr *>();
	for (auto p: ctx->blockItem()) {
		p->accept(this);
		auto instrsBlockItem = retInstrs.restore<std::list<ircode::IRInstr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrsBlockItem));
	}
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitBlockItem(SysYParser::BlockItemContext * ctx) {
	// blockItem -> decl | stmt
	return visitChildren(ctx);
}

antlrcpp::Any ASTVisitor::visitAssignment(SysYParser::AssignmentContext * ctx) {
	// stmt -> lVal '=' exp ';' # assignment
	auto instrsRes = std::list<ircode::IRInstr *>();
	ctx->exp()->accept(this);
	auto pExpAddr = retVal.restore<ircode::AddrOperand *>();
	auto instrsExp = retInstrs.restore<std::list<ircode::IRInstr *>>();
	instrsRes.splice(instrsRes.end(), std::move(instrsExp));
	setWithAutoRestorer(info.stat.visitingAssignment, true);
	ctx->lVal()->accept(this);
	auto * pLValMemAddr = retVal.restore<ircode::AddrVariable *>();
	auto instrsLVal = retInstrs.restore<std::list<ircode::IRInstr *>>();
	instrsRes.splice(instrsRes.end(), std::move(instrsLVal));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrStore(pExpAddr, pLValMemAddr)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitExpStmt(SysYParser::ExpStmtContext * ctx) {
	// stmt -> (exp)? ';' # expStmt
	auto instrsRes = std::list<ircode::IRInstr *>();
	if (ctx->exp()) {
		ctx->exp()->accept(this);
		[[maybe_unused]] auto * pValAddr = retVal.restore<ircode::AddrOperand *>();
		auto instrs = retInstrs.restore<std::list<ircode::IRInstr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrs));
	}
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitBlockStmt(SysYParser::BlockStmtContext * ctx) {
	// stmt -> block # blockStmt
	setWithAutoRestorer(symbolTable.pScopeNow, symbolTable.pScopeNow->addSonScope());
	ctx->block()->accept(this);
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitIfStmt1(SysYParser::IfStmt1Context * ctx) {
	// stmt -> 'if' '(' cond ')' stmt # ifStmt1
	auto instrsRes = std::list<ircode::IRInstr *>();
	auto * pLabelThen = ir.addrPool.emplace_back(ircode::AddrJumpLabel("if_then"));
	auto * pLabelEnd = ir.addrPool.emplace_back(ircode::AddrJumpLabel("if_end"));
	setWithAutoRestorer(info.cond.jumpLabelTrue, std::move(pLabelThen));//NOLINT
	setWithAutoRestorer(info.cond.jumpLabelFalse, std::move(pLabelEnd));//NOLINT
	//  visit cond
	ctx->cond()->accept(this);
	auto instrsCond = retInstrs.restore<std::list<ircode::IRInstr *>>();
	//  visit stmt
	ctx->stmt()->accept(this);
	auto instrsStmt = retInstrs.restore<std::list<ircode::IRInstr *>>();
	//  combine all
	instrsRes.splice(instrsRes.end(), std::move(instrsCond));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(info.cond.jumpLabelTrue))
	);
	instrsRes.splice(instrsRes.end(), std::move(instrsStmt));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrBr(info.cond.jumpLabelFalse)
		)
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(info.cond.jumpLabelFalse))
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitIfStmt2(SysYParser::IfStmt2Context * ctx) {
	// stmt -> 'if' '(' cond ')' stmt 'else' stmt # ifStmt2
	auto instrsRes = std::list<ircode::IRInstr *>();
	auto * pLabelThen = ir.addrPool.emplace_back(ircode::AddrJumpLabel("if_then"));
	auto * pLabelElse = ir.addrPool.emplace_back(ircode::AddrJumpLabel("if_else"));
	auto * pLabelEnd = ir.addrPool.emplace_back(ircode::AddrJumpLabel("if_end"));
	setWithAutoRestorer(info.cond.jumpLabelTrue, std::move(pLabelThen));//NOLINT
	setWithAutoRestorer(info.cond.jumpLabelFalse, std::move(pLabelElse));//NOLINT
	//  visit cond
	ctx->cond()->accept(this);
	auto instrsCond = retInstrs.restore<std::list<ircode::IRInstr *>>();
	//  visit stmt on if-then
	ctx->stmt(0)->accept(this);
	auto instrsStmtThen = retInstrs.restore<std::list<ircode::IRInstr *>>();
	//  visit stmt on if-else
	ctx->stmt(1)->accept(this);
	auto instrsStmtElse = retInstrs.restore<std::list<ircode::IRInstr *>>();
	//  combine all
	instrsRes.splice(instrsRes.end(), std::move(instrsCond));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(info.cond.jumpLabelTrue))
	);
	instrsRes.splice(instrsRes.end(), std::move(instrsStmtThen));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrBr(pLabelEnd))
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(info.cond.jumpLabelFalse))
	);
	instrsRes.splice(instrsRes.end(), std::move(instrsStmtElse));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrBr(pLabelEnd))
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(pLabelEnd))
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitWhileStmt(SysYParser::WhileStmtContext * ctx) {
	// stmt -> 'while' '(' cond ')' stmt # whileStmt
	auto instrsRes = std::list<ircode::IRInstr *>();
	auto * pLabelWhileBody
		= ir.addrPool.emplace_back(ircode::AddrJumpLabel("while_body"));
	auto * pLabelWhileEnd
		= ir.addrPool.emplace_back(ircode::AddrJumpLabel("while_end"));
	auto * pLabelWhileCond
		= ir.addrPool.emplace_back(ircode::AddrJumpLabel("while_cond"));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrBr(pLabelWhileCond))
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(pLabelWhileCond))
	);
	{
		setWithAutoRestorer(info.cond.jumpLabelTrue,
		                    std::move(pLabelWhileBody));//NOLINT
		setWithAutoRestorer(info.cond.jumpLabelFalse,
		                    std::move(pLabelWhileEnd));//NOLINT
		ctx->cond()->accept(this);
		instrsRes.splice(
			instrsRes.end(), retInstrs.restore<std::list<ircode::IRInstr *>>()
		);
	}
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(pLabelWhileBody))//NOLINT
	);
	setWithAutoRestorer(info.cond.whileCond, std::move(pLabelWhileCond));//NOLINT
	setWithAutoRestorer(info.cond.whileEnd, std::move(pLabelWhileEnd));//NOLINT
	ctx->stmt()->accept(this);
	instrsRes.splice(
		instrsRes.end(), retInstrs.restore<std::list<ircode::IRInstr *>>()
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrBr(pLabelWhileCond))//NOLINT
	);
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(ircode::InstrLabel(pLabelWhileEnd))//NOLINT
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitBreakStmt(SysYParser::BreakStmtContext * ctx) {
	// stmt -> 'break' ';' # breakStmt
	com::Assert(info.cond.whileEnd, "", CODEPOS);
	auto instrsRes = std::list<ircode::IRInstr *>();
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrBr(info.cond.whileEnd)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitContinueStmt(SysYParser::ContinueStmtContext * ctx) {
	// stmt -> 'continue' ';' # continueStmt
	com::Assert(info.cond.whileCond, "", CODEPOS);
	auto instrsRes = std::list<ircode::IRInstr *>();
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrBr(info.cond.whileCond)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitReturnStmt(SysYParser::ReturnStmtContext * ctx) {
	// stmt -> 'return' (exp)? ';' # returnStmt
	auto instrsRes = std::list<ircode::IRInstr *>();
	if (ctx->exp()) {
		ctx->exp()->accept(this);
		auto * pRetValAddr = retVal.restore<ircode::AddrOperand *>();
		auto instrsExp = retInstrs.restore<std::list<ircode::IRInstr *>>();
		const auto & retValTypeInfo =
			info.func.pFuncDef->getFuncAddrPtr()->getReturnTypeInfo();
		auto [pTmp, type, instrsConversion] =
			genAddrConversion(ir, pRetValAddr, retValTypeInfo);
		instrsRes.splice(instrsRes.end(), std::move(instrsExp));
		instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrStore(pTmp, info.func.pRetvalMem)
			)
		);
	}
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrBr(info.func.pRetBlockLabel)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitCond(SysYParser::CondContext * ctx) {
	// cond -> lOrExp
	return ctx->lOrExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitBType(SysYParser::BTypeContext * ctx) {
	retVal.save<BType>(strToBType(ctx->getText()));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitFuncType(SysYParser::FuncTypeContext * ctx) {
	retVal.save(strToFuncType(ctx->getText()));
	return nullptr;
}

ASTVisitor::BType ASTVisitor::strToBType(const std::string & str) {
	if (str == "float") {
		return BType::Float;
	} else if (str == "int") {
		return BType::Int;
	} else {
		com::Throw("Unknown BType[" + str + "]!", CODEPOS);
	}
}

ASTVisitor::FuncType ASTVisitor::strToFuncType(const std::string & str) {
	if (str == "float") {
		return FuncType::Float;
	} else if (str == "int") {
		return FuncType::Int;
	} else if (str == "void") {
		return FuncType::Void;
	} else {
		com::Throw("Unknown BType[" + str + "]!", CODEPOS);
	}
}

std::unique_ptr<TypeInfo> ASTVisitor::bTypeToTypeInfoUPtr(
	ASTVisitor::BType btype, const std::vector<int> & shape
) {
	if (shape.empty()) {
		switch (btype) {
			case BType::Float: {
				return std::make_unique<FloatType>();
			}
			case BType::Int: {
				return std::make_unique<IntType>();
			}
			default: {
				com::Throw("Error BType!", CODEPOS);
			}
		}
	} else {
		switch (btype) {
			case BType::Float: {
				return std::make_unique<FloatArrayType>(shape);
			}
			case BType::Int: {
				return std::make_unique<IntArrayType>(shape);
			}
			default: {
				com::Throw("Error BType!", CODEPOS);
			}
		}
	}
}


}