//
// Created by lee on 7/18/22.
//

#include <common.hpp>

#include "pass-common.hpp"
#include "pass/preprocessPass.hpp"
#include "pass/backendPass.hpp"


namespace pass {

IRPass::IRPass(ircode::IRModule & ir, std::string name) :
	ir(ir), name(std::move(name)) {
	com::addRuntimeWarning(
		"Consider adding pass dependence management.", CODEPOS,
		com::addWarningOnlyOnce
	);
}

int passMain(ircode::IRModule & ir) {
	std::vector<std::unique_ptr<IRPass>> passes;
	passes.emplace_back(std::make_unique<AddBrToNextBB>(ir));
	passes.emplace_back(std::make_unique<RegisterAssign>(ir));
	for (auto & p: passes) {
		if (auto retCode = p->run()) {
			return retCode;
		}
	}
	return 0;
}

}
