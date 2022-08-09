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

int lastPosOfShape(const VI & shape) {
	return std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
}

int idxToPos(const VI & idx, const VI & shape) {
	com::Assert(idx.size() == shape.size(), "", CODEPOS);
	auto n = int(idx.size());
	auto pos = 0;
	for (auto i = n - 1; i >= 0; --i) {
		com::Assert(idx[i] < shape[i], "", CODEPOS);
		pos += std::accumulate(
			shape.begin() + i + 1, shape.end(), idx[i], std::multiplies<>()
		);
	}
	return pos;
}

}