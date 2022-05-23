#include "frontend/frontendHeader.hpp"

namespace ircode{
    std::string LLVMable::toLLVMIR() const {
        com::Throw("`toLLVMIR` method of some class is not implemented!");
    }
    std::string floatToString(float f){
        static char buf[50];
        sprintf(buf,"%a",f);
        return buf;
    }
    std::string intToString(int i){
        static char buf[50];
        sprintf(buf,"%d",i);
        return buf;
    }
}
