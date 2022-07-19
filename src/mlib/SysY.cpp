//
// Created by lee on 6/13/22.
//

#include <getopt.h>

#include "SysY.hpp"
#include "mdb.hpp"


static_assert(sizeof(int) == 4, "Should compile on machine where int is 32-bit.");

namespace SysY {

std::ifstream source;

std::ofstream dest;


Options options;

void parseArgs(int argc, char ** argv) {
	std::string inFilePath, outFilePath;
	int c;
	while (true) {
		int optionIndex = 0;
		static struct option longOptions[] = {
			{"verbose",              no_argument, nullptr, 0},
			{"print-debug-info",     no_argument, nullptr, 0},
			{"emit-llvm",            no_argument, nullptr, 0},
			{"float-dec-format",     no_argument, nullptr, 0},
			{"show-runtime-warning", no_argument, nullptr, 0},
			{"without-any-pass",     no_argument, nullptr, 0},
			{nullptr, 0,                          nullptr, 0}
		};
		c = getopt_long(argc, argv, "-o:SO:", longOptions, &optionIndex);
		if (c == -1) {
			break;
		}
		switch (c) {
			case 0: {
				com::regSwitch(
					longOptions[optionIndex].name, {
						{
							"print-debug-info",     []() {
							options.debug.set(true);
						}},
						{
							"verbose",              []() {
							options.verbose.set(true);
						}},
						{
							"emit-llvm",            []() {
							options.emitLLVM.set(true);
						}},
						{
							"float-dec-format",     []() {
							options.floatDecFormat.set(true);
						}},
						{
							"show-runtime-warning", []() {
							options.showRuntimeWarnings.set(true);
						}},
						{
							"without-any-pass",     []() {
							options.withoutAnyPass.set(true);
						}}
					}
				);
				break;
			}
			case 'S': {
				break;
			}
			case 'd': {
				com::mdb::setSysEnable() = true;
			}
			case 'O': {
				options.optimizationLevel.set(std::stoi(optarg));
				break;
			}
			case 'o': {
				outFilePath = optarg;
				break;
			}
			case 1: {
				if (!inFilePath.empty()) {
					com::cprt("Duplicate sy file!", com::color::warning);
				}
				inFilePath = optarg;
				break;
			}
			default: {
				com::Throw(
					"getopt_long return character code " + std::to_string(c),
					CODEPOS
				);
			}
		}
	}
	source = std::ifstream(inFilePath);
	com::Assert(source.is_open(), "Cannot open input file!", CODEPOS);
	dest = std::ofstream(outFilePath);
}


const char * llvmHeader =
	"target datalayout = \"e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64\"\n"\
    "target triple = \"thumbv7m-unknown-unknown-unknown\"\n";
}
