#include <stack>
#include <utility>

#include "frontend/IRAddr.hpp"

int ircode::Scope::cnt = 0;


ircode::Scope * ircode::Scope::addSonScope() {
	sons.emplace_back(this);
	return sons.rbegin()->get();
}

ircode::Scope::Scope(ircode::Scope * pFather) : father(pFather), id(++cnt) {
}

ircode::Scope * ircode::Scope::getFather() const {
	return father;
}

ircode::Scope * ircode::Scope::getThis() {
	return this;
}

void ircode::Scope::bindDominateVar(
	const std::string & str, IdType idType, ircode::Addr * addrVar
) {
	if (addrMap.count(str)) {
		com::Throw("Same name!", CODEPOS);
	}
	addrMap[str] = std::make_tuple(idType, addrVar);
}


std::string ircode::Scope::getIdChain() const {
	const Scope * pScope = this;
	std::stack<int> idSt;
	while (pScope) {
		idSt.push(pScope->id);
		pScope = pScope->father;
	}
	std::string buf;
	while (!idSt.empty()) {
		buf = to_string("S") + to_string(idSt.top());
		idSt.pop();
	}
	return buf;
}

std::tuple<ircode::IdType, ircode::Addr *>
ircode::Scope::findIdInThisScope(const std::string & name) const {
	if (!addrMap.count(name)) {
		return std::make_tuple(IdType::Error, nullptr);
	} else {
		return addrMap.find(name)->second;
	}
}


int ircode::Addr::cnt = 0;

ircode::Addr::Addr() : id(++cnt) {
}

ircode::Addr::Addr(const ircode::Addr &) : id(++cnt) {
}

std::unique_ptr<moeconcept::Cloneable> ircode::Addr::_cloneToUniquePtr() const {
	com::Throw("This function should not be invoked!", CODEPOS);
}

ircode::AddrJumpLabel::AddrJumpLabel(std::string name) : labelName(
	std::move(name)) {
}

std::string ircode::AddrJumpLabel::toLLVMIR() const {
	std::string buf = "L." + to_string(id) + "." + labelName;
	return buf;
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrJumpLabel::_cloneToUniquePtr() const {
	return std::make_unique<AddrJumpLabel>(*this);
}

ircode::AddrPara::AddrPara(
	std::string name, const ircode::TypeInfo & typeInfo, int number
) : number(number), name(std::move(name)),
    uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(
	    typeInfo.cloneToUniquePtr())) {
}

std::string ircode::AddrPara::toLLVMIR() const {
	com::TODO("", CODEPOS);
}

ircode::AddrPara::AddrPara(const ircode::AddrPara & addrPara)
	: Addr(addrPara),
	  number(addrPara.number),
	  name(addrPara.name),
	  uPtrTypeInfo(
		  com::dynamic_cast_uPtr<TypeInfo>(
			  addrPara.uPtrTypeInfo->cloneToUniquePtr())) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrPara::_cloneToUniquePtr() const {
	return std::make_unique<AddrPara>(*this);
}

ircode::AddrFunction::AddrFunction(std::string name) : name(std::move(name)) {
}

ircode::TypeInfo *
ircode::AddrFunction::setReturnTypeInfo(const ircode::TypeInfo & typeInfo) {
	uPtrReturnTypeInfo = com::dynamic_cast_uPtr<TypeInfo>(
		typeInfo.cloneToUniquePtr());
	return uPtrReturnTypeInfo.get();
}

const ircode::TypeInfo * ircode::AddrFunction::getReturnTypeInfo() const {
	return uPtrReturnTypeInfo.get();
}

void ircode::AddrFunction::pushParameter(const ircode::AddrPara * addrPara) {
	vecPtrAddrPara.push_back(addrPara);
}

const ircode::AddrPara *
ircode::AddrFunction::getNumberThParameterTypeInfo(int number) const {
	return vecPtrAddrPara[number];
}

std::string ircode::AddrFunction::toLLVMIR() const {
	com::TODO("", CODEPOS);
}

ircode::AddrFunction::AddrFunction(const ircode::AddrFunction & addrFun)
	: Addr(addrFun), name(addrFun.name), uPtrReturnTypeInfo(
	com::dynamic_cast_uPtr<TypeInfo>(
		addrFun.uPtrReturnTypeInfo->cloneToUniquePtr())),
	  vecPtrAddrPara(addrFun.vecPtrAddrPara) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrFunction::_cloneToUniquePtr() const {
	return std::make_unique<AddrFunction>(*this);
}


ircode::AddrPool::AddrPool() : pScopeRoot(std::make_unique<Scope>(nullptr)) {
}

std::tuple<ircode::IdType, ircode::Addr *>
ircode::AddrPool::findAddrDownToRoot( /*NOLINT*/
	const ircode::Scope * pFrom, const std::string & name
) {
	if (pFrom == nullptr) {
		com::Throw("Var [" + name + "] Not Fount", CODEPOS);
	}
	auto res = pFrom->findIdInThisScope(name);
	if (std::get<1>(res) != nullptr) {
		return res;
	} else {
		return findAddrDownToRoot(pFrom->getFather(), name);
	}
	
}

ircode::Addr * ircode::AddrPool::addAddrToScope(
	const ircode::Addr & addr, ircode::Scope * pScope,
	ircode::IdType idType, const std::string & name
) {
	pool.emplace_back(com::dynamic_cast_uPtr<Addr>(addr.cloneToUniquePtr()));
	pScope->bindDominateVar(name, idType, pool.rbegin()->get());
	return pool.rbegin()->get();
}

ircode::Scope * ircode::AddrPool::getRootScope() const {
	return pScopeRoot.get();
}

ircode::Addr *
ircode::AddrPool::addAddrWithoutScope(const ircode::Addr & addr) {
	pool.emplace_back(com::dynamic_cast_uPtr<Addr>(addr.cloneToUniquePtr()));
	return pool.rbegin()->get();
}


ircode::AddrVariable::AddrVariable(
	const ircode::TypeInfo & typeInfo, std::string name
) : Addr(), name(std::move(name)), isConst(false),
    uPtrStaticValue(nullptr), uPtrTypeInfo(
		com::dynamic_cast_uPtr<TypeInfo>(
			typeInfo.cloneToUniquePtr())) {
}

ircode::AddrVariable::AddrVariable(
	const ircode::TypeInfo & typeInfo, std::string name,
	const ircode::StaticValue & staticValue
)
	: Addr(), name(std::move(name)), isConst(true), uPtrStaticValue(
	com::dynamic_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())),
	  uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(
		  typeInfo.cloneToUniquePtr())) {
	com::Assert(uPtrStaticValue->getTypeInfo() == *(uPtrTypeInfo),
	            "Type of static value and variable should be same.", CODEPOS);
}


ircode::AddrVariable::AddrVariable(const AddrVariable & addrVariable)
	: Addr(addrVariable), name(addrVariable.name),
	  isConst(addrVariable.isConst), uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(
			addrVariable.uPtrStaticValue->cloneToUniquePtr())),
	  uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(
		  addrVariable.uPtrTypeInfo->cloneToUniquePtr())) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrVariable>(*this);
}

std::string ircode::AddrVariable::toLLVMIR() const {
	if (name.length()) {
		return "@V." + name;
	} else {
		return "@V." + std::to_string(id);
	}
}

ircode::AddrGlobalVariable::AddrGlobalVariable(
	const ircode::TypeInfo & typeInfo, std::string name,
	const ircode::StaticValue & staticValue
) : Addr(), name(std::move(name)), isConst(true),
    uPtrStaticValue(
	    com::dynamic_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())
    ),
    uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())) {
	com::Assert(uPtrStaticValue->getTypeInfo() == *(uPtrTypeInfo),
	            "Type of static value and type of variable should be same.",
	            CODEPOS);
}

// ircode::AddrGlobalVariable::AddrGlobalVariable(
// 		const ircode::TypeInfo & typeInfo, std::string name
// )
// 		: Addr(), name(std::move(name)), isConst(false),
// 		  uPtrStaticValue(nullptr), uPtrTypeInfo(
// 				com::dynamic_cast_uPtr<TypeInfo>(
// 						typeInfo.cloneToUniquePtr())) {
// }

ircode::AddrGlobalVariable::AddrGlobalVariable(
	const ircode::AddrGlobalVariable & addr
) : Addr(addr), name(addr.name), isConst(addr.isConst), uPtrStaticValue(
	com::dynamic_cast_uPtr<StaticValue>(
		addr.uPtrStaticValue->cloneToUniquePtr())), uPtrTypeInfo(
	com::dynamic_cast_uPtr<TypeInfo>(
		addr.uPtrTypeInfo->cloneToUniquePtr())) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrGlobalVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrGlobalVariable>(*this);
}

std::string ircode::AddrGlobalVariable::toLLVMIR() const {
	return "@G." + name;
}

ircode::AddrStaticValue::AddrStaticValue(
	const TypeInfo & typeInfo, const StaticValue & staticValue
)
	: uPtrTypeInfo(
	com::dynamic_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())),
	  uPtrStaticValue(com::dynamic_cast_uPtr<StaticValue>(
		  staticValue.cloneToUniquePtr())) {
	com::Assert(staticValue.getTypeInfo() == typeInfo,"",CODEPOS);
}

ircode::AddrStaticValue::AddrStaticValue(const AddrStaticValue & addr) :
	Addr(addr),
	uPtrTypeInfo(com::dynamic_cast_uPtr<TypeInfo>(
		addr.uPtrTypeInfo->cloneToUniquePtr())),
	uPtrStaticValue(
		com::dynamic_cast_uPtr<StaticValue>(
			addr.uPtrStaticValue->cloneToUniquePtr())) {
}

std::string ircode::AddrStaticValue::toLLVMIR() const {
	return uPtrStaticValue->toLLVMIR();
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<AddrStaticValue>(*this);
}
