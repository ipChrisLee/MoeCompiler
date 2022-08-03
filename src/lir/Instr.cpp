//
// Created by lee on 7/31/22.
//

#include "Instr.hpp"

#include <utility>


namespace lir {
std::string to_string(Cond cond) {
	auto res = std::string();
	switch (cond) {
		case Cond::EQ: res = "eq";
			break;
		case Cond::NE: res = "ne";
			break;
		case Cond::PL: res = "pl";
			break;
		case Cond::HI: res = "hi";
			break;
		case Cond::GE: res = "ge";
			break;
		case Cond::LT: res = "lt";
			break;
		case Cond::GT: res = "gt";
			break;
		case Cond::LE: res = "le";
			break;
		case Cond::AL: res = "";
			break;
	}
	return res;
}

int Instr::cnt = 0;

Instr::Instr(InstrType instrType, Cond cond) :
	id(++cnt), instrType(instrType), cond(cond) {}

std::string InstrADD::toLIR() const {
	return "add" + to_string(cond) + " " +
		rd.toLIR() + ", " + rn.toLIR() + ", " + op2.toLIR();
}

InstrADD::InstrADD(VRegR rd, VRegR rn, Operand2 op2, Cond cond) :
	Instr(InstrType::ADD, cond),
	rd(std::move(rd)), rn(std::move(rn)), op2(std::move(op2)) {
}

InstrSUB::InstrSUB(VRegR rd, VRegR rn, Operand2 op2, Cond cond) :
	Instr(InstrType::SUB, cond),
	rd(std::move(rd)), rn(std::move(rn)), op2(std::move(op2)) {
}

std::string InstrSUB::toLIR() const {
	return "sub" + to_string(cond) + " " +
		rd.toLIR() + ", " + rn.toLIR() + ", " + op2.toLIR();
}

std::string InstrMOV::toLIR() const {
	return "mov" + to_string(cond) + " " + rd.toLIR() + ", " + op2.toLIR();
}

InstrMOV::InstrMOV(VRegR rd, Operand2 op2, Cond cond) :
	Instr(InstrType::MOV, cond), rd(std::move(rd)), op2(std::move(op2)) {
}

InstrLDROnImmOffset::InstrLDROnImmOffset(
	VRegR rd, VRegR rn,
	const Imm<ImmType::ImmOffset> & offset, Cond cond
) : Instr(InstrType::LDROnImmOffset, cond),
    rd(std::move(rd)), rn(std::move(rn)), offset(offset) {
}

std::string InstrLDROnImmOffset::toLIR() const {
	return "ldr" + to_string(cond) + " " +
		rd.toLIR() + ", [" + rn.toLIR() + ", " + offset.toLIR() + "]";
}

InstrSTROnImmOffset::InstrSTROnImmOffset(
	VRegR rd, VRegR rn, const Imm<ImmType::ImmOffset> & offset,
	Cond cond
) : Instr(InstrType::STROnImmOffset, cond),
    rd(std::move(rd)), rn(std::move(rn)), offset(offset) {
}

std::string InstrSTROnImmOffset::toLIR() const {
	return "str" + to_string(cond) + " " +
		rd.toLIR() + ", [" + rn.toLIR() + ", " + offset.toLIR() + "]";
}

InstrLABEL::InstrLABEL(Label * label) : Instr(InstrType::LABEL), label(label) {
}

std::string InstrLABEL::toLIR() const {
	return label->toLIR() + ":";
}

InstrLOAD::InstrLOAD(VRegR * dst, MemPtr * fromMem) :
	Instr(InstrType::LOAD), dst(dst), fromMem(fromMem) {
	com::Assert(dst && fromMem);
}

InstrLOAD::InstrLOAD(VRegR * dst, StkPtr * fromStk) :
	Instr(InstrType::LOAD), dst(dst), fromStk(fromStk) {
	com::Assert(dst && fromStk);
}

std::string InstrLOAD::toLIR() const {
	if (fromMem) {
		return "load " + dst->toLIR() + " from " + fromMem->toLIR();
	} else {
		return "load " + dst->toLIR() + " from " + fromStk->toLIR();
	}
}

InstrSTR::InstrSTR(VRegR * rd, Ptr * ptr, Cond cond) :
	Instr(InstrType::STR, cond), rd(rd), ptr(ptr) {}

std::string InstrSTR::toLIR() const {
	return "str " + rd->toLIR() + " " + ptr->toLIR();
}

InstrMOVImm::InstrMOVImm(VRegR * rd, Imm<ImmType::Unk> * imm, Cond cond) :
	Instr(InstrType::MOVImm, cond), rd(rd), imm(imm) {}

std::string InstrMOVImm::toLIR() const {
	return rd->toLIR() + " = " + imm->toLIR();
}
}

