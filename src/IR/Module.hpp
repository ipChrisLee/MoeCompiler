#pragma once

#include <list>
#include <span>

#include "IR/Instr.hpp"
#include "IR/Addr.hpp"

#include "support/support-common.hpp"


namespace mir {

class Module;

class AddrPool : public sup::LLVMable {
  protected:
	std::vector<std::unique_ptr<Addr>> pool;
	std::vector<AddrGlobalVariable *> globalVars;
  public:

	AddrPool();

	AddrPool(const AddrPool &) = delete;

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value &&
				std::is_base_of<Addr, T>::value
		>::type
	>
	T * emplace_back(T && addr) {
		pool.emplace_back(
			com::dynamic_cast_uPtr<Addr>(
				com::cutToUniquePtr(std::forward<T>(addr))));
		Addr * p = pool.rbegin()->get();
		if (auto p2 = dynamic_cast<AddrGlobalVariable *>(p)) {
			globalVars.template emplace_back(p2);
		}
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	std::string toLLVMIR() const override;

	const std::vector<AddrGlobalVariable *> & getGlobalVars() const;
};

class InstrPool {
  protected:
	std::vector<std::unique_ptr<Instr>> pool;
  public:
	InstrPool();

	InstrPool(const InstrPool &) = delete;

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<Instr, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(std::unique_ptr<T> && instr) {
		pool.template emplace_back(std::move(instr));
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<Instr, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(T && instr) {
		pool.emplace_back(
			com::dynamic_cast_uPtr<Instr>(
				com::cutToUniquePtr(std::forward<T>(instr))));
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	/**
	 * @brief This is just for debugging.
	 */
	void printAll(std::ostream &) const;
};

class FuncDef;

class FuncDefPool {
  protected:
	std::vector<std::unique_ptr<FuncDef>> pool;
  public:
	std::vector<FuncDef *> funcDefs;

	FuncDef * emplace_back(FuncDef &&);

	auto begin() { return funcDefs.begin(); }

	auto begin() const { return funcDefs.begin(); }

	auto end() { return funcDefs.end(); }

	auto end() const { return funcDefs.end(); }
};

/**
 * @brief Block of instructions.
 * @note If @c instrs form a basic block, @c thisIsBasicBlock will be @c true .
 */
class FuncBlock : public sup::LLVMable {
  public:
	std::list<Instr *> instrs;

	FuncBlock();

	FuncBlock(FuncBlock &&) = default;

	std::string toLLVMIR() const override;
};

/**
 * @brief Composition of Module.
 */
class FuncDef : public sup::LLVMable {
  protected:
	void rearrangeAlloca();

	AddrJumpLabel * addEntryLabelInstr(Module & ir);

	std::vector<std::unique_ptr<FuncBlock>> pool;

	bool loadFinished = false;
  public:
	AddrFunction * pAddrFun;
	std::list<Instr *> instrs;
	std::list<FuncBlock *> blocks;

	Instr * emplace_back(Instr *);

	void emplace_back(std::list<Instr *> &&);

	FuncBlock * emplace_back(FuncBlock &&);

	explicit FuncDef(AddrFunction * pAddrFun);

	FuncDef(FuncDef &&) = default;

	void finishLoading(Module & ir);

	std::string toLLVMIR() const override;

	const AddrFunction * getFuncAddrPtr() const { return pAddrFun; }
};

class Module : public sup::LLVMable {
	bool sysyFuncAdded = false;
  public:
	InstrPool instrPool;
	AddrPool addrPool;
	FuncDefPool funcPool;

	Module() = default;

	void finishLoading();

	std::vector<AddrFunction *> generateSysYDecl();

	std::string toLLVMIR() const override;
};
}

namespace sup {

/**
 * @brief Instructions generator for binary operate instruction.
 * @note Type @c opL and @c opR should both be int/float. (basic type @b except bool)
 * @note If type of @c opD is not bool, type of @c opL, @c opR and @c opD should be same.
 */
std::list<mir::Instr *> genBinaryOperationInstrs(
	mir::Module & ir, mir::AddrOperand * opL, const std::string & op,
	mir::AddrOperand * opR, mir::AddrVariable * opD
);

/**
 * @brief Instructions generator for unary operate instruction.
 * @note If type of @c opD is bool, type of @c opR can be bool/int/float and op should be "!"
 * @note If type of @c opD is int, type of @c opR can be bool/int, this function will help you do conversion. At this case, @c op should @b NOT be "!".
 * @note If type of @c opD is float, type of @c opR should be floated. At this case, @c op should @b NOT be "!".
 * @note You can use @c genSuitableAddr to deduce type from @c opR .
 */
std::list<mir::Instr *> genUnaryOperationInstrs(
	mir::Module & ir, const std::string & op, mir::AddrOperand * opR,
	mir::AddrVariable * opD
);

/**
 * @brief Instructions generator for runtime type conversion.
 * @return new left operand
 * @return new right operand
 * @return converted type info
 * @return instructions generated
 */
std::tuple<
	mir::AddrOperand *, mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>,
	std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOpL, mir::AddrOperand * preOpR
);

std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOp,
	const sup::TypeInfo & typeInfoD
);

std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOp, sup::Type type
);

/**
 * @brief This is used with @c genUnaryOperationInstrs , to generate addr with proper type.
 * @example <tt>int x=+-!!!a;</tt>
 */
mir::AddrVariable * genSuitableAddr(
	mir::Module & ir, const std::string & op, mir::AddrOperand * preOp
);

/**
 * @brief Instructions generator for store value from @c from to @c saveTo .
 * @param saveTo @c PointerType variable.
 * @return instructions perform saving.
 * @note conversion will do type conversion.
 */
std::list<mir::Instr *> genStoreInstrInFunction(
	mir::Module & ir, mir::AddrVariable * saveTo, mir::AddrVariable * from
);

//std::tuple<mir::AddrVariable *, std::list<mir::Instr *>>
//genValueGetInstrsInFunctionFromVar(
//	mir::Module & ir, mir::AddrVariable * from
//);

}
