#pragma once

#include <string>
#include <tuple>

#include "backend/Opnd.hpp"
#include "backend/to_asm.hpp"
#include <stlpro.hpp>


namespace backend {

inline std::string to_asm(int32_t x) {
	static char buf[50];
	sprintf(buf, "0x%x", x);
	return "#" + std::to_string(x);
}

inline std::string to_asm(const char * s) { return s; }

inline std::string to_asm(const std::string & s) { return s; }

inline std::string to_asm(const char & s) {
	static char buf[10];
	sprintf(buf, "%c", s);
	return buf;
}

extern int instrCnt;

inline bool inLabelRange(int from, int to) {
	//  is ( from-8MB+bias < to < from +8MB-bias) => ( |to-from| < (8MB-bias) )
	int64_t f = from;
	int64_t t = to;
	const int64_t bias = 1024;
	const int64_t range = 1 << 23;
	auto d = std::abs(t - f);
	return d < range - bias;
}

template<typename Tf, typename ...Ts>
std::string toASM(Tf && tf, Ts && ... args) {
	auto res = to_asm(tf) + " ";
	((res += to_asm(args) + ","), ...);
	res.pop_back();
	res += "\n";
	++instrCnt;
	return res;
}

}

