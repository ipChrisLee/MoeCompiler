#pragma once

#include <memory>

#include "moeconcept.hpp"

#include "frontend/frontendHeader.hpp"

namespace ircode {
class TypeInfo : public LLVMable, public moeconcept::Cloneable {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override = 0;
  
  public:
	enum class Type {
		Float_t, Int_t, FloatArray_t, IntArray_t, Pointer_t, Unknown, Bool_t
	} type;
	
	virtual bool operator ==(const TypeInfo & typeInfo) const;
	
	virtual bool operator !=(const TypeInfo & typeInfo) const;
	
	explicit TypeInfo(Type type);
};

class IntType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	explicit IntType();
	
	IntType(const IntType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class FloatType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	explicit FloatType();
	
	FloatType(const FloatType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class IntArrayType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	
	explicit IntArrayType(std::vector<int> shape);
	
	IntArrayType(const IntArrayType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	bool operator ==(const TypeInfo & typeInfo) const override;
};

class FloatArrayType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	
	explicit FloatArrayType(std::vector<int> shape);
	
	FloatArrayType(const FloatArrayType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	bool operator ==(const TypeInfo & typeInfo) const override;
};

class PointerType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::unique_ptr<TypeInfo> pointTo; // This should not be a `PointerType`.
	int pointLevel;
	
	PointerType(const ircode::PointerType & pointerType);
	
	explicit PointerType(const TypeInfo & pointToType);
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	bool operator ==(const TypeInfo & typeInfo) const override;
};

class BoolType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	explicit BoolType();
	
	BoolType(const BoolType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};
}