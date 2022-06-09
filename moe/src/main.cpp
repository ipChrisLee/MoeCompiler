#include <unistd.h>
#include <vector>
#include <memory>

#include "cprt.hpp"
#include "common.hpp"
#include "mdb.hpp"


int Main(int argc,char ** argv){
    /* Invoke different functions by command line.
     * */
    
    if(argc<2){
        com::Throw("You should specify input file path.");
    }
    int opt;
    std::string inFilePath=argv[1],outFilePath,subFunName;
    optind=2;
    char cmd='\0';
    while( (opt=getopt(argc,argv,":dlpsiSf:o:")) != -1 ){
        switch (opt){
            case 'd':
                com::mdb::setSysEnable()=true;
                com::ccdbg.setEnable()=true;
                break;
            case 'l':
                cmd='l';break;
            case 'f':
                subFunName=optarg;cmd='f';
                break;
            case 'S':
                cmd='S';
                com::TODO("Compiler not implemented.");
            case 'o':
                outFilePath=optarg;
                break;
            default:
                com::Throw("Error when parsing command line arguments");
        }
    }
    if(optind>argc){ com::Throw("Expected argument after options"); }
    com::Assert(!outFilePath.empty(),"-o should have argument.",CODEPOS);

    switch(cmd){
        case 'f':   {
            std::vector<std::string>subMainArgs;
            subMainArgs.push_back(inFilePath);
            subMainArgs.push_back(outFilePath);
            break;
        }
        //case 'l':
            //com::TODO("Not support -l for now.");
            //break;
        //case 'p':
            //com::TODO("Not support -p for now.");
            //break;
        //case 's':
            //com::TODO("Not support -s for now.");
            //break;
        //case 'i':
            //com::TODO("Not support -i for now.");
            //break;
        //case 'S':
            //com::TODO("Not support -S for now.");
            //break;
        default:
            com::Throw("No command is specified.");
    }
    return 0;
}

int main(int argc,char ** argv){
    //  For better exception handle.
    //  https://www.digitalpeer.com/blog/find-where-a-cpp-exception-is-thrown
    /*  To get where an exception is thrown, make the files, and gdb it.
     *  Then type `catch throw` and `run` and `where`.
     *  For this project, gdb is called with '-ex="catch throw"' for the target `gdb`.
     *  So there is no need to type `catch throw` anymore.
     * */
    try{
        return Main(argc,argv);
    }catch(const std::exception & e){
        com::ccerr.cprintLn(e.what());
        return -1;
    }
}
