#include "support-common.hpp"
#include <sstream>


namespace sup {
std::string LLVMable::toLLVMIR() const {
	com::Throw("`toLLVMIR` method of some class is not implemented!");
}
}
namespace sup {

static bool is_little_endian() {
	union {
		const uint16_t i;
		const char c[2];
	} v = {0x0001};
	//                 c: 0 1
	//  little endian: 0x0100
	//  greate endian: 0x0001
	return v.c[0];
}

static bool is_big_endian() {
	union {
		const uint16_t i;
		const char c[2];
	} v = {0x0001};
	//                 c: 0 1
	//  little endian: 0x0100
	//  greate endian: 0x0001
	return v.c[1];
}

std::string valueToHexInLittleEndian(const void * _p, const size_t _bytes) {
	const uint8_t * p = static_cast<const uint8_t *>(_p);//NOLINT
	const int bytes = int(_bytes);
	static char buf[50];
	char * pBuf = buf;

	if (is_little_endian()) {
		pBuf += sprintf(pBuf, "0x");
		for (int i = 0; i < bytes; ++i) {
			pBuf += sprintf(pBuf, "%02X", p[i] & 0xFF);
		}
	} else {
		pBuf += sprintf(pBuf, "0x");
		for (int i = bytes - 1; i >= 0; --i) {
			pBuf += sprintf(pBuf, "%02X", p[i] & 0xFF);
		}
	}
	return buf;
}

std::string valueToHexInBigEndian(const void * _p, const size_t _bytes) {
	const uint8_t * p = static_cast<const uint8_t *>(_p);//NOLINT
	const int bytes = int(_bytes);
	static char buf[50];
	char * pBuf = buf;

	if (is_big_endian()) {
		pBuf += sprintf(pBuf, "0x");
		for (int i = 0; i < bytes; ++i) {
			pBuf += sprintf(pBuf, "%02X", p[i] & 0xFF);
		}
	} else {
		pBuf += sprintf(pBuf, "0x");
		for (int i = bytes - 1; i >= 0; --i) {
			pBuf += sprintf(pBuf, "%02X", p[i] & 0xFF);
		}
	}
	return buf;
}

std::string floatToString(float f) {
	static char buf[50];
	if (SysY::options.floatDecFormat.get()) {
		sprintf(buf, "%f", f);
	} else {
		double d = f;
		return valueToHexInBigEndian(&d, 8);
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
