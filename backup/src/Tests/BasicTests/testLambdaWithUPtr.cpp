//
// Created by lee on 6/14/22.
//
#ifdef TESTCODE

#include "gtest/gtest.h"

std::unique_ptr<int> xx() {
	std::unique_ptr<int> up;
	std::cout << (unsigned long long) (&up) << std::endl;
	static auto fun = [&up]() { // Here is trap.
		up = std::make_unique<int>(2);
		std::cout << (unsigned long long) (&up) << std::endl;
	};
	fun();
	return up;
}

TEST(TestUPtr, test1) {
	GTEST_SKIP();
	auto res = xx();
	auto res2 = std::move(res);
	res = xx(); // BOOM!
	EXPECT_EQ(*res, 2);
}


#endif