#include "frontend/IRInstr.hpp"
#include "common.hpp"

#include <submain.hpp>
#include <mdb.hpp>

#ifdef CODING

//  IRInstrPool
namespace ircode{

    std::string BasicBlock::toLLVMIR() const {
        com::TODO(FUNINFO,CODEPOS);
    }


    FuncField::FuncField(bool isStatic):isStatic(isStatic){
    }

    bool FuncField::rangeAsBlocked() const {
        com::TODO(FUNINFO,CODEPOS);
    }

    std::string FuncField::toLLVMIR() const {
        com::TODO("TODO",CODEPOS);
    }

    IRInstrPool::IRInstrPool(){
        funcPool.emplace_back(std::make_unique<FuncField>(true));
    }

    std::string IRInstrPool::toLLVMIR() const {
        com::TODO("TODO",CODEPOS);
    }

    FuncField * IRInstrPool::addEmptyFuncField() {
        funcPool.emplace_back(std::make_unique<FuncField>(false));
        return funcPool.rend()->get();
    }
    
    FuncField * IRInstrPool::getStaticFuncFieldPtr() const {
        return funcPool[0].get();
    }

    IRInstr * IRInstrPool::addIRInstr(const IRInstr & irInstr,FuncField * ptrFuncFieldToAdd){
        instrPool.emplace_back(
            com::dynamic_cast_unique_ptr<moeconcept::Cloneable,IRInstr>(
                irInstr.cloneToUniquePtr()
            )
        );
        return ptrFuncFieldToAdd->pushInstrOnBack(instrPool.rbegin()->get());
    }

}


//  IRInstr
//  Terminator Instructions
//      `ret`
//      `br`
namespace ircode{

    IRInstr_RET::IRInstr_RET(Addr * retAddr):retAddr(retAddr){
    }

    std::string IRInstr_RET::toLLVMIR() const {
        if(retAddr){
            return "ret "+retAddr->toLLVMIR();
        }else{
            return "ret void";
        }
    }
}
#endif
