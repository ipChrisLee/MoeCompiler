#pragma once

#include <memory>

#include "mlib/moeconcept.hpp"

#include "SupportFunc.hpp"


namespace sup {
enum class Type {
	Float_t,
	Int_t,
	FloatArray_t,
	IntArray_t,
	Pointer_t,
	Unknown,
	Bool_t,
	Void_t,
	Any_t
};

class TypeInfo
	: public LLVMable,
	  public moeconcept::Cloneable,
	  public moeconcept::Cutable {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override = 0;

	[[nodiscard]] std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

  public:
	Type type;

	TypeInfo() = delete;

	virtual bool operator==(const TypeInfo & typeInfo) const;

	virtual bool operator!=(const TypeInfo & typeInfo) const;

	explicit TypeInfo(Type type);

	[[nodiscard]] std::string toLLVMIR() const override = 0;
};

class IntType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	explicit IntType();

	IntType(const IntType &) = default;

	IntType(IntType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class FloatType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	explicit FloatType();

	FloatType(const FloatType &) = default;

	FloatType(FloatType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class IntArrayType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	std::vector<int> shape;

	explicit IntArrayType(std::vector<int> shape);

	IntArrayType(const IntArrayType &) = default;

	IntArrayType(IntArrayType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	bool operator==(const TypeInfo & typeInfo) const override;
};

class FloatArrayType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	std::vector<int> shape;

	explicit FloatArrayType(std::vector<int> shape);

	FloatArrayType(const FloatArrayType &) = default;

	FloatArrayType(FloatArrayType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	bool operator==(const TypeInfo & typeInfo) const override;
};

class PointerType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	std::unique_ptr<TypeInfo> pointTo;
	int pointLevel;

	PointerType(const PointerType & pointerType);

	PointerType(PointerType && pointerType) = default;

	explicit PointerType(const TypeInfo & pointToType);

	[[nodiscard]] std::string toLLVMIR() const override;

	bool operator==(const TypeInfo & typeInfo) const override;

	[[nodiscard]] const TypeInfo & getBaseType() const {
		if (pointTo->type == Type::Pointer_t) {
			return dynamic_cast<const PointerType &>(*pointTo).getBaseType();
		} else {
			return *pointTo;
		}
	}
};

class BoolType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	explicit BoolType();

	BoolType(const BoolType &) = default;

	BoolType(BoolType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

class VoidType :
	public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override CLONEABLE_DEFAULT_IMPLEMENT;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

  public:
	VoidType();

	VoidType(const VoidType &) = default;

	VoidType(VoidType &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;
};

std::unique_ptr<TypeInfo> typeDeduce(const TypeInfo & from, size_t dep);

}