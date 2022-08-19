#pragma once

#include <list>

#include <moeconcept.hpp>

#include "IR/IRInstr.hpp"
#include "IR/IRAddr.hpp"

#include "support/support-common.hpp"


namespace ircode {

class IRModule;

class IRAddrPool : public sup::LLVMable, public moeconcept::Pool<IRAddr> {
  protected:
	std::vector<std::unique_ptr<IRAddr>> pool;
	std::vector<AddrGlobalVariable *> globalVars;
  public:

	IRAddrPool();

	IRAddrPool(const IRAddrPool &) = delete;

	std::string toLLVMIR() const override;

	const std::vector<AddrGlobalVariable *> & getGlobalVars() const;
};

class IRInstrPool : public moeconcept::Pool<IRInstr> {
  public:
	IRInstrPool() = default;

	IRInstrPool(const IRInstrPool &) = delete;

	/**
	 * @brief This is just for debugging.
	 */
	void printAll(std::ostream &) const;
};

class IRFuncDef;

class IRFuncDefPool : public moeconcept::Pool<IRFuncDef> {
  public:
	IRFuncDefPool();
};

/**
 * @brief Composition of IRModule.
 */
class IRFuncDef : public sup::LLVMable {
  protected:
	void rearrangeAlloca();

	AddrJumpLabel * addEntryLabelInstr(IRModule & ir);


	bool loadFinished = false;
  public:
	AddrFunction * pAddrFun;
	std::list<IRInstr *> instrs;

	IRInstr * emplace_back(IRInstr *);

	void emplace_back(std::list<IRInstr *> &&);

	explicit IRFuncDef(AddrFunction * pAddrFun);

	IRFuncDef(IRFuncDef &&) = default;

	void finishLoading(IRModule & ir);

	std::string toLLVMIR() const override;

	const AddrFunction * getFuncAddrPtr() const { return pAddrFun; }
};

class IRModule : public sup::LLVMable {
	bool sysyFuncAdded = false;
  public:
	IRInstrPool instrPool;
	IRAddrPool addrPool;
	IRFuncDefPool funcPool;

	IRModule() = default;

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
std::list<ircode::IRInstr *> genBinaryOperationInstrs(
	ircode::IRModule & ir, ircode::AddrOperand * opL, const std::string & op,
	ircode::AddrOperand * opR, ircode::AddrVariable * opD
);

/**
 * @brief Instructions generator for unary operate instruction.
 * @note If type of @c opD is bool, type of @c opR can be bool/int/float and op should be "!"
 * @note If type of @c opD is int, type of @c opR can be bool/int, this function will help you do conversion. At this case, @c op should @b NOT be "!".
 * @note If type of @c opD is float, type of @c opR should be float. At this case, @c op should @b NOT be "!".
 * @note You can use @c genSuitableAddr to deduce type from @c opR .
 */
std::list<ircode::IRInstr *> genUnaryOperationInstrs(
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
 * @brief This is used with @c genUnaryOperationInstrs , to generate addr with proper type.
 * @example <tt>int x=+-!!!a;</tt>
 */
ircode::AddrVariable * genSuitableAddr(
	ircode::IRModule & ir, const std::string & op, ircode::AddrOperand * preOp
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
