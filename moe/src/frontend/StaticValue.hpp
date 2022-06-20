#pragma once

#include <memory>
#include <any>

#include "moeconcept.hpp"

#include "frontend/frontendHeader.hpp"
#include "frontend/TypeInfo.hpp"


namespace ircode {
class StaticValue : public LLVMable, public moeconcept::Cloneable {
  protected:
	std::unique_ptr<TypeInfo>
		uPtrInfo; //  For some reason, this is not const, but it should be const.
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override = 0;
	
	explicit StaticValue() = default;
	
	explicit StaticValue(const TypeInfo & typeInfo);
  
  public:
	
	[[nodiscard]] const TypeInfo & getTypeInfo() const { return *uPtrInfo; }
	
	StaticValue(const StaticValue & staticValue);
	
	~StaticValue() override = default;
	
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const = 0;
	
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const;
	
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	calc(const std::string & op) const;
	
	/**
	 * @brief Static value conversion from @c fromStaticValue to type @c toType.
	 * @note bool->int->float, other type of conversion will cause exception.
	 */
	virtual std::unique_ptr<StaticValue> convertTo(const TypeInfo & toType);
};

class FloatStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	float value;
	
	explicit FloatStaticValue(const std::string & literal = "0");
	
	explicit FloatStaticValue(float value);
	
	FloatStaticValue(const FloatStaticValue &) = default;
	
	~FloatStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;
	
	std::unique_ptr<StaticValue> convertTo(const TypeInfo & toType) override;
};

class IntStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	int value;
	
	explicit IntStaticValue(const std::string & literal = "0");
	
	explicit IntStaticValue(int value);
	
	IntStaticValue(const IntStaticValue &) = default;
	
	~IntStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;
	
	std::unique_ptr<StaticValue> convertTo(const TypeInfo & toType) override;
};

class BoolStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	bool value;
	
	explicit BoolStaticValue(const std::string & literal = "0");
	
	explicit BoolStaticValue(bool value);
	
	BoolStaticValue(const BoolStaticValue &) = default;
	
	~BoolStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	calc(const std::string & op) const override;
	
	std::unique_ptr<StaticValue> convertTo(const TypeInfo & toType) override;
};

class FloatArrayStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	std::vector<FloatStaticValue> value;
	
	FloatArrayStaticValue() = delete;
	
	explicit FloatArrayStaticValue(std::vector<int>shape);
	
	
	//  Create new 1-d, `len` long float array.
	explicit FloatArrayStaticValue(int len, std::vector<FloatStaticValue> vi);
	
	/*  Create new array from arrays.
	 *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
	 *  is `(2,3,4)` array.
	 *  This constructor will detect legality of arrays. (They should be same in shape.)
	 * */
	explicit FloatArrayStaticValue(
		int len,
		const std::vector<int> & preShape,
		const std::vector<FloatArrayStaticValue> & vi
	);
	
	explicit FloatArrayStaticValue(
		int len,
		const std::vector<int> & preShape,
		const std::vector<FloatArrayStaticValue> & vi,
		const std::vector<FloatStaticValue> & rest
	);
	
	FloatArrayStaticValue(const FloatArrayStaticValue &) = default;
	
	~FloatArrayStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> & ind) const override;
};

class IntArrayStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	std::vector<IntStaticValue> value;
	
	explicit IntArrayStaticValue(std::vector<int> shape);
	
	//  Create new 1-d, `len` long float array.
	explicit IntArrayStaticValue(int len, std::vector<IntStaticValue> vi);
	
	/*  Create new array from arrays.
	 *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
	 *  is `(2,3,4)` array.
	 *  This constructor will detect legality of arrays. (They should be same in shape.)
	 * */
	explicit IntArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<IntArrayStaticValue> & vi
	);
	
	explicit IntArrayStaticValue(
		int len, const std::vector<int> & preShape,
		const std::vector<IntArrayStaticValue> & vi,
		const std::vector<IntStaticValue> & rest
	);
	
	IntArrayStaticValue(const IntArrayStaticValue &) = default;
	
	~IntArrayStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> & ind) const override;
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
zeroExtensionValueOfType(const TypeInfo & typeInfo);

}
