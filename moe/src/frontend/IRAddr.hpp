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


  class Addr :public LLVMable, public moeconcept::Cloneable {
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

class AddrGlobalVariable : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
  public:
	const std::string name;
	//  For SysY, if the variable is const, its value is compile-time value.
	const bool isConst;
	//  For every global variable, it has its own static init value!
	const std::unique_ptr<const StaticValue> uPtrStaticValue;   // will NOT be nullptr
	
	const std::unique_ptr<const TypeInfo> uPtrTypeInfo;
	
	AddrGlobalVariable(const TypeInfo &, std::string name, const StaticValue &);
	
	AddrGlobalVariable(const AddrGlobalVariable &);
	
	~AddrGlobalVariable() override = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
	
};

/*  For compile time value.
 * */
class AddrStaticValue : public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	std::unique_ptr<TypeInfo> uPtrTypeInfo;
	std::unique_ptr<StaticValue> uPtrStaticValue;
  public:
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

