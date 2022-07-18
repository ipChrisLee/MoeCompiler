//
// Created by lee on 7/18/22.
//

#pragma once

#include "IR/IRModule.hpp"

#include <string>
#include <set>


namespace pass {
class Pass {
  protected:

  public:
	std::string name;
	ircode::IRModule & ir;

	explicit Pass(ircode::IRModule & ir);

	virtual bool run() = 0;

};

int passMain(ircode::IRModule & ir);

}
