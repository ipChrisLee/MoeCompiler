//
// Created by lee on 7/18/22.
//

#include "pass-common.hpp"
#include <common.hpp>


namespace pass {

Pass::Pass(ircode::IRModule & ir) : ir(ir) {
	com::addRuntimeWarning(
		"Consider adding pass dependence management.", CODEPOS,
		com::addWarningOnlyOnce
	);
}

}
