//
// Created by lee on 7/18/22.
//

#pragma once

#include "mir/Module.hpp"
#include "mir/Instr.hpp"

#include <string>
#include <set>


namespace mir {
class Pass {
  protected:

  public:
	mir::Module & ir;
	std::string name;

	explicit Pass(mir::Module & ir, std::string name = "");

	virtual int run() = 0;

	virtual ~Pass() = default;

};

int passMain(mir::Module & ir);


}
