#include <vector>
#include <string>

#include <cprt.hpp>

#include "atn/ParserATNSimulator.h"
#include "frontend/frontend.hpp"
#include "frontend/SysAntlr/SysYBaseVisitor.h"

int frontend::FrontEnd::Main(std::vector<std::string>argv){
    com::ccout.cprintLn("Frontend Main Function.");
    std::ifstream source{argv[0]};
//    ASTVisitor astVisitor(argv[1]);
//    if (!source.is_open()) { com::Throw("Can not open file!"); }
//    antlr4::ANTLRInputStream input(source);
//    SysYLexer lexer(&input);
//    antlr4::CommonTokenStream tokens(&lexer);
//    SysYParser parser(&tokens);
//    parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());
//    parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->setPredictionMode(antlr4::atn::PredictionMode::LL);
//    com::ccout.cprintLn( int(parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->getPredictionMode()));
//    SysYParser::CompUnitContext * root = parser.compUnit();
//    astVisitor.visitCompUnit(root);
//    dfs(root);
    return 0;   
}

