#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <iostream>

#include <common.hpp>
#include <unittest.hpp>

namespace unitest{
    int main(const std::string & toInvoke){
        int ec=TestMain::get().test(toInvoke);
        cprt::cprintLn("unittest::main finished. Exit code of function: "+std::to_string(ec));
        return 0;
    }
    bool TestMain::addTestCase(const std::string & name,testFun_t fun){
        if(testCases.count(name)){
            return false;
        }else{
            testCases[name]=fun;
            return true;
        }
    }

   int TestMain::test(const std::string & name){
        if(!testCases.count(name)){
            cprt::cprintLn(std::tuple{"Test [",name,"] doesn't exist"},std::cerr,cprt::err);
            return returncode::INVALID_ARGUMENT;
        }else{
            return testCases[name]({});
        }
    }

   std::string TestMain::getAll(const std::string & sep) const {
       std::string re;
        auto it=testCases.begin();
        if(it!=testCases.end()) {
            re+=it->first;++it;
            while(it!=testCases.end()){
                re+=sep+it->first;++it;
            }
        }
        return re;
    }
}
