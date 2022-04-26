#pragma once

#include <algorithm>
#include <list>
#include <string>
#include <memory>

#include "common.hpp"

namespace ircode{


    class HIRList{
    public:
        class HIR{
        public:
            int id;
            static int cnt;
            virtual std::string toLLVMIR(){ com::Throw("You Should Implement HIR::toLLVMIR!"); };
        };
        using List=std::list<std::unique_ptr<HIR>>;
        class HIR_TerminatorInstr : public HIR{
        public:
            
        };
        
    };

}
