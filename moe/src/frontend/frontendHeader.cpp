#include <sstream>
#include <cinttypes>

#include "SysY.hpp"

#include "frontend/frontendHeader.hpp"

namespace ircode {
std::string LLVMable::toLLVMIR() const {
	com::Throw("`toLLVMIR` method of some class is not implemented!");
}

std::string floatToString(float f) {
	static char buf[50];
	if(SysY::options.floatDecFormat.get()){
		sprintf(buf,"%f",f);
	}else{
		sprintf(buf, "%a", f);
	}
	return buf;
}

std::string intToString(int i) {
	static char buf[50];
	sprintf(buf, "%d", i);
	return buf;
}

int literalToInt(const std::string & s) {
	uint32_t ui;
	std::stringstream ss(s);
	switch (s.length()) {
		case 0: {
			com::Throw("string s is empty.", CODEPOS);
		}
		case 1: { //
			ss >> std::dec >> ui;
			break;
		}
		case 2: {
			if (s[0] == '0') {
				ss >> std::oct >> ui;
			} else {
				ss >> std::dec >> ui;
			}
			break;
		}
		default: {
			if (s[0] == '0') {
				if (s[1] == 'x' || s[1] == 'X') {
					ss >> std::hex >> ui;
				} else {
					ss >> std::oct >> ui;
				}
			} else {
				ss >> std::dec >> ui;
			}
			break;
		}
	}
	return int(ui);
}

float literalToFloat(const std::string & s) {
	return std::stof(s);
}

}
