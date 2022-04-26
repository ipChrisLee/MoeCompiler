#include "frontend/IRInstr.hpp"

#include <submain.hpp>

namespace ircode{
    int IRInstr::cnt=0;

    std::string IRInstr_staticdef::toLLVMIR() const {
        if(pStaticVar->isconst()){
            return pStaticVar->toLLVMIR()+" = dso_local constant "+pInitValue->toLLVMIR();
        }else{
            return pStaticVar->toLLVMIR()+" = dso_local "+pInitValue->toLLVMIR();
        }
    }

    IRInstr_staticdef::IRInstr_staticdef(AddrStaticVar * pStaticVar,AddrCompileConst*pInitValue)
        :pStaticVar(pStaticVar),pInitValue(pInitValue){
        pStaticVar->setCompileConst(pInitValue);
    };

    AddSubMain(testStaticDefSubMain,IRInstr_staticdef::testStaticDefSubMain);

    int IRInstr_staticdef::testStaticDefSubMain(const std::vector<std::string>&){
        AddrCompileConst::IntInfo i0("0"),i1("1"),i2("2");
        AddrCompileConst::IntArrayInfo ar4(4,{i0,i1,i2});
        AddrCompileConst::IntArrayInfo ar3_4(3,{4},{ar4,ar4});
        AddrCompileConst::IntArrayInfo ar2_3_4(2,{3,4},{ar3_4,ar3_4});
        AddrCompileConst acc(std::make_unique<AddrCompileConst::IntArrayInfo>(ar2_3_4));
        AddrStaticVar var("arr",&acc,false);
        IRInstr_staticdef instr(&var,&acc);
        com::ccout.cprintLn(instr.toLLVMIR());
        return 0;
    }
}
