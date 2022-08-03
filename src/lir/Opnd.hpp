#pragma once

#include <bitset>
#include <stlpro.hpp>
#include <moeconcept.hpp>

#include "support/Support.hpp"
#include "mir/Addr.hpp"


namespace lir {
enum class OpndType {
	Err,
	VRegR,
	VRegS,
	Label,
	Imm,
	StkPtr,
	MemPtr,
};

class Opnd : public LIRable {
  protected:
	explicit Opnd(OpndType opndType = OpndType::Err);

	Opnd(const Opnd &) = default;

	Opnd(Opnd &&) = default;

	static int cnt;
  public:
	const int id;
	const OpndType opndType;

	bool operator==(const Opnd & rhs) const;

	bool operator!=(const Opnd & rhs) const;

};

class OpndPool : public moeconcept::Pool<Opnd> {
  public:
	OpndPool() = default;
};

enum class RId : int {
	r0 = 0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
	sp = r13, lr = r14, pc = r15,
	lhs = r12, rhs = lr,    //  trunk operands reg
	stk = -4,
	no = -3,
	unk = -2,   //  not specified,
	mem = -1,   //  spilling on mem
};

std::string to_string(RId rid);


class VRegR : public Opnd {
  protected:
  public:

	RId rId;

	explicit VRegR(RId rId = RId::unk);

	VRegR(const VRegR &) = default;

	VRegR(VRegR &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

enum class SId : int {
	s0 = 0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15,
	s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31,
	mem = -1,   //  spilling on mem
	stk = -2,   //  spilling on stack
	unk = -3,   //  not specified
	lhs = s30, rhs = s31,   //  thunk operands reg
};

std::string to_string(SId sid);

class VRegS : public Opnd {
  public:
	SId sId;

	explicit VRegS(SId sId = SId::unk);

	VRegS(const VRegS &) = default;

	VRegS(VRegS &&) = default;

	[[nodiscard]] std::string toLIR() const override;

};

enum class ImmType {
	Unk,            //  Undefined
	Imm8m,
	ImmOffset,      //  [-4095,4095]
};

template<ImmType immType>
class Imm : public Opnd {
  protected:
  public:
	int32_t imm;

	explicit Imm(int32_t imm) : Opnd(OpndType::Imm), imm(imm) {
		com::Assert(fitThis(imm), "Check Imm type first!", CODEPOS);
	}

	explicit operator int() const { return imm; }

	static bool fitThis(int32_t val);

	[[nodiscard]] std::string toLIR() const override {
		return "#" + std::to_string(imm);
	}
};


class Operand2 : public Opnd {
  public:
	std::variant<Imm<ImmType::Imm8m>, VRegR> val;

	explicit Operand2(Imm<ImmType::Imm8m> imm);

	explicit Operand2(VRegR vRegR);

	Operand2(const Operand2 &) = default;

	Operand2(Operand2 &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class Label : public Opnd {
  protected:
	std::string labelName;
  public:
	explicit Label(std::string labelName = "");

	[[nodiscard]] std::string toLIR() const override;
};

class Ptr : public Opnd {
  protected:
	explicit Ptr(OpndType opndType);

	Ptr(const Ptr &) = default;

	Ptr(Ptr &&) = default;
};

class StkPtr : public Ptr {
  public:
	int bias, sz;

	explicit StkPtr(int bias, int sz = 4);

	StkPtr(const StkPtr &) = default;

	StkPtr(StkPtr &&) = default;

	[[nodiscard]] std::string toLIR() const override;
};

class MemPtr : public Ptr {
  public:
	mir::AddrGlobalVariable * pAddrG;

	MemPtr(const MemPtr &) = default;

	MemPtr(MemPtr &&) = default;

	explicit MemPtr(mir::AddrGlobalVariable * pAddrG);

	[[nodiscard]] std::string toLIR() const override;
};

}