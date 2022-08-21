#include "CFGIR.hpp"
#include <list>
#include <queue>


#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"

namespace pass {

void
CFG::collectInfoFromAllReachableNode(const std::function<void(const Node *)> & fun) const {
	auto vis = std::map<Node *, bool>();
	auto q = std::queue<Node *>();
	q.push(pEntryNode);
	while (!q.empty()) {
		auto * u = q.front();
		q.pop();
		if (vis[u]) { continue; }
		vis[u] = true;
		fun(u);
		for (auto * v: u->succOnCFG) {
			if (!vis[v]) {
				q.push(v);
			}
		}
	}
}

void CFG::collectInfoFromAllReachableInstr(
	const std::function<void(
		Node *, typename std::list<ircode::IRInstr *>::iterator
	)> & fun
) const {
	auto vis = std::map<Node *, bool>();
	auto q = std::queue<Node *>();
	q.push(pEntryNode);
	while (!q.empty()) {
		auto * u = q.front();
		q.pop();
		if (vis[u]) { continue; }
		vis[u] = true;
		auto itPInstr = u->instrs.begin();
		while (itPInstr != u->instrs.end()) {
			fun(u, itPInstr);
			itPInstr = std::next(itPInstr);
		}
		for (auto * v: u->succOnCFG) {
			if (!vis[v]) {
				q.push(v);
			}
		}
	}
}

std::vector<ircode::AddrVariable *>
CFG::replaceUse(ircode::AddrVariable * pFrom, ircode::AddrOperand * pTo) {
	auto affectedVar = std::vector<ircode::AddrVariable *>();
	if (pTo->addrType == ircode::AddrType::Var) {
		auto * pVarTo = dynamic_cast<ircode::AddrVariable *>(pTo);
		auto & useChainOfFrom = duChain.find(pFrom)->second.use;
		auto itUsePos = useChainOfFrom.begin();
		while (itUsePos != useChainOfFrom.end()) {
//			auto & usePos = *itUsePos;
			auto & usePos = get(itUsePos);
			auto changed = get(usePos.it)->changeUse(pFrom, pTo);
			if (changed) {
				affectedVar.emplace_back(get(usePos.it)->getDef());
				duChain.find(pVarTo)->second.use.emplace_back(
					usePos.pNode, usePos.it
				);
				itUsePos = useChainOfFrom.erase(itUsePos);
			} else {
				itUsePos = std::next(itUsePos);
//				std::cerr<<"?"<<std::endl;
			}
		}
	} else if (pTo->addrType == ircode::AddrType::StaticValue) {
		auto & useChain = duChain.find(pFrom)->second.use;
		auto itUsePos = useChain.begin();
		while (itUsePos != useChain.end()) {
			auto & usePos = get(itUsePos);
			auto changed = get(usePos.it)->changeUse(pFrom, pTo);
			if (changed) {
				affectedVar.emplace_back(get(usePos.it)->getDef());
				itUsePos = useChain.erase(itUsePos);
			} else {
				itUsePos = std::next(itUsePos);
			}
		}
	}
	affectedVar.erase(
		std::remove_if(
			affectedVar.begin(), affectedVar.end(), [](auto p) { return p == nullptr; }
		), affectedVar.end()
	);
	return affectedVar;
}

/**
 * @brief remove define of pFrom.
 * @param pFrom
 */
void CFG::removeDef(ircode::AddrVariable * pFrom) {
	com::Assert(duChain.find(pFrom)->second.use.empty(), "", CODEPOS);
	const auto & posDef = duChain.find(pFrom)->second.def;
	posDef.pNode->instrs.erase(posDef.it);
	duChain.erase(pFrom);
}

/**
 * @brief remove pNodeSrc and all nodes pNodeSrc dominates. Notice that variables will be deleted also.
 * @param pNodeSrc
 */
void CFG::removeNodeAndDom(Node * pNodeSrc) {
	com::Assert(pNodeSrc != pEntryNode, "", CODEPOS);
	auto deleteLabelOnPhi
		= [pNodeSrc](const std::list<ircode::IRInstr *> & ls) {
			std::for_each(
				ls.begin(), ls.end(), [pNodeSrc](ircode::IRInstr * pInstr) -> void {
					if (pInstr->instrType == ircode::InstrType::Phi) {
						auto * pPhi = dynamic_cast<ircode::InstrPhi *>(pInstr);
						pPhi->vecPair.erase(pNodeSrc->pIRLabel);
					}
				}
			);
		};
	auto defVars = std::set<ircode::AddrVariable *>();
	auto nodesToDelete = std::set<Node *>();
	for (auto * pNodeNow: dom[pNodeSrc]) {
		for (auto * pNodeNxt: pNodeNow->succOnCFG) {
			pNodeNxt->predOnCFG.erase(pNodeNow);
			if (domF[pNodeSrc].count(pNodeNxt)) {
				deleteLabelOnPhi(pNodeNxt->instrs);
			}
		}
	}
	for (auto * pNodePre: pNodeSrc->predOnCFG) {
		pNodePre->succOnCFG.erase(pNodeSrc);
	}
}

Node * CFG::lcaOnDomTree(Node * u, Node * v) {
	auto du = 0, dv = 0;
	auto n = u;
	while (n != pEntryNode) {
		n = iDom[n];
		++du;
		com::Assert(n, "", CODEPOS);
	}
	n = v;
	while (n != pEntryNode) {
		n = iDom[n];
		++dv;
		com::Assert(n, "", CODEPOS);
	}
	if (du > dv) {
		std::swap(du, dv);
		std::swap(u, v);
	}
	while (dv > du) {
		v = iDom[v];
		--dv;
	}
	while (u != v) {
		u = iDom[u];
		v = iDom[v];
	}
	return u;
}

Node * CFG::getSubOf(const std::vector<Node *> & vec) {
	if (vec.empty()) { return nullptr; }
	auto * pNode = vec[0];
	for (auto * pNodeOther: vec) {
		com::Assert(pNodeOther, "", CODEPOS);
		if (dom[pNode].count(pNodeOther)) {
			pNode = pNodeOther;
		} else if (dom[pNodeOther].count(pNode)) {
			//  do nothing
		} else {
			return nullptr;
		}
	}
	return pNode;
}

void CFG::moveInstr(
	Node * pNodeFrom, std::list<ircode::IRInstr *>::iterator itPInstrFrom,
	Node * pNodeTo, std::list<ircode::IRInstr *>::iterator itPInstrTo
) {
	com::Assert(dom[pNodeTo].count(pNodeFrom), "", CODEPOS);
	auto * pInstr = get(itPInstrFrom);
	itPInstrTo = pNodeTo->instrs.emplace(itPInstrTo, pInstr);
	for (auto * pOperand: pInstr->getUse()) {
		auto * pVarUsed = dynamic_cast<ircode::AddrVariable *>(pOperand);
		if (pVarUsed && duChain.count(pVarUsed)) {
			auto & duChainOfUse = duChain.find(pVarUsed)->second;
			for (auto & i: duChainOfUse.use) {
				if (i.pNode == pNodeFrom && i.it == itPInstrFrom) {
					i.pNode = pNodeTo;
					i.it = itPInstrTo;
				}
			}
		}
	}
	pNodeFrom->instrs.erase(itPInstrFrom);
	auto * pVarDef = pInstr->getDef();
	if (pVarDef) {
		duChain.find(pVarDef)->second.def = {pNodeTo, itPInstrTo};
	}
}

void CFG::clearAllEdgesOutOf(Node * u) {
	for (auto * v: u->succOnCFG) {
		v->predOnCFG.erase(u);
	}
	u->succOnCFG.clear();
}

void CFG::changePhiLabels(
	Node * pNodeNow, ircode::AddrJumpLabel * pJL, ircode::AddrJumpLabel * pJLNew
) {
	for (auto * pInstr: pNodeNow->instrs) {
		if (pInstr->instrType == ircode::InstrType::Phi) {
			auto * pPhi = dynamic_cast<ircode::InstrPhi *>(pInstr);
			if (pPhi->vecPair.count(pJL)) {
				pPhi->insertPair(pJLNew, pPhi->vecPair[pJL]);
				pPhi->vecPair.erase(pJL);
			}
		}
	}
}

}
#pragma clang diagnostic pop