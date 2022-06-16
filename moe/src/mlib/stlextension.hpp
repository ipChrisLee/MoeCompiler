#pragma once

#include <vector>

#include "common.hpp"

namespace stlextension {
namespace vector {
template<typename T>
void PushBackByIterators(
	std::vector<T> & vec,
	typename std::vector<T>::const_iterator beginIt,
	typename std::vector<T>::const_iterator endIt
) {
	for (typename std::vector<T>::const_iterator it = beginIt; it != endIt; ++it) {
		vec.emplace_back(*it);
	}
}

template<typename T>
void PushBackByNumberAndInstance(
	std::vector<T> & vec,
	const int number,
	const T & instance
) {
	com::Assert(number >= 0,
	            "The number of instance to push should not be negative.", CODEPOS);
	for (int i = 0; i < number; ++i) {
		vec.emplace_back(instance);
	}
}
}
}
