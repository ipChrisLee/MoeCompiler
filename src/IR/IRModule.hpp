#pragma once

#include <list>
#include <span>

#include "IR/IRInstr.hpp"
#include "IR/IRAddr.hpp"

#include "support/support-common.hpp"


namespace ircode {

class IRModule;

class IRAddrPool : public sup::LLVMable {
  protected:
	std::vector<std::unique_ptr<IRAddr>> pool;
	std::vector<AddrGlobalVariable *> globalVars;
  public:
	IRAddrPool();

	IRAddrPool(const IRAddrPool &) = delete;

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value &&
				std::is_base_of<IRAddr, T>::value
		>::type
	>
	T * emplace_back(T && addr) {
		pool.emplace_back(
			com::dynamic_cast_uPtr<IRAddr>(
				com::cutToUniquePtr(std::forward<T>(addr))));
		IRAddr * p = pool.rbegin()->get();
		if (auto p2 = dynamic_cast<AddrGlobalVariable *>(p)) {
			globalVars.template emplace_back(p2);
		}
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	std::string toLLVMIR() const override;
};

class IRInstrPool {
  protected:
	std::vector<std::unique_ptr<IRInstr>> pool;
  public:
	IRInstrPool();

	IRInstrPool(const IRInstrPool &) = delete;

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<IRInstr, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(std::unique_ptr<T> && instr) {
		pool.template emplace_back(std::move(instr));
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<IRInstr, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(T && instr) {
		pool.emplace_back(
			com::dynamic_cast_uPtr<IRInstr>(
				com::cutToUniquePtr(std::forward<T>(instr))));
		return dynamic_cast<T *>(pool.rbegin()->get());
	}

	/**
	 * @brief This is just for debugging.
	 */
	void printAll(std::ostream &) const;
};

class IRFuncDef;

class IRFuncDeclPool {
  protected:
	std::vector<std::unique_ptr<IRFuncDef>> pool;
	std::vector<IRFuncDef *> vec;
  public:
	IRFuncDef * emplace_back(IRFuncDef &&);

	auto begin() { return vec.begin(); }

	auto begin() const { return vec.begin(); }

	auto end() { return vec.end(); }

	auto end() const { return vec.end(); }
};

/**
 * @brief Block of instructions.
 * @note If @c instrs form a basic block, @c thisIsBasicBlock will be @c true .
 */
class IRFuncBlock : public sup::LLVMable {
  protected:
	std::list<IRInstr *> instrs;

  public:
	IRFuncBlock();

	std::string toLLVMIR() const override;
};

/**
 * @brief Composition of IRModule.
 */
class IRFuncDef : public sup::LLVMable {
  protected:
	bool loadFinished = false;
	AddrFunction * pAddrFun;
	std::list<IRFuncBlock> blocks;

	void rearrangeAlloca();

	void toBasicBlocks();

	AddrJumpLabel * addEntryLabelInstr(IRModule & ir);

	std::list<IRInstr *> instrs;
  public:
	IRInstr * emplace_back(IRInstr *);

	void emplace_back(std::list<IRInstr *> &&);

	explicit IRFuncDef(AddrFunction * pAddrFun);

	IRFuncDef(IRFuncDef &&) = default;

	void finishLoading(IRModule & ir);

	std::string toLLVMIR() const override;

	const AddrFunction * getFuncAddrPtr() const { return pAddrFun; }
};

class IRModule : public sup::LLVMable {
  public:
	IRInstrPool instrPool;
	IRAddrPool addrPool;
	IRFuncDeclPool funcPool;

	IRModule() = default;

	void allFuncToBasicBlockFunc();

	std::string toLLVMIR() const override;
};
}

namespace sup {

/**
 * @brief Instructions generator for binary operate instruction.
 * @note Type @c opL and @c opR should both be int/float. (basic type @b except bool)
 * @note If type of @c opD is not bool, type of @c opL, @c opR and @c opD should be same.
 */
std::list<ircode::IRInstr *> genBinaryOperationInstrs(
	ircode::IRModule & ir, ircode::AddrOperand * opL, const std::string & op,
	ircode::AddrOperand * opR, ircode::AddrVariable * opD
);

/**
 * @brief Instructions generator for unary operate instruction.
 * @note If type of @c opD is bool, type of @c opR can be bool/int/float.
 * @note If type of @c opD is @b NOT bool, type of @c opR and @c opD should both be int/float.
 */
std::list<ircode::IRInstr *> genUnaryOperationInstrs(
	ircode::IRModule & ir, const std::string & op, ircode::AddrOperand * opR,
	ircode::AddrVariable * opD
);

/**
 * @brief Instructions generator for `!x` and `x`
 * @param op empty for `x` and "!" for `!x`
 */
std::list<ircode::IRInstr *> genCondInstrs(
	ircode::IRModule & ir, const std::string & op, ircode::AddrOperand * opR,
	ircode::AddrVariable * opD
);

/**
 * @brief Instructions generator for runtime type conversion.
 * @return new left operand
 * @return new right operand
 * @return converted type info
 * @return instructions generated
 */
std::tuple<
	ircode::AddrOperand *, ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>,
	std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir, ircode::AddrOperand * preOpL, ircode::AddrOperand * preOpR
);

std::tuple<
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir, ircode::AddrOperand * preOp,
	const sup::TypeInfo & typeInfoD
);

std::tuple<
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir, ircode::AddrOperand * preOp, sup::Type type
);

/**
 * @brief Instructions generator for store value from @c from to @c saveTo .
 * @param saveTo @c PointerType variable.
 * @return instructions perform saving.
 * @note conversion will do type conversion.
 */
std::list<ircode::IRInstr *> genStoreInstrInFunction(
	ircode::IRModule & ir, ircode::AddrVariable * saveTo, ircode::AddrVariable * from
);

//std::tuple<ircode::AddrVariable *, std::list<ircode::IRInstr *>>
//genValueGetInstrsInFunctionFromVar(
//	ircode::IRModule & ir, ircode::AddrVariable * from
//);

}
