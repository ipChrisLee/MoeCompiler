#include <bit>
#include <bitset>

#include "Opnd.hpp"


namespace backend {

int Opnd::cnt = 0;

Opnd::Opnd() : id(++cnt) {

}

OpndType VRegR::getOpndType() const {
	return OpndType::VRegR;
}

VRegR::VRegR(RId rid, int offset) : rid(rid), offset(offset) {
}

OpndType VRegS::getOpndType() const {
	return OpndType::VRegS;
}

VRegS::VRegS(SId sid, int offset) : sid(sid), offset(offset) {
}

OpndPool::OpndPool() {
	afterEmplace = [this](Opnd * p) {
		if (auto * pStkPtr = dynamic_cast<StkPtr *>(p)) {
			stkVars.emplace_back(pStkPtr);
		}
	};
}

OpndType StkPtr::getOpndType() const {
	return OpndType::StkPtr;
}

StkPtr::StkPtr(int offset, int sz) : offset(offset), sz(sz) {
}

Label::Label(ircode::AddrFunction * pAddrFunc) : labelStr(pAddrFunc->getName()) {
}

Label::Label(ircode::AddrJumpLabel * pJumpLabel) :
	labelStr(pJumpLabel->labelName) {
	labelStr = "L" + std::to_string(id) + "." + labelStr;
}

Label::Label(ircode::AddrGlobalVariable * pAddrGVar) :
	labelStr(pAddrGVar->getName()) {
	labelStr = "L" + std::to_string(id) + "." + labelStr;
}

bool isGPR(RId rid) {
	return 0 <= int(rid) && int(rid) <= 11;
}

bool isCallerSave(RId rid) {
	return 0 <= int(rid) && int(rid) <= 3;
}

bool isCalleeSave(RId rid) {
	return 4 <= int(rid) && int(rid) <= 11;
}

std::string to_asm(RId rid) {
	if (0 <= int(rid) && int(rid) <= 12) {
		return "r" + std::to_string(int(rid));
	} else if (rid == RId::sp) {
		return "sp";
	} else if (rid == RId::lr) {
		return "lr";
	} else if (rid == RId::pc) {
		return "pc";
	} else {
		com::Throw("", CODEPOS);
	}
}

bool isGPR(SId sid) {
	return (0 <= int(sid) && int(sid) <= 13) || (16 <= int(sid) && int(sid) <= 31);
}

bool isCallerSave(SId sid) {
	return 0 <= int(sid) && int(sid) <= 13;
}

bool isCalleeSave(SId sid) {
	return 16 <= int(sid) && int(sid) <= 31;
}

std::string to_asm(SId sid) {
	return "s" + std::to_string(int(sid));
}


template<>
bool Imm<ImmType::ImmOffset>::fitThis(int32_t x) {
	return -4095 < x && x < 4095;
}

template<>
bool Imm<ImmType::Imm8m>::fitThis(int32_t x) {
	auto u = (uint32_t) x;
	auto mask = ~(uint32_t) 0xFF;

	for (int i = 0; i < 32; i = i + 2) {
		if (!(u & mask)) {
			return true;
		}
		u = (u >> 2) | ((u & 0x00000003) << 30);
	}
	return false;
}

template<>
bool Imm<ImmType::Immed>::fitThis(int32_t x) {
	return 0 <= x && x <= 1020 && x % 4 == 0;
}

std::tuple<int32_t, int32_t> splitNumber(int32_t x) {
	uint32_t u = x;
	uint16_t uH = u >> 16;
	uint16_t uL = u & 0x0000FFFF;
	return {uH, uL};
}

std::tuple<int32_t, int32_t> splitNumber(float x) {
	uint32_t u = *reinterpret_cast<uint32_t *>(&x);
	uint16_t uH = u >> 16;
	uint16_t uL = u & 0x0000FFFF;
	return {uH, uL};
}

}
