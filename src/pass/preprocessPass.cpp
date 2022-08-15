//
// Created by lee on 7/18/22.
//

#include "preprocessPass.hpp"


namespace pass {
using namespace ircode;


int AddBrToNextBB::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) { continue; }
		run(pFuncDef->instrs);
	}
	return 0;
}

AddBrToNextBB::AddBrToNextBB(ircode::IRModule & ir, std::string name) :
	IRPass(ir, std::move(name)) {
}

int AddBrToNextBB::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstrNow = instrs.begin();
	while (itPInstrNow != instrs.end()) {
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != instrs.end() &&
			!isTerminalInstr(get(itPInstrNow)->instrType) &&
			get(itPInstrNxt)->instrType == InstrType::Label) {
			instrs.emplace(
				itPInstrNxt, ir.instrPool.emplace_back(
					ircode::InstrBr(
						dynamic_cast<InstrLabel *>(get(itPInstrNxt))->pAddrLabel
					)
				)
			);
		}
		itPInstrNow = std::next(itPInstrNow);
	}
	return 0;
}

int EliminateBrAfterBr::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) { continue; }
		run(pFuncDef->instrs);
	}
	return 0;
}


EliminateBrAfterBr::EliminateBrAfterBr(IRModule & ir, const std::string & name) :
	IRPass(ir, name) {
}

int EliminateBrAfterBr::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstrNow = instrs.begin();
	while (itPInstrNow != instrs.end()) {
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != instrs.end()) {
			if (get(itPInstrNow)->instrType == ircode::InstrType::Br &&
				get(itPInstrNxt)->instrType == ircode::InstrType::Br) {
				auto * pBrNow = dynamic_cast<ircode::InstrBr *>(get(itPInstrNow));
				com::Assert(
					(pBrNow->pLabelTrue && pBrNow->pLabelFalse && !pBrNow->pCond) ||
						(pBrNow->pCond && pBrNow->pLabelTrue), "", CODEPOS
				);
				instrs.erase(itPInstrNxt);
			}
		}
		itPInstrNow = std::next(itPInstrNow);
	}
	return 0;
}

}
