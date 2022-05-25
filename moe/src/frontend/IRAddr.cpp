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

void ircode::Scope::bindDominateVar(ircode::Addr * addrVar) {
	com::TODO();
}

ircode::Addr * ircode::Scope::findIdInThisScope(const std::string & varname) const {
	if (addrMap.count(varname) == 0) {
		return nullptr;
	} else {
		return addrMap.find(varname)->second;
	}
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


ircode::TypeInfo::TypeInfo(ircode::TypeInfo::Type type) : type(type) {
}

std::unique_ptr<moeconcept::Cloneable> ircode::IntType::_cloneToUniquePtr() const {
	return std::make_unique<IntType>(*this);
}

ircode::IntType::IntType() : TypeInfo(TypeInfo::Type::Int_t) {
}

std::string ircode::IntType::toLLVMIR() const {
	return "i32";
}

std::unique_ptr<moeconcept::Cloneable> ircode::FloatType::_cloneToUniquePtr() const {
	return std::make_unique<FloatType>(*this);
}

ircode::FloatType::FloatType() : TypeInfo(TypeInfo::Type::Float_t) {
}

std::string ircode::FloatType::toLLVMIR() const {
	return "float";
}

std::unique_ptr<moeconcept::Cloneable> ircode::IntArrayType::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayType>(*this);
}

ircode::IntArrayType::IntArrayType(std::vector<int> shape)
		: TypeInfo(TypeInfo::Type::IntArray_t), shape(std::move(shape)) {
}

std::string ircode::IntArrayType::toLLVMIR() const {
	std::string buf;
	for (int len : shape) {
		buf += to_string("[") + to_string(len) + to_string(" x ");
	}
	buf += to_string("i32");
	for (int i = int(shape.size()); i; --i) {
		buf += "]";
	}
	return buf;
}


std::unique_ptr<moeconcept::Cloneable> ircode::FloatArrayType::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayType>(*this);
}

ircode::FloatArrayType::FloatArrayType(std::vector<int> shape)
		: TypeInfo(TypeInfo::Type::FloatArray_t), shape(std::move(shape)) {
}

std::string ircode::FloatArrayType::toLLVMIR() const {
	std::string buf;
	for (int len : shape) {
		buf += to_string("[") + to_string(len) + to_string(" x ");
	}
	buf += to_string("float");
	for (int i = int(shape.size()); i; --i) {
		buf += "]";
	}
	return buf;
}


std::unique_ptr<moeconcept::Cloneable> ircode::PointerType::_cloneToUniquePtr() const {
	return std::make_unique<PointerType>(*this);
}

ircode::PointerType::PointerType(const ircode::PointerType & pointerType)
		: TypeInfo(Type::Pointer_t),pointLevel(pointerType.pointLevel) {
	pointTo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,PointerType>(
			pointerType.pointTo->cloneToUniquePtr()
			);
}

ircode::PointerType::PointerType(const ircode::TypeInfo & pointToType)
		: TypeInfo(Type::Pointer_t){
	pointTo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,PointerType>(
			pointToType.cloneToUniquePtr()
	);
	if(pointToType.type==Type::Pointer_t){
		pointLevel=dynamic_cast<const PointerType&>(pointToType).pointLevel+1;
	}else{
		pointLevel=1;
	}
}

std::string ircode::PointerType::toLLVMIR() const {
	return pointTo->toLLVMIR()+"*";
}

std::unique_ptr<moeconcept::Cloneable> ircode::BoolType::_cloneToUniquePtr() const {
	return std::make_unique<BoolType>(*this);
}

ircode::BoolType::BoolType() :TypeInfo(Type::Bool_t){
}

std::string ircode::BoolType::toLLVMIR() const {
	return "i1";
}
