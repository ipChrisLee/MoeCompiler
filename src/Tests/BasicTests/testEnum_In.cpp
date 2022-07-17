//
// Created by lee on 6/29/22.
//

#ifdef TESTCODE

#include "gtest/gtest.h"

#include <common.hpp>

enum class EC {
	a, b, c, d
};

enum E {
	a, b, c, d
};

TEST(enum_fun_in, test1) {
	EC x = EC::a;
	EXPECT_TRUE(com::enum_fun::in(x, {EC::a, EC::b, EC::c}));
	EXPECT_TRUE(com::enum_fun::in(x, {EC::a}));
	EXPECT_FALSE(com::enum_fun::in(x, {EC::b, EC::c}));
	EXPECT_FALSE(com::enum_fun::in(x, { }));
	E y = E::a;
	EXPECT_TRUE(com::enum_fun::in(y, {E::a, E::b, E::c}));
	EXPECT_TRUE(com::enum_fun::in(y, {E::a}));
	EXPECT_FALSE(com::enum_fun::in(y, {E::b, E::c}));
	EXPECT_FALSE(com::enum_fun::in(y, { }));
}


#endif
