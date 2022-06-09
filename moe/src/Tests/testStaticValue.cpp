#ifdef TESTCODE

#include "gtest/gtest.h"

#include "frontend/StaticValue.hpp"

TEST(StaticValue, add) {
	std::unique_ptr<ircode::StaticValue> pSV1 = std::make_unique<ircode::FloatStaticValue>(
			"1.0");
	std::unique_ptr<ircode::StaticValue> pSV2 = std::make_unique<ircode::IntStaticValue>(
			"1");
	pSV1->calc(*pSV2,"+");
}

#endif