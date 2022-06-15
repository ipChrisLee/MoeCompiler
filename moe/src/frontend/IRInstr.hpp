#pragma once

#include <string>
#include <vector>
#include <list>
#include <functional>

#include "common.hpp"
#include "moeconcept.hpp"

#include "frontend/IRAddr.hpp"
#include "frontend/frontendHeader.hpp"


namespace ircode {

namespace instr {
class IRInstr : public LLVMable, public moeconcept::Cloneable { // maybe cloneable?
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	int id;
	static int cnt;
  public:
	IRInstr() : id(++cnt) { };
	
	IRInstr(const IRInstr &) : id(++cnt) { }
	
	IRInstr & operator =(const IRInstr &) = delete;
	
	[[nodiscard]] std::string toLLVMIR() const override = 0;
	
	~IRInstr() override = default;
};

/**
 * @brief IR-Instr. Decl a global variable.
 * @example
 */
class DeclGlobal : public IRInstr {
  protected:
	[[nodiscard]] std::unique_ptr<Cloneable> _cloneToUniquePtr() const override;
	
	AddrGlobalVariable * pAddr;
  public:
	DeclGlobal() = delete;
	
	explicit DeclGlobal(AddrGlobalVariable * pAddr);
	
	DeclGlobal(const DeclGlobal &) = default;
	
	[[nodiscard]] std::string toLLVMIR() const override;
};

}
class IRInstrPool {
  protected:
	std::vector<std::unique_ptr<instr::IRInstr>> instrs;
  public:
	instr::IRInstr * addInstrToPool(const instr::IRInstr &);
	
	void printAll(std::ostream &) const;
};


}
