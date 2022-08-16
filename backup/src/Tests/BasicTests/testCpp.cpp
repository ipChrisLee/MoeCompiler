//
// Created by lee on 6/29/22.
//
#ifdef TESTCODE

#include <functional>

#include "gtest/gtest.h"


TEST(testCpp, testRefWrapper) {
	int x = 1;
	auto rx = std::ref(x);
}

#endif