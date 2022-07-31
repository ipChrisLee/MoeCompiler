//
// Created by lee on 7/18/22.
//

#include "PreprocessPass.hpp"
#include "mlib/stlpro.hpp"


namespace mir {


int AddBrToNextBB::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) {
			continue;
		}
		run(pFuncDef->instrs);
	}
	return 0;
}

AddBrToNextBB::AddBrToNextBB(Module & ir, std::string name) :
	Pass(ir, std::move(name)) {
}

int AddBrToNextBB::run(std::list<Instr *> & instrs) {
	auto itPInstrNow = instrs.begin();
	while (itPInstrNow != instrs.end()) {
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != instrs.end() &&
			!isTerminalInstr(get(itPInstrNow)->instrType) &&
			get(itPInstrNxt)->instrType == InstrType::Label) {
			instrs.emplace(
				itPInstrNxt, ir.instrPool.emplace_back(
					InstrBr(
						dynamic_cast<InstrLabel *>(get(itPInstrNxt))->pAddrLabel
					)
				)
			);
		}
		++itPInstrNow;
	}
	return 0;
}

}
