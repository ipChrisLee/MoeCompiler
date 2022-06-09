#ifndef IGNORE_WHEN_TESTING

#include "ASTVisitor.hpp"


namespace frontend {

ASTVisitor::ASTVisitor() : addrPool(), info() {
	pScope = addrPool.getRootScope();
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
	return nullptr;
}

std::any ASTVisitor::visitConstDef(SysYParser::ConstDefContext * ctx) {
	//  ConstDef -> Ident {"[" ConstExp "]"}} "=" ConstInitVal
	std::string varname = ctx->Identifier()->getText();
	if (!ctx->constExp().empty()) {   //    Defining an array.
		std::vector<int> shape;
		for (auto son : ctx->constExp()) {
			std::any a = son->accept(this);
			//  TODO: a maybe StaticValue.
			shape.push_back(std::any_cast<int>(a));
		}
	}
	return SysYBaseVisitor::visitConstDef(ctx);
}

std::any ASTVisitor::visitConstExp(SysYParser::ConstExpContext * ctx) {
	//  ConstExp -> AddExp
	info.visitingConst=true;
	std::any ret=ctx->addExp()->accept(this);
	info.visitingConst=false;
	return ret;
}

std::any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext * ctx) {
	info.inGlobal = false;
	notFinished("DOING");
	info.inGlobal = true;
	return nullptr;
}

std::any ASTVisitor::visitVarDecl(SysYParser::VarDeclContext * ctx) {
	info.isConst = false;
	notFinished("DOING");
	return nullptr;
}

std::any ASTVisitor::visitBType(SysYParser::BTypeContext * ctx) {
	info.btype = strToBType(ctx->getText());
	return nullptr;
}


}
#endif