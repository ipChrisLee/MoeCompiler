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
			{"verbose", no_argument, nullptr, 0},
			{"debug",   no_argument, nullptr, 0},
			{nullptr, 0,             nullptr, 0}
		};
		c = getopt_long(argc, argv, "-o:SO:", longOptions, &optionIndex);
		if (c == -1) {
			break;
		}
		switch (c) {
			case 0: {
				com::regSwitch(
					longOptions[optionIndex].name, {
						{"debug",   []() {
							options.debug.set(true);
						}},
						{"verbose", []() {
							options.verbose.set(true);
						}},
					});
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
				inFilePath = optarg;
				break;
			}
			default: {
				com::Throw("getopt_long return character code " + std::to_string(c),
				           CODEPOS);
			}
		}
	}
	source = std::ifstream(inFilePath);
	com::Assert(source.is_open(), "Cannot open input file!", CODEPOS);
	dest = std::ofstream(outFilePath);
}


}
