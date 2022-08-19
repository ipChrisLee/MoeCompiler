#include <queue>

#include "pass/CFGIR.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"
namespace pass {

void CFG::mem2reg() {
	//  reference:
	//      Insert Phi
	//          https://buaa-se-compiling.github.io/miniSysY-tutorial/challenge/mem2reg/help.html
	//      Rename Variable
	//          Design by ipLee, refer to https://buaa-se-compiling.github.io/miniSysY-tutorial/challenge/mem2reg/help.html
	auto promotable = [](ircode::AddrLocalVariable * pLVAddr) -> bool {
		const auto & pointTo = dynamic_cast<const sup::PointerType &>(
			pLVAddr->getType()
		);
		return com::enum_fun::in(
			pointTo.pointTo->type,
			{sup::Type::Int_t, sup::Type::Float_t, sup::Type::Pointer_t}
		);
	};
	auto setOfLV = std::set<ircode::AddrLocalVariable *>();
	auto setOfBBDefLV = std::map<ircode::AddrLocalVariable *, std::set<Node *>>();
	//  find local var definition position
	[&setOfLV, &setOfBBDefLV, &promotable, this]() {
		for (auto * pInstr: pEntryNode->instrs) {
			if (pInstr->instrType == ircode::InstrType::Alloca) {
				auto * pAlloca = dynamic_cast<ircode::InstrAlloca *>(pInstr);
				if (promotable(pAlloca->allocaTo)) {
					setOfLV.emplace(pAlloca->allocaTo);
				}
			}
		}
		auto q = std::queue<Node *>();
		auto vis = std::map<Node *, bool>();
		q.push(pEntryNode);
		while (!q.empty()) {
			auto * pNow = q.front();
			q.pop();
			if (vis[pNow]) {
				continue;
			}
			for (auto * pInstr: pNow->instrs) {
				if (pInstr->instrType == ircode::InstrType::Store) {
					auto * pStore = dynamic_cast<ircode::InstrStore *>(pInstr);
					auto * pStoreTo = dynamic_cast<ircode::AddrLocalVariable *>(pStore->to);
					if (pStoreTo && setOfLV.count(pStoreTo)) {
						com::Assert(promotable(pStoreTo), "", CODEPOS);
						setOfBBDefLV[pStoreTo].emplace(pNow);
					}
				}
			}
			vis[pNow] = true;
			for (auto * pNxt: pNow->succOnCFG) {
				if (!vis[pNxt]) { q.push(pNxt); }
			}
		}
	}();

	auto phiInstrs
		= std::map<Node *, std::map<ircode::AddrLocalVariable *, ircode::InstrPhi *>>();
	//  Insert Phi
	for (auto * pLV: setOfLV) {
		auto setOfBBToInsertPhi = std::set<Node *>();   //  F
		const auto & defBBs = setOfBBDefLV[pLV];  //  Defs(v)
		auto wSet = defBBs; //  W
		while (!wSet.empty()) {
			auto * bbX = get(wSet.begin()); //  remove X from W
			wSet.erase(wSet.begin());
			for (auto * bbY: domF[bbX]) {   //  Y in DF(X)
				if (setOfBBToInsertPhi.find(bbY) == setOfBBToInsertPhi.end()) { //  Y not in F
					//  add phi instr at bbY
					auto * pNewPhiVar = ir.addrPool.emplace_back(
						ircode::AddrVariable(
							*dynamic_cast<const sup::PointerType &>(pLV->getType()).pointTo
						)
					);
					auto * pPhiInstr = ir.instrPool.emplace_back(
						ircode::InstrPhi(pNewPhiVar, pLV)
					);
					phiInstrs[bbY][pLV] = pPhiInstr;
//					bbY->phiInstrs[pLV] = pPhiInstr;
					//  F <- F U {Y}
					setOfBBToInsertPhi.emplace(bbY);
					//  Y not in Defs(v)
					if (defBBs.find(bbY) == defBBs.end()) {
						wSet.emplace(bbY);  //  W <- W U {Y}
					}
				}
			}
		}
	}

	//  Rename Variable
	[&phiInstrs, &setOfLV, this]() {
		//  lvAddrLastVar[pLV][pNode]=pVal : at exit of pNode, value of pLV is pVal.
		auto
			lvAddrLastVar =
			std::map<ircode::AddrLocalVariable *, std::map<Node *, ircode::AddrOperand *
			>>();
		//  mappingWithOp[pVar]=pVal : pVar should be replaced with pVal
		auto mappingWithOp =
			std::map<ircode::AddrVariable *, ircode::AddrOperand *>();
		//  pB[0]=pNodeBegin pB[i+1]=idom(pB[i])
		//  find first pB[x] that lvAddrLastVar[pLV].count(pB[i])
		auto findLVMappingValDownToEntry =
			[&lvAddrLastVar, this](
				ircode::AddrLocalVariable * pLV, Node * pNodeBegin
			) {
				while (!lvAddrLastVar[pLV].count(pNodeBegin)) {
					pNodeBegin = iDom[pNodeBegin];
				}
				return lvAddrLastVar[pLV][pNodeBegin];
			};

		auto vis = std::map<Node *, bool>();
		std::function<void(Node *)> dfs =
			[
				&vis, &dfs, &lvAddrLastVar, &mappingWithOp, &setOfLV, &phiInstrs,
				&findLVMappingValDownToEntry, this
			](Node * pNodeNow) {
				vis[pNodeNow] = true;
				//  phi node may kill use of a local var.
//				for (auto [pLVAddr, pPhi]: pNodeNow->phiInstrs)
				for (auto [pLVAddr, pPhi]: phiInstrs[pNodeNow]) {
					lvAddrLastVar[pLVAddr][pNodeNow] = pPhi->newDefVar;
				}
				//  For all instrs:
				auto itPInstr = pNodeNow->instrs.begin();
				while (itPInstr != pNodeNow->instrs.end()) {
					if (get(itPInstr)->instrType == ircode::InstrType::Alloca) {
						//  For alloca, if dest is promotable (in setOfLV), delete this instr.
						//  Add set default value to lvAddrLastVar
						auto * pAlloca = dynamic_cast<ircode::InstrAlloca *>(
							get(itPInstr)
						);
						if (setOfLV.count(pAlloca->allocaTo)) {
							const auto & allocaType = *pAlloca->uPtrTypeToAlloca;
							lvAddrLastVar[pAlloca->allocaTo][pNodeNow] =
								ir.addrPool.emplace_back(
									ircode::AddrStaticValue(allocaType)
								);
							itPInstr = pNodeNow->instrs.erase(itPInstr);
							continue;
						}
					} else if (get(itPInstr)->instrType == ircode::InstrType::Load) {
						//  For load, if load->from is promotable:
						//      delete this instr and insert
						//          [load->to, findLVMappingValDownToEntry(load->from, pNodeNow)]
						//      into mappingWithOp.
						auto * pLoad = dynamic_cast<ircode::InstrLoad *>(get(itPInstr));
						auto * pLoadFrom = dynamic_cast<ircode::AddrLocalVariable *>(
							pLoad->from
						);
						if (pLoadFrom && setOfLV.count(pLoadFrom)) {
							mappingWithOp.emplace(
								pLoad->to, findLVMappingValDownToEntry(pLoadFrom, pNodeNow)
							);
							itPInstr = pNodeNow->instrs.erase(itPInstr);
							continue;
						}
					} else if (get(itPInstr)->instrType == ircode::InstrType::Store) {
						//  For store, if store->to is promotable:
						//      delete this instr and let
						auto * pStore = dynamic_cast<ircode::InstrStore *>(get(itPInstr));
						auto pStoreTo = dynamic_cast<ircode::AddrLocalVariable *>(
							pStore->to
						);
						if (pStoreTo && setOfLV.count(pStoreTo)) {
							auto * pVarAddr =
								dynamic_cast<ircode::AddrVariable *>(pStore->from);
							if (pVarAddr && mappingWithOp.count(pVarAddr)) {
								lvAddrLastVar[pStoreTo][pNodeNow] = mappingWithOp[pVarAddr];
							} else {
								lvAddrLastVar[pStoreTo][pNodeNow] = pStore->from;
							}
							itPInstr = pNodeNow->instrs.erase(itPInstr);
							continue;
						}
					}
					for (auto * pOperandAddr: get(itPInstr)->getUse()) {
						auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOperandAddr);
						if (mappingWithOp.count(pVarAddr)) {
							get(itPInstr)->changeUse(pVarAddr, mappingWithOp[pVarAddr]);
						}
					}
					itPInstr = std::next(itPInstr);
				}
				//  For immediate successors pNodeNxt of pNodeNow:
				//      Insert value pair to phi instr of pNodeNxt
				//      If pNodeNxt haven't been visited, visit it.
				for (auto * pNodeNxt: pNodeNow->succOnCFG) {
					for (auto [pLVAddr, pPhi]: phiInstrs[pNodeNxt]) {
//					for (auto [pLVAddr, pPhi]: pNodeNxt->phiInstrs) {
						pPhi->insertPair(
							pNodeNow->pIRLabel,
							findLVMappingValDownToEntry(pLVAddr, pNodeNow)
						);
					}
					if (!vis[pNodeNxt]) {
						dfs(pNodeNxt);
					}
				}
			};
		dfs(pEntryNode);
	}();

	//  Insert phi at beginning of node.
	for (auto & [pNode, lvValPair]: phiInstrs) {
		for (auto & [pLVAddr, pPhi]: lvValPair) {
			pNode->instrs.emplace(std::next(pNode->instrs.begin()), pPhi);
		}
	}
}

void CFG::resolvePhi() {
	auto copyInstrs = std::map<Node *, ircode::InstrParallelCopy>();
	auto vis = std::map<Node *, bool>();
	auto q = std::queue<Node *>();
	q.push(pEntryNode);
	while (!q.empty()) {
		auto * pNodeNow = q.front();
		q.pop();
		if (vis[pNodeNow]) {
			continue;
		}
		vis[pNodeNow] = true;
		for (auto * pInstr: pNodeNow->instrs) {
			if (pInstr->instrType == ircode::InstrType::Phi) {
				auto * pPhi = dynamic_cast<ircode::InstrPhi *>(pInstr);
				auto * pPhiVal = pPhi->newDefVar;
				for (auto [pJumpLabel, pVal]: pPhi->vecPair) {
					auto * pPreNode = getNodeByLabel(pJumpLabel);
					copyInstrs[pPreNode].insert(pVal, pPhiVal, ir);
				}
			}
		}
		pNodeNow->instrs.erase(
			std::remove_if(
				pNodeNow->instrs.begin(), pNodeNow->instrs.end(),
				[](ircode::IRInstr * pInstr) {
					return pInstr->instrType == ircode::InstrType::Phi;
				}
			), pNodeNow->instrs.end()
		);
		for (auto * pNodeNxt: pNodeNow->succOnCFG) {
			if (!vis[pNodeNxt]) {
				q.push(pNodeNxt);
			}
		}
	}
	for (auto [pNode, copyInstr]: copyInstrs) {
		pNode->instrs.emplace(
			std::prev(pNode->instrs.end()),
			ir.instrPool.emplace_back(std::move(copyInstr))
		);
	}
}

void CFG::addMarker() {
	auto defIn = std::map<ircode::AddrVariable *, std::set<Node *>>();
	auto useIn = std::map<ircode::AddrVariable *, std::set<Node *>>();
	[&defIn, &useIn, this]() {
		auto vis = std::map<Node *, bool>();
		auto q = std::queue<Node *>();
		q.emplace(pEntryNode);
		while (!q.empty()) {
			auto * pNodeNow = q.front();
			q.pop();
			if (vis[pNodeNow]) { continue; }
			vis[pNodeNow] = true;
			for (auto * pInstr: pNodeNow->instrs) {
				if (pInstr->instrType == ircode::InstrType::Copy) {
					auto * pPMov = dynamic_cast<ircode::InstrParallelCopy *>(pInstr);
					for (auto [from, to, _]: pPMov->copies) {
						auto * pVar = to;
						if (pVar && pVar->addrType == ircode::AddrType::Var) {
							defIn[pVar].emplace(pNodeNow);
						} else { com::Throw("??", CODEPOS); }
						pVar = dynamic_cast<ircode::AddrVariable *>(from);
						if (pVar && pVar->addrType == ircode::AddrType::Var) {
							useIn[pVar].emplace(pNodeNow);
						}
					}
				} else {
					auto * pVar = pInstr->getDef();
					if (pVar && pVar->addrType == ircode::AddrType::Var) {
						defIn[pVar].emplace(pNodeNow);
					}
					for (auto * pOperandVar: pInstr->getUse()) {
						pVar = dynamic_cast<ircode::AddrVariable *>(pOperandVar);
						if (pVar && pVar->addrType == ircode::AddrType::Var) {
							useIn[pVar].emplace(pNodeNow);
						}
					}
				}
			}
			for (auto * pNodeNxt: pNodeNow->succOnCFG) {
				if (!vis[pNodeNxt]) {
					q.push(pNodeNxt);
				}
			}
		}
	}();
	auto allInOneBB = [](
		const std::set<Node *> & vecNodesUseIn, const std::set<Node *> & vecNodesDefIn
	) {
		com::Assert(!vecNodesDefIn.empty(), "", CODEPOS);
		auto p = get(vecNodesDefIn.begin());
		auto res =
			std::all_of(
				vecNodesDefIn.begin(), vecNodesDefIn.end(),
				[p](Node * pp) { return p == pp; }
			) && std::all_of(
				vecNodesUseIn.begin(), vecNodesUseIn.end(),
				[p](Node * pp) { return p == pp; }
			);
		return res;
	};
	for (const auto & [pVarDef, vecNodesDefIn]: defIn) {
		const auto & vecNodesUseIn = useIn[pVarDef];
		if (allInOneBB(vecNodesUseIn, vecNodesDefIn)) {
			//  If def and use are in same node(bb), there is no need to add mark.
		} else {
			//  For every def node, add marker on nodes it dominates.
			for (auto pNodeDom: vecNodesDefIn) {
				for (auto pNodeSub: dom[pNodeDom]) {
					pNodeSub->markInstr.vecPVar.emplace(pVarDef);
				}
			}
		}

	}
}

}

#pragma clang diagnostic pop