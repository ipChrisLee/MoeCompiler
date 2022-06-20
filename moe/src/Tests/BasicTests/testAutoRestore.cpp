//
// Created by lee on 6/18/22.
//

#ifdef TESTCODE

#include <iostream>
#include <memory>

#include "gtest/gtest.h"

#include "common.hpp"
#include "mdb.hpp"

struct TT {
	std::unique_ptr<int> p;
	
	explicit TT(int x) : p(std::make_unique<int>(x)) {
		std::cout << FUNINFO << std::endl;
	}
	
	TT(TT && tt) noexcept : p(std::move(tt.p)) {
		std::cout << FUNINFO << std::endl;
	}
	
	TT & operator =(TT && tt) noexcept {
		p = std::move(tt.p);
		std::cout << FUNINFO << std::endl;
		return *this;
	}
	
	~TT() {
		std::cout << FUNINFO << std::endl;
	}
};

static TT t(1);
TEST(testAutoRestorer, TEST1) {
	std::cout << *t.p << std::endl;
	setWithAutoRestorer(t, TT(2));
	std::cout << *t.p << std::endl;
	setWithAutoRestorer(t, TT(3));
	std::cout << *t.p << std::endl;
}

#endif