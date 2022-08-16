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
	//          https://www.zhihu.com/question/41999500/answer/93243408
	auto promotable = [](ircode::AddrLocalVariable * pLVAddr) -> bool {
		return com::enum_fun::in(
			pLVAddr->getType().type,
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
				if (promotable(pAlloca->allocaTo)) { setOfLV.emplace(pAlloca->allocaTo); }
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
					if (pStoreTo && setOfLV.find(pStoreTo) != setOfLV.end()) {
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
						ircode::InstrPhi(pNewPhiVar)
					);
					bbY->phiInstrs[pLV] = pPhiInstr;
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
	[&setOfLV, this]() {
		//  set default value, to avoid poison value
		auto incomingVar = std::map<ircode::AddrLocalVariable *, ircode::AddrOperand *>();
		for (auto * pLVAddr: setOfLV) {
			auto * pSV0 = ir.addrPool.emplace_back(
				ircode::AddrStaticValue(
					*dynamic_cast<const sup::PointerType &>(pLVAddr->getType()).pointTo
				)
			);
			incomingVar.emplace(pLVAddr, pSV0);
		}
		//  map from var to incoming var
		//  %x = load i32* %p; mapping with %x and val of %p now.
		auto mappingWithOp = std::map<ircode::AddrVariable *, ircode::AddrOperand *>();
		//  rename
		auto workList = std::queue<Node *>();
		workList.emplace(pEntryNode);
		auto vis = std::map<Node *, bool>();
		while (!workList.empty()) {
			auto * pNodeNow = workList.front();
			workList.pop();
			if (vis[pNodeNow]) { continue; }
			vis[pNodeNow] = true;
			for (auto [pLVAddr, pPhi]: pNodeNow->phiInstrs) {
				incomingVar[pLVAddr] = pPhi->newDefVar;
			}
			auto itInstrNow = pNodeNow->instrs.begin();
			while (itInstrNow != pNodeNow->instrs.end()) {
				if (get(itInstrNow)->instrType == ircode::InstrType::Load) {
					//  use(maybe)
					auto * pLoad = dynamic_cast<ircode::InstrLoad *>(get(itInstrNow));
					auto * pLoadFrom = dynamic_cast<ircode::AddrLocalVariable *>(
						pLoad->from
					);
					if (pLoadFrom && incomingVar.find(pLoadFrom) != incomingVar.end()) {
						mappingWithOp[pLoad->to] = incomingVar[pLoadFrom];
						itInstrNow = pNodeNow->instrs.erase(itInstrNow);
						continue;
					}
				} else if (get(itInstrNow)->instrType == ircode::InstrType::Store) {
					//  def(maybe)
					auto * pStore = dynamic_cast<ircode::InstrStore *>(get(itInstrNow));
					auto pStoreTo = dynamic_cast<ircode::AddrLocalVariable *>(
						pStore->to
					);
					if (pStoreTo && incomingVar.find(pStoreTo) != incomingVar.end()) {
						incomingVar[pStoreTo] = pStore->from;
						itInstrNow = pNodeNow->instrs.erase(itInstrNow);
						continue;
					}
				} else if (get(itInstrNow)->instrType == ircode::InstrType::Alloca) {
					//  alloca, delete if is on reg
					auto * pAlloca = dynamic_cast<ircode::InstrAlloca *>(get(itInstrNow));
					if (setOfLV.find(pAlloca->allocaTo) != setOfLV.end()) {
						itInstrNow = pNodeNow->instrs.erase(itInstrNow);
						continue;
					}
				}
				for (auto * pOperandAddr: get(itInstrNow)->getOperands()) {
					auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOperandAddr);
					if (mappingWithOp.find(pVarAddr) != mappingWithOp.end()) {
						get(itInstrNow)->changeOperand(pVarAddr, mappingWithOp[pVarAddr]);
					}
				}
				itInstrNow = std::next(itInstrNow);
			}

			for (auto * pNodeNxt: pNodeNow->succOnCFG) {
				for (auto [pLVAddr, pPhi]: pNodeNxt->phiInstrs) {
					pPhi->insertPair(pNodeNow->pIRLabel, incomingVar[pLVAddr]);
				}
				if (!vis[pNodeNxt]) { workList.emplace(pNodeNxt); }
			}
		}
	}();
}

}

#pragma clang diagnostic pop
#pragma clang diagnostic pop