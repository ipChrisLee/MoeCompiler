#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <string>
#include <functional>
#include <map>

#include <common.hpp>
#include <unittest.hpp>

namespace unitest{
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
            std::cout<<"Test ["<<name<<"] doesn't exist!"<<std::endl;
            return exitcode::INVALID_ARGUMENT;
        }else{
            return testCases[name]({});
        }
    }

    void TestMain::printAll(std::ostream & os, const std::string & sep) const {
        auto it=testCases.begin();
        if(it!=testCases.end()) {
            os<<it->first;++it;
            while(it!=testCases.end()){
                os<<sep<<it->first;++it;
            }
        }
    }
}
