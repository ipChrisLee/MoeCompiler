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

enum class InstrType {
	Err,
	Alloca,
	Label,
	Store,
	Ret,
	BinaryOp,
	ConversionOp,
	Load,
	Br,
	Call,
	Getelementptr,
	ICmp,
	FCmp,
	Sitofp,
	Fptosi,
	SExt,
	ZExt,
};

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
};

class InstrAlloca : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	AddrVariable * allocaTo;
	std::unique_ptr<sup::TypeInfo> uPtrTypeToAlloca;
  public:
	InstrAlloca(AddrVariable * allocaTo, const sup::TypeInfo & typeToAlloca);

	/**
	 * @brief %c allocaTo = alloca %c type, align 4
	 * @param allocaTo must be pointer type!
	 */
	explicit InstrAlloca(AddrVariable * allocaTo);

	InstrAlloca(const InstrAlloca &);

	InstrAlloca(InstrAlloca &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrStore : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	AddrOperand * from;
	AddrVariable * to;
  public:

	InstrStore(AddrOperand * from, AddrVariable * to);

	InstrStore(const InstrStore &) = default;

	InstrStore(InstrStore &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrLoad : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	AddrVariable * from, * to;
  public:

	InstrLoad(AddrVariable * from, AddrVariable * to);

	InstrLoad(const InstrLoad &) = default;

	InstrLoad(InstrLoad &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrLabel : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	AddrJumpLabel * pAddrLabel;
  public:

	explicit InstrLabel(AddrJumpLabel * pAddrLabel);

	InstrLabel(const InstrLabel &) = default;

	InstrLabel(InstrLabel &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrBr : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	AddrOperand * pCond;
	AddrJumpLabel * pLabelTrue, * pLabelFalse;
  public:
	//  unconditional jump
	explicit InstrBr(AddrJumpLabel * pLabel);

	InstrBr(
		AddrOperand * pCond, AddrJumpLabel * pLabelTrue, AddrJumpLabel * pLabelFalse
	);

	InstrBr(const InstrBr &) = default;

	InstrBr(InstrBr &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrRet : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	AddrOperand * retAddr;
  public:
	explicit InstrRet(AddrOperand * pAddr);

	InstrRet(const InstrRet &) = default;

	InstrRet(InstrRet &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrBinaryOp : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override = 0;

	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

  public:
	AddrOperand * left, * right, * res;

	//  Will check if three addrs have same types.
	InstrBinaryOp(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrBinaryOp(const InstrBinaryOp &) = default;

	InstrBinaryOp(InstrBinaryOp &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override = 0;
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
	InstrAdd(AddrOperand * left, AddrOperand * right, AddrOperand * res);

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
	InstrFAdd(AddrOperand * left, AddrOperand * right, AddrOperand * res);

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
	InstrSub(AddrOperand * left, AddrOperand * right, AddrOperand * res);

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
	InstrFSub(AddrOperand * left, AddrOperand * right, AddrOperand * res);

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
	InstrMul(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrMul(InstrMul &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFMul : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;
  public:
	//  Will check if the type of these addr is Float.
	InstrFMul(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrFMul(InstrFMul &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrDiv : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:

	//  Will check if the type of these addr is Int.
	InstrDiv(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrDiv(InstrDiv &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrFDiv : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;
  public:
	//  Will check if the type of these addr is Float.
	InstrFDiv(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrFDiv(InstrFDiv &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class InstrSrem : public InstrBinaryOp {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:

	//  Will check if the type of these addr is Int.
	InstrSrem(AddrOperand * left, AddrOperand * right, AddrOperand * res);

	InstrSrem(InstrSrem &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};


class InstrCall : public IRInstr {
  protected:
	// [[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	AddrFunction * func;
	std::vector<AddrOperand *> paramsPassing;
	AddrVariable * retAddr;
  public:
	InstrCall(
		AddrFunction * func, std::vector<AddrOperand *> paramsToPass,
		AddrVariable * retAddr
	);

	InstrCall(InstrCall &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;

};

class InstrGetelementptr : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	AddrVariable * to;
	AddrVariable * from;
	std::vector<AddrOperand *> idxs;
  public:
	InstrGetelementptr(
		AddrVariable * to, AddrVariable * from, std::vector<AddrOperand *> idxs
	);

	InstrGetelementptr(InstrGetelementptr &&) = default;

	std::string toLLVMIR() const override;
};

class InstrCompare : public IRInstr {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	AddrVariable * dest;
	AddrOperand * leftOp, * rightOp;

	InstrCompare(
		AddrVariable * dest, AddrOperand * leftOp, AddrOperand * rightOp,
		InstrType instrType
	);

  public:

	InstrCompare(InstrCompare &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

enum class ICMP {
	//  ref : https://llvm.org/docs/LangRef.html#other-operations
	EQ,     //  ==
	NE,     //  !=
	SGT,    //  >
	SGE,    //  >=
	SLT,    //  >
	SLE,    //  <=
	ERR,    //  For error handle
};

std::string to_string(ICMP icmp);

ICMP strToICMP(const std::string & str);

class InstrICmp : public InstrCompare {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	ICMP icmp;
  public:
	InstrICmp(
		AddrVariable * dest, AddrOperand * leftOp, ICMP icmp, AddrOperand * rightOp
	);

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

std::string to_string(FCMP fcmp);

FCMP strToFCMP(const std::string & str);

class InstrFCmp : public InstrCompare {
  protected:
	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	FCMP fcmp;
  public:
	InstrFCmp(
		AddrVariable * dest, AddrOperand * leftOp, FCMP fcmp, AddrOperand * rightOp
	);

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
}

