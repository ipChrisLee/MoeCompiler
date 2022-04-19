#include <string>
#include <regex>
#include <stdexcept>
#include <iostream>

#include "common.hpp"
#include "cprt.hpp"

//bool mdb::sysEnable=false;

namespace com{
    [[noreturn]] void Throw(const std::string_view msg){
        throw MException("Moe Exception! {"+std::string(msg)+"}");
    }
    [[noreturn]] void ThrowSingletonNotInited(const std::string_view className){
        com::Throw("Using uninited singleton ["+std::string(className)+"]"); 
    }
    [[noreturn]] void TODO(const std::string_view msg){
        Throw("Not implemeted error by \'TODO()\'["+std::string(msg)+"].");
    }
    void notFinished(const std::string_view msg,const std::string_view codepos){
        com::ccerr.cprintLn(std::tuple("Not finished code [",msg,"] in ",codepos));
    }
    void Assert(bool b,const std::string & msg,const std::string & codepos){
        if(!b) {
            std::string buf;
            if(codepos.length()) buf+="Error in "+codepos+".";
            buf+=" Message : ["+msg+"]";
            com::Throw(buf);
        }
    }

    void Assert(std::function<bool(void)>fun,const std::string & msg,const std::string & codepos){
        if(!fun()) {
            std::string buf;
            if(codepos.length()) buf+="Error in "+codepos+".";
            buf+=" Message : ["+msg+"]";
            Throw(buf);
        }
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

