#pragma once

#include <functional>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <utility>

#include "mdb.hpp" // for CODEPOS

namespace com {
/*  testFun_t is a function type for test function.  */
using mainFun_t = std::function<int(const std::vector<std::string>&)>;

/*  Singleton struct for sub main system.
 *  The methods in this class use `com::ccout` and `com::ccerr` as output instance.
 *  You can add function with macro `AddSubMain`
 *  e.g. : See submain.cpp .
 * */
#define AddSubMain(name, fun) \
    static bool __SubMain ## name ## added = com::submain::getSingle().addCase(#name,fun,CODEPOS)

class submain {
  public:
	struct SubMainCase {
		std::set<std::string> positions;
		mainFun_t mainFunction;
		
		SubMainCase() = default;
		
		explicit SubMainCase(mainFun_t mainFunction) : positions(), mainFunction(std::move(mainFunction)) { }
		
		SubMainCase & operator =(const SubMainCase &) = default;
	};
  
  private:
	static int errCase(const std::vector<std::string>&) {
		return 0;
	}
	
	std::map<std::string, SubMainCase> cases;
	
	submain() = default;
  
  public:
	submain(const submain &) = delete;
	
	void operator =(const submain &) = delete;
	
	static std::unique_ptr<submain> single;
	
	static void init() { single = std::unique_ptr<submain>(new submain()); }
	
	static submain & getSingle() {
		if (!single) init();
		return *single;
	}
	
	bool addCase(
			const std::string & caseName,
			mainFun_t mainfun,
			const std::string & codepos
	) {
		if (!cases.count(caseName)) {
			cases.try_emplace(caseName, mainfun);
		}
		auto & it = cases.at(caseName);
		it.positions.insert(codepos);
		return true;
	}
	
	[[nodiscard]] const std::map<std::string, SubMainCase> & getCases() const { return cases; }
	
	void runMain(
			const std::string & name = "listall",
			const std::vector<std::string> & parameters = { }
	) {
		if (!cases.count(name)) {
			com::ccerr.cprintLn(std::tuple("No such function named [", name, "] !"));
		} else {
			int exitcode = cases[name].mainFunction(parameters);
			if (exitcode) {
				com::ccerr.cprintLn(
						std::tuple("Function [", name, "] exits with ", exitcode, " !")
				);
			} else {
				com::ccout.cprintLn(std::tuple("Function [", name, "] exits normally."));
			}
		}
	}
};
}

