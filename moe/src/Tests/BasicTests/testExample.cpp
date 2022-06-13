#ifdef TESTCODE

#include "gtest/gtest.h"

#include "mtype.hpp"

TEST(Example,test_mtype){
	GTEST_SKIP_("This is just for example.");
	mtype::quat_t q1(mtype::qTrue),q2(mtype::qFalse);
	EXPECT_EQ(q1&&q2,mtype::qFalse);
}

#endif