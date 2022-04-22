#include <vector>
#include <string>

#include <submain.hpp>
#include <cprt.hpp>

#include "frontend/frontend.hpp"
#include "frontend/ASTVisitor.hpp"
#include "frontend/SysAntlr/SysYBaseVisitor.h"

AddSubMain(frontendParseTest,
    [](const std::vector<std::string>&argv)->int{
        std::ifstream source{argv[0]};
        SysYBaseVisitor visitor;
        if (!source.is_open()) { com::Throw("Can not open file!"); }
        antlr4::ANTLRInputStream input(source);
        SysYLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        SysYParser parser(&tokens);
        parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());
        SysYParser::CompUnitContext * root = parser.compUnit();
        visitor.visitCompUnit(root);
        return 0;
    }
);

AddSubMain(frontendmain,frontend::FrontEnd::Main);

int frontend::FrontEnd::Main(std::vector<std::string>argv){
    com::ccout.cprintLn("Frontend Main Function.");
    std::ifstream source{argv[0]};
    ASTVisitor astVisitor(argv[1]);
    if (!source.is_open()) { com::Throw("Can not open file!"); }
    antlr4::ANTLRInputStream input(source);
    SysYLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    SysYParser parser(&tokens);
    parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());
    parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->setPredictionMode(antlr4::atn::PredictionMode::LL);
    com::ccout.cprintLn( int(parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->getPredictionMode()));
    SysYParser::CompUnitContext * root = parser.compUnit();
    astVisitor.visitCompUnit(root);
    dfs(root);
    return 0;   
}

void frontend::FrontEnd::dfs(antlr4::tree::ParseTree * rt){
    com::ccout.cprintLn(
        std::tuple(
            rt->getText(),
            " ",
            int(rt->getTreeType())
        )
    );
    size_t n=rt->children.size();
    for(size_t i=0;i<n;++i){
        dfs(rt->children[i]);
    }
}
