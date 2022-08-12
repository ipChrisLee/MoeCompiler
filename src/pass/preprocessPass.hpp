#pragma once

#include <unordered_map>

#include "pass/pass-common.hpp"
#include "IR/IRInstr.hpp"


namespace pass {

/**
 * @brief Clear Continuous Jump Labels.
 * @example <tt>INSTRS;label_2:</tt> -> <tt>INSTRS;br label_2;label_2:</tt> (INSTRS not end with br)
 */
class AddBrToNextBB : public IRPass {
  protected:
	int run(std::list<ircode::IRInstr *> & instrs);

  public:
	int run() override;

	explicit AddBrToNextBB(ircode::IRModule & ir, std::string name = "");
};


}