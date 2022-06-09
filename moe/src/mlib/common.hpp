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

#ifndef CODEPOS //  prevent redefinition from mdb.hpp
#define STR(t) #t
#define LINE_STR(v) STR(v)
#define CODEPOS "File["  __FILE__  "] Line["  LINE_STR(__LINE__) "] "
#endif

//  Difference between `notFinished` and `TODO_`:
//  Your code will throw exception when running to `TODO_`.
//  Your code will not be compiled if `notFinished` exists.
#define notFinished(msg) static_assert(0,"Not finished in {" CODEPOS "}, msg={" msg "}.")

namespace com {
std::string concatToString(std::initializer_list<std::string> listOfStr);

class MException : public std::exception {
  public:
	std::string msg = std::string();
	
	MException() = default;
	
	explicit MException(std::string msg) : msg(std::move(msg)) { }
	
	[[nodiscard]] const char * what() const noexcept override {
		return msg.data();
	}
};

namespace WarningList {
extern std::vector<std::pair<std::string, std::string>> msgAndCodepos;
};

void addWarning(const std::string & msg, std::string_view codepos);

void showAllWarning(const std::string & filePath);

[[noreturn]] void
TODO(std::string_view msg = std::string_view(), std::string_view codepos = "");


/*  To handle code that hasn't been finished. This may avoid some bugs
 *  caused by coders forgetting to implement.
 *  Usually usage:
 *      com::notFinished(FUNINFO,CODEPOS);
 * */
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
void
regSwitch(const std::string & str, const std::vector<RegexSwitchCase> & cases);

/*  bmeBrace = begin-middle-end
 *  Brace which let some execute-when-entering codes and execute-when-leaving
 *  codes come first in source codes.
 * */
void bmeBrace(
		const std::function<void(void)> & begin,
		const std::function<void(void)> & end,
		const std::function<void(void)> & middle
);

/*  dynamic_cast for `unique_ptr`.
 * */
template<typename From, typename To>
std::unique_ptr<To> dynamic_cast_unique_ptr(std::unique_ptr<From> && fromP) {
	//  return nullptr if source pointer is nullptr.
	if (!fromP) { return std::unique_ptr<To>(nullptr); }
	To * p = dynamic_cast<To *>(fromP.release());
	Assert(p, concatToString({
			                         "dynamic_cast_unique_ptr failed. From [",
			                         typeid(From).name(), "*] to [",
			                         typeid(To).name(), "*]."
	                         }));
	return std::unique_ptr<To>(p);
}

/*  NOTICE:
 *      If you use this function, you should NOT use ptr you pass to this function any more!
 *  e.g. :
 *      std::unique_ptr<Base>upBase=dynamic_cast_unique_ptr<Cloneable,Base>(upDerivedB1->cloneToUniquePtr());
 *      std::unique_ptr<DerivedB>upDerivedB2=dynamic_cast_unique_ptr<Base,DerivedB>(upBase)
 *      upBase->fun(); // Dangerous! upBase has been released.
 * */
template<typename From, typename To>
std::unique_ptr<To> dynamic_cast_unique_ptr(std::unique_ptr<From> & fromP) {
	//  return nullptr if source pointer is nullptr.
	if (!fromP) { return std::unique_ptr<To>(nullptr); }
	To * p = dynamic_cast<To *>(fromP.release());
	Assert(p, concatToString({
			                         "dynamic_cast_unique_ptr failed. From [",
			                         typeid(From).name(), "*] to [",
			                         typeid(To).name(), "*]."
	                         }));
	return std::unique_ptr<To>(p);
}
}

