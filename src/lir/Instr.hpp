#pragma once

#include <string>

#include "support/Support.hpp"
#include "lir/Opnd.hpp"


namespace lir {
enum class Cond {
	//  ~ means not of result.
	EQ,         //  ICMP::EQ    ~FCMP::UNE
	NE,         //  ICMP::NE    ~FCMP::OEQ
	PL,         //              ~FCMP::OLT
	HI,         //              ~FCMP::OLE
	GE,         //  ICMP::SGE
	LT,         //  ICMP::SLT   ~FCMP::OGE
	GT,         //  ICMP::SGT
	LE,         //  ICMP::SLE   ~FCMP::OGT
	AL,         //  No condition
};

std::string to_string(Cond cond);

enum class InstrType {
	ADD, SUB,
	MOV,
	MOVImm,
	STR, STROnImmOffset,
	LDROnImmOffset,
	LABEL,
	LOAD,
};

class Instr : public LIRable {
  protected:
	static int cnt;

	explicit Instr(InstrType instrType, Cond cond = Cond::AL);

	Instr(const Instr &) = default;

	Instr(Instr &&) = default;

  public:
	const int id;
	const InstrType instrType;
	const Cond cond;

	[[nodiscard]] std::string toLIR() const override = 0;

};

class InstrLOAD : public Instr {
  public:
	VRegR * dst;
	MemPtr * fromMem = nullptr;
	StkPtr * fromStk = nullptr;

	InstrLOAD(VRegR * dst, MemPtr * fromMem);

	InstrLOAD(VRegR * dst, StkPtr * fromStk);

	[[nodiscard]] std::string toLIR() const override;
};

class InstrMOVImm : public Instr {
  public:
	VRegR * rd;
	Imm<ImmType::Unk> * imm;

	InstrMOVImm(VRegR * rd, Imm<ImmType::Unk> * imm, Cond cond = Cond::AL);

	[[nodiscard]] std::string toLIR() const override;
};

class InstrSTR : public Instr {
  public:
	VRegR * rd;
	Ptr * ptr;

	InstrSTR(VRegR * rd, Ptr * ptr, Cond cond = Cond::AL);

	InstrSTR(const InstrSTR &) = default;

	InstrSTR(InstrSTR &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrADD : public Instr {
  public:
	VRegR rd, rn;
	Operand2 op2;

	InstrADD(VRegR rd, VRegR rn, Operand2 op2, Cond cond = Cond::AL);

	InstrADD(const InstrADD &) = default;

	InstrADD(InstrADD &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrSUB : public Instr {
  public:
	VRegR rd, rn;
	Operand2 op2;

	InstrSUB(VRegR rd, VRegR rn, Operand2 op2, Cond cond = Cond::AL);

	InstrSUB(const InstrSUB &) = default;

	InstrSUB(InstrSUB &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrMOV : public Instr {
  public:
	VRegR rd;
	Operand2 op2;

	InstrMOV(VRegR rd, Operand2 op2, Cond cond = Cond::AL);

	InstrMOV(const InstrMOV &) = default;

	InstrMOV(InstrMOV &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrLDROnImmOffset : public Instr {
  public:
	VRegR rd, rn;
	Imm<ImmType::ImmOffset> offset;

	InstrLDROnImmOffset(
		VRegR rd, VRegR rn,
		const Imm<ImmType::ImmOffset> & offset, Cond cond = Cond::AL
	);

	InstrLDROnImmOffset(const InstrLDROnImmOffset &) = default;

	InstrLDROnImmOffset(InstrLDROnImmOffset &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrSTROnImmOffset : public Instr {
  public:
	VRegR rd, rn;
	Imm<ImmType::ImmOffset> offset;

	InstrSTROnImmOffset(
		VRegR rd, VRegR rn,
		const Imm<ImmType::ImmOffset> & offset, Cond cond = Cond::AL
	);

	InstrSTROnImmOffset(const InstrSTROnImmOffset &) = default;

	InstrSTROnImmOffset(InstrSTROnImmOffset &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class InstrLABEL : public Instr {
  public:
	Label * label;

	explicit InstrLABEL(Label * label);

	InstrLABEL(const InstrLABEL &) = default;

	InstrLABEL(InstrLABEL &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};
}