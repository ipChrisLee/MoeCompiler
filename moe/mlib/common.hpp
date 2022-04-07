/*  cprt mdb com module is defined here.
 * */
#pragma once

#include <string_view>
#include <functional>
#include <initializer_list>
#include <string>
#include <unittest.hpp>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <utility>
#include <memory>

// https://zhuanlan.zhihu.com/p/141690132
#define STR(t) #t 
#define LINE_STR(v) STR(v)
#define CODEPOS "File : "  __FILE__  ", Line : "  LINE_STR(__LINE__)
//  https://stackoverflow.com/questions/733056/is-there-a-way-to-get-function-name-inside-a-c-function
#define FUNINFO __PRETTY_FUNCTION__

class cprt{  
    /*  We will use cprt to avoid using ostream explicitly.
     *  Methods defined here mimics java io unit design.
     *  cprt = colored print
     *  Defect reports:
     *      1.  cprt cannot handle pointer except char pointer. (e.g. `int *`,`double *`)
     *      2.  cprt cannot handle classes provided by other libs. (e.g. `vector`,`queue`)
     * */

public:
    struct TOS{
        std::string operator () (const char * s){
            return std::string(s);
        }

        std::string operator () (char * s){
            return std::string(s);
        }

        template<typename T> std::string operator () (T * p){
            return std::to_string((unsigned long long)p);
        }

        std::string operator () (std::string s){ return s; }

        std::string operator () (std::string_view s){ return std::string(s); }

        std::string operator () (int32_t i){ return std::to_string(i); }

        std::string operator () (char c){ return std::to_string(c); }

        std::string operator () (double d){ return std::to_string(d); }
        
        std::string operator () (int64_t i){ return std::to_string(i); }

        std::string operator () (uint32_t u){ return std::to_string(u); }

        std::string operator () (uint64_t u){ return std::to_string(u); }

        template<typename T>std::string operator () (T & t){
            return t.to_string();
        }
    };
    static TOS tos;

    /*  Colored output method.
     *  When using this method, you should only think about WHAT to print.
     *  You can decide WHERE to print by changing `std::ostream` and decide
     *  text decoration by `col`;
     *  Usage:
     *      cprt::cPrint(tuple(1,2,'a',"S",0.5,S()),std::cout,cprt::red);
     *  Ref:
     *      https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
     * */
    template<class ... Types>
    static void cprint(\
        std::tuple<Types...>t, /* Use std::tuple to package things to print. */\
        std::ostream & os=std::cout, /* Ostream. */\
        std::string_view col=iterct /* Decoration of text. */\
    ){
        os<<col;
        apply([&](auto && ... args){ ((os<<tos(args)),...); },t);
        os<<noc;
    }

    /*  Colored output method ended with '\n' and flow flush.
     *  When using this method, you should only think about
     *  HOW to print text by argument `col`.
     *  You can decide WHERE to print by changing `std::ostream`.
     *  Usage:
     *      cprt::cprintLn(tuple(1,2,'a',"S",0.5,S()),std::cout,cprt::red);
     * */
    template<class ... Types>
    static std::ostream & cprintLn(
        std::tuple<Types...>t, /* Use std::tuple to package things to print. */\
        std::ostream & os=std::cout, /* Ostream. */\
        std::string_view col=iterct /* Decoration of text. */\
    ){
        os<<col;
        apply([&](auto && ... args){ ((os<<tos(args)),...); },t);
        os<<noc<<std::endl;
        return os;
    }

    /*  Colored output method for one argument printting. 
     * */
    template<typename T>static void cprint(\
        T v, /* Text to print. */\
        std::ostream & os=std::cout, /* Ostream. */\
        std::string_view col=iterct /* Decoration of text. */\
    ){
        os<<col<<tos(v)<<noc;
    }

    /*  Colored output method for one argument printting ending with newline.
     * */
    template<typename T>static void cprintLn(\
        T v, /* Text to print. */\
        std::ostream & os=std::cout, /* Ostream. */\
        std::string_view col=iterct /* Decoration of text. */\
    ){
        os<<col<<tos(v)<<noc<<std::endl;
    }

    /*  Output method without decoration.
     *  When using this method, you should only think about WHAT to print.
     *  You can decide WHERE to print by changing `std::ostream` and decide
     *  text decoration by `col`;
     *  Usage:
     *      cprt::cprint(tuple(1,2,'a',"S",0.5,S()),std::cout,cprt::red);
     *  Ref:
     *      https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
     * */
    template<class ... Types>
    static void print(\
        std::tuple<Types...>t, /* Use std::tuple to package things to print. */\
        std::ostream & os=std::cout /* Ostream. */\
    ){
        apply([&](auto && ... args){ ((os<<tos(args)),...); },t);
    }

    /*  Colored output method ended with '\n' and flow flush.
     *  When using this method, you should only think about
     *  HOW to print text by argument `col`.
     *  You can decide WHERE to print by changing `std::ostream`.
     *  Usage:
     *      cprt::cprintLn(tuple(1,2,'a',"S",0.5,S()),std::cout,cprt::red);
     * */
    template<class ... Types>
    static std::ostream & printLn(
        std::tuple<Types...>t, /* Use std::tuple to package things to print. */\
        std::ostream & os=std::cout /* Ostream. */\
    ){
        apply([&](auto && ... args){ ((os<<tos(args)),...); },t);
        os<<std::endl;
    }

    /*  Colored output method for one argument printting. 
     * */
    template<typename T>static void print(\
        T v, /* Text to print. */\
        std::ostream & os=std::cout /* Ostream. */\
    ){
        os<<tos(v);
    }

    /*  Colored output method for one argument printting ending with newline.
     * */
    template<typename T>static void printLn(\
        T v, /* Text to print. */\
        std::ostream & os=std::cout /* Ostream. */\
    ){
        os<<tos(v)<<std::endl;
    }
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
    /*  Moe debug tool. You can disable some mdb tool for any time.
     *  When you initialzing one instance, you should consider WHERE and print format
     *  by setting `std::ostream & os` (for where and settings such as double format)
     *  and `std::string_view col` (for decoration of text).
     *  However, when you using this model, you should only think of WHAT to debug.
     *
     *  <private>::
     *      bool enable : whether this mdb is enabled.
     *      std::ostream & outs : output stream for this mdb.
     *          NOTICE: If you want to print to file, please let filename end with '.log'
     *      std::string_view headstr : headstr of very lines. '[mdb] ' is default.
     *      std::string_view col : color of text. See 'cprt::'. `cprt::warning` is default. 
     *  <static private>::
     *      bool sysEnable : whehter ALL mdb is enabled.
     * */
private:
    bool enable;
    std::ostream & outs;
    const std::string headstr;
    const std::string_view col;
    static bool sysEnable;
public:
    mdb(\
        std::ostream & outs,/* out stream, can be ofstream (can be closed automatically) */\
        bool enable=false,/* default enable status */\
        const std::string_view & headstr="[mdb] ",/* headstr of every line */\
        const std::string_view & col=cprt::warning/* color of text */\
       ):enable(enable),outs(outs),headstr(headstr),col(col){
    }
    ~mdb(){
        outs.flush();
        // For ofstream, as its deconstructure, it will invoke close().
        // So there is no need to close manually.
    }
    mdb(const mdb &)=delete;
    mdb & operator=(const mdb &)=delete;

    /*  Get a static mdb tool, which use std::cout as output stream, disabled default. */
    static mdb & getStatic(){
        static mdb single(std::cout);return single;
    }
    
    /*  method to enable mdb or disable it. */
    bool & setEnable(){
        return enable;
    }

    /*  method to enable all mdb or disable them. */
    static bool & setSysEnable(){
        return sysEnable;
    }

    /*  method to print messages to outs stream. It will detect '\n' and print 
     *  newline (and flush stream) and head string (e.g. '[mdb] ') for every '\n'
     *  and at the end of printing.
     *  if you don't want text to be decorated, set decorated=false;
     *  detect enable flag automaticlly.
     *  to print the location of message, let printLoc=CODEPOS. (see defination of CODELOC)
     * */
    void msgPrint(\
        const std::string & msg, /* Message to print. */\
        const std::string & printLoc="", /* If not empty, print location at beginning */ \
        bool decorated=true\
    ){
        if(enable && sysEnable) {
            if(decorated) {
                if(printLoc.length()) cprt::cprintLn(headstr+printLoc,outs,col);
            }else{
                if(printLoc.length()) cprt::printLn(headstr+printLoc,outs);
            }
            std::string buf=headstr;
            for(auto ch:msg){
                if(ch=='\n'){
                    if(decorated) cprt::cprintLn(buf,outs,col);
                    else cprt::printLn(buf,outs);
                }else{
                    buf.push_back(ch);
                }
            }
            if(decorated) cprt::cprintLn(buf,outs,col);
            else cprt::printLn(buf,outs);
        }

    }

    /*  method to print 'list' of 'pair' of string and object. One line one object.
     *  it will print head string (e.g. '[mdb] ') for every line.
     *  it will noT print newline after the end of string.
     *  you should limit the count of the lines of S.to_string() to one.
     *  if you don't want text to be decorated, set decorated=false;
     *  detect enable flag automaticlly.
     *  to print the location of message, let printLoc=CODEPOS. (see defination of CODELOC)
     *  usage : 
     *      mdb::getStatic()
     *          .infoPrint(std::tuple( std::pair("n",n),(std::pair("t",t)) ),CODEPOS);
     * */
    template<typename Type>
    void infoPrint(\
        std::pair<const char *,Type>t, /* Message to print. */\
        const std::string & printLoc="", /* If not empty, print location at beginning */ \
        bool decorated=true\
    ) {
        if(decorated){
            if(printLoc.length()) cprt::cprintLn(headstr+printLoc,outs,col);
            cprt::cprintLn(headstr+std::string(t.first)+" : "+cprt::tos(t.second),outs,col);
        }else{
            if(printLoc.length()) cprt::printLn(headstr+printLoc,outs);
            cprt::printLn(headstr+std::string(t.first)+" : "+cprt::tos(t.second),outs);
        }
    }

    /*  method to print 'list' of 'pair' of string and object. One line one object.
     *  it will print head string (e.g. '[mdb] ') for every line.
     *  it will noT print newline after the end of string.
     *  you should limit the count of the lines of S.to_string() to one.
     *  if you don't want text to be decorated, set decorated=false;
     *  detect enable flag automaticlly.
     *  to print the location of message, let printLoc=CODEPOS. (see defination of CODELOC)
     *  usage : 
     *      mdb::getStatic()
     *          .infoPrint(std::tuple( std::pair("n",n),(std::pair("t",t)) ),CODEPOS);
     * */
    template<typename ... Types>
    void infoPrint(\
        std::tuple<std::pair<const char *,Types>...>t, /* Message to print. */\
        const std::string & printLoc="", /* If not empty, print location at beginning */ \
        bool decorated=true\
    ) {
        if(decorated){
            if(printLoc.length()) cprt::cprintLn(headstr+printLoc,outs,col);
            apply([&](auto && ... args){ 
                ((cprt::cprintLn(headstr+std::string(args.first)+" : "+cprt::tos(args.second),outs,col)),...);
            },t);
        }else{
            if(printLoc.length()) cprt::printLn(headstr+printLoc,outs);
            apply([&](auto && ... args){ 
                ((cprt::printLn(headstr+std::string(args.first)+" : "+cprt::tos(args.second),outs)),...);
            },t);
        }
    }

};

namespace returncode{
    enum{
        NORMAL, /* all right */
        FAULT_USAGE, /* use code in wrong way */
        NOT_IMPLEMETED, /* have not been implemented */
        INVALID_ARGUMENT, /* function get invalid arguments */
        TEST_FAILED, /* failed to pass some test, since fault complement */
        TESTING /* just testing */
    };
}


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

    [[noreturn]] void TODO(const std::string_view msg=std::string_view());

    /*  To handle code that hasn't been finished. This may avoid some bugs
     *  caused by coders forgetting to implement.
     *  Usually usage:
     *      com::notFinished(FUNINFO,CODEPOS);
     * */
    void notFinished(const std::string_view msg=std::string_view(),const std::string_view codepos=std::string_view());
    [[noreturn]] void Throw(const std::string_view s=std::string_view());
    [[noreturn]] void ThrowSingletonNotInited(const std::string_view className=std::string_view());

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

namespace unitest{
    /*  Test of regexSwitch. */
    /*  Need to be edit.*/
    int regexSwitchTest(std::vector<std::string>);
    /*  Issue : constructor attribute function can NOT use cprt.
     *  */
    __attribute__((constructor)) bool comTestAdded();
}

