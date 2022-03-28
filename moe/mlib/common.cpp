#include <common.hpp>
#include <string>
#include <regex>
#include <stdexcept>
#include <iostream>


namespace com{
    void TODO(const std::string_view msg){
        std::cerr<<"Not Implemented!"<<std::endl;
        throw std::logic_error("Not implemeted error by \'TODO()\'"+std::string(msg)+"");
    }
}

void com::regSwitch(const std::string & str,std::initializer_list<RegexSwitchCase> cases){
    for(auto kase:cases){
        if(std::regex_match(str,std::regex(kase.regex))) {
            kase.fun();break;
        }
    }
}

namespace unitest{
    int regexSwitchTest(std::vector<std::string>){
        std::string s="123";
        com::regSwitch(s, {
            { "[0-9]+", [&](){ std::cout<<"match \"[0-9]+\""<<std::endl; } },
            { "[a-z]+", [&](){ std::cout<<"match \"[a-z]+\""<<std::endl; } },
            { "[A-Z]+", [&](){ std::cout<<"match \"[A-Z]+\""<<std::endl; } },
            { "*", [&](){ std::cout<<"match failed"<<std::endl; } }
        }); 
        return 0;
    }
}


