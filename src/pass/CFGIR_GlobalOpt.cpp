#include "CFGIR.hpp"
#include <queue>


namespace pass {

void CFG::constantPropagation() {
	//  if pVar is promoted to constant, then any def whose instr use pVar may be promoted to constant.
	auto qConstVar
		= std::queue<std::pair<ircode::AddrVariable *, ircode::AddrStaticValue *>>();
	auto checkIfPromotable =
		[&qConstVar, this](Node * pNode, auto it) -> void {
			switch (get(it)->instrType) {
				case ircode::InstrType::SRem:
				case ircode::InstrType::Sub:
				case ircode::InstrType::FSub:
				case ircode::InstrType::Mul:
				case ircode::InstrType::FMul:
				case ircode::InstrType::SDiv:
				case ircode::InstrType::FDiv:
				case ircode::InstrType::FAdd:
				case ircode::InstrType::Add: {
					auto * pBOp = dynamic_cast<ircode::InstrBinaryOp *>(get(it));
					if (pBOp->left->addrType == ircode::AddrType::StaticValue &&
						pBOp->right->addrType == ircode::AddrType::StaticValue) {
						const auto & leftSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pBOp->left
							)->getStaticValue();
						const auto & rightSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pBOp->right
							)->getStaticValue();
						auto upSV = sup::calcOfSV(leftSV, rightSV, pBOp->op);
						auto * pSV = ir.addrPool.template emplace_back(
							ircode::AddrStaticValue(std::move(upSV))
						);
						qConstVar.template emplace(pBOp->res, pSV);
					}
					break;
				}
				case ircode::InstrType::Sitofp: {
					auto * pSitofp = dynamic_cast<ircode::InstrSitofp *>(get(it));
					if (pSitofp->from->addrType == ircode::AddrType::StaticValue) {
						auto * pSVAddr
							= dynamic_cast<ircode::AddrStaticValue *>(pSitofp->from);
						auto value = dynamic_cast<const sup::IntStaticValue &>(
							pSVAddr->getStaticValue()
						).value;
						auto * pFloatSV = ir.addrPool.template emplace_back<>(
							ircode::AddrStaticValue(sup::FloatStaticValue(float(value)))
						);
						qConstVar.template emplace(pSitofp->to, pFloatSV);
					}
					break;
				}
				case ircode::InstrType::Fptosi: {
					auto * pSitofp = dynamic_cast<ircode::InstrFptosi *>(get(it));
					if (pSitofp->from->addrType == ircode::AddrType::StaticValue) {
						auto * pSVAddr
							= dynamic_cast<ircode::AddrStaticValue *>(pSitofp->from);
						auto value = dynamic_cast<const sup::FloatStaticValue &>(
							pSVAddr->getStaticValue()
						).value;
						auto * pIntSV = ir.addrPool.template emplace_back<>(
							ircode::AddrStaticValue(sup::IntStaticValue(int(value)))
						);
						qConstVar.template emplace(pSitofp->to, pIntSV);
					}
					break;
				}
				default:break;
			}
		};
	collectInfoFromAllReachableInstr(checkIfPromotable);
	while (!qConstVar.empty()) {
		auto [pVar, pSV] = qConstVar.front();
		qConstVar.pop();
		auto affectedVar = replaceUse(pVar, pSV);
		for (auto * pVarAffected: affectedVar) {
			auto & defPos = duChain.find(pVarAffected)->second.def;
			checkIfPromotable(defPos.pNode, defPos.it);
		}
		removeDef(pVar);
	}
}

}
