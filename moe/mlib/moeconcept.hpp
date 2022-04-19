#pragma once 

#include <string>

namespace moeconcept{
    struct Stringable{
        virtual std::string toString() const = 0;
    };
}
