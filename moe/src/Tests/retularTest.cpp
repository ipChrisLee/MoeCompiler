//
// Created by lee on 6/10/22.
//

#ifdef TESTCODE

#include <any>
#include <iostream>

#include "gtest/gtest.h"
#include "mdb.hpp"

#include "frontend/StaticValue.hpp"

std::variant<std::unique_ptr<ircode::StaticValue>> retval;

TEST(Main, testOfAny) {
}


#endif