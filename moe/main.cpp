#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <unittest.hpp>
#include <common.hpp>

namespace unitest{
    /* main function of unitest. */
    int main(\
        std::vector<std::string>& toTest, /* names of test case */\
        bool toPrintAll=false /* whether to print all the name of test cases */
    ){
        if(toPrintAll){
            std::cout<<"All Test Cases: ";
            TestMain::get().printAll(std::cout);
            std::cout<<std::endl;
        }
        mdb::getStatic().print([&](){
                std::string r="value of test names:";for(auto s:toTest) r+=s;return r;
        });
        mdb::getStatic().setEnable()=false;
        for(auto name : toTest){
            int ret=TestMain::get().test(name);
            if(ret!=exitcode::NORMAL){
                std::cout<<"Test ["<<name<<"] failed!"<<std::endl;
                return exitcode::TEST_FAILED;
            }
        }
        std::cout<<"All test passed!"<<std::endl;
        return 0;
    }
}

int main(int argc,char ** argv){
    /* Inkove different functions by command line.
     * For compile: 'compiler testcase.sysy -S -o testcase.s'
     * */
    if(argc>3 && !strcmp(argv[2],"-S")){
        com::TODO();
        return 0;
    }
    mdb::getStatic().setEnable()=true;
    int opt;
    while((opt=getopt(argc,argv,":t:")) != -1){
        switch (opt){
            case 't': // -t -p regexSeitchTest ...
            {
                mdb::getStatic().print([&](){
                        std::string r;
                        r+="argc : "+std::to_string(argc)+"\n";
                        r+="optarg : "+std::string(optarg)+"\n";
                        return r;
                });
                bool toPrintAll=false;
                if(!strcmp(optarg,"-p")){
                    toPrintAll=true;
                }
                std::vector<std::string>list;
                for(;optind<argc;++optind){
                    list.push_back(argv[optind]);
                }
                return unitest::main(list,toPrintAll);
            }
            default:
                std::cerr<<"Error when parsing command line arguments"<<std::endl;
                exit(exitcode::FAULT_USAGE);
        }

    }
    return 0;
}
