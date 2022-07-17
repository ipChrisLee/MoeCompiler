//
// Created by lee on 6/13/22.
//
#ifdef TESTCODE

#include <climits>
#include <cinttypes>

#include "gtest/gtest.h"

#include "frontend/frontendHeader.hpp"

TEST(test_stringToInt, minV) {
	int32_t imn = -(int32_t(0x80000000));
	EXPECT_EQ(imn, -imn);
	
	std::vector<int>vec={
		ircode::literalToInt("2147483648"),
		-ircode::literalToInt("2147483648"),
		ircode::literalToInt("0x80000000"),
		ircode::literalToInt("020000000000"),
	};
	for(auto x:vec){
		EXPECT_EQ(x,imn);
	}
}

TEST(test_literalToFloat,regular){
	// TODO: how to test literalToFloat function?
}

#endif