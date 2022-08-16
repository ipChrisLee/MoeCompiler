#include "ASTVisitor.hpp"


using namespace sup;
namespace frontend {

antlrcpp::Any ASTVisitor::visitConstDecl(SysYParser::ConstDeclContext * ctx) {
	auto instrsRes = std::list<ircode::IRInstr *>();
	//  ConstDecl -> "const" BType ConstDef {"," ConstDef};
	setWithAutoRestorer(info.var.definingConstVar, true);
	//  Get BType.
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.var.btype, retVal.restore<BType>());
	if (info.stat.inGlobal) {
		for (auto son: ctx->constDef()) {
			son->accept(this);
		}
		return nullptr;
	} else {
		for (auto son: ctx->constDef()) {
			son->accept(this);
			instrsRes.splice(
				instrsRes.end(),
				retInstrs.restore<std::list<ircode::IRInstr * >>()
			);
		}
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitConstDef(SysYParser::ConstDefContext * ctx) {
	//  ConstDef -> Ident {"[" ConstExp "]"}} "=" ConstInitVal
	//  Save name of Ident to `varName`.
	auto varName = ctx->Identifier()->getText();
	//  Create definition.
	auto shape = [ctx, this]() -> std::vector<int> {
		auto ret = std::vector<int>();
		for (auto son: ctx->constExp()) {
			son->accept(this);
			auto len = retVal.restore<std::unique_ptr<StaticValue >>();
			ret.push_back(
				com::dynamic_cast_uPtr_get<IntStaticValue>(len)->value
			);
		}
		return ret;
	}();
	setWithAutoRestorer(info.var.shapeOfDefiningVar, std::move(shape));
	auto varTypeInfo =
		bTypeToTypeInfoUPtr(info.var.btype, info.var.shapeOfDefiningVar);
	auto eleTypeInfo = bTypeToTypeInfoUPtr(info.var.btype);
	//  define first
	if (info.stat.inGlobal) {
		auto pAddr = ir.addrPool.emplace_back(
			ircode::AddrGlobalVariable(*varTypeInfo, varName, true)
		);
		symbolTable.pScopeNow->bindDominateVar(varName, IdType::GlobalVarName, pAddr);
		info.var.pAddrGlobalVarDefining = pAddr;
	} else {
		auto pVarAddr = ir.addrPool.emplace_back(
			ircode::AddrLocalVariable(*varTypeInfo, varName, true)
		);
		symbolTable.pScopeNow->bindDominateVar(varName, IdType::LocalVarName, pVarAddr);
		info.var.pAddrLocalVarDefining = pVarAddr;
	}
	//  Get init val by visiting `constInitVal`.
	setWithAutoRestorer(info.var.idxView, IdxView(info.var.shapeOfDefiningVar));
	setWithAutoRestorer(info.var.ndim, -1);
	setWithAutoRestorer(info.var.staticArrayItems, {});
	ctx->constInitVal()->accept(this);
	//  Create addr of static var.
	if (info.stat.inGlobal) {
		info.var.pAddrGlobalVarDefining = nullptr;
		return nullptr;
	} else {
		auto pVarAddr = dynamic_cast<ircode::AddrLocalVariable *>(
			get<1>(symbolTable.pScopeNow->findIdDownToRoot(varName))
		);
		auto pSVAddr = ir.addrPool.emplace_back(
			ircode::AddrStaticValue(pVarAddr->getStaticValue())
		);
		auto instrsRes = std::list<ircode::IRInstr *>();
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrAlloca(pVarAddr)
			)
		);
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrStore(pSVAddr, pVarAddr)
			)
		);
		retInstrs.save(std::move(instrsRes));
		info.var.pAddrLocalVarDefining = nullptr;
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitConstExp(SysYParser::ConstExpContext * ctx) {
	//  ConstExp -> AddExp
	setWithAutoRestorer(info.stat.calculatingStaticValue, true);
	return ctx->addExp()->accept(this);
}

antlrcpp::Any
ASTVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext * ctx) {
	// constInitVal -> constExp # scalarConstInitVal
	ctx->constExp()->accept(this);
	auto upStaticVar = retVal.restore<std::unique_ptr<StaticValue >>();
	if (info.stat.inGlobal) {
		info.var.pAddrGlobalVarDefining->uPtrStaticValue->insertValue(
			info.var.idxView.idx, *upStaticVar
		);
	} else {
		info.var.pAddrLocalVarDefining->uPtrStaticValue->insertValue(
			info.var.idxView.idx, *upStaticVar
		);
	}
	if (info.var.definingArray()) {
		info.var.idxView.addOnDimN(-1, 1);
	}
	return nullptr;
}

antlrcpp::Any
ASTVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext * ctx) {
	// constInitVal -> '{' (constInitVal (',)* )? '}' # listConstInitVal
	auto added = false;
	info.var.idxView.set0AfterNDim(info.var.ndim);
	++info.var.ndim;
	//  visiting elements index on `ndim`
	for (auto p: ctx->constInitVal()) {
		p->accept(this);
		added = true;
	}
	--info.var.ndim;
	if (!info.var.idxView.isAll0AfterNDim(info.var.ndim) || !added) {
		info.var.idxView.set0AfterNDimAndCarry(info.var.ndim);
	}
	return nullptr;
}


}