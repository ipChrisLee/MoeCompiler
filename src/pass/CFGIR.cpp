#include <queue>

#include "CFGIR.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "LocalValueEscapesScope"
namespace pass {

CFG::CFG(ircode::IRModule & ir, ircode::IRFuncDef * pFuncDefThis) :
	ir(ir), pFuncDefThis(pFuncDefThis) {
	buildCFG();
}

Node::Node(ircode::AddrJumpLabel * pIRLabel) : pIRLabel(pIRLabel) {
	com::Assert(pIRLabel, "", CODEPOS);
}

std::list<ircode::IRInstr *> Node::toLLVMIRForm() {
	auto res = instrs;
	for (auto [pLVAddr, pPhi]: phiInstrs) {
		res.insert(std::next(res.begin()), pPhi);
	}
	return res;
}

void CFG::buildCFG() {
	com::Assert(!pFuncDefThis->instrs.empty(), "", CODEPOS);
	auto itPInstrNow = pFuncDefThis->instrs.begin();
	com::Assert(get(itPInstrNow)->instrType == ircode::InstrType::Label, "", CODEPOS);
	//  Build node of basic blocks first
	auto * pNodeNow = new_node(
		dynamic_cast<ircode::InstrLabel *>(get(itPInstrNow))->pAddrLabel
	);
	pEntryNode = pNodeNow;
	while (itPInstrNow != pFuncDefThis->instrs.end()) {
		pNodeNow->instrs.emplace_back(get(itPInstrNow));
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != pFuncDefThis->instrs.end() &&
			get(itPInstrNxt)->instrType == ircode::InstrType::Label) {
			com::Assert(ircode::isTerminalInstr(get(itPInstrNow)->instrType), "", CODEPOS);
			pNodeNow = new_node(
				dynamic_cast<ircode::InstrLabel *>(get(itPInstrNxt))->pAddrLabel
			);
		}
		itPInstrNow = std::next(itPInstrNow);
	}
	//  Build CFG and generate information to be used
	itPInstrNow = pFuncDefThis->instrs.begin();
	while (itPInstrNow != pFuncDefThis->instrs.end()) {
		if (get(itPInstrNow)->instrType == ircode::InstrType::Label) {
			auto * pInstrLabel = dynamic_cast<ircode::InstrLabel *>(get(itPInstrNow));
			pNodeNow = getNodeByLabel(pInstrLabel->pAddrLabel);
		} else if (get(itPInstrNow)->instrType == ircode::InstrType::Br) {
			auto * pInstrBr = dynamic_cast<ircode::InstrBr *>(get(itPInstrNow));
			if (pInstrBr->pLabelTrue) {
				auto * pNodeNxt = getNodeByLabel(pInstrBr->pLabelTrue);
				add_edge(pNodeNow, pNodeNxt);
			}
			if (pInstrBr->pLabelFalse) {
				auto * pNodeNxt = getNodeByLabel(pInstrBr->pLabelFalse);
				add_edge(pNodeNow, pNodeNxt);
			}
		}
		itPInstrNow = std::next(itPInstrNow);
	}
}

void CFG::simplifyCFG() {
	auto vis = std::map<Node *, bool>();
	std::function<void(Node *)> dfs = [&dfs, &vis, this](Node * pNodeNow) {
		vis[pNodeNow] = true;
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
}

void CFG::calculateIDomAndDF() {
	//  "A Simple, Fast Dominance Algorithm"
	//  I can't understand it, I am shocked.
	//  doms array
	auto doms = std::vector<int>();
	auto dfn = std::map<Node *, int>();
	auto postorder = std::map<int, Node *>();
	auto vis = std::map<Node *, bool>();
	std::function<void(Node *)>
		dfs = [&postorder, &doms, &dfs, &vis, &dfn](Node * pNow) {
		vis[pNow] = true;
		for (auto * pNxt: pNow->succOnCFG) {
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
				if (it == itNewIDom) { break; }
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
		iDom.emplace(nD, nN);
	}
	for (auto iB = 0; iB < int(doms.size()); ++iB) {
		auto * nB = postorder[iB];
		if (nB->predOnCFG.size() >= 2) {
			for (auto * nP: nB->predOnCFG) {
				auto * nRunner = nP;
				auto iRunner = dfn[nRunner];
				while (iRunner != doms[iB]) {
					domF[nRunner].emplace(nB);
					iRunner = doms[iRunner];
					nRunner = postorder[iRunner];
				}
			}
		}
	}
}

void CFG::add_edge(Node * pNodeFrom, Node * pNodeTo) {
	pNodeFrom->succOnCFG.emplace(pNodeTo);
	pNodeTo->predOnCFG.emplace(pNodeFrom);
}

Node * CFG::getNodeByLabel(ircode::AddrJumpLabel * pLabel) {
	com::Assert(_label2Node.find(pLabel) != _label2Node.end(), "", CODEPOS);
	return _label2Node[pLabel];
}

Node * CFG::new_node(ircode::AddrJumpLabel * pLabel) {
	com::Assert(_label2Node.find(pLabel) == _label2Node.end(), "", CODEPOS);
	auto * pNode = _nodePool.emplace_back(Node(pLabel));
	_label2Node[pNode->pIRLabel] = pNode;
	return pNode;
}

Node * CFG::try_merge_node(Node * pNodeFrom, PNode & pNodeTo) {
	if (pNodeFrom->succOnCFG.size() == 1 && pNodeTo->predOnCFG.size() == 1 &&
		get(pNodeFrom->succOnCFG.begin()) == pNodeTo &&
		get(pNodeTo->predOnCFG.begin()) == pNodeFrom &&
		pNodeFrom != pNodeTo
		) {
		//  merge instrs
		pNodeFrom->instrs.erase(std::prev(pNodeFrom->instrs.end()));
		pNodeTo->instrs.erase(pNodeTo->instrs.begin());
		STLPro::list::merge_to(pNodeFrom->instrs, std::move(pNodeTo->instrs));
		//  merge edges, process on data of succ of pNodeTo
		for (auto * p: pNodeTo->succOnCFG) {
			p->predOnCFG.erase(pNodeTo);
			p->predOnCFG.insert(pNodeFrom);
		}
		//  remove edge
		pNodeFrom->succOnCFG.erase(pNodeTo);
		//  merge edges, process on data of pNodeFrom
		pNodeFrom->succOnCFG.merge(pNodeTo->succOnCFG);
		//  process data on CFG
		_label2Node.erase(pNodeTo->pIRLabel);
		pNodeTo->clear();
		pNodeTo = nullptr;
		return pNodeFrom;
	} else {
		return nullptr;
	}
}

std::list<ircode::IRInstr *> CFG::toDeSSAForm() {
	com::Throw("", CODEPOS);
	return { };
}

std::list<ircode::IRInstr *> CFG::toLLVMIRFrom() {
	auto res = std::list<ircode::IRInstr *>();
	auto q = std::queue<Node *>();
	auto vis = std::map<Node *, bool>();
	q.emplace(pEntryNode);
	while (!q.empty()) {
		auto * pNodeNow = q.front();
		q.pop();
		if (vis[pNodeNow]) {
			continue;
		}
		vis[pNodeNow] = true;
		STLPro::list::merge_to(res, pNodeNow->toLLVMIRForm());
		for (auto * pNodeNxt: pNodeNow->succOnCFG) {
			if (!vis[pNodeNxt]) {
				q.push(pNodeNxt);
			}
		}
	}
	return res;
}

int CFG::opti() {
	simplifyCFG();
	calculateIDomAndDF();
	mem2reg();
	//  TODO : Some other global/local opti
	return 0;
}

CFGIR::CFGIR(ircode::IRModule & ir) : ir(ir), cfgPool(funcDef2CFG) {
}

int CFGIR::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) {
			continue;
		} else {
			cfgPool.emplace_back(CFG(ir, pFuncDef));
		}
	}
	return 0;
}

int CFGIR::opti() {
	//  local passes:
	for (auto [pFuncDef, pCFG]: funcDef2CFG) {
		pCFG->opti();
	}
	//  TODO : Global passes
	//  can use Pool::erase to inline
	return 0;
}

int CFGIR::genLLVMFormRes() {
	for (auto [pFuncDef, pCFG]: funcDef2CFG) {
		pFuncDef->instrs = pCFG->toLLVMIRFrom();
	}
	return 0;
}

int CFGIR::genDeSSAFormRes() {
	for (auto [pFuncDef, pCFG]: funcDef2CFG) {
		pFuncDef->instrs = pCFG->toDeSSAForm();
	}
	return 0;
}


CFGIR::CFGPool::CFGPool(std::map<ircode::IRFuncDef *, CFG *> & mp) :
	Pool() {
	afterEmplace = [&mp](CFG * pCFG) {
		mp.emplace(pCFG->pFuncDefThis, pCFG);
	};
}
}

#pragma clang diagnostic pop