#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <tuple>
#include <unittest.hpp>
#include <common.hpp>

namespace unitest{
    /* main function of unitest. */
    int main(int argc,char ** argv){
        std::vector<std::string> toTest;/* names of test case */
        bool toPrintAll=false;/* whether to print all the name of test cases */
        int opt;
        while( (opt=getopt(argc,argv,":p")) != -1 ){
            switch(opt){
                case 'p':
                    {
                        toPrintAll=true;break;
                    }
                default:
                    cprt::cprintLn("Error when parsing command line arguments in unittest::main" ,std::cerr,cprt::err);
                    exit(exitcode::FAULT_USAGE);
            }
        }
        mdb::getStatic().infoPrint(std::tuple(
            std::pair("optind",optind),
            std::pair("optopt",optopt),
            std::pair("opterr",opterr),
            std::pair("argc",argc)
        ),CODEPOS);
        for(;optind<argc;++optind) toTest.push_back(argv[optind]);
        if(toPrintAll) {
            cprt::cprint("All test units : ");
            cprt::cprintLn(unitest::TestMain::get().getAll());
        }

        cprt::cprint("To test : ");
        for(auto s : toTest) cprt::cprint(s+" ; ");
        cprt::cprintLn("");
        for(auto name : toTest){
            int ret=TestMain::get().test(name);
            if(ret!=exitcode::NORMAL){
                cprt::cprintLn("Test ["+name+"] failed!");
                return exitcode::TEST_FAILED;
            }
        }
        cprt::cprintLn("All test passed!");
        return 0;
    }
}

int Main(int argc,char ** argv){
    /* Inkove different functions by command line.
     * */
    int opt;
    while( (opt=getopt(argc,argv,":tdS")) != -1 ){
        mdb::getStatic().msgPrint("???");
        switch (opt){
            case 't': 
            {
                return unitest::main(argc,argv);
            }
            case 'd':
            {
                mdb::setSysEnable()=true;
                mdb::getStatic().setEnable()=true;
                break;
            }
            case 'S':
            {
                com::TODO("Compiler not implemented.");
            }
            default:
                std::cerr<<"Error when parsing command line arguments"<<std::endl;
                exit(exitcode::FAULT_USAGE);
        }
    }
    if(optind>=argc){
        std::cerr<<"Expected argument after options"<<std::endl;
        exit(exitcode::FAULT_USAGE);
    }
    return 0;
}

int main(int argc,char ** argv){
    // For better exception handle.
    // https://www.digitalpeer.com/blog/find-where-a-cpp-exception-is-thrown
    /*  To get where an exception is thorwn, make the files, and gdb it.
     *  Then type `catch throw` and `run` and `where`.
     *  For this project, gdb is called with '-ex="catch throw"' for the target `gdb`.
     *  So there is no need to type `catch throw` any more.
     * */
    try{
        return Main(argc,argv);
    }catch(const std::exception & e){
        std::cerr<<e.what()<<std::endl;
    }
}
