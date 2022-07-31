//
// Created by lee on 7/18/22.
//

#include "mlib/common.hpp"

#include "Pass.hpp"
#include "PreprocessPass.hpp"


namespace mir {

Pass::Pass(mir::Module & ir, std::string name) :
	ir(ir), name(std::move(name)) {
	com::addRuntimeWarning(
		"Consider adding pass dependence management.", CODEPOS,
		com::addWarningOnlyOnce
	);
}

int passMain(mir::Module & ir) {
	std::vector<std::unique_ptr<Pass>> passes;
	passes.emplace_back(std::make_unique<AddBrToNextBB>(ir));
	for (auto & p: passes) {
		if (auto retCode = p->run()) {
			return retCode;
		}
	}
	return 0;
}

}
