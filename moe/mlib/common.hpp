#pragma once

#include <string_view>
#include <functional>
#include <initializer_list>
#include <string>
#include <unittest.hpp>
#include <map>
#include <iostream>
#include <fstream>

struct tcolor{  
    static constexpr std::string_view black=   "\033[0;30m";
    static constexpr std::string_view red=     "\033[0;31m";
    static constexpr std::string_view err=     "\033[0;31m";
    static constexpr std::string_view green=   "\033[0;32m";
    static constexpr std::string_view fin=     "\033[0;32m";
    static constexpr std::string_view yellow=  "\033[0;33m";
    static constexpr std::string_view iterct=  "\033[0;33m";
    static constexpr std::string_view blue=    "\033[0;34m";
    static constexpr std::string_view purple=  "\033[0;35m";
    static constexpr std::string_view cyan=    "\033[0;36m";
    static constexpr std::string_view working= "\033[0;36m";
    static constexpr std::string_view bpurple= "\033[1;35m";
    static constexpr std::string_view warning= "\033[1;35m";
    static constexpr std::string_view noc=     "\033[0m";
};

class mdb{ 
    /* moe debug tool 
     * You can disable some mdb tool for any time.
     * <private>::
     *     bool enable : whether this mdb is enabled.
     *     std::ostream & outs : output stream for this mdb.
     * */
private:
    bool enable;
    std::ostream & outs;
    const std::string_view headstr;
    const std::string_view col;
public:
    mdb(\
        std::ostream & outs,/* out stream, can be ofstream */\
        bool enable=false,/* default enable status */\
        const std::string_view & headstr="[mdb] ",/* headstr of every line */\
        const std::string_view & col=tcolor::warning/* color of text */\
    ):enable(enable),outs(outs),headstr(headstr),col(col){}
    ~mdb(){
        outs.flush();
        // For ofstream, as its deconstructure, it will invoke close().
        // So there is no need to close manually.
    }
    mdb(const mdb &)=delete;
    mdb & operator=(const mdb &)=delete;

    /* Get a static mdb tool, which use std::cout as output stream, disabled default. */
    static mdb & getStatic(){
        static mdb single(std::cout);return single;
    }
    
    /* Method to enable mdb or disable it. */
    bool & setEnable(){
        return enable;
    }

    /* Method to print content to outs stream, use 'sep' as seperate string in the end.
     * Detect enable flag automaticlly.
     * */
    void print(\
        std::function<std::string(void)>fun /* variable handling function  */
    ) {
        if(enable) {
            bool lastLine=false;
            for(char ch:fun()){
                if(ch=='\n'){
                    outs<<tcolor::noc<<std::endl;outs.flush();lastLine=true;
                }else{
                    if(lastLine) outs<<col<<headstr;
                    outs<<ch;lastLine=false;
                }
            }
            if(!lastLine) {
                outs<<tcolor::noc<<std::endl;outs.flush();
            }
        }
    }

};

namespace exitcode{
    enum{
        NORMAL, /* all right */
        FAULT_USAGE, /* use code in wrong way */
        NOT_IMPLEMETED, /* have not been implemented */
        INVALID_ARGUMENT, /* function get invalid arguments */
        TEST_FAILED /* failed to pass some test, since fault complement */
    };
}


namespace com{
    void TODO(const std::string_view s=std::string_view());
}

namespace com{
    struct RegexSwitchCase{
        const char * const regex;
        std::function<void(void)>fun;
    };
    void regSwitch(const std::string & str,std::initializer_list<RegexSwitchCase>);
}

namespace unitest{
    /* Test of regexSwitch. */
    int regexSwitchTest(std::vector<std::string>);
    static bool comTestAdded=
        unitest::TestMain::get().addTestCase("regexSwitchTest",regexSwitchTest);
}


