#include <common.hpp>
#include <string>
#include <regex>
#include <stdexcept>
#include <iostream>

bool mdb::sysEnable=false;
cprt::TOS cprt::tos=cprt::TOS();

namespace com{
    void TODO(const std::string_view msg){
        cprt::cprintLn("Not Implemented!",std::cerr,cprt::err);
        throw std::logic_error("Not implemeted error by \'TODO()\', msg["+std::string(msg)+"].");
    }
    void bmeBrace(std::function<void(void)>begin,std::function<void(void)>end,std::function<void(void)>middle){
        begin();middle();end();
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


