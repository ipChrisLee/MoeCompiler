//
// Created by lee on 6/7/22.
//

#include "TypeInfo.hpp"

std::unique_ptr<moeconcept::Cloneable>
ircode::BoolType::_cloneToUniquePtr() const {
	return std::make_unique<BoolType>(*this);
}

ircode::BoolType::BoolType() : TypeInfo(Type::Bool_t) {
}

std::string ircode::BoolType::toLLVMIR() const {
	return "i1";
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
