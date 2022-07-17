//
// Created by lee on 6/13/22.
//

#ifdef TESTCODE

#include "gtest/gtest.h"

#include <climits>

#include "mdb.hpp"

TEST(IntTest,maxV){
	int mx=INT_MAX;
	EXPECT_EQ(mx,-(-mx));
}

TEST(IntTest,minV){
	int mn=INT_MIN;
	EXPECT_EQ(mn,-mn);
}


#endif