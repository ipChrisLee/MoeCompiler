#pragma once

#include <memory>
#include <any>

#include "moeconcept.hpp"

#include "TypeInfo.hpp"
#include "support/support-common.hpp"
#include "support/TypeInfo.hpp"
#include "support/Idx.hpp"


namespace sup {

class StaticValue;

struct CalcOnStaticValue {
	std::unique_ptr<StaticValue>
	operator()(const StaticValue &, const std::string &, const StaticValue &) const;

	std::unique_ptr<StaticValue>
	operator()(const std::string &, const StaticValue &) const;
};

extern CalcOnStaticValue calcOnSV;

class StaticValue
	: public LLVMable,
	  public moeconcept::Cloneable,
	  public moeconcept::Cutable {
	friend CalcOnStaticValue;
  protected:
	std::unique_ptr<TypeInfo>
		uPtrInfo; //  For some reason, this is not const, but it should be const.
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override = 0;

	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

	explicit StaticValue(const TypeInfo & typeInfo);

	// this op sv
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	calc(const std::string & op, const StaticValue & sv) const;

	// op this
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	calc(const std::string & op) const;

  public:
	[[nodiscard]] const TypeInfo & getType() const { return *uPtrInfo; }

	StaticValue(const StaticValue & staticValue);

	StaticValue(StaticValue && staticValue) = default;

	~StaticValue() override = default;

	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	getValue(const VI &) const = 0;

	virtual void insertValue(const VI & idx, StaticValue & staticValue);
};

class FloatStaticValue
	: public StaticValue {
	friend CalcOnStaticValue;
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op, const StaticValue & sv) const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;

  public:
	float value;

	explicit FloatStaticValue(const std::string & literal = "0");

	explicit FloatStaticValue(float value);

	FloatStaticValue(const FloatStaticValue &) = default;

	FloatStaticValue(FloatStaticValue &&) = default;

	~FloatStaticValue() override = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const VI &) const override;
};

class IntStaticValue : public StaticValue {
	friend CalcOnStaticValue;
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op, const StaticValue & sv) const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;

  public:
	int value;

	explicit IntStaticValue(const std::string & literal = "0");

	explicit IntStaticValue(int value);

	IntStaticValue(const IntStaticValue &) = default;

	IntStaticValue(IntStaticValue &&) = default;

	~IntStaticValue() override = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const VI &) const override;
};

class BoolStaticValue : public StaticValue {
	friend CalcOnStaticValue;
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op, const StaticValue & sv) const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;

  public:
	bool value;

	explicit BoolStaticValue(const std::string & literal = "0");

	explicit BoolStaticValue(bool value);

	BoolStaticValue(const BoolStaticValue &) = default;

	BoolStaticValue(BoolStaticValue &&) = default;

	~BoolStaticValue() override = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const VI &) const override;

};

class FloatArrayStaticValue
	: public StaticValue {
	friend CalcOnStaticValue;
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	std::map<VI, FloatStaticValue> value;
	std::vector<int> shape;
	FloatStaticValue zero;

	FloatArrayStaticValue() = delete;

	//  Init with default values.
	explicit FloatArrayStaticValue(std::vector<int> shape);

	FloatArrayStaticValue(const FloatArrayStaticValue &) = default;

	FloatArrayStaticValue(FloatArrayStaticValue &&) = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const VI & ind) const override;

	void insertValue(const VI & idx, StaticValue & staticValue) override;
};

class IntArrayStaticValue
	: public StaticValue {
	friend CalcOnStaticValue;
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	std::map<VI, IntStaticValue> value;
	std::vector<int> shape;
	IntStaticValue zero;

	explicit IntArrayStaticValue(VI shape);

	IntArrayStaticValue(const IntArrayStaticValue &) = default;

	IntArrayStaticValue(IntArrayStaticValue &&) = default;

	~IntArrayStaticValue() override = default;

	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const VI & ind) const override;

	void insertValue(const VI & idx, StaticValue & staticValue) override;
};

//  If `op` is "!", `fr` is ignored.
std::unique_ptr<StaticValue>
calcOfFloat(float fl, float fr, const std::string & op);

//  If `op` is "!", `fr` is ignored.
std::unique_ptr<StaticValue>
calcOfInt(int il, int ir, const std::string & op);

//  If `op` is "!", `fr` is ignored.
std::unique_ptr<StaticValue>
calcOfBool(bool bl, bool br, const std::string & op);

std::unique_ptr<StaticValue>
fromTypeInfoToStaticValue(const TypeInfo & typeInfo);

//  Static value conversion
std::unique_ptr<StaticValue>
convertOnSV(const StaticValue & from, const TypeInfo & to);

}
