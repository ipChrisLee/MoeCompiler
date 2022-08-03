#include "ASTVisitor.hpp"


using namespace mir;

namespace frontend {

antlrcpp::Any ASTVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext * ctx) {
	//  varDef -> Identifier ('[' constExp ']')* # uninitVarDef
	auto varname = ctx->Identifier()->getText();
	auto shape = [ctx, this]() -> std::vector<int> {
		auto ret = std::vector<int>();
		for (auto p: ctx->constExp()) {
			p->accept(this);
			auto psv = retVal.restore<std::unique_ptr<StaticValue>>();
			ret.push_back(
				com::dynamic_cast_uPtr_get<IntStaticValue>(psv)->value
			);
		}
		return ret;
	}();
	setWithAutoRestorer(info.var.shapeOfDefiningVar, std::move(shape));
	auto varTypInfo =
		bTypeToTypeInfoUPtr(info.var.btype, info.var.shapeOfDefiningVar);
	if (info.stat.inGlobal) {
		auto pAddr = ir.addrPool.emplace_back(
			mir::AddrGlobalVariable(*varTypInfo, varname)
		);
		symbolTable.pScopeNow->bindDominateVar(
			varname, IdType::GlobalVarName, pAddr
		);
		return nullptr;
	} else {
		auto pAddr = ir.addrPool.emplace_back(
			mir::AddrLocalVariable(*varTypInfo, varname)
		);
		auto instrsRes = std::list<mir::Instr *>();
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				mir::InstrAlloca(pAddr)
			)
		);
		symbolTable.pScopeNow->bindDominateVar(
			varname, IdType::LocalVarName, pAddr
		);
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitInitVarDef(SysYParser::InitVarDefContext * ctx) {
	// varDef -> Identifier ('[' constExp ']')* '=' initVal # initVarDef
	auto varName = ctx->Identifier()->getText();
	auto shape = [ctx, this]() -> std::vector<int> {
		auto ret = std::vector<int>();
		for (auto p: ctx->constExp()) {
			p->accept(this);
			auto psv = retVal.restore<std::unique_ptr<StaticValue>>();
			ret.push_back(
				com::dynamic_cast_uPtr_get<IntStaticValue>(psv)->value
			);
		}
		return ret;
	}();
	setWithAutoRestorer(info.var.shapeOfDefiningVar, std::move(shape));
	auto varTypeInfo =
		bTypeToTypeInfoUPtr(info.var.btype, info.var.shapeOfDefiningVar);
	auto eleTypeInfo = bTypeToTypeInfoUPtr(info.var.btype);
	if (info.stat.inGlobal) {
		//  In global decl, every val in `initVal` is constVar or static value.
		//  So the return value of `visitInitVal` is `std::unique_ptr<StaticValue>`
		//  Get init val by visiting `constInitVal`.
		setWithAutoRestorer(info.stat.calculatingStaticValue, true);
		setWithAutoRestorer(info.var.idxView, IdxView(info.var.shapeOfDefiningVar));
		setWithAutoRestorer(info.var.ndim, -1);
		setWithAutoRestorer(info.var.staticArrayItems, {});
		ctx->initVal()->accept(this);
		//  Create addr of static var.
		auto pSV = fromArrayItemsToStaticValue(
			ir, info.var.staticArrayItems, info.var.shapeOfDefiningVar, *eleTypeInfo
		);
		auto pAddr = ir.addrPool.emplace_back(
			mir::AddrGlobalVariable(*varTypeInfo, varName, *pSV)
		);
		symbolTable.pScopeNow->bindDominateVar(
			varName, IdType::GlobalVarName, pAddr
		);
		return nullptr;
	} else {
		auto instrsRes = std::list<mir::Instr *>();
		//  In local decl, val in `initVal` may be other value, and so will return
		setWithAutoRestorer(info.stat.calculatingStaticValue, false);
		setWithAutoRestorer(info.var.idxView, IdxView(info.var.shapeOfDefiningVar));
		setWithAutoRestorer(info.var.ndim, -1);
		setWithAutoRestorer(info.var.localArrayItems, {});
		ctx->initVal()->accept(this);
		auto pVarAddr = ir.addrPool.emplace_back(
			mir::AddrLocalVariable(*varTypeInfo, varName)
		);
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				mir::InstrAlloca(pVarAddr)
			)
		);
		instrsRes.splice(
			instrsRes.end(), fromArrayItemsToInstrs(
				ir, std::move(info.var.localArrayItems), info.var.shapeOfDefiningVar,
				pVarAddr, *eleTypeInfo
			)
		);
		symbolTable.pScopeNow->bindDominateVar(
			varName, IdType::LocalVarName, pVarAddr
		);
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any
ASTVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext * ctx) {
	// initVal -> exp # scalarInitVal
	ctx->exp()->accept(this);
	if (info.stat.inGlobal) {
		info.var.staticArrayItems.emplace_back(
			info.var.idxView.idx, retVal.restore<std::unique_ptr<StaticValue>>()
		);
	} else {
		auto * pAddr = retVal.restore<mir::AddrOperand *>();
		auto instrs = retInstrs.restore<std::list<mir::Instr *>>();
		info.var.localArrayItems.emplace_back(
			info.var.idxView.idx, std::move(pAddr), std::move(instrs)//NOLINT
		);
	}
	if (info.var.definingArray()) {
		info.var.idxView.addOnDimN(-1, 1);
	}
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitListInitval(SysYParser::ListInitvalContext * ctx) {
	// initVal -> '{' (initVal (',' initVal)* )? '}' # listInitval
	auto added = false;
	info.var.idxView.set0AfterNDim(info.var.ndim);
	++info.var.ndim;
	//  visiting elements index on `ndim`
	for (auto p: ctx->initVal()) {
		p->accept(this);
		added = true;
	}
	--info.var.ndim;
	while (!info.var.idxView.isAll0AfterNDim(info.var.ndim) || !added) {
		auto upTypeInfo = bTypeToTypeInfoUPtr(info.var.btype);
		auto * pZero = ir.addrPool.emplace_back(
			mir::AddrStaticValue(*upTypeInfo)
		);
		info.var.localArrayItems.emplace_back(
			info.var.idxView.idx, pZero, std::list<mir::Instr *>()
		);
		info.var.idxView.addOnDimN(-1, 1);
		added = true;
	}
	return nullptr;
}

}