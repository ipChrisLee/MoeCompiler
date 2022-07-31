#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>


namespace mir {
class Addr;
}

namespace frontend {

enum class IdType {
	GlobalVarName,
	FunctionName,
	ParameterName,
	LocalVarName,
	ReservedWord,
	BuiltInFunction,
	Error
};


class Scope {
  protected:
	std::map<std::string, std::tuple<IdType, mir::Addr *>> addrMap;
	Scope * const father;
	std::vector<std::unique_ptr<Scope>> sons;
	static int cnt;
  public:
	const int id;

	explicit Scope(Scope * pFather = nullptr);

	Scope(const Scope &) = delete;

	Scope * addSonScope();

	[[nodiscard]] Scope * getFather() const;

	void bindDominateVar(
		const std::string & str, IdType idType, mir::Addr * addrVar
	);

	//  Find the Addr* of varname. Return <IdType::Error,nullptr> if not found.
	[[nodiscard]] std::tuple<IdType, mir::Addr *>
	findIdDownToRoot(const std::string & varname) const;
};

class SymbolTable {
  protected:
	std::unique_ptr<Scope> root;
  public:
	SymbolTable();

	Scope * pScopeNow;
};

};