#pragma once

#include <map>
#include <vector>
#include <moeconcept.hpp>

#include "IR/IRAddr.hpp"
#include "pass/pass-common.hpp"


namespace pass {


class Node {
  protected:
  public:
	explicit Node(ircode::AddrJumpLabel * pIRLabel);
	ircode::AddrJumpLabel * pIRLabel;
	std::vector<Node *> out;
};

class NodePool : public moeconcept::Pool<Node> {
  public:
	std::map<ircode::AddrJumpLabel *, Node *> label2Node;

	NodePool();

};

class CFG {
  protected:
	ircode::IRModule & ir;
	moeconcept::Pool<Node> nodePool;
  public:
	explicit CFG(ircode::IRModule & ir);

};

}
