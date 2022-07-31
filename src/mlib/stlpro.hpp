#pragma once

#include <vector>
#include <list>

#include "common.hpp"


namespace STLPro {
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
	com::Assert(
		number >= 0,
		"The number of instance to push should not be negative.", CODEPOS
	);
	for (int i = 0; i < number; ++i) {
		vec.emplace_back(instance);
	}
}
}
namespace list {
template<typename T>
void move_all_to_front(std::list<T> & ls, const T & t) {
	std::list<T> found;
	auto it = ls.begin();
	while (it != ls.end()) {
		auto jt = std::find_if(it, ls.end(), t);
		if (jt == ls.end()) {
			break;
		}
		found.emplace_back(std::move(*jt));
		it = std::next(jt);
		ls.erase(jt);
	}
	ls.splice(ls.begin(), found);
}

template<typename T, class UnaryPredicate>
void move_all_to_front(std::list<T> & ls, UnaryPredicate f) {
	static_assert(
		std::is_same<
			decltype(std::declval<UnaryPredicate>()(std::declval<T>())), bool
		>::value,
		"UnaryPredicate should be a function with parameter T and return type bool!."
	);
	std::list<T> found;
	auto it = ls.begin();
	while (it != ls.end()) {
		auto jt = std::find_if(it, ls.end(), f);
		if (jt == ls.end()) {
			break;
		}
		found.emplace_back(std::move(*jt));
		it = std::next(jt);
		ls.erase(jt);
	}
	ls.splice(ls.begin(), found);
}

}
}

template<typename Iter>
typename std::iterator_traits<Iter>::value_type & get(Iter & it) {
	return *it;
}
