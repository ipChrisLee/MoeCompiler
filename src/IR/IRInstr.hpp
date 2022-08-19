#pragma once

#include <string>
#include <vector>
#include <list>
#include <functional>

#include "common.hpp"
#include "moeconcept.hpp"

#include "IR/IRAddr.hpp"
#include "support/support-common.hpp"


namespace ircode {

class IRModule;

enum class InstrType {
	Err,                //
	Alloca,
	Label,
	Store,
	Ret,                //  Terminal Instruction
	Add, Sub, Mul, SDiv, SRem,
	FAdd, FSub, FMul, FDiv,
	ConversionOp,
	Load,
	Br,                 //  Terminal Instruction
	Call,               //  Terminal Instruction
	Getelementptr,
	ICmp,
	FCmp,
	Sitofp,
	Fptosi,
	SExt,
	ZExt,
	Phi,
	ParaMov,
	Copy,
	Mark,
};


bool isTerminalInstr(InstrType instrType);

class IRInstr
	: public sup::LLVMable, public moeconcept::Cutable {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

	static int cnt;
  public:
	const int id;
	InstrType instrType;

	explicit IRInstr(InstrType instrType) : id(++cnt), instrType(instrType) {};

	IRInstr(const IRInstr &);

	IRInstr(IRInstr &&) = default;

	IRInstr & operator=(const IRInstr &) = delete;

	[[nodiscard]] std::string toLLVMIR() const override = 0;

	~IRInstr() override = default;

	virtual bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) = 0;
	[[nodiscard]] virtual std::vector<ircode::AddrOperand *> getUse() const = 0;
	[[nodiscard]] virtual ircode::AddrVariable * getDef() const = 0;
};

class InstrAlloca : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	AddrLocalVariable * allocaTo;
	std::unique_ptr<sup::TypeInfo> uPtrTypeToAlloca;

	InstrAlloca(AddrLocalVariable * allocaTo, const sup::TypeInfo & typeToAlloca);

	/**
	 * @brief %c allocaTo = alloca %c type, align 4
	 * @param allocaTo must be pointer type!
	 */
	explicit InstrAlloca(AddrLocalVariable * allocaTo);

	InstrAlloca(const InstrAlloca &);

	InstrAlloca(InstrAlloca &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] AddrVariable * getDef() const override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
};

class InstrStore : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	AddrOperand * from;
	AddrVariable * to;

	InstrStore(AddrOperand * from, AddrVariable * to);
	InstrStore(const InstrStore &) = default;
	InstrStore(InstrStore &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] AddrVariable * getDef() const override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
};

class InstrLoad : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	AddrVariable * from, * to;

	InstrLoad(AddrVariable * from, AddrVariable * to);
	InstrLoad(const InstrLoad &) = default;
	InstrLoad(InstrLoad &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] AddrVariable * getDef() const override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
};

class InstrLabel : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	AddrJumpLabel * pAddrLabel;

	explicit InstrLabel(AddrJumpLabel * pAddrLabel);
	InstrLabel(const InstrLabel &) = default;
	InstrLabel(InstrLabel &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrBr : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	AddrOperand * pCond;
	AddrJumpLabel * pLabelTrue, * pLabelFalse;

	//  unconditional jump
	explicit InstrBr(AddrJumpLabel * pLabel);
	InstrBr(AddrOperand * pCond, AddrJumpLabel * pLabelTrue, AddrJumpLabel * pLabelFalse);
	InstrBr(const InstrBr &) = default;
	InstrBr(InstrBr &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrRet : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	AddrOperand * retAddr;

	explicit InstrRet(AddrOperand * pAddr);
	InstrRet(const InstrRet &) = default;
	InstrRet(InstrRet &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrBinaryOp : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override = 0;
	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

	//  Will check if three addrs have same types.
	InstrBinaryOp(
		AddrOperand * left, AddrOperand * right, AddrVariable * res,
		InstrType instrType, std::string op
	);

  public:
	AddrOperand * left, * right;
	AddrVariable * res;
	std::string op;

	InstrBinaryOp(const InstrBinaryOp &) = default;
	InstrBinaryOp(InstrBinaryOp &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override = 0;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrConversionOp : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override = 0;

	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

	InstrConversionOp(AddrOperand * from, AddrVariable * to, InstrType instrType);

  public:
	AddrOperand * from;
	AddrVariable * to;

	InstrConversionOp(const InstrConversionOp &) = default;
	InstrConversionOp(InstrConversionOp &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override = 0;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrSitofp : public InstrConversionOp {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	InstrSitofp(const InstrSitofp &) = default;
	InstrSitofp(InstrSitofp &&) = default;
	InstrSitofp(AddrOperand * from, AddrVariable * to);

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFptosi : public InstrConversionOp {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	InstrFptosi(const InstrFptosi &) = default;
	InstrFptosi(InstrFptosi &&) = default;
	InstrFptosi(AddrOperand * from, AddrVariable * to);

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrAdd : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	//  Will check if the type of these addr is Int.
	InstrAdd(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrAdd(const InstrAdd &) = default;
	InstrAdd(InstrAdd &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFAdd : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	//  Will check if the type of these addr is Float.
	InstrFAdd(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrFAdd(const InstrFAdd &) = default;
	InstrFAdd(InstrFAdd &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrSub : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Int.
	InstrSub(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrSub(const InstrSub &) = default;
	InstrSub(InstrSub &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFSub : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Float.
	InstrFSub(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrFSub(const InstrFSub &) = default;
	InstrFSub(InstrFSub &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrMul : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Int.
	InstrMul(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrMul(InstrMul &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFMul : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Float.
	InstrFMul(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrFMul(InstrFMul &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrSDiv : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Int.
	InstrSDiv(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrSDiv(InstrSDiv &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFDiv : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Float.
	InstrFDiv(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrFDiv(InstrFDiv &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrSrem : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	//  Will check if the type of these addr is Int.
	InstrSrem(AddrOperand * left, AddrOperand * right, AddrVariable * res);
	InstrSrem(InstrSrem &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};


class InstrCall : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	AddrFunction * func;
	std::vector<AddrOperand *> paramsPassing;
	AddrVariable * retAddr;

	InstrCall(
		AddrFunction * func, std::vector<AddrOperand *> paramsToPass,
		AddrVariable * retAddr
	);
	InstrCall(InstrCall &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	std::vector<ircode::AddrOperand *> getUse() const override;
	AddrVariable * getDef() const override;
};

class InstrGetelementptr : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	AddrVariable * to;
	AddrVariable * from;
	std::vector<AddrOperand *> idxs;

	InstrGetelementptr(
		AddrVariable * to, AddrVariable * from, std::vector<AddrOperand *> idxs
	);
	InstrGetelementptr(InstrGetelementptr &&) = default;

	std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	std::vector<ircode::AddrOperand *> getUse() const override;
	AddrVariable * getDef() const override;
};

class InstrCompare : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	InstrCompare(
		AddrVariable * dest, AddrOperand * leftOp, AddrOperand * rightOp,
		InstrType instrType
	);

  public:
	AddrVariable * dest;
	AddrOperand * leftOp, * rightOp;

	InstrCompare(InstrCompare &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

enum class ICMP {
	//  ref : https://llvm.org/docs/LangRef.html#other-operations
	EQ,     //  ==
	NE,     //  !=
	SGT,    //  >
	SGE,    //  >=
	SLT,    //  <
	SLE,    //  <=
	ERR,    //  For error handle
};

ICMP getReverse(ICMP icmp);

std::string to_string(ICMP icmp);

ICMP strToICMP(const std::string & str);

class InstrICmp : public InstrCompare {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	ICMP icmp;

	InstrICmp(AddrVariable * dest, AddrOperand * leftOp, ICMP icmp, AddrOperand * rightOp);
	InstrICmp(InstrICmp &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

enum class FCMP {
	//  According to ref [https://stackoverflow.com/a/31225334/17924585], we take NaN into consideration.
	OEQ,    //  ==
	OGT,    //  >
	OGE,    //  >=
	OLT,    //  <
	OLE,    //  <=
	UNE,    //  !=
	ERR,    //  For error handle
};

FCMP getReverse(FCMP fcmp);

std::string to_string(FCMP fcmp);

FCMP strToFCMP(const std::string & str);

class InstrFCmp : public InstrCompare {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	FCMP fcmp;

	InstrFCmp(AddrVariable * dest, AddrOperand * leftOp, FCMP fcmp, AddrOperand * rightOp);
	InstrFCmp(InstrFCmp &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrSExt : public InstrConversionOp {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	InstrSExt(const InstrSExt &) = default;
	InstrSExt(InstrSExt &&) = default;
	InstrSExt(AddrOperand * from, AddrVariable * to);

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrZExt : public InstrConversionOp {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	InstrZExt(const InstrZExt &) = default;
	InstrZExt(InstrZExt &&) = default;
	InstrZExt(AddrOperand * from, AddrVariable * to);

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrPhi : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;
  public:
	ircode::AddrLocalVariable * mappingLV;
	ircode::AddrVariable * newDefVar;
	std::map<ircode::AddrJumpLabel *, ircode::AddrOperand *> vecPair;

	explicit InstrPhi(ircode::AddrVariable *, ircode::AddrLocalVariable * mappingLV = nullptr);
	void insertPair(ircode::AddrJumpLabel * pLabel, ircode::AddrOperand *);

	InstrPhi(const InstrPhi &) = default;
	InstrPhi(InstrPhi &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	std::vector<ircode::AddrOperand *> getUse() const override;
	AddrVariable * getDef() const override;
};

class InstrParaMov : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;
	ircode::IRInstr * stand;
  public:
	//  assign a var with a para.
	//  support: Float, Int, Pointer.
	ircode::AddrVariable * to;
	ircode::AddrPara * from;

	InstrParaMov(ircode::AddrVariable * to, ircode::AddrPara * from, ircode::IRModule & ir);
	InstrParaMov(const InstrParaMov &) = default;
	InstrParaMov(InstrParaMov &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

class InstrParallelCopy : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;
  public:
	std::vector<
		std::tuple<ircode::AddrOperand *, ircode::AddrVariable *, ircode::IRInstr *>
	> copies;

	InstrParallelCopy();
	InstrParallelCopy(const InstrParallelCopy &) = default;
	InstrParallelCopy(InstrParallelCopy &&) = default;

	void insert(ircode::AddrOperand * from, ircode::AddrVariable * to, ircode::IRModule & ir);

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

//  This is ir-instruction to solve living-range-out-of-bb problem.
/*  Example:
 *      BB1:
 *      %x = add i32 1, 1
 *      BB2:
 *      %y = add i32 %x, 1
 *      %z = add i32 1, 2
 *      [mark %x]
 *      br BB1
 *  %x is still living when %z is initializing! So %x is interfered to %y!
 *  But for flow-form ir analyzing, I can NOT find this fact.
 *  So for every `var`, we add `mark` instruction at every exit at basic blocks dominated by
 *  basic block %x defined.
 *  Notice that: For `var` that every use is in the same bb as def, we do NOT add `mark` instruction,
 *  since there is no living-range-out-of-bb problem.
 *  This instruction is added after phi resolution, before register allocation.
 *  This is not mapped to any llvm-ir instruction.
 */
class InstrMarkVars : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	std::set<ircode::AddrVariable *> vecPVar;

	InstrMarkVars();
	InstrMarkVars(const InstrMarkVars &) = default;
	InstrMarkVars(InstrMarkVars &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
	bool changeUse(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) override;
	[[nodiscard]] std::vector<ircode::AddrOperand *> getUse() const override;
	[[nodiscard]] AddrVariable * getDef() const override;
};

}

