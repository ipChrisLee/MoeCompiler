#pragma once

#include <string_view>
#include <functional>
#include <initializer_list>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <utility>
#include <memory>

namespace com{
    class MException : public std::exception {
    public:
        std::string msg=std::string();
        MException(){}
        MException(const std::string & msg):msg(msg){}
        const char * what() const noexcept {
            return msg.data();
        }
    };

    [[noreturn]] void TODO(std::string_view msg=std::string_view());

    /*  To handle code that hasn't been finished. This may avoid some bugs
     *  caused by coders forgetting to implement.
     *  Usually usage:
     *      com::notFinished(FUNINFO,CODEPOS);
     * */
    void notFinished(std::string_view msg=std::string_view(),std::string_view codepos=std::string_view());
    [[noreturn]] void Throw(std::string_view s=std::string_view());
    [[noreturn]] void ThrowSingletonNotInited(std::string_view className=std::string_view());

    /*  Take place in `assert`. This version use a bool value as assert condition.
     * */
    void Assert(bool b,const std::string & msg="",const std::string & codepos="");

    /*  Take place in `assert`. 
     *  This version use a function returning bool as assert condition.
     * */
    void Assert(std::function<bool(void)>fun,const std::string & msg="",const std::string & codepos="");

    /*  Struct for regex switch.
     * */
    struct RegexSwitchCase{
        const char * const regex;
        std::function<void(void)>fun;
    };
    void regSwitch(const std::string & str,std::initializer_list<RegexSwitchCase>);

    /*  bmeBrace = begin-middle-end brace
     *  Brace which let some execute-when-entering codes and execute-when-leaving
     *  codes come first in source codes.
     * */
    void bmeBrace(std::function<void(void)>begin,std::function<void(void)>end,std::function<void(void)>middle);

}

