#pragma once 
#include <set>
#include <string>
#include <fstream>
#include <memory>
#include <iostream>

#include <common.hpp>

class sysy{
    /*  Singleton. Here store some sysy system variables.
     *  */
private:
    std::ifstream inStream;
    std::ofstream outStream;
    static std::unique_ptr<sysy>single;
    sysy(const std::string & inFilePath,const std::string & outFilePath):
        inStream(inFilePath),outStream(outFilePath){
    }
public:
    sysy()=delete;
    sysy & operator = (const sysy &) = delete;
    ~sysy(){
        inStream.close();outStream.close();
    }
    void resetIn(){
        inStream.seekg(0);
    }
    std::istream & getInStream(){
        return inStream;
    } 
    std::ostream & getOutStream(){
        return outStream;
    }
    static void init(const std::string & inFilePath,const std::string & outFilePath){
        single=std::unique_ptr<sysy>(new sysy(inFilePath,outFilePath));
    }
    static sysy & get(){
        if(single==nullptr){
            com::ThrowSingletonNotInited("sysy");
        }
        return *single;
    }
    static const std::set<std::string>reservedWords;
    static const std::set<std::string>operators;
    static const std::set<std::string>delimiter;
    static const std::set<char>blanks;
};

