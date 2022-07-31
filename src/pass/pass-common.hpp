//
// Created by lee on 7/18/22.
//

#pragma once

#include "IR/Module.hpp"
#include "IR/Instr.hpp"

#include <string>
#include <set>


namespace pass {
class IRPass {
  protected:

  public:
	mir::Module & ir;
	std::string name;

	explicit IRPass(mir::Module & ir, std::string name = "");

	virtual int run() = 0;

	virtual ~IRPass() = default;

};

int passMain(mir::Module & ir);

template<typename Iter>
typename std::iterator_traits<Iter>::value_type & g(Iter & it) {
	return *it;
}

}
