//
// Created by lee on 7/18/22.
//

#pragma once

#include <unordered_map>

#include "pass/pass-common.hpp"
#include "IR/Instr.hpp"


namespace pass {

/**
 * @brief Clear Continuous Jump Labels.
 * @example <tt>INSTRS;label_2:</tt> -> <tt>INSTRS;br label_2;label_2:</tt> (INSTRS not end with br)
 */
class AddBrToNextBB : public IRPass {
  protected:
	int run(std::list<mir::Instr *> & instrs);

  public:
	int run() override;

	explicit AddBrToNextBB(mir::Module & ir, std::string name = "");
};


}