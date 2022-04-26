#pragma once

#include <string>
#include <vector>

#include <common.hpp>

#include "frontend/IRAddr.hpp"
#include "frontend/frontendHeader.hpp"


namespace ircode{
    /*  class for IR instruction.
     *  Notice that, `IRInstr` has no control of pointers it saves, which means you should
     *  always save addresses in `AddressPool`, and then get T* from unique_ptr.
     * */
    class IRInstr : public LLVMable{
    private:
        int id;
        static int cnt;
    public:
        IRInstr():id(++cnt){}
    };


    /*  Class for static value init.
     * */
    class IRInstr_staticdef : public IRInstr{
    private:
        AddrStaticVar * pStaticVar; //  It contains pStaticVar->Add, but just add another pointer.
        AddrCompileConst * pInitValue;
    public:
        IRInstr_staticdef(AddrStaticVar* pStaticVar,AddrCompileConst*pInitValue);
        std::string toLLVMIR() const override;
        static int testStaticDefSubMain(const std::vector<std::string>&);
    };
}
