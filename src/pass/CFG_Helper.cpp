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
		com::TODO("", CODEPOS);
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

void CFG::removeDef(ircode::AddrVariable * pFrom) {
	com::Assert(duChain.find(pFrom)->second.use.empty(), "", CODEPOS);
	const auto & posDef = duChain.find(pFrom)->second.def;
	posDef.pNode->instrs.erase(posDef.it);
	duChain.erase(pFrom);
}

}
#pragma clang diagnostic pop