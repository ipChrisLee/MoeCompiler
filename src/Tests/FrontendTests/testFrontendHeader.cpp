//
// Created by lee on 6/13/22.
//
#ifdef TESTCODE

#include <climits>
#include <cinttypes>

#include "gtest/gtest.h"

#include "frontend/helper.hpp"

TEST(test_stringToInt, minV) {
	int32_t imn = -(int32_t(0x80000000));
	EXPECT_EQ(imn, -imn);
	
	std::vector<int>vec={
		mir::literalToInt("2147483648"),
		-mir::literalToInt("2147483648"),
		mir::literalToInt("0x80000000"),
		mir::literalToInt("020000000000"),
	};
	for(auto x:vec){
		EXPECT_EQ(x,imn);
	}
}

TEST(test_literalToFloat,regular){
	// TODO: how to test literalToFloat function?
}

#endif