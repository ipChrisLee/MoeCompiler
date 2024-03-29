#include "pass/CFGIR.hpp"
#include <queue>


namespace pass {
void CFG::calculateIDomAndDFAndDom() {
	//  "A Simple, Fast Dominance Algorithm"
	//  I can't understand it, I am shocked.
	//  doms array
	iDom.clear();
	domF.clear();
	dom.clear();
	auto doms = std::vector<int>();
	auto dfn = std::map<Node *, int>();
	auto postorder = std::map<int, Node *>();
	auto vis = std::map<Node *, bool>();
	std::function<void(Node *)>
		dfs = [&postorder, &doms, &dfs, &vis, &dfn](Node * pNow) {
		vis[pNow] = true;
		for (auto * pNxt: pNow->succOnCFG) {
//			std::cerr << pNow->pIRLabel->toLLVMLabelName() << " -> "
//			          << pNxt->pIRLabel->toLLVMLabelName() << std::endl;
			if (!vis[pNxt]) {
				dfs(pNxt);
			}
		}
		auto id = int(doms.size());
		dfn[pNow] = id;
		postorder[id] = pNow;
		doms.emplace_back(-1);
	};
	dfs(pEntryNode);
//	std::cerr << std::endl;
	auto intersect = [&doms](int b1, int b2) {
		while (b1 != b2) {
			while (b1 < b2) {
				b1 = doms[b1];
			}
			while (b2 < b1) {
				b2 = doms[b2];
			}
		}
		return b1;
	};
	doms[dfn[pEntryNode]] = dfn[pEntryNode];
	auto changed = true;
	while (changed) {
		changed = false;
		for (auto iB = 0; iB < int(doms.size()) - 1; ++iB) {
			const auto * nB = postorder[iB];
			auto itNewIDom = nB->predOnCFG.begin();
			while (itNewIDom != nB->predOnCFG.end()) {
				if (doms[dfn[get(itNewIDom)]] != -1) {
					break;
				}
				itNewIDom = std::next(itNewIDom);
			}
			if (itNewIDom == nB->predOnCFG.end()) { continue; }
			auto iNewIDom = dfn[get(itNewIDom)];
			for (auto it = nB->predOnCFG.begin(); it != nB->predOnCFG.end(); ++it) {
				if (it == itNewIDom) { continue; }
				auto iP = dfn[get(it)];
				if (doms[iP] != -1) {
					iNewIDom = intersect(iP, iNewIDom);
				}
			}
			if (doms[iB] != iNewIDom) {
				doms[iB] = iNewIDom;
				changed = true;
			}
		}
	}
	for (auto i = 0; i < int(doms.size()); ++i) {
		auto * nN = postorder[i];
		auto * nD = postorder[doms[i]];
//		std::cerr << nD->pIRLabel->toLLVMLabelName() << " -> "
//		          << nN->pIRLabel->toLLVMLabelName() << " [color=blue]" << std::endl;
		iDom.emplace(nN, nD);
	}
	for (auto i = 0; i < int(doms.size()); ++i) {
		auto * nN = postorder[i];
		auto * nD = nN;
		while (nD != pEntryNode) {
			dom[nD].emplace(nN);
			nD = iDom[nD];
		}
		dom[nD].emplace(nN);
	}
	for (auto iB = 0; iB < int(doms.size()); ++iB) {
		auto * nB = postorder[iB];
		if (nB->predOnCFG.size() >= 2) {
			for (auto * nP: nB->predOnCFG) {
				auto * nRunner = nP;
				auto iRunner = dfn[nRunner];
				while (iRunner != doms[iB]) {
					if (!domF[nRunner].emplace(nB).second) {
						break;
					}
					iRunner = doms[iRunner];
					nRunner = postorder[iRunner];
				}
			}
		}
	}
}

void CFG::getDUChain() {
	duChain.clear();
	auto visitedLabel = std::set<ircode::AddrJumpLabel *>();
	auto findDefine
		= [&visitedLabel, this](
			Node * pNode, typename std::list<ircode::IRInstr *>::iterator pInstr
		) {
			visitedLabel.insert(pNode->pIRLabel);
			auto * pVar = get(pInstr)->getDef();
			if (pVar) {
				duChain.emplace(pVar, DUChain(pNode, pInstr));
			}
		};
	collectInfoFromAllReachableInstr(findDefine);
	auto findUse
		= [&visitedLabel, this](
			Node * pNode, typename std::list<ircode::IRInstr *>::iterator itPInstr
		) {
			//  for phi node, if not defined, may because of structure of cfg changed.
			auto * pInstr = get(itPInstr);
			if (pInstr->instrType == ircode::InstrType::Phi) {
				auto * pPhi = dynamic_cast<ircode::InstrPhi *>(pInstr);
				auto labelToDelete = std::set<ircode::AddrJumpLabel *>();
				for (auto [pLabel, pOperand]: pPhi->vecPair) {
					if (!visitedLabel.count(pLabel)) {
						labelToDelete.insert(pLabel);
					}
				}
				for (auto pLabel: labelToDelete) {
					pPhi->vecPair.erase(pLabel);
				}
			}
			for (auto * pOperand: get(itPInstr)->getUse()) {
				if (com::enum_fun::in(
					pOperand->addrType, {
						ircode::AddrType::Var, ircode::AddrType::LocalVar
					}
				)) {
					auto * pVar = dynamic_cast<ircode::AddrVariable *>(pOperand);
					com::Assert(duChain.count(pVar), "", CODEPOS);
					duChain.find(pVar)->second.insertUseInfo(pNode, itPInstr);
				}
			}
		};
	collectInfoFromAllReachableInstr(findUse);
}

}