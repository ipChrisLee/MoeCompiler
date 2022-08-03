#pragma once

#include <list>
#include <span>

#include "Instr.hpp"
#include "Addr.hpp"

#include "support/Support.hpp"


namespace mir {

class Module;

class AddrPool : public mir::LLVMable, public moeconcept::Pool<mir::Addr> {
  protected:
	std::vector<AddrGlobalVariable *> globalVars;
  public:
	AddrPool();

	AddrPool(const AddrPool &) = delete;

	std::string toLLVMIR() const override;

	const std::vector<AddrGlobalVariable *> & getGlobalVars() const;
};

class InstrPool : public moeconcept::Pool<mir::Instr> {
  public:
	InstrPool();

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
class FuncBlock : public mir::LLVMable {
  public:
	std::list<Instr *> instrs;

	FuncBlock();

	FuncBlock(FuncBlock &&) = default;

	std::string toLLVMIR() const override;
};

/**
 * @brief Composition of Module.
 */
class FuncDef : public mir::LLVMable {
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

class Module : public mir::LLVMable {
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

namespace mir {

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
	mir::AddrOperand *, mir::AddrOperand *, std::unique_ptr<mir::TypeInfo>,
	std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOpL, mir::AddrOperand * preOpR
);

std::tuple<
	mir::AddrOperand *, std::unique_ptr<mir::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOp,
	const mir::TypeInfo & typeInfoD
);

std::tuple<
	mir::AddrOperand *, std::unique_ptr<mir::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOp, mir::Type type
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
