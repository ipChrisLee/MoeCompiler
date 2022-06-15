#include "frontend/IRInstr.hpp"
#include "common.hpp"

#include <mdb.hpp>


//  IRInstrPool
namespace ircode {

namespace instr {
int IRInstr::cnt = 0;

std::unique_ptr<moeconcept::Cloneable> IRInstr::_cloneToUniquePtr() const {
	com::Throw("This function should not be invoked!", CODEPOS);
}

std::unique_ptr<moeconcept::Cloneable> DeclGlobal::_cloneToUniquePtr() const {
	return std::make_unique<DeclGlobal>(*this);
}

DeclGlobal::DeclGlobal(AddrGlobalVariable * pAddr) : pAddr(pAddr) {
}

std::string DeclGlobal::toLLVMIR() const {
	return pAddr->toLLVMIR() +
	       " = dso_local" +
	       (pAddr->isConst ? " constant " : " ") +
	       pAddr->uPtrStaticValue->toLLVMIR() +
	       ", align 4";
}

}


instr::IRInstr * IRInstrPool::addInstrToPool(const instr::IRInstr & instr) {
	instrs.emplace_back(
		com::dynamic_cast_uPtr<instr::IRInstr>(instr.cloneToUniquePtr()));
	return instrs.rbegin()->get();
}

void IRInstrPool::printAll(std::ostream & os) const {
	for (const auto& p : instrs) {
		os<<p->toLLVMIR()<<std::endl;
	}
}

}