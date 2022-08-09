#pragma once

#include <vector>
#include <numeric>

#include <common.hpp>


namespace sup {

using VI = std::vector<int>;
VI posToIdx(int pos, const VI & shape);
int idxToPos(const VI & idx, const VI & shape);

}
