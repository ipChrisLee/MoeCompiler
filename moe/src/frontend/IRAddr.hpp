/*  Intro:
 *      `Addr` is a base class of address.
 *          `AddrCompileConst` is address for compile time const. This inclue a info structure
 *          which provides information basing on type of const variable(`int`,`float`,`intArray` 
 *          and `floatArray`)
 *              >Notice:
 *                  Legal expression of operator, const BASIC-TYPE value and literals is
 *                  compile time const.
 *          `AddrOperand` is address for variable declared in source program.
 *              `AddrStaticVar` is a address for static variable (since sysy does not have
 *              key word like `static`, this is for global variable.).
 *              `AddrLocalVar` is a address for local variable.
 *      `AddrPool` is like a memory pool of Addr. It also contains a dominator tree where 
 *      every tree node is a scope in source code.
 *          >Notice:
 *              AddrPool has a class member variable `loop` in type `vector<unique_ptr<Addr>>`.
 *              This `pool` is in control of every `Addr`
 *      `AddrPool::Scope` is a scope descriptor.
 * */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <mdb.hpp>
#include <common.hpp>
#include <moeconcept.hpp>
#include <stlextension.hpp>

#include "frontend/frontendHeader.hpp"


namespace ircode {
class Addr;

class AddrOperand;

class AddrLocalVar;

class AddrStaticVar;

class StaticValue;

class Scope {
  protected:
	std::map<std::string, Addr *> addrMap;
	Scope * const father;
	std::vector<std::unique_ptr<Scope>> sons;
	const int id;
	static int cnt;
  public:
	explicit Scope(Scope * pFather);
	
	Scope(const Scope &) = delete;
	
	Scope * addSonScope();
	
	[[nodiscard]] Scope * getFather() const;
	
	Scope * getThis();
	
	void bindDominateVar(Addr * addrVar);
	
	[[nodiscard]] Addr * findIdInThisScope(const std::string & varname) const;
	
	[[nodiscard]] std::string getIdChain() const;
};

class AddrPool {
  public:
  
  protected:
	std::vector<std::unique_ptr<Addr>> pool;
	std::unique_ptr<Scope> pBlockRoot;
  public:
	AddrPool();
	
	AddrPool(const AddrPool &) = delete;
	
	AddrLocalVar * addAddrLocalVar(const AddrLocalVar &, Scope * pScope);
	
	AddrStaticVar * addAddrStaticVar(const AddrStaticVar &, const StaticValue &);
	
	Addr * addAddr(const Addr &);
	
	//  Add a scope as sub of pFather. This is the only way you can create scope.
	Scope * addScope(Scope * pFather);
	
	//  Get the root scope.
	Scope * getRootScopePointer();
	
	//  Find var named `varname` from scopes. Search from `pFrom` up to scope root.
	Addr * findAddrDownToRoot(const Scope * pFrom, const std::string & varname);
};

class TypeInfo : public LLVMable, public moeconcept::Cloneable {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override = 0;
  
  public:
	enum class Type {
		Float_t, Int_t, FloatArray_t, IntArray_t, Pointer_t, Unknown, Bool_t
	} type;
	
	explicit TypeInfo(Type type);
};

class IntType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	explicit IntType();
	
	IntType(const IntType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class FloatType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	explicit FloatType();
	
	FloatType(const FloatType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class IntArrayType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	
	explicit IntArrayType(std::vector<int> shape);
	
	IntArrayType(const IntArrayType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class FloatArrayType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	
	explicit FloatArrayType(std::vector<int> shape);
	
	FloatArrayType(const FloatArrayType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class PointerType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	std::unique_ptr<TypeInfo> pointTo; // This should not be a `PointerType`.
	int pointLevel;
	
	PointerType(const ircode::PointerType & pointerType);
	
	explicit PointerType(const TypeInfo & pointToType);
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class BoolType : public TypeInfo {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	explicit BoolType();
	
	BoolType(const BoolType &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class StaticValue : public LLVMable, public moeconcept::Cloneable {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override = 0;
  
  public:
	std::unique_ptr<TypeInfo> uPtrInfo;
	
	StaticValue();
	
	explicit StaticValue(const TypeInfo & typeInfo);
	
	StaticValue(const StaticValue & staticValue);
	
	~StaticValue() override = default;
};

class FloatStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	float value;
	
	explicit FloatStaticValue(const std::string & literal = "0");
	
	FloatStaticValue(const FloatStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class IntStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	int value;
	
	explicit IntStaticValue(const std::string & literal = "0");
	
	IntStaticValue(const IntStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class FloatArrayStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	std::vector<FloatStaticValue> value;
	
	FloatArrayStaticValue() = delete;
	
	//  Create new 1-d, `len` long float array.
	explicit FloatArrayStaticValue(int len, std::vector<FloatStaticValue> vi);
	
	/*  Create new array from arrays.
	 *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
	 *  is `(2,3,4)` array.
	 *  This constructor will detect legality of arrays. (They should have same shape.)
	 * */
	explicit FloatArrayStaticValue(
			int len,
			const std::vector<int> & preShape,
			const std::vector<FloatArrayStaticValue> & vi
	);
	
	FloatArrayStaticValue(const FloatArrayStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

class IntArrayStaticValue : public StaticValue {
  protected:
	[[nodiscard]] std::unique_ptr<moeconcept::Cloneable> _cloneToUniquePtr() const override;
  
  public:
	std::vector<int> shape;
	std::vector<IntStaticValue> value;
	
	//  Create new 1-d, `len` long float array.
	explicit IntArrayStaticValue(int len, std::vector<IntStaticValue> vi);
	
	/*  Create new array from arrays.
	 *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
	 *  is `(2,3,4)` array.
	 *  This constructor will detect legality of arrays. (They should have same shape.)
	 * */
	explicit IntArrayStaticValue(
			int len, const std::vector<int> & preShape,
			const std::vector<IntArrayStaticValue> & vi
	);
	
	IntArrayStaticValue(const IntArrayStaticValue &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

/*  First part of IR design: Address used in three-address-code.
 *  NOTICE: Copy constructor of Addr is deleted.
 * */
class Addr : public LLVMable {
  protected:
	static int cnt;
  public:
	const int id;
	
	Addr();
	
	Addr(const Addr &) = delete;
	
	virtual ~Addr() { }
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const = 0;
};

/*  Address for LLVM-IR variable.
 *  This is inherited by `AddrNamedVar` and `AddrTmpVar`.
 * */
class AddrOperand : public Addr {
  protected:
	bool isConst;
	std::string name;
	std::unique_ptr<TypeInfo> uPtrTypeInfo;
  public:
	explicit AddrOperand() = delete;
	
	explicit AddrOperand(const std::string & name, const TypeInfo &, bool isConst = false);
	
	explicit AddrOperand(const AddrOperand &) = delete;
	
	virtual ~AddrOperand() = default;
	
	bool isconst() const;
	
	std::string getVarName() const;
};

/*  Addr from source code.
 *  All addr of this type has a name from source code, and can be indexed in symbol table.
 *  For variable has static value, `AddrNamedOperand` has a pointer to its static value.
 * */
class AddrMemVar : public AddrOperand { // may it should be inherited from `Addr`
  protected:
	std::unique_ptr<StaticValue> uPtrStaticValue;
	std::string varname;
	bool isGlobal;
	bool isConst;
  public:
	explicit AddrMemVar(const std::string & name, const TypeInfo & typeInfo, bool isConst = false);
	
	StaticValue * setStaticValue(const StaticValue &);
	
	const StaticValue * getStaticValue() const;
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};

/*  Variable generated in processing source code.
 *  For example, `int x=1+2+3;` => `add t1,1,2;add x,t1,3;`, `t1` is temporary variable.
 * */
class AddrRegOperand : public AddrOperand {
  public:
	explicit AddrRegOperand() = delete;
	
	explicit AddrRegOperand(const TypeInfo &);
	
	explicit AddrRegOperand(const AddrRegOperand &) = delete;
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};

/*  Static operand. The value of this type operand can be caculated in compile time.
 * */
class AddrStaticOperand : public AddrOperand {
  public:
	explicit AddrStaticOperand() = delete;
	
	explicit AddrStaticOperand(const TypeInfo &);
	
	explicit AddrStaticOperand(const AddrStaticOperand &) = delete;
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};

class AddrJumpLabel : public Addr {
  protected:
	std::string name;
  public:
	explicit AddrJumpLabel(const std::string & name = "");
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};

class AddrPara : public Addr {
  protected:
	int number; // number-th parameter
	std::string name;
	std::unique_ptr<TypeInfo> uPtrTypeInfo;
  public:
	explicit AddrPara() = delete;
	
	explicit AddrPara(const std::string name, const TypeInfo &, int number);
	
	explicit AddrPara(const AddrPara &) = delete;
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};

class AddrFunction : public Addr {
  protected:
	std::string name;
	std::unique_ptr<TypeInfo> uPtrReturnTypeInfo; // nullptr for void
	std::vector<AddrPara *> vecPtrAddrPara;
  public:
	explicit AddrFunction() = delete;
	
	explicit AddrFunction(const std::string & name);
	
	explicit AddrFunction(const AddrFunction &) = delete;
	
	TypeInfo * setReturnTypeInfo(const TypeInfo &);
	
	const TypeInfo * getReturnTypeInfo() const;
	
	void pushParameter(const AddrPara &);
	
	void pushParameter(const std::string &, const TypeInfo &);
	
	const AddrPara * getNumberThParameterTypeInfo(int) const;
	
	virtual std::string toLLVMIR() const override;
	
	virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
};
}

