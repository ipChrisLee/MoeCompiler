#include "CFGIR.hpp"


namespace pass {

void CFGIR::globalVarOpt() {
	//  for int/float:
	//      if a variable is only used (load), promote to constant
	//          This is implemented with replacing `%x=load %p` by `%x=add 1,0`
	//      if a variable is only stored or even not used, delete it.
	auto loaded = std::set<ircode::AddrGlobalVariable *>();
	auto stored = std::set<ircode::AddrGlobalVariable *>();
	for (auto pFuncDef: ir.funcPool) {
		for (auto * pInstr: pFuncDef->instrs) {
			switch (pInstr->instrType) {
				case ircode::InstrType::Load: {
					auto * pLoad = dynamic_cast<ircode::InstrLoad *>(pInstr);
					if (pLoad->from->addrType == ircode::AddrType::GlobalVar) {
						auto * pGV = dynamic_cast<ircode::AddrGlobalVariable *>(pLoad->from);
						loaded.insert(pGV);
					}
					break;
				}
				case ircode::InstrType::Store: {
					auto * pStore = dynamic_cast<ircode::InstrStore *>(pInstr);
					if (pStore->to->addrType == ircode::AddrType::GlobalVar) {
						auto * pGV = dynamic_cast<ircode::AddrGlobalVariable *>(pStore->to);
						stored.insert(pGV);
					}
					break;
				}
				default:break;
			}
		}
	}
	auto constGV = std::set<ircode::AddrGlobalVariable *>();
	auto deadGV = std::set<ircode::AddrGlobalVariable *>();
	for (auto pGV: ir.addrPool.getGlobalVars()) {
		const auto & pointerType = dynamic_cast<const sup::PointerType &>(pGV->getType());
		if (com::enum_fun::in(
			pointerType.pointTo->type, {sup::Type::Int_t, sup::Type::Float_t}
		)) {
			if (loaded.count(pGV) && !stored.count(pGV)) {
				//  only loaded but not stored. promote to const.
				constGV.insert(pGV);
			} else if (!loaded.count(pGV)) {
				//  never be loaded. mark as dead.
				deadGV.insert(pGV);
			}
		}
	}
	for (auto pFuncDef: ir.funcPool) {
		auto itPInstr = pFuncDef->instrs.begin();
		while (itPInstr != pFuncDef->instrs.end()) {
			auto * pInstr = get(itPInstr);
			switch (pInstr->instrType) {
				case ircode::InstrType::Load: {
					auto * pLoad = dynamic_cast<ircode::InstrLoad *>(pInstr);
					auto * pGV = dynamic_cast<ircode::AddrGlobalVariable *>(pLoad->from);
					auto * pTo = dynamic_cast<ircode::AddrVariable *>(pLoad->to);
					if (pGV && constGV.count(pGV)) {
						auto * pSV0 = ir.addrPool.emplace_back(
							ircode::AddrStaticValue(pGV->getStaticValue().getType())
						);
						auto * pSVVal = ir.addrPool.emplace_back(
							ircode::AddrStaticValue(pGV->getStaticValue())
						);
						if (pSV0->getType().type == sup::Type::Int_t) {
							pInstr = ir.instrPool.emplace_back(
								ircode::InstrAdd(pSV0, pSVVal, pTo)
							);
						} else if (pSV0->getType().type == sup::Type::Float_t) {
							pInstr = ir.instrPool.emplace_back(
								ircode::InstrFAdd(pSV0, pSVVal, pTo)
							);
						} else {
							com::Throw("", CODEPOS);
						}
						itPInstr = pFuncDef->instrs.erase(itPInstr);
						pFuncDef->instrs.emplace(itPInstr, pInstr);
					}
					itPInstr = std::next(itPInstr);
					break;
				}
				case ircode::InstrType::Store: {
					auto * pStore = dynamic_cast<ircode::InstrStore *>(pInstr);
					auto pGV = dynamic_cast<ircode::AddrGlobalVariable *>(pStore->to);
					if (pGV && deadGV.count(pGV)) {
						itPInstr = pFuncDef->instrs.erase(itPInstr);
					} else {
						itPInstr = std::next(itPInstr);
					}
					break;
				}
				default: {
					itPInstr = std::next(itPInstr);
					break;
				}
			}
		}
	}
	for (auto * pGV: deadGV) {
		ir.addrPool.erase(pGV);
	}
}

}