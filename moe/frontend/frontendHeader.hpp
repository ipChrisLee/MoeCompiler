#pragma once

#include <string>

#include <common.hpp>

namespace ircode{
    struct LLVMable{
        virtual std::string toLLVMIR() const = 0;
    };
}
