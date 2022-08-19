//
// Created by lee on 7/18/22.
//

#pragma once

#include <stlpro.hpp>

#include "IR/IRModule.hpp"
#include "IR/IRInstr.hpp"

#include <string>
#include <set>


namespace pass {
class IRPass {
  protected:

  public:
	ircode::IRModule & ir;
	std::string name;

	explicit IRPass(ircode::IRModule & ir, std::string name = "");

	virtual int run() = 0;

	virtual ~IRPass() = default;

};

int passMain(ircode::IRModule & ir);

}
