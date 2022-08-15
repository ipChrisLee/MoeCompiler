#include "mlib/common.hpp"
#include "mlib/mdb.hpp"
#include <utility>

#include "IRInstr.hpp"


using namespace sup;
namespace ircode {

int IRInstr::cnt = 0;

IRInstr::IRInstr(const IRInstr & irInstr) :
	id(++cnt), instrType(irInstr.instrType) {
	com::addRuntimeWarning(
		"You should NOT call copy constructor of IRInstr.", CODEPOS
	);
}

std::string InstrAlloca::toLLVMIR() const {
	std::string res = "\t";
	res += allocaTo->toLLVMIR() +
		" = alloca " +
		uPtrTypeToAlloca->toLLVMIR() +
		", align 4";
	return res;
}

InstrAlloca::InstrAlloca(
	AddrLocalVariable * allocaTo, const TypeInfo & typeToAlloca
) :
	IRInstr(InstrType::Alloca), allocaTo(allocaTo),
	uPtrTypeToAlloca(
		com::dynamic_cast_uPtr<TypeInfo>(typeToAlloca.cloneToUniquePtr())
	) {
	com::Assert(
		allocaTo->getType() == PointerType(typeToAlloca),
		"Type of %allocaTo should be same as typeToAlloca!", CODEPOS
	);
}

InstrAlloca::InstrAlloca(const InstrAlloca & other) :
	IRInstr(other), allocaTo(other.allocaTo),
	uPtrTypeToAlloca(
		com::dynamic_cast_uPtr<TypeInfo>(other.uPtrTypeToAlloca->cloneToUniquePtr())
	) {
}

std::unique_ptr<moeconcept::Cutable> InstrAlloca::_cutToUniquePtr() {
	return std::make_unique<InstrAlloca>(std::move(*this));
}

InstrAlloca::InstrAlloca(AddrLocalVariable * allocaTo) :
	IRInstr(InstrType::Alloca), allocaTo(allocaTo) {
	try {
		const auto
			& type = dynamic_cast<const PointerType &>(allocaTo->getType());
		uPtrTypeToAlloca =
			com::dynamic_cast_uPtr<TypeInfo>(type.pointTo->cloneToUniquePtr());
	} catch (std::bad_cast & e) {
		com::Throw("Failed cast of ref.", CODEPOS);
	}
}

void InstrAlloca::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	//  do nothing
}

AddrVariable * InstrAlloca::getDest() const {
	return allocaTo;
}

std::vector<ircode::AddrOperand *> InstrAlloca::getOperands() const {
	//  do nothing
	return { };
}

std::string InstrLabel::toLLVMIR() const {
	return pAddrLabel->toLLVMLabelName() + ":";
}

// std::unique_ptr<moeconcept::Cloneable> InstrLabel::_cloneToUniquePtr() const {
// 	return std::make_unique<InstrLabel>(*this);
// }

std::unique_ptr<moeconcept::Cutable> InstrLabel::_cutToUniquePtr() {
	return std::make_unique<InstrLabel>(std::move(*this));
}

InstrLabel::InstrLabel(AddrJumpLabel * pAddrLabel) :
	IRInstr(InstrType::Label), pAddrLabel(pAddrLabel) {
}

void InstrLabel::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
}

AddrVariable * InstrLabel::getDest() const {
	return nullptr;
}

std::vector<ircode::AddrOperand *> InstrLabel::getOperands() const {
	return { };
}

// std::unique_ptr<moeconcept::Cloneable> InstrStore::_cloneToUniquePtr() const {
// 	return std::make_unique<InstrStore>(*this);
// }

std::unique_ptr<moeconcept::Cutable> InstrStore::_cutToUniquePtr() {
	return std::make_unique<InstrStore>(std::move(*this));
}

std::string InstrStore::toLLVMIR() const {
	return "\tstore " + from->getType().toLLVMIR() + " " + from->toLLVMIR() + ", " +
		to->getType().toLLVMIR() + " " + to->toLLVMIR() + ", align 4";
}

InstrStore::InstrStore(AddrOperand * from, AddrVariable * to) :
	IRInstr(InstrType::Store), from(from), to(to) {
	//  Like `*to=from` in C.
	const auto & fromType = from->getType();
	const auto & toType = dynamic_cast<const PointerType &>(to->getType());
	com::Assert(
		*toType.pointTo == fromType,
		"Type of %to should be pointer to type of %from.", CODEPOS
	);
}

void InstrStore::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	if (chgFrom == from) {
		from = to;
	}
}

AddrVariable * InstrStore::getDest() const {
	return to;
}

std::vector<ircode::AddrOperand *> InstrStore::getOperands() const {
	return {from};
}

InstrRet::InstrRet(AddrOperand * pAddr) :
	IRInstr(InstrType::Ret), retAddr(pAddr) {
	com::Assert(
		retAddr == nullptr ||
			com::enum_fun::in(
				retAddr->getType().type, {Type::Int_t, Type::Float_t}
			), "type of return value of function should be one of int and float",
		CODEPOS
	);
}

std::unique_ptr<moeconcept::Cutable> InstrRet::_cutToUniquePtr() {
	return std::make_unique<InstrRet>(std::move(*this));
}

std::string InstrRet::toLLVMIR() const {
	if (!retAddr) {
		return "\tret void";
	} else {
		return "\tret " + retAddr->getType().toLLVMIR() + " " + retAddr->toLLVMIR();
	}
}

void InstrRet::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	if (chgFrom == retAddr) {
		retAddr = chgTo;
	}
}

AddrVariable * InstrRet::getDest() const {
	return nullptr;
}

std::vector<ircode::AddrOperand *> InstrRet::getOperands() const {
	if (retAddr) {
		return {retAddr};
	} else {
		return { };
	}
}

InstrBinaryOp::InstrBinaryOp(
	AddrOperand * left, AddrOperand * right, AddrVariable * res,
	InstrType instrType
) :
	IRInstr(instrType), left(left), right(right), res(res) {
	com::Assert(
		left->getType() == right->getType() && right->getType() == res->getType(),
		"Type of operand and result should be same!",
		CODEPOS
	);
}

void InstrBinaryOp::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	if (left == chgFrom) {
		left = chgTo;
	}
	if (right == chgFrom) {
		right = chgTo;
	}
}

AddrVariable * InstrBinaryOp::getDest() const {
	return res;
}

std::vector<ircode::AddrOperand *> InstrBinaryOp::getOperands() const {
	return {left, right};
}

InstrAdd::InstrAdd(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::Add) {
	com::Assert(
		left->getType() == IntType(),
		"Type of addr in `add` should be `IntType`!", CODEPOS
	);
}

std::string InstrAdd::toLLVMIR() const {
	return "\t" +
		res->toLLVMIR() + " = add i32 " +
		left->toLLVMIR() + ", " + right->toLLVMIR();
}

// std::unique_ptr<moeconcept::Cloneable> InstrAdd::_cloneToUniquePtr() const {
// 	return std::make_unique<InstrAdd>(*this);
// }

std::unique_ptr<moeconcept::Cutable> InstrAdd::_cutToUniquePtr() {
	return std::make_unique<InstrAdd>(std::move(*this));
}

// std::unique_ptr<moeconcept::Cloneable> InstrFAdd::_cloneToUniquePtr() const {
// 	return std::make_unique<InstrFAdd>(*this);
// }

std::unique_ptr<moeconcept::Cutable> InstrFAdd::_cutToUniquePtr() {
	return std::make_unique<InstrFAdd>(std::move(*this));
}

InstrFAdd::InstrFAdd(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::FAdd) {
	com::Assert(
		left->getType() == FloatType(),
		"Type of addr in `fadd` should be `FloatType`!", CODEPOS
	);

}

std::string InstrFAdd::toLLVMIR() const {
	return "\t" + res->toLLVMIR() +
		" = fadd float " + left->toLLVMIR() + ", " + right->toLLVMIR();
}


InstrConversionOp::InstrConversionOp(
	AddrOperand * from, AddrVariable * to, InstrType instrType
) :
	IRInstr(instrType), from(from), to(to) {
}

void
InstrConversionOp::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	if (from == chgFrom) {
		from = chgTo;
	}
}

AddrVariable * InstrConversionOp::getDest() const {
	return to;
}

std::vector<ircode::AddrOperand *> InstrConversionOp::getOperands() const {
	return {from};
}

std::unique_ptr<moeconcept::Cutable> InstrSitofp::_cutToUniquePtr() {
	return std::make_unique<InstrSitofp>(std::move(*this));
}

std::string InstrSitofp::toLLVMIR() const {
	return
		"\t" +
			to->toLLVMIR() + " = sitofp " + from->getType().toLLVMIR() + " " +
			from->toLLVMIR() + " to " + to->getType().toLLVMIR();
}

InstrSitofp::InstrSitofp(AddrOperand * from, AddrVariable * to) :
	InstrConversionOp(from, to, InstrType::Sitofp) {
	com::Assert(
		from->getType().type == Type::Int_t &&
			to->getType().type == Type::Float_t,
		"Type of `from` should be `int` and type of `to` should be `float`.",
		CODEPOS
	);
}

std::unique_ptr<moeconcept::Cutable> InstrLoad::_cutToUniquePtr() {
	return std::make_unique<InstrLoad>(std::move(*this));
}

InstrLoad::InstrLoad(AddrVariable * from, AddrVariable * to) :
	IRInstr(InstrType::Load), from(from), to(to) {
	//  Like `to=*from` in C;
	const auto & fromType = dynamic_cast<const PointerType &>(from->getType());
	const auto & toType = to->getType();
	com::Assert(
		toType == *fromType.pointTo,
		"Type of %to should be pointer to type of %from.", CODEPOS
	);
	com::Assert(
		to->addrType == AddrType::Var,
		"", CODEPOS
	);
}

std::string InstrLoad::toLLVMIR() const {
	return "\t" + to->toLLVMIR() + " = load " + to->getType().toLLVMIR() + ", "
		+ from->getType().toLLVMIR() + " " + from->toLLVMIR() + ", align 4";
}

void InstrLoad::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	// ?
}

AddrVariable * InstrLoad::getDest() const {
	return to;
}

std::vector<ircode::AddrOperand *> InstrLoad::getOperands() const {
	// ?
	return { };
}

std::string InstrBr::toLLVMIR() const {
	if (pCond) {
		return
			"\tbr " + pCond->getType().toLLVMIR() + " " + pCond->toLLVMIR() +
				", label " + pLabelTrue->toLLVMIR() +
				", label " + pLabelFalse->toLLVMIR();
	} else {
		return "\tbr label " + pLabelTrue->toLLVMIR();
	}
}

InstrBr::InstrBr(AddrJumpLabel * pLabel) :
	IRInstr(InstrType::Br),
	pCond(nullptr),
	pLabelTrue(pLabel),
	pLabelFalse(nullptr) {
	com::Assert(pLabel, "pLabelTrue should not be nullptr.", CODEPOS);
}

InstrBr::InstrBr(
	AddrOperand * pCond, AddrJumpLabel * pLabelTrue, AddrJumpLabel * pLabelFalse
) :
	IRInstr(InstrType::Br),
	pCond(pCond), pLabelTrue(pLabelTrue), pLabelFalse(pLabelFalse) {
	com::Assert(
		pCond && pLabelTrue && pLabelFalse, "label and cond should not be nullptr",
		CODEPOS
	);
	com::Assert(
		pCond->getType().type == Type::Bool_t, "cond should be bool.", CODEPOS
	);
}

void InstrBr::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
}

AddrVariable * InstrBr::getDest() const {
	return nullptr;
}

std::vector<ircode::AddrOperand *> InstrBr::getOperands() const {
	return { };
}

InstrMul::InstrMul(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::Mul) {
	com::Assert(
		left->getType() == IntType(),
		"Type of addr in `mul` should be `IntType`!", CODEPOS
	);
}

std::string InstrMul::toLLVMIR() const {
	return "\t" +
		res->toLLVMIR() + " = mul i32 " +
		left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrFMul::InstrFMul(AddrOperand * left, AddrOperand * right, AddrVariable * res)
	:
	InstrBinaryOp(left, right, res, InstrType::FMul) {
	com::Assert(
		left->getType() == FloatType(),
		"Type of addr in `fmul` should be `FloatType`!", CODEPOS
	);
}

std::string InstrFMul::toLLVMIR() const {
	return "\t" + res->toLLVMIR() +
		" = fmul float " + left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrSub::InstrSub(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::Sub) {
	com::Assert(
		left->getType() == IntType(),
		"Type of addr in `sub` should be `IntType`!", CODEPOS
	);
}

std::string InstrSub::toLLVMIR() const {
	return "\t" +
		res->toLLVMIR() + " = sub i32 " +
		left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrFSub::InstrFSub(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::FSub) {
	com::Assert(
		left->getType() == FloatType(),
		"Type of addr in `fsub` should be `FloatType`!", CODEPOS
	);
}

std::string InstrFSub::toLLVMIR() const {
	return "\t" + res->toLLVMIR() +
		" = fsub float " + left->toLLVMIR() + ", " + right->toLLVMIR();
}


std::string InstrCall::toLLVMIR() const {
	//  int f(x,y) -> %call = call arm_aapcscc i32 %f(i32 %V1.x, i32 %V2.y), align 4
	//  int putint() -> %call = call arm_aapcscc i32 bitcase (i32 (...)* @getint to i32 ()*)()
	auto res = std::string("\t");
	if (retAddr) {
		res += retAddr->toLLVMIR() + " = "; // %call = call arm_aapcscc
	}
	res += "call ";
//	res += "arm_aapcscc ";
	res += func->getReturnTypeInfo().toLLVMIR() + " " + func->toLLVMIR() + "(";
	for (auto pParaAddrToPass: paramsPassing) {
		res += pParaAddrToPass->getType().toLLVMIR() + " " +
			pParaAddrToPass->toLLVMIR() + ", "; // i32 %V1.x,
	}
	if (!paramsPassing.empty()) {
		res.pop_back();
		res.pop_back();
	}
	res += ")";
	return res;
}

InstrCall::InstrCall(
	AddrFunction * func, std::vector<AddrOperand *> paramsToPass,
	AddrVariable * retAddr
) :
	IRInstr(InstrType::Call),
	func(func), paramsPassing(std::move(paramsToPass)), retAddr(retAddr) {
	com::Assert(
		int(func->getNumberOfParameter()) == int(paramsPassing.size()), "Same Size!",
		CODEPOS
	);
	for (int i = 0; i < func->getNumberOfParameter(); ++i) {
		com::Assert(
			func->getNumberThParameterTypeInfo(i) ==
				*typeDeduce(paramsPassing[i]->getType(), 0),
			"Same Type!", CODEPOS
		);
	}
	com::Assert(
		!retAddr || func->getReturnTypeInfo() == retAddr->getType(),
		"Same Return Type!", CODEPOS
	);
}

void InstrCall::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	for (auto & p: paramsPassing) {
		if (p == chgFrom) {
			p = chgTo;
		}
	}

}

AddrVariable * InstrCall::getDest() const {
	return retAddr;
}

std::vector<ircode::AddrOperand *> InstrCall::getOperands() const {
	return paramsPassing;
}


InstrFptosi::InstrFptosi(AddrOperand * from, AddrVariable * to) :
	InstrConversionOp(from, to, InstrType::Fptosi) {
	com::Assert(
		from->getType().type == Type::Float_t &&
			to->getType().type == Type::Int_t,
		"Type of `from` should be `float` and type of `to` should be `int`.",
		CODEPOS
	);
}

std::string InstrFptosi::toLLVMIR() const {
	return
		"\t" +
			to->toLLVMIR() + " = fptosi " + from->getType().toLLVMIR() + " " +
			from->toLLVMIR() + " to " + to->getType().toLLVMIR();
}

InstrSDiv::InstrSDiv(AddrOperand * left, AddrOperand * right, AddrVariable * res)
	:
	InstrBinaryOp(left, right, res, InstrType::SDiv) {
	com::Assert(
		left->getType() == IntType(),
		"Type of addr in `div` should be `IntType`!", CODEPOS
	);
}

std::string InstrSDiv::toLLVMIR() const {
	return "\t" +
		res->toLLVMIR() + " = sdiv i32 " +
		left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrFDiv::InstrFDiv(AddrOperand * left, AddrOperand * right, AddrVariable * res) :
	InstrBinaryOp(left, right, res, InstrType::FDiv) {
	com::Assert(
		left->getType() == FloatType(),
		"Type of addr in `fdiv` should be `FloatType`!", CODEPOS
	);
}

std::string InstrFDiv::toLLVMIR() const {
	return "\t" + res->toLLVMIR() +
		" = fdiv float " + left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrSrem::InstrSrem(AddrOperand * left, AddrOperand * right, AddrVariable * res)
	:
	InstrBinaryOp(left, right, res, InstrType::SRem) {
	com::Assert(
		left->getType() == IntType(),
		"Type of addr in `rem` should be `IntType`!", CODEPOS
	);
}

std::string InstrSrem::toLLVMIR() const {
	return "\t" +
		res->toLLVMIR() + " = srem i32 " +
		left->toLLVMIR() + ", " + right->toLLVMIR();
}

InstrGetelementptr::InstrGetelementptr(
	AddrVariable * to, AddrVariable * from, std::vector<AddrOperand *> _idxs
) :
	IRInstr(InstrType::Getelementptr),
	to(to), from(from), idxs(std::move(_idxs)) {
	try {
		auto && _a = dynamic_cast<const PointerType &>(from->getType());
		auto && _b = dynamic_cast<const PointerType &>(to->getType());
		com::addRuntimeWarning(
			"Upper two statements may be optimized under -O2.", CODEPOS, true
		);
	} catch (std::bad_cast &) {
		com::Throw("`from` and `to` should be pointer!", CODEPOS);
	}
	for (auto addr: idxs) {
		com::Assert(
			addr->getType().type == Type::Int_t,
			"addr of idx should be int value.", CODEPOS
		);
	}
	com::addRuntimeWarning(
		"Consider type checking of `getelementptr`.", CODEPOS, true
	);
}

std::string InstrGetelementptr::toLLVMIR() const {
	std::string res =
		"\t" +
			to->toLLVMIR() + " = getelementptr " +
			dynamic_cast<const PointerType &>(from->getType()).pointTo->toLLVMIR() +
			", " + from->getType().toLLVMIR() + " " + from->toLLVMIR();
	for (auto * addr: idxs) {
		res += ", " + addr->getType().toLLVMIR() + " " + addr->toLLVMIR();
	}
	return res;
}

void
InstrGetelementptr::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	for (auto & p: idxs) {
		if (p == chgFrom) {
			p = chgTo;
		}
	}
}

AddrVariable * InstrGetelementptr::getDest() const {
	return to;
}

std::vector<ircode::AddrOperand *> InstrGetelementptr::getOperands() const {
	return idxs;
}

InstrCompare::InstrCompare(
	AddrVariable * dest, AddrOperand * leftOp, AddrOperand * rightOp,
	InstrType instrType
) : IRInstr(instrType),
    dest(dest), leftOp(leftOp), rightOp(rightOp) {
	com::Assert(
		leftOp->getType() == rightOp->getType(),
		"Operands of instr compare should have same type.", CODEPOS
	);
	com::Assert(
		dest->getType().type == Type::Bool_t,
		"dest should be bool in compare instruction.", CODEPOS
	);
}

std::string InstrCompare::toLLVMIR() const {
	com::Throw("This method should not be called.", CODEPOS);
}

void InstrCompare::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	if (leftOp == chgFrom) {
		leftOp = chgTo;
	}
	if (rightOp == chgFrom) {
		rightOp = chgTo;
	}
}

AddrVariable * InstrCompare::getDest() const {
	return dest;
}

std::vector<ircode::AddrOperand *> InstrCompare::getOperands() const {
	return {leftOp, rightOp};
}


ICMP getReverse(ICMP icmp) {
	switch (icmp) {
		case ICMP::EQ:return ICMP::NE;
		case ICMP::NE:return ICMP::EQ;
		case ICMP::SGT:return ICMP::SLE;
		case ICMP::SLT:return ICMP::SGE;
		case ICMP::SLE:return ICMP::SGT;
		case ICMP::SGE:return ICMP::SLT;
		case ICMP::ERR:return ICMP::ERR;
	}
}

FCMP getReverse(FCMP fcmp) {
	com::TODO("", CODEPOS);
}

std::string to_string(ICMP icmp) {
	switch (icmp) {
		case ICMP::EQ: return "eq";
		case ICMP::NE: return "ne";
		case ICMP::SGT: return "sgt";
		case ICMP::SGE: return "sge";
		case ICMP::SLT: return "slt";
		case ICMP::SLE: return "sle";
		case ICMP::ERR: return "err";
	}
}

InstrICmp::InstrICmp(
	AddrVariable * dest, AddrOperand * leftOp, ICMP icmp, AddrOperand * rightOp
) : InstrCompare(dest, leftOp, rightOp, InstrType::ICmp), icmp(icmp) {
	com::Assert(
		com::enum_fun::in(leftOp->getType().type, {Type::Int_t, Type::Bool_t}), "",
		CODEPOS
	);
}

std::string InstrICmp::toLLVMIR() const {
	return "\t" +
		dest->toLLVMIR() + " = icmp " + to_string(icmp) + " " +
		leftOp->getType().toLLVMIR() + " " +
		leftOp->toLLVMIR() + ", " + rightOp->toLLVMIR();
}

std::string to_string(FCMP fcmp) {
	switch (fcmp) {
		case FCMP::OEQ: return "oeq";
		case FCMP::OGT: return "ogt";
		case FCMP::OGE: return "oge";
		case FCMP::OLT: return "olt";
		case FCMP::OLE: return "ole";
		case FCMP::UNE: return "une";
		case FCMP::ERR: return "err";
	}
}

InstrFCmp::InstrFCmp(
	AddrVariable * dest, AddrOperand * leftOp, FCMP fcmp, AddrOperand * rightOp
) : InstrCompare(dest, leftOp, rightOp, InstrType::FCmp), fcmp(fcmp) {
	com::Assert(leftOp->getType().type == Type::Float_t, "", CODEPOS);
	com::Assert(fcmp != FCMP::ERR, "", CODEPOS);
}

std::string InstrFCmp::toLLVMIR() const {
	return "\t" +
		dest->toLLVMIR() + " = fcmp " + to_string(fcmp) + " " +
		leftOp->getType().toLLVMIR() + " " +
		leftOp->toLLVMIR() + ", " + rightOp->toLLVMIR();
}

ICMP strToICMP(const std::string & str) {
	ICMP icmp = ICMP::ERR;
	if (str == "<") {
		icmp = ICMP::SLT;
	} else if (str == "<=") {
		icmp = ICMP::SLE;
	} else if (str == ">") {
		icmp = ICMP::SGT;
	} else if (str == ">=") {
		icmp = ICMP::SGE;
	} else if (str == "==") {
		icmp = ICMP::EQ;
	} else if (str == "!=") {
		icmp = ICMP::NE;
	}
	return icmp;
}

FCMP strToFCMP(const std::string & str) {
	FCMP fcmp = FCMP::ERR;
	if (str == "<") {
		fcmp = FCMP::OLT;
	} else if (str == "<=") {
		fcmp = FCMP::OLE;
	} else if (str == ">") {
		fcmp = FCMP::OGT;
	} else if (str == ">=") {
		fcmp = FCMP::OGE;
	} else if (str == "==") {
		fcmp = FCMP::OEQ;
	} else if (str == "!=") {
		fcmp = FCMP::UNE;
	}
	return fcmp;
}

bool isTerminalInstr(InstrType instrType) {
	return com::enum_fun::in(
		instrType, {InstrType::Br, InstrType::Ret}
	);
}

InstrSExt::InstrSExt(AddrOperand * from, AddrVariable * to) :
	InstrConversionOp(from, to, InstrType::SExt) {
	com::Assert(
		from->getType().type == Type::Bool_t && to->getType().type == Type::Int_t,
		"", CODEPOS
	);

}

std::string InstrSExt::toLLVMIR() const {
	return "\t" +
		to->toLLVMIR() + " = sext " + from->getType().toLLVMIR() + " " +
		from->toLLVMIR() + " to " + to->getType().toLLVMIR();
}

InstrZExt::InstrZExt(AddrOperand * from, AddrVariable * to) :
	InstrConversionOp(from, to, InstrType::ZExt) {
	com::Assert(
		from->getType().type == Type::Bool_t && to->getType().type == Type::Int_t,
		"", CODEPOS
	);
}

std::string InstrZExt::toLLVMIR() const {
	return "\t" +
		to->toLLVMIR() + " = zext " + from->getType().toLLVMIR() + " " +
		from->toLLVMIR() + " to " + to->getType().toLLVMIR();
}

InstrPhi::InstrPhi(ircode::AddrVariable * newDefVar) :
	IRInstr(InstrType::Phi), newDefVar(newDefVar) {
}

std::string InstrPhi::toLLVMIR() const {
	auto res = std::string("\t");
	res += newDefVar->toLLVMIR() + " = phi " + newDefVar->getType().toLLVMIR() + " ";
	com::Assert(!vecPair.empty(), "", CODEPOS);
	for (auto [pLabel, pOpnd]: vecPair) {
		res += "[ " + pOpnd->toLLVMIR() + ", " + pLabel->toLLVMIR() + " ], ";
	}
	res.pop_back();
	res.pop_back();
	return res;
}

void InstrPhi::insertPair(ircode::AddrJumpLabel * pLabel, ircode::AddrOperand * pOperandAddr) {
	vecPair.emplace(pLabel, pOperandAddr);
}

void InstrPhi::changeOperand(ircode::AddrOperand * chgFrom, ircode::AddrOperand * chgTo) {
	for (auto & p: vecPair) {
		if (p.second == chgFrom) {
			p.second = chgTo;
		}
	}
}

AddrVariable * InstrPhi::getDest() const {
	return newDefVar;
}

std::vector<ircode::AddrOperand *> InstrPhi::getOperands() const {
	auto res = std::vector<ircode::AddrOperand *>();
	for (auto p: vecPair) {
		res.emplace_back(p.second);
	}
	return res;
}
}

