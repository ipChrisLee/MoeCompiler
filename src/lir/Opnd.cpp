#include "Opnd.hpp"


namespace lir {
int Opnd::cnt = 0;

Opnd::Opnd(OpndType opndType) : id(++cnt), opndType(opndType) {
}

bool Opnd::operator==(const Opnd & rhs) const {
	return id == rhs.id;
}

bool Opnd::operator!=(const Opnd & rhs) const {
	return !(rhs == *this);
}


std::string to_string(RId rid) {
	if (0 <= int(rid) && int(rid) <= 12) {
		return "r" + std::to_string(int(rid));
	} else if (rid == RId::sp) {
		return "sp";
	} else if (rid == RId::lr) {
		return "lr";
	} else if (rid == RId::pc) {
		return "pc";
	} else if (rid == RId::mem) {
		return "mem";
	} else if (rid == RId::unk) {
		return "unk";
	} else {
		return "?";
	}
}

VRegR::VRegR(RId rId) : Opnd(OpndType::VRegR), rId(rId) {
}

std::string VRegR::toLIR() const {
	return "{VRegR: id=" + std::to_string(id) + ", rId=", to_string(rId) + "}";
}

std::string to_string(SId sid) {
	if (0 <= int(sid) && int(sid) <= 31) {
		return "s" + std::to_string(int(sid));
	} else if (sid == SId::mem) {
		return "mem";
	} else if (sid == SId::unk) {
		return "unk";
	} else {
		return "?";
	}
}

VRegS::VRegS(SId sId) : sId(sId) {
}

std::string VRegS::toLIR() const {
	return "{VRegS: id=" + std::to_string(id) + ", rId=", to_string(sId) + "}";
}

Operand2::Operand2(Imm<ImmType::Imm8m> imm) : Opnd(OpndType::Imm), val(imm) {
}

Operand2::Operand2(VRegR vRegR) : Opnd(OpndType::VRegR), val(vRegR) {
}

std::string Operand2::toLIR() const {
	return std::visit([](auto && v) { return v.toLIR(); }, val);
}

template<>
bool Imm<ImmType::Imm8m>::fitThis(int32_t val) {
	auto bs = std::bitset<32>(val);
	auto msk = std::bitset<32>(~0xFF);
	for (int i = 0; i < 32; i += 2) {
		if ((bs & msk).none()) {
			return true;
		}
		STLPro::bitset::rotate_right(bs, 2);
	}
	return false;
}

template<>
bool Imm<ImmType::ImmOffset>::fitThis(int32_t val) {
	return -4095 <= val && val <= 4095;
}

template<>
bool Imm<ImmType::Unk>::fitThis(int32_t val) {
	return true;
}

Label::Label(std::string labelName) :
	Opnd(OpndType::Label), labelName(std::move(labelName)) {
}

std::string Label::toLIR() const {
	if (!labelName.empty()) {
		return "L" + std::to_string(id) + "." + labelName;
	} else {
		return "L" + std::to_string(id);
	}
}

Ptr::Ptr(OpndType opndType) : Opnd(opndType) {
}

StkPtr::StkPtr(int bias, int sz) : Ptr(OpndType::StkPtr), bias(bias), sz(sz) {

}

std::string StkPtr::toLIR() const {
	return "[sp+" + std::to_string(bias) + ", " + std::to_string(sz) + "]";
}


std::string MemPtr::toLIR() const {
	return pAddrG->getName();
}

MemPtr::MemPtr(mir::AddrGlobalVariable * pAddrG) :
	Ptr(OpndType::MemPtr), pAddrG(pAddrG) {
}
}