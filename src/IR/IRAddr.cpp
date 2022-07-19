#include <stack>
#include <utility>

#include "IRAddr.hpp"


using namespace sup;

namespace ircode {

int IRAddr::cnt = 0;

IRAddr::IRAddr() :
	id(++cnt) {
}

IRAddr::IRAddr(const IRAddr &) :
	id(++cnt) {
}

const TypeInfo & IRAddr::getType() const {
	com::Throw(
		com::concatToString(
			{
				"This addr [", typeid(*this).name(),
				"] has no type info."
			}
		), CODEPOS
	);
}

AddrJumpLabel::AddrJumpLabel(std::string name) :
	labelName(std::move(name)) {
}

std::string AddrJumpLabel::toLLVMIR() const {
	std::string buf = "%L" + to_string(id) + "." + labelName;
	return buf;
}

std::unique_ptr<moeconcept::Cloneable>
AddrJumpLabel::_cloneToUniquePtr() const {
	return std::make_unique<AddrJumpLabel>(*this);
}

std::unique_ptr<moeconcept::Cutable> AddrJumpLabel::_cutToUniquePtr() {
	return std::make_unique<AddrJumpLabel>(std::move(*this));
}

std::string AddrJumpLabel::toLLVMLabelName() const {
	return "L" + to_string(id) + "." + labelName;
}

AddrPara::AddrPara(
	const TypeInfo & typeInfo, std::string name
) :
	AddrVariable(typeInfo, std::move(name)) {
}

std::string AddrPara::toLLVMIR() const {
	return "%P" + to_string(id) + "." + name;
}

std::unique_ptr<moeconcept::Cloneable>
AddrPara::_cloneToUniquePtr() const {
	return std::make_unique<AddrPara>(*this);
}

std::unique_ptr<moeconcept::Cutable> AddrPara::_cutToUniquePtr() {
	return std::make_unique<AddrPara>(std::move(*this));
}

AddrFunction::AddrFunction(std::string name)
	:
	uPtrReturnTypeInfo(std::make_unique<VoidType>()), name(std::move(name)) {
}

AddrFunction::AddrFunction(
	std::string name, std::vector<AddrPara *> vecPara
) :
	uPtrReturnTypeInfo(std::make_unique<VoidType>()),
	vecPtrAddrPara(std::move(vecPara)),
	name(std::move(name)) {
}

const TypeInfo & AddrFunction::getReturnTypeInfo() const {
	return *uPtrReturnTypeInfo;
}

void AddrFunction::pushParameter(AddrPara * addrPara) {
	vecPtrAddrPara.push_back(addrPara);
}

std::string AddrFunction::toLLVMIR() const {
	auto res = std::string();
	if (vecPtrAddrPara.empty()) {
		if (justDeclare) {
			res = "bitcast (" + uPtrReturnTypeInfo->toLLVMIR() + " (...)* @" + name +
				" to " + uPtrReturnTypeInfo->toLLVMIR() + " ()*)";
		} else {
			res = "@F." + name;
		}
	} else {
		if (justDeclare) {
			res = "@" + name;
		} else {
			res = "@F." + name;
		}
	}
	return res;
}

std::string AddrFunction::declLLVMIR() const {
	auto res = std::string();
	if (justDeclare) {
		res += "declare ";
	} else {
		res += "define ";
	}
	res += "dso_local " + uPtrReturnTypeInfo->toLLVMIR();
	if (justDeclare) {
		res += " @" + name + "(";
	} else {
		res += " @F." + name + "(";
	}
	if (justDeclare) {
		if (!vecPtrAddrPara.empty()) {
			for (auto * p: vecPtrAddrPara) {
				res += p->getType().toLLVMIR() + ", ";
			}
		} else {
			res += "...";
		}
	} else {
		for (auto p: vecPtrAddrPara) {
			res += p->getType().toLLVMIR() + " " + p->toLLVMIR() + ", ";
		}
	}
	if (!vecPtrAddrPara.empty()) {
		res.pop_back();
		res.pop_back();
	}
	res += ")";
	return res;
}

AddrFunction::AddrFunction(const AddrFunction & addrFun)
	:
	IRAddr(addrFun),
	uPtrReturnTypeInfo(
		com::dynamic_cast_uPtr<TypeInfo>(
			addrFun.uPtrReturnTypeInfo->cloneToUniquePtr())
	), vecPtrAddrPara(addrFun.vecPtrAddrPara),
	name(addrFun.name) {
}

std::unique_ptr<moeconcept::Cloneable>
AddrFunction::_cloneToUniquePtr() const {
	return std::make_unique<AddrFunction>(*this);
}

AddrFunction::AddrFunction(
	std::string name, std::vector<AddrPara *> vecPara,
	const TypeInfo & retType
) :
	uPtrReturnTypeInfo(
		com::dynamic_cast_uPtr<TypeInfo>(retType.cloneToUniquePtr())),
	vecPtrAddrPara(std::move(vecPara)),
	name(std::move(name)) {
}

AddrFunction::AddrFunction(
	std::string name, const TypeInfo & retType
) :
	uPtrReturnTypeInfo(
		com::dynamic_cast_uPtr<TypeInfo>(retType.cloneToUniquePtr())),
	name(std::move(name)) {
}

std::unique_ptr<moeconcept::Cutable> AddrFunction::_cutToUniquePtr() {
	return std::make_unique<AddrFunction>(std::move(*this));
}

const TypeInfo & AddrFunction::getNumberThParameterTypeInfo(int i) const {
	return vecPtrAddrPara[i]->getType();
}

int AddrFunction::getNumberOfParameter() const {
	return int(vecPtrAddrPara.size());
}

AddrVariable::AddrVariable(
	const TypeInfo & typeInfo, std::string name
) :
	AddrOperand(typeInfo),
	name(std::move(name)) {
}

std::unique_ptr<moeconcept::Cloneable>
AddrVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrVariable>(*this);
}

std::string AddrVariable::toLLVMIR() const {
	return "%T" + to_string(id) + (name.empty() ? "" : "." + name);
}

AddrVariable::AddrVariable(const AddrPara & addrPara) :
	AddrOperand(PointerType(addrPara.getType())),
	name(addrPara.name) {
}

std::unique_ptr<moeconcept::Cutable> AddrVariable::_cutToUniquePtr() {
	return std::make_unique<AddrVariable>(std::move(*this));
}

AddrVariable::AddrVariable(sup::Type type, std::string _name) :
	AddrOperand(
		[type]() -> std::unique_ptr<TypeInfo> {
			switch (type) {
				case Type::Int_t: return std::make_unique<IntType>();
				case Type::Float_t: return std::make_unique<FloatType>();
				case Type::Bool_t: return std::make_unique<BoolType>();
				default:
					com::Throw(
						"`type` should be one of {`Type::Int_t`, `Type::Float_t`, `Type::Bool_t`}.",
						CODEPOS
					);
			}
		}()
	),
	name(std::move(_name)) {
}

AddrGlobalVariable::AddrGlobalVariable(
	const TypeInfo & typeInfo, std::string name,
	const StaticValue & staticValue, bool isConst
) :
	AddrVariable(
		PointerType(typeInfo),
		std::move(name)
	),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(
			staticValue
				.cloneToUniquePtr())
	),
	isConst(isConst) {
	com::Assert(
		staticValue.getType() == typeInfo,
		"Type of static value and type of variable should be same.",
		CODEPOS
	);
}

AddrGlobalVariable::AddrGlobalVariable(
	const TypeInfo & typeInfo, std::string name, bool isConst
) :
	AddrVariable(
		PointerType(typeInfo),
		std::move(name)
	),
	uPtrStaticValue(
		zeroExtensionValueOfType(typeInfo)
	),
	isConst(isConst) {
}

AddrGlobalVariable::AddrGlobalVariable(const AddrGlobalVariable & addr) :
	AddrVariable(addr),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(addr.uPtrStaticValue->cloneToUniquePtr())
	) {
}

std::unique_ptr<moeconcept::Cloneable>
AddrGlobalVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrGlobalVariable>(*this);
}

std::string AddrGlobalVariable::toLLVMIR() const {
	return "@G" + to_string(id) + "." + name;
}

std::string AddrGlobalVariable::toDeclIR() const {
	return toLLVMIR() +
		" = dso_local " +
		std::string(isConstVar() ? "constant " : "global ") +
		uPtrStaticValue->getType().toLLVMIR() + " " +
		uPtrStaticValue->toLLVMIR() +
		", align 4";
}

std::unique_ptr<moeconcept::Cutable> AddrGlobalVariable::_cutToUniquePtr() {
	return std::make_unique<AddrGlobalVariable>(std::move(*this));
}

const sup::StaticValue & AddrGlobalVariable::getStaticValue() const {
	return *uPtrStaticValue;
}

AddrStaticValue::AddrStaticValue(
	const TypeInfo & typeInfo, const StaticValue & staticValue
) :
	AddrOperand(typeInfo),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())
	) {
	com::Assert(staticValue.getType() == typeInfo, "", CODEPOS);
}

AddrStaticValue::AddrStaticValue(const AddrStaticValue & addr) :
	AddrOperand(addr),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(addr.uPtrStaticValue->cloneToUniquePtr())
	) {
}

std::string AddrStaticValue::toLLVMIR() const {
	return uPtrStaticValue->toLLVMIR();
}

std::unique_ptr<moeconcept::Cloneable>
AddrStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<AddrStaticValue>(*this);
}

std::unique_ptr<moeconcept::Cutable> AddrStaticValue::_cutToUniquePtr() {
	return std::make_unique<AddrStaticValue>(std::move(*this));
}

AddrStaticValue::AddrStaticValue(const StaticValue & staticValue) :
	AddrOperand(staticValue.getType()),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())
	) {
}

AddrStaticValue::AddrStaticValue(const TypeInfo & typeInfo) :
	AddrOperand(typeInfo),
	uPtrStaticValue(zeroExtensionValueOfType(typeInfo)) {
}

AddrStaticValue::AddrStaticValue(std::unique_ptr<sup::StaticValue> && up) :
	AddrOperand(up->getType()), uPtrStaticValue(std::move(up)) {
}

AddrOperand::AddrOperand(const AddrOperand & addr) :
	IRAddr(addr),
	uPtrTypeInfo(
		com::dynamic_cast_uPtr<TypeInfo>(addr.uPtrTypeInfo->cloneToUniquePtr())
	) {
}

AddrOperand::AddrOperand(const TypeInfo & typeInfo) :
	uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())) {
}

AddrOperand::AddrOperand(std::unique_ptr<sup::TypeInfo> && uPtrTypeInfo) :
	uPtrTypeInfo(std::move(uPtrTypeInfo)) {
}

AddrLocalVariable::AddrLocalVariable(
	const TypeInfo & typeInfo, std::string name
) :
	AddrVariable(PointerType(typeInfo), std::move(name)),
	isConst(false), uPtrStaticValue(nullptr) {
}

std::string AddrLocalVariable::toLLVMIR() const {
	return "%V" + to_string(id) + "." + name + (isConstVar() ? ".C" : "");
}

AddrLocalVariable::AddrLocalVariable(const AddrLocalVariable & addr) :
	AddrVariable(addr),
	isConst(addr.isConst),
	uPtrStaticValue(com::dynamic_cast_uPtr<StaticValue>(addr.cloneToUniquePtr())) {
}

AddrLocalVariable::AddrLocalVariable(
	const TypeInfo & typeInfo, std::string name, const StaticValue & staticValue
) :
	AddrVariable(PointerType(typeInfo), std::move(name)),
	isConst(true),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())
	) {
}

const sup::StaticValue & AddrLocalVariable::getStaticValue() const {
	return *uPtrStaticValue;
}

}