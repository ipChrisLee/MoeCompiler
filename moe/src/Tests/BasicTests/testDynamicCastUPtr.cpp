//
// Created by lee on 6/12/22.
//

#ifdef TESTCODE

#include "gtest/gtest.h"

#include "common.hpp"
#include "frontend/StaticValue.hpp"

TEST(DynamicCastUPtrTest, test1) {
	std::unique_ptr<ircode::StaticValue> up = std::make_unique<ircode::IntStaticValue>(
		1);
	EXPECT_THROW(
		{
			com::dynamic_cast_uPtr<ircode::FloatStaticValue>(std::move(up));
		}, com::MException);
	up = std::make_unique<ircode::IntStaticValue>(2);
	EXPECT_EQ(com::dynamic_cast_uPtr<ircode::IntStaticValue>(std::move(up))->value,
	          2);
	up = std::make_unique<ircode::IntStaticValue>(3);
	EXPECT_THROW(
		{
			com::dynamic_cast_uPtr<ircode::FloatStaticValue>(std::move(up));
		}, com::MException);
	
	up = std::make_unique<ircode::IntStaticValue>(4);
	EXPECT_EQ(com::dynamic_cast_uPtr<ircode::IntStaticValue>(up)->value,4);
	EXPECT_NE(up.get(),nullptr);
	EXPECT_EQ(com::dynamic_cast_uPtr<ircode::IntStaticValue>(std::move(up))->value,
	          4);
	EXPECT_EQ(up.get(),nullptr);
}


#endif