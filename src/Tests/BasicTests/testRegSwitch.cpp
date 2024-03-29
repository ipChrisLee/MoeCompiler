//
// Created by lee on 6/9/22.
//
#ifdef TESTCODE

#include <initializer_list>

#include "gtest/gtest.h"

#include "common.hpp"


TEST(RegSwitch, InitListCases) {
	std::vector<std::string> ans;
	std::vector<std::string> vec = {
			"*", "-", "+", "/", "&&", "||", "|", "!"
	};
	for (const auto & s : vec) {
		com::regSwitch(s, {
				{"\\*",    [&ans]() { ans.emplace_back("*"); }},
				{"\\-",    [&ans]() { ans.emplace_back("-"); }},
				{"\\+",    [&ans]() { ans.emplace_back("+"); }},
				{"\\/",    [&ans]() { ans.emplace_back("/"); }},
				{"&&",     [&ans]() { ans.emplace_back("&&"); }},
				{"\\|\\|", [&ans]() { ans.emplace_back("||"); }},
				{"\\|",    [&ans]() { ans.emplace_back("|"); }},
				{"!",      [&ans]() { ans.emplace_back("!"); }},
		});
	}
	EXPECT_EQ(vec, ans);
}

#endif
