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


ircode::TypeInfo::TypeInfo(ircode::TypeInfo::Type type) : type(type) {
}

bool ircode::TypeInfo::operator ==(const ircode::TypeInfo & typeInfo) const {
	if (type == Type::Unknown || typeInfo.type == Type::Unknown) {
		return false;
	} else if (type == Type::Int_t && typeInfo.type == Type::Int_t) {
		return true;
	} else if (type == Type::Float_t && typeInfo.type == Type::Float_t) {
		return true;
	} else if (type == Type::Bool_t && typeInfo.type == Type::Bool_t) {
		return true;
	} else {
		com::Throw(
				"You should not invoke this override of ==, check your code!",
				CODEPOS);
	}
}

bool ircode::TypeInfo::operator !=(const ircode::TypeInfo & typeInfo) const {
	return !operator ==(typeInfo);
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntType::_cloneToUniquePtr() const {
	return std::make_unique<IntType>(*this);
}

ircode::IntType::IntType() : TypeInfo(TypeInfo::Type::Int_t) {
}

std::string ircode::IntType::toLLVMIR() const {
	return "i32";
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatType::_cloneToUniquePtr() const {
	return std::make_unique<FloatType>(*this);
}

ircode::FloatType::FloatType() : TypeInfo(TypeInfo::Type::Float_t) {
}

std::string ircode::FloatType::toLLVMIR() const {
	return "float";
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntArrayType::_cloneToUniquePtr() const {
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

bool
ircode::IntArrayType::operator ==(const ircode::TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::IntArray_t) {
		return false;
	} else {
		return shape == (dynamic_cast<const IntArrayType &>(typeInfo).shape);
	}
}


std::unique_ptr<moeconcept::Cloneable>
ircode::FloatArrayType::_cloneToUniquePtr() const {
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

bool
ircode::FloatArrayType::operator ==(const ircode::TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::FloatArray_t) {
		return false;
	} else {
		return shape == (dynamic_cast<const FloatArrayType &>(typeInfo).shape);
	}
}


std::unique_ptr<moeconcept::Cloneable>
ircode::PointerType::_cloneToUniquePtr() const {
	return std::make_unique<PointerType>(*this);
}

ircode::PointerType::PointerType(const ircode::PointerType & pointerType)
		: TypeInfo(Type::Pointer_t), pointLevel(pointerType.pointLevel) {
	pointTo = com::dynamic_cast_unique_ptr<moeconcept::Cloneable, PointerType>(
			pointerType.pointTo->cloneToUniquePtr()
	);
}

ircode::PointerType::PointerType(const ircode::TypeInfo & pointToType)
		: TypeInfo(Type::Pointer_t) {
	pointTo = com::dynamic_cast_unique_ptr<moeconcept::Cloneable, PointerType>(
			pointToType.cloneToUniquePtr()
	);
	if (pointToType.type == Type::Pointer_t) {
		pointLevel =
				dynamic_cast<const PointerType &>(pointToType).pointLevel + 1;
	} else {
		pointLevel = 1;
	}
}

std::string ircode::PointerType::toLLVMIR() const {
	return pointTo->toLLVMIR() + "*";
}

bool ircode::PointerType::operator ==(const ircode::TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::Pointer_t) {
		return false;
	} else {
		return *pointTo ==
		       *(dynamic_cast<const PointerType &>(typeInfo).pointTo);
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::BoolType::_cloneToUniquePtr() const {
	return std::make_unique<BoolType>(*this);
}

ircode::BoolType::BoolType() : TypeInfo(Type::Bool_t) {
}

std::string ircode::BoolType::toLLVMIR() const {
	return "i1";
}

ircode::StaticValue::StaticValue(const ircode::TypeInfo & typeInfo)
		: uPtrInfo(
		com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
				typeInfo.cloneToUniquePtr()
		)) {
}

ircode::StaticValue::StaticValue(const ircode::StaticValue & staticValue)
		: uPtrInfo(
		com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
				staticValue.uPtrInfo->cloneToUniquePtr())
) {
}

ircode::StaticValue::StaticValue() : uPtrInfo(nullptr) {
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatStaticValue>(*this);
}

ircode::FloatStaticValue::FloatStaticValue(const std::string & literal)
		: StaticValue(FloatType()), value(std::stof(literal)) {
}

std::string ircode::FloatStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + floatToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
			cloneToUniquePtr());
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntStaticValue>(*this);
}

ircode::IntStaticValue::IntStaticValue(const std::string & literal)
		: StaticValue(IntType()), value(std::stoi(literal)) {
}

std::string ircode::IntStaticValue::toLLVMIR() const {
	return uPtrInfo->toLLVMIR() + " " + intToString(value);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntStaticValue::getValue(const std::vector<int> &) const {
	return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
			cloneToUniquePtr());
}

std::unique_ptr<moeconcept::Cloneable>
ircode::FloatArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayStaticValue>(*this);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
		int len, std::vector<FloatStaticValue> vi
)
		: StaticValue(), shape({len}), value(std::move(vi)) {
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

ircode::FloatArrayStaticValue::FloatArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<FloatArrayStaticValue> & vi
) {
	com::Assert(len <= (int) vi.size(), "len should less than vi.size().",
	            CODEPOS);
	shape = {len};
	stlextension::vector::PushBackByIterators(shape, preShape.begin(),
	                                          preShape.end());
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(value, fsa.value.begin(),
		                                          fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) sz *= x;
	stlextension::vector::PushBackByNumberAndInstance(
			value, (len - int(vi.size()) * sz), FloatStaticValue());
	StaticValue::uPtrInfo = std::make_unique<FloatArrayType>(shape);
}

std::string ircode::FloatArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
			int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i)
			nowShape.push_back(shape[i]);
		FloatArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) sz *= shape[i];
		for (int i = 0; i < shape[dim]; ++i) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) sz *= x;
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::FloatArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
				value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::IntArrayStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayStaticValue>(*this);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
		int len, std::vector<IntStaticValue> vi
) : StaticValue(), shape({len}), value(std::move(vi)) {
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

ircode::IntArrayStaticValue::IntArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<IntArrayStaticValue> & vi
) {
	
	com::Assert(len <= (int) vi.size(), "len should less than vi.size().",
	            CODEPOS);
	shape = {len};
	stlextension::vector::PushBackByIterators(shape, preShape.begin(),
	                                          preShape.end());
	for (const auto & fsa : vi) {
		if (fsa.shape != preShape) {
			com::Throw("Shape not same.", CODEPOS);
		}
		stlextension::vector::PushBackByIterators(value, fsa.value.begin(),
		                                          fsa.value.end());
	}
	int sz = 1;
	for (int x : preShape) sz *= x;
	stlextension::vector::PushBackByNumberAndInstance(
			value, (len - int(vi.size()) * sz), IntStaticValue());
	StaticValue::uPtrInfo = std::make_unique<IntArrayType>(shape);
}

std::string ircode::IntArrayStaticValue::toLLVMIR() const {
	std::function<std::string(int, int, int)> fun = [&](
			int from, int to, int dim
	) -> std::string {
		if (dim == int(shape.size())) {
			std::string buf;
			for (int i = from; i < to; ++i) {
				buf += value[i].toLLVMIR() + ", ";
			}
			buf.pop_back();
			buf.pop_back();
			return buf;
		}
		std::vector<int> nowShape;
		for (int i = dim; i < int(shape.size()); ++i)
			nowShape.push_back(shape[i]);
		FloatArrayType fat(nowShape);
		std::string buf = fat.toLLVMIR() + " [";
		int sz = 1;
		for (int i = dim + 1; i < int(shape.size()); ++i) sz *= shape[i];
		for (int i = 0; i < shape[dim]; ++i) {
			buf += fun(from + sz * i, from + sz * (i + 1), dim + 1) + ", ";
		}
		buf.pop_back();
		buf.pop_back();
		return buf + "]";
	};
	int sz = 1;
	for (int x : shape) sz *= x;
	return fun(0, sz, 0);
}

std::unique_ptr<ircode::StaticValue>
ircode::IntArrayStaticValue::getValue(const std::vector<int> & ind) const {
	if (ind.size() == shape.size()) {
		size_t idx = 0, stride = 1;
		for (auto itInd = ind.rbegin(), itShape = shape.rbegin();
		     itInd != ind.rend(); ++itInd, ++itShape) {
			idx += *itInd * stride;
			stride *= *itShape;
		}
		return com::dynamic_cast_unique_ptr<moeconcept::Cloneable, StaticValue>(
				value[idx].cloneToUniquePtr());
	} else {
		com::TODO("Maybe do not need this.", CODEPOS);
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
    uPtrTypeInfo(com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
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
				  com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
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
	uPtrReturnTypeInfo = com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
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
		com::dynamic_cast_unique_ptr<moeconcept::Cloneable, TypeInfo>(
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
	pool.emplace_back(com::cloneable_cast_uPtr<Addr>(addr.cloneToUniquePtr()));
	pScope->bindDominateVar(name, idType, pool.rbegin()->get());
	return pool.rbegin()->get();
}

ircode::Scope * ircode::AddrPool::getRootScope() const {
	return pScopeRoot.get();
}

ircode::Addr *
ircode::AddrPool::addAddrWithoutScope(const ircode::Addr & addr) {
	pool.emplace_back(com::cloneable_cast_uPtr<Addr>(addr.cloneToUniquePtr()));
	return pool.rbegin()->get();
}


ircode::AddrVariable::AddrVariable(
		const ircode::TypeInfo & typeInfo, std::string name
) : Addr(), name(std::move(name)), isConst(false),
    uPtrStaticValue(nullptr), uPtrTypeInfo(
				com::cloneable_cast_uPtr<TypeInfo>(
						typeInfo.cloneToUniquePtr())) {
}

ircode::AddrVariable::AddrVariable(
		const ircode::TypeInfo & typeInfo, std::string name,
		const ircode::StaticValue & staticValue
)
		: Addr(), name(std::move(name)), isConst(true), uPtrStaticValue(
		com::cloneable_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())),
		  uPtrTypeInfo(com::cloneable_cast_uPtr<TypeInfo>(
				  typeInfo.cloneToUniquePtr())) {
}

ircode::AddrVariable::AddrVariable(const AddrVariable & addrVariable)
		: Addr(addrVariable), name(addrVariable.name),
		  isConst(addrVariable.isConst), uPtrStaticValue(
				com::cloneable_cast_uPtr<StaticValue>(
						addrVariable.uPtrStaticValue->cloneToUniquePtr())),
		  uPtrTypeInfo(com::cloneable_cast_uPtr<TypeInfo>(
				  addrVariable.uPtrTypeInfo->cloneToUniquePtr())) {
}

std::string ircode::AddrVariable::toLLVMIR() const {
	if (name.length()) {
		return "V." + name;
	} else {
		return "V." + std::to_string(id);
	}
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrVariable>(*this);
}

ircode::AddrGlobalVariable::AddrGlobalVariable(
		const ircode::TypeInfo & typeInfo, std::string name,
		const ircode::StaticValue & staticValue
)
		: Addr(), name(std::move(name)), isConst(true), uPtrStaticValue(
		com::cloneable_cast_uPtr<StaticValue>(staticValue.cloneToUniquePtr())),
		  uPtrTypeInfo(com::cloneable_cast_uPtr<TypeInfo>(
				  typeInfo.cloneToUniquePtr())) {
}

ircode::AddrGlobalVariable::AddrGlobalVariable(
		const ircode::TypeInfo & typeInfo, std::string name
)
		: Addr(), name(std::move(name)), isConst(false),
		  uPtrStaticValue(nullptr), uPtrTypeInfo(
				com::cloneable_cast_uPtr<TypeInfo>(
						typeInfo.cloneToUniquePtr())) {
}

ircode::AddrGlobalVariable::AddrGlobalVariable(
		const ircode::AddrGlobalVariable & addr
) : Addr(addr), name(addr.name), isConst(addr.isConst), uPtrStaticValue(
		com::cloneable_cast_uPtr<StaticValue>(
				addr.uPtrStaticValue->cloneToUniquePtr())), uPtrTypeInfo(
		com::cloneable_cast_uPtr<TypeInfo>(
				addr.uPtrTypeInfo->cloneToUniquePtr())) {
}

std::string ircode::AddrGlobalVariable::toLLVMIR() const {
	return "G." + name;
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrGlobalVariable::_cloneToUniquePtr() const {
	return std::make_unique<AddrGlobalVariable>(*this);
}

ircode::AddrStaticValue::AddrStaticValue(
		const TypeInfo & typeInfo, const StaticValue & staticValue
)
		: uPtrTypeInfo(
		com::cloneable_cast_uPtr<TypeInfo>(typeInfo.cloneToUniquePtr())),
		  uPtrStaticValue(com::cloneable_cast_uPtr<StaticValue>(
				  staticValue.cloneToUniquePtr())) {
	com::Assert(*staticValue.uPtrInfo == typeInfo);
}

ircode::AddrStaticValue::AddrStaticValue(const AddrStaticValue & addr) :
		Addr(addr),
		uPtrTypeInfo(com::cloneable_cast_uPtr<TypeInfo>(
				addr.uPtrTypeInfo->cloneToUniquePtr())),
		uPtrStaticValue(
				com::cloneable_cast_uPtr<StaticValue>(
						addr.uPtrStaticValue->cloneToUniquePtr())) {
}

std::string ircode::AddrStaticValue::toLLVMIR() const {
	return uPtrStaticValue->toLLVMIR();
}

std::unique_ptr<moeconcept::Cloneable>
ircode::AddrStaticValue::_cloneToUniquePtr() const {
	return std::make_unique<AddrStaticValue>(*this);
}
