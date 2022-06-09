#pragma once

#include <memory>

#include "moeconcept.hpp"

#include "frontend/frontendHeader.hpp"
#include "frontend/TypeInfo.hpp"

namespace ircode {
class StaticValue : public LLVMable, public moeconcept::Cloneable {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override = 0;
  
  public:
	std::unique_ptr<TypeInfo> uPtrInfo;
	
	StaticValue();
	
	explicit StaticValue(const TypeInfo & typeInfo);
	
	StaticValue(const StaticValue & staticValue);
	
	~StaticValue() override = default;
	
	
	[[nodiscard]] virtual std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const = 0;
	
	//  op : {"+","-"}
	virtual std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const;
};

class FloatStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	float value;
	
	explicit FloatStaticValue(const std::string & literal = "0");
	
	FloatStaticValue(const FloatStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const override;
	
	std::unique_ptr<StaticValue>
	calc(const StaticValue &, const std::string & op) const override;
};

class IntStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	int value;
	
	explicit IntStaticValue(const std::string & literal = "0");
	
	IntStaticValue(const IntStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> &) const override;
};

class FloatArrayStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable>
	_cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	std::vector<FloatStaticValue> value;
	
	FloatArrayStaticValue() = delete;
	
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
	
	FloatArrayStaticValue(const FloatArrayStaticValue &) = default;
	
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
	
	IntArrayStaticValue(const IntArrayStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
	[[nodiscard]] std::unique_ptr<StaticValue>
	getValue(const std::vector<int> & ind) const override;
};
}