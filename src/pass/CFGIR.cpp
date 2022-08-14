#include "CFGIR.hpp"


namespace pass {

NodePool::NodePool() {
	afterEmplace = [this](Node * p) {
		com::Assert(
			p && p->pIRLabel,
			"", CODEPOS
		);
		label2Node.emplace(p->pIRLabel, p);
	};
}

CFG::CFG(ircode::IRModule & ir) : ir(ir) {

}

Node::Node(ircode::AddrJumpLabel * pIRLabel) : pIRLabel(pIRLabel) {

}

}
