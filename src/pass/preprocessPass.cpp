//
// Created by lee on 7/18/22.
//

#include "preprocessPass.hpp"


namespace pass {
using namespace ircode;


int AddBrToNextBB::run() {
	for (auto * pFuncDef: ir.funcPool) {
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
			!isTerminalInstr(g(itPInstrNow)->instrType) &&
			g(itPInstrNxt)->instrType == InstrType::Label) {
			instrs.emplace(
				itPInstrNxt, ir.instrPool.emplace_back(
					ircode::InstrBr(
						dynamic_cast<InstrLabel *>(g(itPInstrNxt))->pAddrLabel
					)
				)
			);
		}
		++itPInstrNow;
	}
	return 0;
}

}
