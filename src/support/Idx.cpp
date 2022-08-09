#include "Idx.hpp"


namespace sup {

VI posToIdx(int pos, const VI & shape) {
	int n = int(shape.size());
	auto res = VI(n, 0);
	for (auto i = n - 1; i >= 0; --i) {
		res[i] = pos % shape[i];
		pos /= shape[i];
	}
	return res;
}

int idxToPos(const VI & idx, const VI & shape) {
	com::Assert(idx.size() == shape.size(), "", CODEPOS);
	auto n = int(idx.size());
	auto pos = 0;
	for (auto i = n - 1; i >= 0; --i) {
		pos += std::accumulate(
			shape.begin() + i + 1, shape.end(), idx[i], std::multiplies<>()
		);
	}
	return pos;
}

}