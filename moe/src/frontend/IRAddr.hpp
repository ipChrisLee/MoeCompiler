#pragma once

#include <string>
#include <vector>
#include <memory>

#include <mdb.hpp>
#include <common.hpp>
#include <moeconcept.hpp>
#include <stlextension.hpp>

#include "frontend/frontendHeader.hpp"
#include "frontend/TypeInfo.hpp"
#include "frontend/StaticValue.hpp"

namespace ircode {

class Addr;

enum class IdType {
	GlobalVarName,
	FunctionName,
	ParameterName,
	LocalVarName,
	ReservedWord,
	BuildInFunction,
	Error
};

class Scope {
  protected:
	std::map<std::string, std::tuple<IdType, Addr *>> addrMap;
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
	
	void
	bindDominateVar(const std::string & str, IdType idType, Addr * addrVar);
	
	//  Find the Addr* of varname. Return <IdType::Error,nullptr> if not found.
	[[nodiscard]] std::tuple<IdType, Addr *>
	findIdInThisScope(const std::string & varname) const;
	
	[[nodiscard]] std::string getIdChain() const;
};

class AddrPool {
  protected:
	std::vector<std::unique_ptr<Addr>> pool;
	std::unique_ptr<Scope> pScopeRoot;
  public:
	AddrPool();
	
	AddrPool(const AddrPool &) = delete;
	
	//  Find var named `varname` from scopes. Search from `pFrom` up to scope root.
	std::tuple<IdType, Addr *>
	findAddrDownToRoot(const Scope * pFrom, const std::string & name);
	
	Addr * addAddrToScope(
		const Addr & addr, Scope * pScope, IdType idType,
		const std::string & name
	);
	
	Addr * addAddrWithoutScope(const Addr & addr);
	
	[[nodiscard]] Scope * getRootScope() const;
};


class Addr : public LLVMable, public moeconcept::Cloneable {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	static int cnt;
  public:
	const int id;
	
	Addr();
	
	Addr(const Addr &);
	
	Addr & operator =(const Addr &) = delete;
	
	~Addr() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override = 0;
};

/*  For LLVM-IR variable.
 *  In LLVM-IR, the `AddrVariable` is like `%V.name` (variable from source code)
 *  or `%V.1` (for temporary).
 * */
class AddrVariable : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
  
  public:
	//  For temporary variable, name is empty.
	const std::string name;
	//  For SysY, if the variable is const, its value is compile-time value.
	const bool isConst;
	const std::unique_ptr<const StaticValue> uPtrStaticValue;
	
	const std::unique_ptr<const TypeInfo> uPtrTypeInfo;
	
	explicit AddrVariable(const TypeInfo &, std::string name = "");
	
	AddrVariable(
		const TypeInfo &, std::string name, const StaticValue &
	);
	
	AddrVariable(const AddrVariable &);
	
	~AddrVariable() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};


/**
 * @brief addr of global variable from source code
 * @var @c name : The name from source code.
 * @var @c isConst : is the variable in the source code const?
 * @var @c uPtrStaticValue :
 * 				The static value of variable.
 * 				For constant, this is always the value.
 * 				Otherwise, this is just compile-time init value.
 * @var @c uPtrTypeInfo : Type of this @b addr
 * 			(@b NOT the type of variable in source code!)
 * @note Since global variable is always allocated on memory,
 * 			@c uPtrTypeInfo is always @c PointerType (point-level=1).
 * @note @c uPtrStaticValue will not be @c nullptr at any time.
 */
class AddrGlobalVariable : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
  
  public:
	const std::string name;
	//  For SysY, if the variable is const, its value is compile-time value.
	const bool isConst;
	//  For every global variable, it has its own static init value!
	const std::unique_ptr<const StaticValue> uPtrStaticValue;
	
	const std::unique_ptr<const TypeInfo> uPtrTypeInfo;
	
	/**
	 * @brief Create new addr for @b const variable from source code.
	 * @param typeInfo type from source code.
	 * @param name name from source code.
	 * @param staticValue static value from source code.
	 * @param isConst if the variable in source code const.
	 * @note The type deduced from @c typeInfo should be same as that from @c staticValue.
	 * @note For SysY grammar, const decl must have @c staticValue explicit.
	 * 			(i.e. @code const int x;@endcode is illegal.)
	 */
	AddrGlobalVariable(
		const TypeInfo & typeInfo,
		std::string name,
		const StaticValue & staticValue,
		bool isConst = true
	);
	
	/**
	 * @brief Create new addr for @b changeable global variable .
	 * @param typeInfo type from source code.
	 * @param name name from source code.
	 */
	AddrGlobalVariable(
		const TypeInfo & typeInfo,
		std::string name
	);
	
	/**
	 * @brief Copy constructor. The id will increase for new addr.
	 */
	AddrGlobalVariable(const AddrGlobalVariable &);
	
	~AddrGlobalVariable() override = default;
	
	/**
	 * @brief Return \@G.[name] .
	 */
	[[nodiscard]] std::string toLLVMIR() const override;
	
};

/*  For compile time value.
 * */
class AddrStaticValue : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
  
  public:
	const std::unique_ptr<const TypeInfo> uPtrTypeInfo;
	const std::unique_ptr<const StaticValue> uPtrStaticValue;
	
	AddrStaticValue(const TypeInfo &, const StaticValue &);
	
	AddrStaticValue(const AddrStaticValue &);
	
	~AddrStaticValue() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

/*  For LLVM-IR jump label.
 *  In LLVM-IR, `AddrJumpLabel` is like `L.123.if.then` where `123` is id of addr,
 *  `.if.then` is `labelName`.
 * */
class AddrJumpLabel : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	std::string labelName;
  public:
	explicit AddrJumpLabel(std::string labelName = "");
	
	AddrJumpLabel(const AddrJumpLabel &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

/*  For LLVM-IR parameter.
 *  In LLVM-IR, `AddrPara` is like `%P.name`, where name is the name from source code.
 * */
class AddrPara : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	int number; // number-th parameter
	std::string name;
	std::unique_ptr<TypeInfo> uPtrTypeInfo;
  public:
	explicit AddrPara(std::string name, const TypeInfo &, int number);
	
	AddrPara(const AddrPara &);
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

/*  For LLVM-IR function.
 *  In LLVM-IR, the `AddrFunction` is like `F.name` where name is the name of function
 *  from source code.
 * */
class AddrFunction : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	std::string name;
	std::unique_ptr<TypeInfo> uPtrReturnTypeInfo; // nullptr for void
	std::vector<const AddrPara *> vecPtrAddrPara;
  public:
	explicit AddrFunction() = delete;
	
	explicit AddrFunction(std::string name);
	
	AddrFunction(const AddrFunction &);
	
	TypeInfo * setReturnTypeInfo(const TypeInfo &);
	
	[[nodiscard]] const TypeInfo * getReturnTypeInfo() const;
	
	void pushParameter(const AddrPara *);
	
	[[nodiscard]] const AddrPara * getNumberThParameterTypeInfo(int) const;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};
}

