#include "ASTVisitor.hpp"


namespace frontend {

ASTVisitor::ASTVisitor() : addrPool(),inGlobal(true),btype(BType::Error),isConst(false) {
	pScope=addrPool.getRootScope();
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
	btype = strToBType(ctx->bType()->getText());
	isConst=true;
	for (auto son : ctx->constDef()) {
		son->accept(this);
	}
	notFinished("DOING");
}

std::any ASTVisitor::visitConstDef(SysYParser::ConstDefContext * ctx) {
	//  ConstDef -> Ident {"[" ConstExp "]"}} "=" ConstInitVal
	std::string varname=ctx->Identifier()->getText();
	if(!ctx->constExp().empty()){   //  ctx is an array.
		std::vector<int>shape;
		for(auto son:ctx->constExp()){
			shape.push_back(std::any_cast<int>(visitConstExp(son)));
		}
	}
	return SysYBaseVisitor::visitConstDef(ctx);
}

std::any ASTVisitor::visitConstExp(SysYParser::ConstExpContext * ctx) {
	notFinished("DOING");
	return SysYBaseVisitor::visitConstExp(ctx);
}


}
