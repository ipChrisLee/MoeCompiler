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
				case ircode::InstrType::ICmp: {
					auto * pICmp = dynamic_cast<ircode::InstrICmp *>(get(it));
					if (pICmp->leftOp->addrType == ircode::AddrType::StaticValue &&
						pICmp->rightOp->addrType == ircode::AddrType::StaticValue) {
						const auto & leftSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pICmp->leftOp
							)->getStaticValue();
						const auto & rightSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pICmp->rightOp
							)->getStaticValue();
						auto op = ircode::to_op(pICmp->icmp);
						auto upSV = sup::calcOfSV(leftSV, rightSV, op);
						auto * pSV = ir.addrPool.template emplace_back(
							ircode::AddrStaticValue(std::move(upSV))
						);
						qConstVar.template emplace(pICmp->dest, pSV);
					}
					break;
				}
				case ircode::InstrType::FCmp: {
					auto * pFCmp = dynamic_cast<ircode::InstrFCmp *>(get(it));
					if (pFCmp->leftOp->addrType == ircode::AddrType::StaticValue &&
						pFCmp->rightOp->addrType == ircode::AddrType::StaticValue) {
						const auto & leftSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pFCmp->leftOp
							)->getStaticValue();
						const auto & rightSV
							= dynamic_cast<ircode::AddrStaticValue *>(
								pFCmp->rightOp
							)->getStaticValue();
						auto op = ircode::to_op(pFCmp->fcmp);
						auto upSV = sup::calcOfSV(leftSV, rightSV, op);
						auto * pSV = ir.addrPool.template emplace_back(
							ircode::AddrStaticValue(std::move(upSV))
						);
						qConstVar.template emplace(pFCmp->dest, pSV);
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

void CFG::sccp() {
	auto vecPNodeFound = std::vector<Node *>();
	auto vecItPInstrBr = std::vector<std::list<ircode::IRInstr *>::iterator>();

	collectInfoFromAllReachableInstr(
		[&vecPNodeFound, &vecItPInstrBr](Node * pNodeNow, auto it) -> void {
			auto * pInstr = get(it);
			if (pInstr->instrType == ircode::InstrType::Br) {
				auto * pBr = dynamic_cast<ircode::InstrBr *>(pInstr);
				if (pBr->pCond && pBr->pCond->addrType == ircode::AddrType::StaticValue) {
					vecPNodeFound.template emplace_back(pNodeNow);
					vecItPInstrBr.template emplace_back(it);
				}
			}
		}
	);
	auto n = int(vecPNodeFound.size());
	for (auto i = 0; i < n; ++i) {
		auto * pNodeFound = vecPNodeFound[i];
		auto * pInstr = get(vecItPInstrBr[i]);
		if (!pNodeFound) {
		} else {
			auto * pBr = dynamic_cast<ircode::InstrBr *>(pInstr);
			auto * pCond = dynamic_cast<ircode::AddrStaticValue *>(pBr->pCond);
			auto cond = dynamic_cast<const sup::BoolStaticValue &>(
				pCond->getStaticValue()
			).value;
			auto * pNodeToDelete = static_cast<Node *>(nullptr);
			auto * pRemainedNode = static_cast<Node *>(nullptr);
			if (cond) {
				pNodeToDelete = getNodeByLabel(pBr->pLabelFalse);
				pRemainedNode = getNodeByLabel(pBr->pLabelTrue);
			} else {
				pNodeToDelete = getNodeByLabel(pBr->pLabelTrue);
				pRemainedNode = getNodeByLabel(pBr->pLabelFalse);
			}
			pNodeToDelete->predOnCFG.erase(pNodeFound);
			pNodeFound->succOnCFG.erase(pNodeToDelete);
			pBr->pLabelTrue = pRemainedNode->pIRLabel;
			pBr->pLabelFalse = nullptr;
			pBr->pCond = nullptr;
		}
	}
	calculateIDomAndDFAndDom();
	getDUChain();
}

void CFG::dce() {
	auto varsToBeRemoved = std::set<ircode::AddrVariable *>();
	for (const auto & [pVar, du]: duChain) {
		if (pVar && du.use.empty() &&
			!com::enum_fun::in(
				get(du.def.it)->instrType,
				{ircode::InstrType::Call, ircode::InstrType::ParaMov}
			)) {
			varsToBeRemoved.insert(pVar);
		}
	}
	for (auto * pVar: varsToBeRemoved) {
		removeDef(pVar);
	}
	calculateIDomAndDFAndDom();
	getDUChain();
}

bool CFG::cse() {
	auto changedSth = false;
	auto hashTable = std::map<ircode::InstrHashVal_t, std::vector<ircode::IRInstr *>>();
	collectInfoFromAllReachableInstr(
		[&hashTable](
			Node * pNodeNow, std::list<ircode::IRInstr *>::iterator itPInstr
		) -> void {
			auto * pInstr = get(itPInstr);
			if (com::enum_fun::in(
				pInstr->instrType, {ircode::InstrType::ICmp, ircode::InstrType::FCmp}
			)) {
				return;
			}
			if (pInstr->getDef()) {
				auto h = ircode::hashInstr(pInstr);
				hashTable[h].emplace_back(pInstr);
//				std::cerr << h << " :: " << pInstr->toLLVMIR() << std::endl;
			}
		}
	);
	for (auto & [hv, vecInstr]: hashTable) {
		if (vecInstr.size() <= 1) { continue; }
		auto unchangedHash = true;
		for (auto * pInstr: vecInstr) {
			if (hv != ircode::hashInstr(pInstr)) {
				unchangedHash = false;
				break;
			}
		}
		if (!unchangedHash) { continue; }
		auto * pInstrFixed = *vecInstr.rbegin();
		vecInstr.pop_back();
		auto * pVarDef = pInstrFixed->getDef();
		if (!duChain.count(pVarDef)) { continue; }
		auto vecPNodeOperandOn = std::vector<Node *>();
		for (auto * pOperand: get(duChain.find(pVarDef)->second.def.it)->getUse()) {
			if (pOperand->addrType == ircode::AddrType::Var) {
				auto * pVarUse = dynamic_cast<ircode::AddrVariable *>(pOperand);
				vecPNodeOperandOn.emplace_back(duChain.find(pVarUse)->second.def.pNode);
			}
		}
		auto * pLstNode = getSubOf(vecPNodeOperandOn);
		if (pLstNode) {
			for (auto * pInstr: vecInstr) {
				auto * pVar = pInstr->getDef();
				replaceUse(pVar, pVarDef);
				removeDef(pVar);
			}
			auto found = false;
			auto itPInstrLstNode = pLstNode->instrs.begin();
			while (itPInstrLstNode != pLstNode->instrs.end()) {
				auto * pInstr = get(itPInstrLstNode);
				for (auto * pOperand: pInstr->getUse()) {
					auto * pVar = dynamic_cast<ircode::AddrVariable *>(pOperand);
					if (pVar == pVarDef) {
						found = true;
						break;
					}
				}
				if (found) { break; }
				++itPInstrLstNode;
			}
			if (!found) {
				itPInstrLstNode = std::prev(itPInstrLstNode);
			}
			moveInstr(
				duChain.find(pVarDef)->second.def.pNode,
				duChain.find(pVarDef)->second.def.it,
				pLstNode, itPInstrLstNode
			);
			changedSth = true;
		}
	}
	return changedSth;
}

void CFG::adSimplifyCFG() {
/*  For
 *      0.  rebuild cfg
 *      1.  For edge <u,v>, try to merge (u,v)
 */
	auto vis = std::map<Node *, bool>();
	std::function<void(Node *)> dfs = [&dfs, &vis, this](Node * pNodeNow) {
		vis[pNodeNow] = true;
		auto * pInstrLst = get(pNodeNow->instrs.rbegin());
		clearAllEdgesOutOf(pNodeNow);
		if (pInstrLst->instrType == ircode::InstrType::Br) {
			auto * pInstrBr = dynamic_cast<ircode::InstrBr *>(pInstrLst);
			if (pInstrBr->pLabelTrue) {
				auto * pNodeNxt = getNodeByLabel(pInstrBr->pLabelTrue);
				add_edge(pNodeNow, pNodeNxt);
			}
			if (pInstrBr->pLabelFalse) {
				auto * pNodeNxt = getNodeByLabel(pInstrBr->pLabelFalse);
				add_edge(pNodeNow, pNodeNxt);
			}
		} else if (pInstrLst->instrType == ircode::InstrType::Ret) {
			//  do nothing
		} else {
			com::Throw("", CODEPOS);
		}
		while (pNodeNow->succOnCFG.size() == 1) {
			auto * pNodeNxt = get(pNodeNow->succOnCFG.begin());
			auto * pNodeRep = try_merge_node(pNodeNow, pNodeNxt);
			if (pNodeRep) { continue; } else { break; }
		}
		for (auto * pNodeNxt: pNodeNow->succOnCFG) {
			if (!vis[pNodeNxt]) {
				dfs(pNodeNxt);
			}
		}
	};
	dfs(pEntryNode);
	calculateIDomAndDFAndDom();
	getDUChain();
}

void CFG::optiInsideInstr() {     //  some optimization inside one instruction

}

void CFG::peephole() {            //  some peephole optimization

}

}
