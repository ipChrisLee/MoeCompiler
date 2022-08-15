#include <common.hpp>

#include "pass-common.hpp"
#include "pass/preprocessPass.hpp"
#include "pass/CFGIR.hpp"


namespace pass {

IRPass::IRPass(ircode::IRModule & ir, std::string name) :
	ir(ir), name(std::move(name)) {
	com::addRuntimeWarning(
		"Consider adding pass dependence management.", CODEPOS,
		com::addWarningOnlyOnce
	);
}

int passMain(ircode::IRModule & ir) {
	std::vector<std::unique_ptr<IRPass>> preprocessPasses;
	preprocessPasses.emplace_back(std::make_unique<AddBrToNextBB>(ir));
	preprocessPasses.emplace_back(std::make_unique<EliminateBrAfterBr>(ir));
	for (auto & p: preprocessPasses) {
		if (auto retCode = p->run()) {
			return retCode;
		}
	}
	CFGIR cfgIR(ir);
	cfgIR.run();
	cfgIR.opti();
	if (SysY::options.emitLLVM) {
		cfgIR.genLLVMFormRes();
	} else {
		cfgIR.genDeSSAFormRes();
	}
	return 0;
}

}
