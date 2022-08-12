#pragma once

#include <climits>
#include <bitset>

#include "moeconcept.hpp"
#include "IR/IRAddr.hpp"


namespace backend {
enum class OpndType {
	Err,
	VRegR,
	VRegS,
	StkPtr,
	Label,
};


class Opnd {
  protected:
	static int cnt;
  public:
	const int id;

	Opnd();

	[[nodiscard]] virtual OpndType getOpndType() const { return OpndType::Err; };

	virtual ~Opnd() = default;
};


enum class RId : int {
	r0 = 0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15,
	sp = r13, lr = r14, pc = r15,
	lhs = r12, rhs = lr,
	unk = INT_MIN,  //  unknown
	err,
	stk,            //  will be spilled to stack
	mem,
};

static const int mxRIdForParameters = 3;

static const int ridCnt = 31;

bool isGPR(RId rid);    //  is general purpose register (r0-r11)
bool isCallerSave(RId rid);
bool isCalleeSave(RId rid);
std::string to_asm(RId rid);

class VRegR : public Opnd {
  protected:
  public:
	[[nodiscard]] OpndType getOpndType() const override;

	RId rid;
	int offset;

	explicit VRegR(RId rid, int offset = INT_MIN);
};

enum class SId : int {
	s0 = 0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15,
	s16, s17, s18, s19, s20, s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31,
	lhs = s14, rhs = s15,
	unk = INT_MIN,  //  unknown
	err,
	stk,            //  will be spilled to stack
	mem,
};

static const int mxSIdForParameters = 13;

static const int sidCnt = 32;

bool isGPR(SId sid);    //  is general purpose register. (s0-s13 s16-s31)
bool isCallerSave(SId sid);
bool isCalleeSave(SId sid);
std::string to_asm(SId sid);

class VRegS : public Opnd {
  protected:
  public:
	[[nodiscard]] OpndType getOpndType() const override;

	SId sid;
	int offset;

	explicit VRegS(SId sid, int offset = INT_MIN);
};

class StkPtr : public Opnd {
  public:
	[[nodiscard]] OpndType getOpndType() const override;

	int offset, sz;

	StkPtr(int offset, int sz);
};


class OpndPool : public moeconcept::Pool<Opnd> {
  protected:
  public:
	std::vector<StkPtr *> stkVars;

	OpndPool();
};

class Label : public Opnd {
  protected:
  public:
	std::string labelStr;
	int lineNum = INT_MIN;

	explicit Label(ircode::AddrFunction * pAddrFunc);

	explicit Label(ircode::AddrJumpLabel * pJumpLabel);

	explicit Label(ircode::AddrGlobalVariable * pAddrGVar);

};

enum class ImmType {
	ImmOffset,
	Imm8m,
	Immed,
};

template<ImmType immType>
class Imm {
  public:
	int32_t i;
	static bool fitThis(int32_t x);
};

//  return a,b where `a` is high 16 bits of `x` and `b` is low 16 bits of `x`.
//  Only low 16 bits of `a` and `b` is used, and upper bits of them are all zero.
std::tuple<int32_t, int32_t> splitNumber(int32_t x);

std::tuple<int32_t, int32_t> splitNumber(float x);

}
