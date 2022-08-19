#include <queue>
#include <utility>

#include "mdb.hpp"

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

void Node::insertBeforeTerm(ircode::IRInstr * pInstr) {
	instrs.emplace(std::prev(instrs.end()), pInstr);
}

std::list<ircode::IRInstr *> Node::toLLVMIRForm() {
	auto res = instrs;
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
	calculateIDomAndDFAndDom();
	mem2reg();
	resolvePhi();
	addMarker();
	auto res = std::list<ircode::IRInstr *>();
	auto vis = std::map<Node *, bool>();
	auto pExitNode = static_cast<Node *>(nullptr);
	auto q = std::queue<Node *>();
	q.emplace(pEntryNode);
	while (!q.empty()) {
		auto * pNodeNow = q.front();
		q.pop();
		if (vis[pNodeNow]) { continue; }
		vis[pNodeNow] = true;
		auto * pLstInstr = get(pNodeNow->instrs.rbegin());
		if (pLstInstr->instrType == ircode::InstrType::Br) {
			res.splice(res.end(), std::list(pNodeNow->instrs)); //  avoid move
			res.emplace_back(
				ir.instrPool.emplace_back(
					ircode::InstrMarkVars(ircode::InstrMarkVars(pNodeNow->markInstr))
				)
			);
			auto * pBr = dynamic_cast<ircode::InstrBr *>(pLstInstr);
			if (!pBr->pCond && pBr->pLabelTrue && !pBr->pLabelFalse) {
				auto * pNodeNxt = getNodeByLabel(pBr->pLabelTrue);
				if (!vis[pNodeNxt]) {
					q.push(pNodeNxt);
				}
			} else if (pBr->pCond && pBr->pLabelTrue && pBr->pLabelFalse) {
				auto * pNodeNxt = getNodeByLabel(pBr->pLabelFalse);
				if (!vis[pNodeNxt]) {
					q.push(pNodeNxt);
				}
				pNodeNxt = getNodeByLabel(pBr->pLabelTrue);
				if (!vis[pNodeNxt]) {
					q.push(pNodeNxt);
				}
			} else { com::Throw("", CODEPOS); }
		} else if (pLstInstr->instrType == ircode::InstrType::Ret) {
			pExitNode = pNodeNow;
			com::Assert(pNodeNow->succOnCFG.empty(), "", CODEPOS);
		} else { com::Throw("", CODEPOS); }
	}
	com::Assert(pExitNode, "", CODEPOS);
	res.splice(res.end(), std::list(pExitNode->instrs));
	res.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrMarkVars(ircode::InstrMarkVars(pExitNode->markInstr))
		)
	);
	return res;
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
	//  analyze cfg
	calculateIDomAndDFAndDom();
	getDUChain();
	//  opti
	constantPropagation();
	return 0;
}

CFGIR::CFGIR(ircode::IRModule & ir) : ir(ir), cfgPool(funcDef2CFG) {
}

int CFGIR::opti() {
	globalVarOpt();
	//  local passes:
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) {
			continue;
		} else {
			cfgPool.emplace_back(CFG(ir, pFuncDef));
		}
	}
	for (auto [pFuncDef, pCFG]: funcDef2CFG) {
		pCFG->opti();
	}
	return 0;
}

int CFGIR::genLLVMFormRes() {
	for (auto [pFuncDef, pCFG]: funcDef2CFG) {
		pFuncDef->instrs = pCFG->toLLVMIRFrom();
	}
	return 0;
}

int CFGIR::genDeSSAFormRes() {
	auto deletedFuncDef = std::set<ircode::IRFuncDef *>();
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) {
			continue;
		} else if (funcDef2CFG.count(pFuncDef)) {
			pFuncDef->instrs = funcDef2CFG[pFuncDef]->toDeSSAForm();
		} else {
			deletedFuncDef.insert(pFuncDef);
		}
	}
	for (auto * pFuncDef: deletedFuncDef) {
		ir.funcPool.erase(pFuncDef);
	}
	return 0;
}


CFGIR::CFGPool::CFGPool(std::map<ircode::IRFuncDef *, CFG *> & mp) : Pool() {
	afterEmplace = [&mp](CFG * pCFG) {
		mp.emplace(pCFG->pFuncDefThis, pCFG);
	};
	onDelete = [&mp](CFG * pCFG) {
		mp.erase(pCFG->pFuncDefThis);
	};
}

DUChain::PosInfo::PosInfo(Node * pNode, std::list<ircode::IRInstr *>::iterator it) :
	pNode(pNode), it(std::move(it)) {
}

DUChain::DUChain(Node * pNodeDef, ItPIRInstr_t instrDef) : def(pNodeDef, std::move(instrDef)) {
}

void DUChain::insertUseInfo(Node * pNodeUse, ItPIRInstr_t instrUse) {
	use.emplace_back(pNodeUse, std::move(instrUse));
}


}

#pragma clang diagnostic pop