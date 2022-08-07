#include <numeric>
#include "TypeInfo.hpp"
#include "cprt.hpp"


namespace sup {
std::unique_ptr<moeconcept::Cloneable>
BoolType::_cloneToUniquePtr() const {
	return std::make_unique<BoolType>(*this);
}

BoolType::BoolType() : TypeInfo(Type::Bool_t) {
}

std::string BoolType::toLLVMIR() const {
	return "i1";
}

std::unique_ptr<moeconcept::Cutable> BoolType::_cutToUniquePtr() {
	return std::make_unique<BoolType>(std::move(*this));
}

int BoolType::getSize() const {
	return 1;
}

TypeInfo::TypeInfo(Type type) : type(type) {
}

bool TypeInfo::operator==(const TypeInfo & typeInfo) const {
	switch (type) {
		case Type::Unknown: {
			return false;
		}
		case Type::Int_t: {
			return typeInfo.type == Type::Int_t;
		}
		case Type::Float_t: {
			return typeInfo.type == Type::Float_t;
		}
		case Type::Bool_t: {
			return typeInfo.type == Type::Bool_t;
		}
		default: {
			com::Throw(
				"You should not invoke this override of ==, check your code!",
				CODEPOS
			);
		}
	}
}

bool TypeInfo::operator!=(const TypeInfo & typeInfo) const {
	return !operator==(typeInfo);
}

std::unique_ptr<moeconcept::Cloneable>
IntType::_cloneToUniquePtr() const {
	return std::make_unique<IntType>(*this);
}

IntType::IntType() : TypeInfo(Type::Int_t) {
}

std::string IntType::toLLVMIR() const {
	return "i32";
}

std::unique_ptr<moeconcept::Cutable> IntType::_cutToUniquePtr() {
	return std::make_unique<IntType>(std::move(*this));
}

int IntType::getSize() const {
	return 4;
}

std::unique_ptr<moeconcept::Cloneable>
FloatType::_cloneToUniquePtr() const {
	return std::make_unique<FloatType>(*this);
}

FloatType::FloatType() : TypeInfo(Type::Float_t) {
}

std::string FloatType::toLLVMIR() const {
	return "float";
}

std::unique_ptr<moeconcept::Cutable> FloatType::_cutToUniquePtr() {
	return std::make_unique<FloatType>(std::move(*this));

}

int FloatType::getSize() const {
	return 4;
}

std::unique_ptr<moeconcept::Cloneable>
IntArrayType::_cloneToUniquePtr() const {
	return std::make_unique<IntArrayType>(*this);
}

IntArrayType::IntArrayType(std::vector<int> shape)
	: TypeInfo(Type::IntArray_t), shape(std::move(shape)) {
}

std::string IntArrayType::toLLVMIR() const {
	std::string buf;
	for (int len: shape) {
		buf += to_string("[") + to_string(len) + to_string(" x ");
	}
	buf += to_string("i32");
	for (int i = int(shape.size()); i; --i) {
		buf += "]";
	}
	return buf;
}

bool
IntArrayType::operator==(const TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::IntArray_t) {
		return false;
	} else {
		return shape == (dynamic_cast<const IntArrayType &>(typeInfo).shape);
	}
}

std::unique_ptr<moeconcept::Cutable> IntArrayType::_cutToUniquePtr() {
	return std::make_unique<IntArrayType>(std::move(*this));
}

int IntArrayType::getSize() const {
	return std::accumulate(shape.begin(), shape.end(), 4, std::multiplies<>());
}


std::unique_ptr<moeconcept::Cloneable>
FloatArrayType::_cloneToUniquePtr() const {
	return std::make_unique<FloatArrayType>(*this);
}

FloatArrayType::FloatArrayType(std::vector<int> shape)
	: TypeInfo(Type::FloatArray_t), shape(std::move(shape)) {
}

std::string FloatArrayType::toLLVMIR() const {
	std::string buf;
	for (int len: shape) {
		buf += to_string("[") + to_string(len) + to_string(" x ");
	}
	buf += to_string("float");
	for (int i = int(shape.size()); i; --i) {
		buf += "]";
	}
	return buf;
}

bool
FloatArrayType::operator==(const TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::FloatArray_t) {
		return false;
	} else {
		return shape == (dynamic_cast<const FloatArrayType &>(typeInfo).shape);
	}
}

std::unique_ptr<moeconcept::Cutable> FloatArrayType::_cutToUniquePtr() {
	return std::make_unique<FloatArrayType>(std::move(*this));

}

int FloatArrayType::getSize() const {
	return std::accumulate(shape.begin(), shape.end(), 4, std::multiplies<>());
}


std::unique_ptr<moeconcept::Cloneable>
PointerType::_cloneToUniquePtr() const {
	return std::make_unique<PointerType>(*this);
}

PointerType::PointerType(const PointerType & pointerType)
	: TypeInfo(Type::Pointer_t), pointLevel(pointerType.pointLevel) {
	pointTo = com::dynamic_cast_uPtr<TypeInfo>(
		pointerType.pointTo->cloneToUniquePtr()
	);
}

PointerType::PointerType(const TypeInfo & pointToType)
	: TypeInfo(Type::Pointer_t) {
	pointTo = com::dynamic_cast_uPtr<TypeInfo>(
		pointToType.cloneToUniquePtr()
	);
	if (pointToType.type == Type::Pointer_t) {
		pointLevel =
			dynamic_cast<const PointerType &>(pointToType).pointLevel + 1;
	} else {
		pointLevel = 1;
	}
}

std::string PointerType::toLLVMIR() const {
	return pointTo->toLLVMIR() + "*";
}

bool PointerType::operator==(const TypeInfo & typeInfo) const {
	if (typeInfo.type != Type::Pointer_t) {
		return false;
	} else {
		return *pointTo ==
			*(dynamic_cast<const PointerType &>(typeInfo).pointTo);
	}
}


std::unique_ptr<moeconcept::Cutable> PointerType::_cutToUniquePtr() {
	return std::make_unique<PointerType>(std::move(*this));
}

int PointerType::getSize() const {
	return 4;
}

VoidType::VoidType() : TypeInfo(Type::Void_t) {
}

std::string VoidType::toLLVMIR() const {
	return "void";
}

int VoidType::getSize() const {
	return 0;
}

std::unique_ptr<TypeInfo> typeDeduce(const TypeInfo & _from, size_t dep) {//NOLINT
	auto type = _from.type;
	switch (type) {
		case Type::FloatArray_t: {
			const auto & from = dynamic_cast<const FloatArrayType &>(_from);
			auto dims = from.shape.size();
			if (dims == dep) {
				return std::make_unique<FloatType>();
			} else if (dims == dep + 1) {
				return std::make_unique<PointerType>(FloatType());
			} else if (dims > dep + 1) {
				auto shape = std::vector(
					from.shape.begin() + dep + 1, from.shape.end()
				);
				return std::make_unique<PointerType>(
					FloatArrayType(std::move(shape))
				);
			} else {
				com::Throw("dep should be less than dims", CODEPOS);
			}
		}
		case Type::IntArray_t: {
			const auto & from = dynamic_cast<const IntArrayType &>(_from);
			auto dims = from.shape.size();
			if (dims == dep) {
				return std::make_unique<IntType>();
			} else if (dims == dep + 1) {
				return std::make_unique<PointerType>(IntType());
			} else if (dims > dep + 1) {
				auto shape = std::vector(
					from.shape.begin() + dep + 1, from.shape.end()
				);
				return std::make_unique<PointerType>(
					IntArrayType(std::move(shape))
				);
			} else {
				com::Throw("dep should be less than dims", CODEPOS);
			}
		}
		case Type::Int_t: {
			com::Assert(
				dep == 0, "When type `from` is int, dep should be one.", CODEPOS
			);
			return std::make_unique<IntType>();
		}
		case Type::Float_t: {
			com::Assert(
				dep == 0, "When type `from` is float, dep should be one.", CODEPOS
			);
			return std::make_unique<FloatType>();
		}
		case Type::Pointer_t: {
			auto & from = dynamic_cast<const PointerType &>(_from);
			if (dep > 0) {
				return typeDeduce(*from.pointTo, dep - 1);
			} else {
				return com::dynamic_cast_uPtr<TypeInfo>(from.cloneToUniquePtr());
			}
		}
		default: {
			com::Throw("This type is not supported to deduce type.", CODEPOS);
		}
	}
}

}