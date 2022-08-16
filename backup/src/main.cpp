#include <sys/resource.h>

#include <vector>

#include "cprt.hpp"
#include "common.hpp"
#include "SysY.hpp"
#include "antlr4-runtime.h"

#include "frontend/SysAntlr/SysYParser.h"
#include "frontend/SysAntlr/SysYLexer.h"
#include "frontend/ASTVisitor.hpp"
#include "pass/pass-common.hpp"


int Main(int argc, char ** argv) {
	if (argc < 2) {
		com::Throw("You should specify input file path.");
	}
	SysY::parseArgs(argc, argv);
	antlr4::ANTLRInputStream input(SysY::preprocessOnSource(SysY::source));
	SysYLexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	SysYParser parser(&tokens);
	parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());
	SysYParser::CompUnitContext * root = parser.compUnit();
	ircode::IRModule ir;
	frontend::ASTVisitor visitor(ir);
	root->accept(&visitor);
	ir.finishLoading();
	if (!SysY::options.withoutAnyPass.get()) {
		pass::passMain(ir);
	}
	SysY::dest << ir.toLLVMIR() << std::endl;
	return 0;
}

int main(int argc, char ** argv) {
	//  For better exception handle.
	//  https://www.digitalpeer.com/blog/find-where-a-cpp-exception-is-thrown
	/*  To get where an exception is thrown, make the files, and gdb it.
	 *  Then type `catch throw` and `run` and `where`.
	 *  For this project, gdb is called with '-ex="catch throw"' for the target `gdb`.
	 *  So there is no need to type `catch throw` anymore.
	 * */
	int retval = 0;
	try {
		retval = Main(argc, argv);
	} catch (const std::exception & e) {
		com::ccerr.cprintLn(e.what());
		retval = -1;
	}
	if (SysY::options.showRuntimeWarnings) {
		com::showAllRuntimeWarnings();
	}
	return retval;
}
