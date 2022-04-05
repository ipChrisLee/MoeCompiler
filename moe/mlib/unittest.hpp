#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace unitest{
    /*  testFun_t is a function type for test function. 
     *  To add uniitest:
     *  namespace unitest{
     *      int regexSwitchTest(std::vector<std::string>); // test function
     *      __attribute__((constructor)) bool comTestAdded(); // unitest::TestMain::get().add
     *  }
     * */
    using testFun_t=std::function<int(std::vector<std::string>)>;

    /* Main function for unit testing. This struct is not constructable. (i.e. Singleton)
     * Use TestMain::get() to get the only 'TestMain'.
     * */
    class TestMain{
    private:
        std::map<std::string,testFun_t>testCases;
        TestMain(){}
    public:
        static TestMain & get(){
            static TestMain instance;return instance;
        }

        TestMain(TestMain const &)=delete;
        void operator=(TestMain const &)=delete;

        /* Add a test case. 
         * To add a testUnit before main, declare a 'bool __added' whose
         * value equals TestMain::get().addTestCase(...).
         * return: true if testUnit is succesfull added.
         * */
        bool addTestCase( \
            const std::string &, /* test name */ \
            testFun_t /* test function */ \
        );

        /* Test specific unit by calling specific function. 
         * For unknown test name, it will return exitcode::INVALID_ARGUMENT.
         * For known test name, it will call the test function and return its return value.
         * */
        int test( \
            const std::string & /* test name */ \
        );

        /* Get all unit test names, seperated by sep. */
        std::string getAll(
            const std::string & sep=", " /* sep string */
        ) const ;
    };
    int main(const std::string & toInvoke=std::string());
}


