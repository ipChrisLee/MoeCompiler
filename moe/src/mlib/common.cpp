#include <string>
#include <regex>

#include "common.hpp"

//bool mdb::sysEnable=false;

namespace com {
std::string concatToString(std::initializer_list<std::string> listOfStr) {
	std::string buffer;
	for (const auto & s : listOfStr) {
		buffer += s;
	}
	return buffer;
}

[[noreturn]] void Throw(const std::string_view msg, std::string_view codepos) {
	std::string buf;
	if (codepos.length())
		buf = "Code Position [" + std::string(codepos) + "] | ";
	buf += "Moe Exception! {" + std::string(msg) + "}";
	throw MException(buf);
}

[[noreturn]] void TODO(const std::string_view msg, std::string_view codepos) {
	Throw("Not implemented error by \'TODO()\'[" + std::string(msg) + "].",
	      codepos);
}


void Assert(bool b, const std::string & msg, const std::string & codepos) {
	if (!b) { com::Throw(" Assertion failed : [" + msg + "]", codepos); }
}

void Assert(
		const std::function<bool(void)> & fun, const std::string & msg,
		const std::string & codepos
) {
	if (!fun()) { com::Throw(" Assertion failed : [" + msg + "]", codepos); }
}

void bmeBrace(
		const std::function<void(void)> & begin,
		const std::function<void(void)> & end,
		const std::function<void(void)> & middle
) {
	begin();
	middle();
	end();
}
}

void com::regSwitch(
		const std::string & str, std::initializer_list<RegexSwitchCase> cases
) {
	for (const auto & kase : cases) {
		if (std::regex_match(str, std::regex(kase.regex))) {
			kase.fun();
			break;
		}
	}
}

void com::regSwitch(
		const std::string & str,
		const std::vector<RegexSwitchCase> & cases
) {
	for (const auto & kase : cases) {
		if (std::regex_match(str, std::regex(kase.regex))) {
			kase.fun();
			break;
		}
	}
}


std::vector<std::pair<std::string, std::string>>com::WarningList::msgAndCodepos;

void com::addWarning(const std::string & msg, std::string_view codepos) {
	WarningList::msgAndCodepos.push_back(
			std::make_pair<>(msg, std::string(codepos)));
}

void com::showAllWarning(const std::string & filePath) {
	com::TODO("common.cpp::showAllWarning not Finished.");
}

