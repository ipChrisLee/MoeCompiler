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
		for (auto [pLVAddr, pPhi]: u->phiInstrs) {
			for (auto * pOpnd: pPhi->getUse()) {

			}
		}
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

}
#pragma clang diagnostic pop