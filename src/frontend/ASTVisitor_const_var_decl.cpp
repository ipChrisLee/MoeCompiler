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
	//  Get init val by visiting `constInitVal`.
	setWithAutoRestorer(info.var.idxView, IdxView(info.var.shapeOfDefiningVar));
	setWithAutoRestorer(info.var.ndim, -1);
	setWithAutoRestorer(info.var.staticArrayItems, {});
	setWithAutoRestorer(info.var.varNameDefining, std::string(varName));
	ctx->constInitVal()->accept(this);
	//  Create addr of static var.
	auto pSV = fromArrayItemsToStaticValue(
		ir, info.var.staticArrayItems, info.var.shapeOfDefiningVar, *eleTypeInfo
	);
	if (info.stat.inGlobal) {
		auto pAddr = ir.addrPool.emplace_back(
			ircode::AddrGlobalVariable(*varTypeInfo, varName, *pSV, true)
		);
		symbolTable.pScopeNow
		           ->bindDominateVar(varName, IdType::GlobalVarName, pAddr);
		return nullptr;
	} else {
		auto pVarAddr = ir.addrPool.emplace_back(
			ircode::AddrLocalVariable(*varTypeInfo, varName, *pSV)
		);
		auto pSVAddr = ir.addrPool.emplace_back(
			ircode::AddrStaticValue(*pSV)
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
		symbolTable.pScopeNow
		           ->bindDominateVar(varName, IdType::LocalVarName, pVarAddr);
		retInstrs.save(std::move(instrsRes));
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
	info.var.staticArrayItems.insert(
		ArrayItem<std::unique_ptr<sup::StaticValue>>(
			info.var.idxView.idx, retVal.restore<std::unique_ptr<StaticValue >>()
		)
	);
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
	while (!info.var.idxView.isAll0AfterNDim(info.var.ndim) || !added) {
		info.var.staticArrayItems.insert(
			ArrayItem<std::unique_ptr<sup::StaticValue>>(
				info.var.idxView.idx,
				zeroExtensionValueOfType(*bTypeToTypeInfoUPtr(info.var.btype))
			)
		);
		info.var.idxView.addOnDimN(-1, 1);
		added = true;
	}
	return nullptr;
}


}