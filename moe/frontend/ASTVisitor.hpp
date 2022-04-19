#pragma once

#include <string>
#include <string_view>

#include <cprt.hpp>

#include "frontend/SysAntlr/SysYBaseVisitor.h"


namespace frontend{

/*  Visitor for AST generated by parser.
 * */
class ASTVisitor : public SysYBaseVisitor {
  private:
    com::cprt printer;
  public:
    ASTVisitor(const std::string & outFilePath=com::nullFilePath):
        printer(outFilePath){
    }
    
};

}