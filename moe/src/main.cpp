#include <sys/resource.h>

#include <vector>

#include "cprt.hpp"
#include "common.hpp"
#include "SysY.hpp"
#include "antlr4-runtime.h"

#include "frontend/SysAntlr/SysYParser.h"
#include "frontend/SysAntlr/SysYLexer.h"
#include "frontend/ASTVisitor.hpp"


int Main(int argc, char ** argv) {
	if (argc < 2) {
		com::Throw("You should specify input file path.");
	}
	SysY::parseArgs(argc, argv);
	
	antlr4::ANTLRInputStream input(SysY::source);
	SysYLexer lexer(&input);
	antlr4::CommonTokenStream tokens(&lexer);
	SysYParser parser(&tokens);
	parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());
	SysYParser::CompUnitContext * root = parser.compUnit();
	frontend::ASTVisitor visitor;
	root->accept(&visitor);

//	try {
//		IR::CompileUnit ir;
//		ASTVisitor visitor(ir);
//		bool found_main = visitor.visitCompUnit(root);
//		if (!found_main) throw MainFuncNotFound();
//		dbg << "```cpp\n" << ir << "```\n";
//		optimize_passes(ir);
//		string arch = global_config.get_arg("arch", "armv7");
//		if (arch == "armv7") {
//			ARMv7::Program prog(&ir);
//			ARMv7::optimize_before_reg_alloc(&prog);
//			ofstream asm_out{filename.second};
//			prog.gen_asm(asm_out);
//		} else if (arch == "rv32") {
//			if (global_config.disabled_passes.find("loop-parallel") ==
//			    global_config.disabled_passes.end()) {
//				cerr << "loop parallel is not supported by rv32 backend yet\n";
//				return EXIT_FAILURE;
//			}
//			RV32::Program prog(&ir);
//			RV32::optimize_before_reg_alloc(&prog);
//			ofstream asm_out{filename.second};
//			prog.gen_asm(asm_out);
//		} else {
//			cerr << "unrecognized architecture\n";
//			return EXIT_FAILURE;
//		}
//		return 0;
//	} catch (SyntaxError &e) {
//		cout << "error: " << e.what() << '\n';
//		return EXIT_FAILURE;
//	}
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
	try {
		return Main(argc, argv);
	} catch (const std::exception & e) {
		com::ccerr.cprintLn(e.what());
		return -1;
	}
}
