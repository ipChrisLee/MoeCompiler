#pragma once

#include <string>
#include <vector>
#include <memory>

#include "mlib/mdb.hpp"
#include "mlib/common.hpp"
#include "mlib/moeconcept.hpp"
#include "mlib/stlpro.hpp"

#include "mir/support/TypeInfo.hpp"
#include "mir/support/StaticValue.hpp"
#include "frontend/SymbolTableAndScope.hpp"


namespace mir {

class AddrPara;

class Addr :
	public sup::LLVMable,
	public moeconcept::Cloneable,
	public moeconcept::Cutable {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override = 0;

	std::unique_ptr<Cutable> _cutToUniquePtr() override = 0;

	static int cnt;
  public:
	const int id;

	Addr();

	Addr(const Addr &);

	Addr(Addr &&) = default;

	Addr & operator=(const Addr &) = delete;

	~Addr() override = default;

	[[nodiscard]] virtual const sup::TypeInfo & getType() const;

	[[nodiscard]] std::string toLLVMIR() const override = 0;
};

class AddrOperand :
	public Addr {
  protected:
	std::unique_ptr<sup::TypeInfo> uPtrTypeInfo;
  public:

	explicit AddrOperand(const sup::TypeInfo & typeInfo);

	explicit AddrOperand(std::unique_ptr<sup::TypeInfo> && uPtrTypeInfo);

	AddrOperand(const AddrOperand &);

	AddrOperand(AddrOperand &&) = default;

	[[nodiscard]] const sup::TypeInfo &
	getType() const override { return *uPtrTypeInfo; }

};

/*  For compile time value.
 * */
class AddrStaticValue :
	public AddrOperand {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	std::unique_ptr<sup::StaticValue> uPtrStaticValue;
  public:

	explicit AddrStaticValue(const sup::StaticValue &);

	explicit AddrStaticValue(std::unique_ptr<sup::StaticValue> && up);

	AddrStaticValue(const sup::TypeInfo &, const sup::StaticValue &);

	//  Default value of type `typeInfo`
	explicit AddrStaticValue(const sup::TypeInfo &);

	AddrStaticValue(const AddrStaticValue &);

	AddrStaticValue(AddrStaticValue &&) = default;

	// {staticValue}
	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] const sup::StaticValue &
	getStaticValue() const { return *uPtrStaticValue; }
};

/*  For LLVM-IR variable.
 *  This is for base class of variable address, and also for temp var.
 * */
class AddrVariable :
	public AddrOperand {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	//  For temporary variable, name can be empty.
	std::string name;
  public:
	[[nodiscard]] virtual const std::string & getName() const { return name; }

	explicit AddrVariable(const sup::TypeInfo &, std::string name = "");

	/**
	 * @brief convert type to addrVariable
	 * @param type one of `Type::Int_t`, `Type::Float_t`, `Type::Bool_t`
	 * @param name name of variable
	 */
	explicit AddrVariable(sup::Type type, std::string name = "");

	AddrVariable(const AddrVariable &) = default;

	AddrVariable(AddrVariable &&) = default;

	/**
	 * @brief Create addrVariable with pointer type pointing to type of para.
	 */
	explicit AddrVariable(const AddrPara &);

	// %T{id}.{name}
	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] virtual bool isConstVar() const { return false; }
};

/*  For LLVM-IR parameter.
 *  In LLVM-IR, `AddrPara` is like `%P{id}.{name}`, where id is Addr::id,
 *  name is the name from source code.
 * */
class AddrPara :
	public AddrVariable {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

  public:
	explicit AddrPara(const sup::TypeInfo &, std::string name);

	AddrPara(const AddrPara &) = default;

	AddrPara(AddrPara &&) = default;

	// %P{id}.{name}
	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] bool isConstVar() const override { return false; }
};

/**
 * @brief addr of global variable from source code
 * @var @c uPtrTypeInfo : Type of this @b addr
 * 			(@b NOT the type of variable in source code!)
 * @note Since global variable is always allocated on memory,
 * 			@c uPtrTypeInfo is always @c PointerType (point-level=1).
 * @note @c uPtrStaticValue will not be @c nullptr at any time.
 */
class AddrGlobalVariable :
	public AddrVariable {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	//  For every global variable, it has its own static init value!
	std::unique_ptr<sup::StaticValue> uPtrStaticValue;
	bool isConst;
  public:
	/**
	 * @brief Create new addr for variable from source code.
	 * @param typeInfo type from source code. (no need to be pointer type.)
	 * @param name name from source code.
	 * @param staticValue static value from source code.
	 * @param isConst if the variable in source code const.
	 * @note The @c typeInfo should be same as @c staticValue.
	 */
	AddrGlobalVariable(
		const sup::TypeInfo & typeInfo, std::string name,
		const sup::StaticValue & staticValue, bool isConst = false
	);

	/**
	 * @brief Create new addr for global variable with default static value.
	 * @param typeInfo type from source code.
	 * @param name name from source code.
	 */
	AddrGlobalVariable(
		const sup::TypeInfo & typeInfo, std::string name, bool isConst = false
	);

	AddrGlobalVariable(const AddrGlobalVariable &);

	AddrGlobalVariable(AddrGlobalVariable &&) = default;

	// @G{id}.{name}
	[[nodiscard]] std::string toLLVMIR() const override;

	// @G{id}.{name} = dso_local [constant] global {staticValue}, align 4
	[[nodiscard]] std::string toDeclIR() const;

	[[nodiscard]] bool isConstVar() const override { return isConst; }

	[[nodiscard]] const sup::StaticValue & getStaticValue() const;
};

class AddrLocalVariable :
	public AddrVariable {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable>
	_cloneToUniquePtr() const override CLONEABLE_DEFAULT_IMPLEMENT;

	std::unique_ptr<Cutable> _cutToUniquePtr() override CUTABLE_DEFAULT_IMPLEMENT;

	bool isConst;
	std::unique_ptr<sup::StaticValue> uPtrStaticValue;
  public:
	AddrLocalVariable(const sup::TypeInfo & typeInfo, std::string name);

	AddrLocalVariable(
		const sup::TypeInfo & typeInfo, std::string name,
		const sup::StaticValue & staticValue
	);

	AddrLocalVariable(const AddrLocalVariable &);

	AddrLocalVariable(AddrLocalVariable &&) = default;

	// @V{id}.{name}
	[[nodiscard]] std::string toLLVMIR() const override;

	[[nodiscard]] bool isConstVar() const override { return isConst; }

	[[nodiscard]] const sup::StaticValue & getStaticValue() const;
};

/*  For LLVM-IR jump label.
 *  In LLVM-IR, `AddrJumpLabel` is like `L123.if.then` where `123` is id of addr,
 *  `.if.then` is `labelName`.
 * */
class AddrJumpLabel :
	public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	std::string labelName;
  public:

	explicit AddrJumpLabel(std::string labelName = "");

	AddrJumpLabel(const AddrJumpLabel &) = default;

	AddrJumpLabel(AddrJumpLabel &&) = default;

	//  "L{id}.{name}"
	[[nodiscard]] std::string toLLVMLabelName() const;

	//  "%L{id}.{name}"
	[[nodiscard]] std::string toLLVMIR() const override;
};

/*  For LLVM-IR function.
 *  In LLVM-IR, the `AddrFunction` is like `F.name` where name is the name of function
 *  from source code.
 * */
class AddrFunction :
	public Addr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;

	std::unique_ptr<Cutable> _cutToUniquePtr() override;

	std::unique_ptr<sup::TypeInfo> uPtrReturnTypeInfo; // nullptr for void
	std::vector<AddrPara *> vecPtrAddrPara;
	std::string name;
  public:
	bool justDeclare = false;

	[[nodiscard]] const std::string & getName() const { return name; }

	/**
	 * @brief For @c void(void)
	 */
	explicit AddrFunction(std::string name);

	/**
	 * @brief For @c void(...)
	 */
	AddrFunction(std::string name, std::vector<AddrPara *> vecPara);

	/**
	 * @brief For @c ...(...)
	 */
	AddrFunction(
		std::string name, std::vector<AddrPara *> vecPara,
		const sup::TypeInfo & retType
	);

	/**
	 * @brief For @c ...()
	 */
	AddrFunction(std::string name, const sup::TypeInfo & retType);

	AddrFunction(const AddrFunction &);

	AddrFunction(AddrFunction &&) = default;

	[[nodiscard]] const sup::TypeInfo & getReturnTypeInfo() const;

	[[nodiscard]] const sup::TypeInfo & getNumberThParameterTypeInfo(int) const;

	[[nodiscard]] int getNumberOfParameter() const;

	void pushParameter(AddrPara *);

	// @F.{name}
	/**
	 * @brief
	 * @return \@F.{name}
	 * @return bitcast ({retType} (...)* @{name} to {retType} ()*)
	 */
	[[nodiscard]] std::string toLLVMIR() const override;

	/**
	 * @brief To decl/def ir statements
	 * @return define dso_local {retType} \@F.{name}({para.type} {para.toLLVMIR})
	 * @return declare dso_local void \@{name}({para.type})
	 */
	std::string declLLVMIR() const;
};

std::string convertLongName(std::string name);

extern const char * llvmSyFunctionAttr;

extern const char * llvmSyLibFunctionAttr;
}

