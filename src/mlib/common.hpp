#pragma once

#include <string_view>
#include <functional>
#include <initializer_list>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <utility>
#include <memory>
#include <typeinfo>
#include <variant>
#include <set>
#include <vector>


#ifndef CODEPOS //  prevent redefinition from mdb.hpp
#define STR(t) #t
#define LINE_STR(v) STR(v)
#define CODEPOS "File["  __FILE__  "] Line["  LINE_STR(__LINE__) "] "
#endif

/**
 * Indicate that some code should be finished before compiling.
 * @note
 * Difference between @c notFinished and @c TODO: \n
 * Your code will throw exception when running @c TODO. \n
 * Your code can not be compiled if @c notFinished exists.
 */
#define notFinished(msg) static_assert(0,"Not finished in {" CODEPOS "}, msg={" msg "}.")

namespace com {
std::string concatToString(std::initializer_list<std::string> listOfStr);

class MException : public std::exception {
  public:
	std::string msg = std::string();
	int exitCode = -1;

	MException() = default;

	explicit MException(std::string msg) : msg(std::move(msg)) {}

	[[nodiscard]] const char * what() const noexcept override {
		return msg.data();
	}
};

namespace WarningList {
extern std::vector<std::pair<std::string, std::string>> msgAndCodepos;

extern std::set<std::string_view> onlyOncePos;
}

extern bool addWarningOnlyOnce;

extern bool addWarningMultipleTimes;

void
addRuntimeWarning(
	const std::string & msg, std::string_view codepos, bool onlyOnce = false
);

void showAllRuntimeWarnings();

[[noreturn]] void
TODO(std::string_view msg = std::string_view(), std::string_view codepos = "");

[[noreturn]] void
Throw(std::string_view s = std::string_view(), std::string_view codepos = "");

/*  Take place in `assert`. This version use a bool value as assert condition.
 * */
void
Assert(bool b, const std::string & msg = "", const std::string & codepos = "");

/*  Take place in `assert`.
 *  This version use a function returning bool as assert condition.
 * */
void Assert(
	const std::function<bool(void)> & fun, const std::string & msg = "",
	const std::string & codepos = ""
);

/*  Struct for regex switch.
 * */
struct RegexSwitchCase {
	const char * const regex;
	std::function<void(void)> fun;
};

void regSwitch(const std::string & str, std::initializer_list<RegexSwitchCase>);

//  use this if you want to save `cases` in a variable.
// void
// regSwitch(const std::string & str, const std::vector<RegexSwitchCase> & cases);
/*
 * Do NOT use this type of declaration, it is dangerous for who don't know how lambda expression work!
 * See Tests/BasicTests/testLambdaWithUPtr to understand why this type of declaration is dangerous.
 */

/*  bmeBrace = begin-middle-end
 *  Brace which let some execute-when-entering codes and execute-when-leaving
 *  codes come first in source codes.
 * */
void bmeBrace(
	const std::function<void(void)> & begin,
	const std::function<void(void)> & end,
	const std::function<void(void)> & middle
);

/**
 * Dynamic cast for @c unique_ptr. \n
 * This function will change the ownership!
 * @note If you use this function, you should @b NOT use ptr you pass to this function anymore!
 */
/* Example:
 * std::unique_ptr<Base>upBase=dynamic_cast_uPtr<Base>(upDerivedB->cloneToUniquePtr());
 * // The second type parameter `From` can be deduced from argument.
 * // Here `upBase` is pointing to a cloned instance of `upDerivedB`, whose type is `DerivedB`.
 * std::unique_ptr<DerivedB>upDerivedB=dynamic_cast_uPtr<DerivedB>(std::move(upBase));
 * // Dynamic cast unique pointer of type `Base` to `DerivedB`, and after this, `upBase`
 * // has passed its ownership to `upDerivedB`. So `upBase.get()` is `nullptr` now.
 * upBase->fun(); // Dangerous! `upBase` has been released.
 */
template<typename To, typename From>
std::unique_ptr<To> dynamic_cast_uPtr(std::unique_ptr<From> && fromP) {
	//  return nullptr if source pointer is nullptr.
	if (!fromP) { return std::unique_ptr<To>(nullptr); }
	To * p = dynamic_cast<To *>(fromP.release());
	Assert(
		p, concatToString(
			{
				"dynamic_cast_uPtr failed. From [",
				typeid(From).name(), "*] to [",
				typeid(To).name(), "*]."
			}
		));
	return std::unique_ptr<To>(p);
}

/**
 * @details
 * Dynamic cast for @c unique_ptr. \n
 * This function will @b NOT change the ownership! \n
 * This equals to @code dynamic_cast<To *>(fromP.get()) @endcode
 * @note @c fromP will not loss its ownership.
 */
/*
 * Example:
 *  Since `std::unique_ptr::unique_ptr(pointer)` is declared as `explicit`, code
 *  like below is forbidden, and will cause compile error.
 *      std::unique_ptr<Base>upBase=dynamic_cast_uPtr<Base>(upDerivedB);
 */
template<typename To, typename From>
To * dynamic_cast_uPtr_get(std::unique_ptr<From> & fromP) {
	return dynamic_cast<To *>(fromP.get());
}

template<typename To, typename From>
To * dynamic_cast_uPtr_get(const std::unique_ptr<From> & fromP) {
	return dynamic_cast<To *>(fromP.get());
}

template<typename T, typename ... Ts>
constexpr bool isTypeTInTypesTs() {
	return std::disjunction_v<std::is_same<T, Ts>...>;
}

/**
 * @brief
 * 		A class store exact one variable of class in class pack everytime.
 * @tparam Types Type pack of the types you want to store.
 *
 * @note You can only preserve one instance for one time.
 * @note You can only restore once for one instance.
 * @note @c std::monostate can not be stored in this class.
 * @note The type of instance you stored should have move constructor.
 *
 * @example
 * @code
 * UnaryVariant<std::unique_ptr<ircode::StaticValue>>uv;
 * uv.save(std::move(up));
 * @endcode
 */
template<typename ... Types>
class UnaryVariant {
  protected:
	std::variant<std::monostate, Types...> box;
  public:
	void clear() {
		box = std::monostate();
	}

	template<typename T>
	void save(T && t) {
		static_assert(
			isTypeTInTypesTs<T, Types...>(),
			"Type is not in type pack. Notice to use `std::move`."
		);
		/*  For example:
		 *      `Int i(1);UnaryVariant uv;uv.save(i);`
		 *      `i` is lvalue! And `int& &&` is deduced to `int &`!
		 *  Ref: https://stackoverflow.com/a/3582313/17924585
		 * */
		if (box.index()) {
			com::Throw(
				"Saving to `UnaryVariant` where has saved variable.",
				CODEPOS
			);
		}
		box = std::forward<T>(t);
	}

	[[nodiscard]] bool savedSomething() const {
		return box.index();
	}

	template<typename T>
	[[nodiscard]]
	T restore() {
		static_assert(isTypeTInTypesTs<T, Types...>(), "Type is not in type pack.");
		if (!std::holds_alternative<T>(box)) {
			com::Throw(
				"Restoring from empty UnaryVariant.",
				CODEPOS
			);
		}
		T ret(std::move(std::get<T>(box)));
		box = std::monostate();
		return ret;
	}

	template<typename T>
	T & changeInPlace() {
		static_assert(isTypeTInTypesTs<T, Types...>(), "Type is not in type pack.");
		if (!std::holds_alternative<T>(box)) {
			com::Throw(
				"Getting reference from UnaryVariant with a type not in type pack ",
				CODEPOS
			);
		}
		return std::get<T>(box);
	}
};

template<
	typename T,
	class=typename std::enable_if<!std::is_lvalue_reference<T>::value>::type
>
class ComRestorer {
	T savedVal;
	T & ref;
  public:

	ComRestorer(T & _ref, T && newVal) : savedVal(std::move(_ref)), ref(_ref) {
		//  This is just for note.
		static_assert(
			!std::is_lvalue_reference<T>::value,
			"newVal should be rvalue. Remember to add `std::move`."
		);
		_ref = std::move(newVal);
	}

	~ComRestorer() {
		ref = std::move(savedVal);
	}
};

namespace enum_fun {
template<typename T>
bool in(T val, std::initializer_list<T> enumSet) {
	static_assert(std::is_enum<T>().value);
	for (auto e: enumSet) {
		if (e == val) {
			return true;
		}
	}
	return false;
}

}
template<typename FT, typename ST>
class BiMap {
  protected:
	std::map<FT, ST> f2s;
	std::map<ST, FT> s2f;
  public:
	void emplace_back(const FT & fT, const ST & sT) {
		f2s[fT] = sT;
		s2f[sT] = fT;
	}

	ST & getST(const FT & fT) { return f2s[fT]; }

	FT & getFT(const ST & sT) { return s2f[sT]; }


	const std::map<FT, ST> & getF2S() const { return f2s; }

	const std::map<ST, FT> & getS2F() const { return s2f; }
};
}

#ifndef __CONCAT
#define __CONCAT(x, y) x ## y
#endif

#ifndef CONCAT
#define CONCAT(x, y) __CONCAT(x,y)
#endif

#ifndef setWithAutoRestorer
/**
 * @brief A macro for auto-restore set.
 * @details The value of @c var will be restored automatically when leaving the scope of this macro used.
 * @ref https://stackoverflow.com/questions/207965/general-way-to-reset-a-member-variable-to-its-original-value-using-the-stack
 * @note The type of @c var should have move-constructor, move-assignment.
 * @note One line can only contain exact one macro.
 * @note Multiple usage of this macro in one scope will @b NOT cause fault.
 */
#define setWithAutoRestorer(var, newVal) com::ComRestorer<decltype(var)> \
                CONCAT(__restorer,__LINE__) (var,newVal)
#endif


