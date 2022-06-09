#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string_view>
#include <memory>

#include <cprt.hpp>
#include <common.hpp>
#include <antlr4-common.h>

#include "ParserRuleContext.h"
#include "frontend/SysAntlr/SysYLexer.h"
#include "frontend/SysAntlr/SysYParser.h"
#include "frontend/ASTVisitor.hpp"


namespace frontend {

class FrontEnd {
  public:
	static int Main(std::vector<std::string> argv);
	
};
	
}
