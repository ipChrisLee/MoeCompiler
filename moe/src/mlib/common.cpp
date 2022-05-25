#include <string>
#include <regex>

#include "common.hpp"
#include "cprt.hpp"

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
	if (codepos.length()) buf = "Code Position [" + std::string(codepos) + "] | ";
	buf += "Moe Exception! {" + std::string(msg) + "}";
	throw MException(buf);
}

[[noreturn]] void ThrowSingletonNotInited(const std::string_view className, std::string_view codepos) {
	com::Throw("Using uninited singleton [" + std::string(className) + "]", codepos);
}

[[noreturn]] void TODO(const std::string_view msg, std::string_view codepos) {
	Throw("Not implemented error by \'TODO()\'[" + std::string(msg) + "].", codepos);
}

void notFinished(const std::string_view msg, const std::string_view codepos) {
	com::ccerr.cprintLn(std::tuple("Not finished code [", msg, "] in ", codepos));
}

void Assert(bool b, const std::string & msg, const std::string & codepos) {
	if (!b) { com::Throw(" Assertion failed : [" + msg + "]", codepos); }
}

void Assert(const std::function<bool(void)> & fun, const std::string & msg, const std::string & codepos) {
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

void com::regSwitch(const std::string & str, std::initializer_list<RegexSwitchCase> cases) {
	for (const auto & kase : cases) {
		if (std::regex_match(str, std::regex(kase.regex))) {
			kase.fun();
			break;
		}
	}
}

