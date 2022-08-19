#pragma once

#include "pass/pass-common.hpp"


namespace pass {

class FirstConstantPropagation : public IRPass {
  protected:
	int run(ircode::IRFuncDef * pFuncDef);

	std::pair<ircode::AddrVariable *, ircode::AddrStaticValue *>
	mapping(ircode::IRInstr * pInstr);

	void useMapping(
		ircode::IRFuncDef * pFuncDef,
		std::pair<ircode::AddrVariable *, ircode::AddrStaticValue *> & mp
	);


  public:
	int run() override;

};


}