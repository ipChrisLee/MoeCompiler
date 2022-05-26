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

ircode::Addr *
ircode::Scope::findIdInThisScope(const std::string & varname) const {
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

int ircode::Addr::cnt=0;

ircode::Addr::Addr() {

}
