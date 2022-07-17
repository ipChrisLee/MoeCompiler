//
// Created by lee on 6/26/22.
//
#ifdef TESTCODE

#include "gtest/gtest.h"

#include <iostream>

#define pfun() std::cout << __PRETTY_FUNCTION__ << std::endl

struct Base {
	Base() {
		pfun();
	}
	
	Base(const Base &) {
		pfun();
	}
};

struct Derived : public Base {
	Derived() {
		pfun();
	}
	
	Derived(const Derived & der) : Base(der) {
		pfun();
	}
	
};


TEST(Inherit, test1) {
	Base * pb1=new Derived();
	Base * pb2=new Base(*pb1);
}

#endif
