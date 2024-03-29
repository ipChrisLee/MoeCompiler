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
template<typename T, typename ... LTS>
void merge_to(std::list<T> & l, LTS && ... ls) {
	static_assert(
		(std::is_same<std::list<T>, typename std::remove_reference<LTS>::type>::value && ...) &&
			(!std::is_lvalue_reference<LTS>::value && ...)
	);
	(l.splice(l.end(), std::move(ls)), ...);
}

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
namespace string {
inline std::string to_string(const char * s) { return s; }

inline std::string to_string(const std::string & s) { return s; }

}


namespace set {

template<typename T>
std::set<T> operator-(std::set<T> from, std::set<T> items_to_remove) {
	std::set<T> res;
	std::set_difference(
		from.begin(), from.end(), items_to_remove.begin(), items_to_remove.end(),
		std::inserter(res, res.end())
	);
	return res;
}

template<typename T>
std::set<T> & operator-=(std::set<T> & from, std::set<T> items_to_remove) {
	for (const auto & x: items_to_remove) {
		from.erase(x);
	}
	return from;
}
}

}

template<typename Iter>
auto & get(Iter & it) {
	return it.operator*();
}

template<typename Iter>
auto & get(Iter && it) {
	return it.operator*();
}