//
// Created by lee on 6/11/22.
//

#ifdef TESTCODE

#include "gtest/gtest.h"

#include "common.hpp"

#include "mir/support/StaticValue.hpp"

TEST(TestMLib, TestUnaryVariant) {
	com::UnaryVariant<std::unique_ptr<int>, std::unique_ptr<float>> uv;
	EXPECT_FALSE(uv.savedSomething());
	uv.save(std::make_unique<int>(1));
	EXPECT_THROW(
		{
			EXPECT_TRUE(uv.savedSomething());
			uv.save(std::make_unique<int>(2));
		}, com::MException);
	EXPECT_THROW(
		{
			EXPECT_TRUE(uv.savedSomething());
			uv.restore<std::unique_ptr<float>>();
		}, com::MException);
	EXPECT_NO_THROW(
		{
			auto p = uv.restore<std::unique_ptr<int>>();
			EXPECT_EQ(*p, 1);
			EXPECT_FALSE(uv.savedSomething());
		});
}

TEST(TESTMLib, TestUnaryVariant_StaticValue) {
	com::UnaryVariant<std::unique_ptr<ircode::StaticValue>> uv;
	uv.save(
		com::dynamic_cast_uPtr<ircode::StaticValue>(
			std::make_unique<ircode::IntStaticValue>(1)));
	auto p = uv.restore<std::unique_ptr<ircode::StaticValue>>();
	EXPECT_EQ(dynamic_cast<ircode::IntStaticValue *>(p.get())->value, 1);
	EXPECT_FALSE(uv.savedSomething());
}

#endif