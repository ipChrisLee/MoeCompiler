#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>


namespace ircode {
class IRAddr;
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
	std::map<std::string, std::tuple<IdType, ircode::IRAddr *>> addrMap;
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
		const std::string & str, IdType idType, ircode::IRAddr * addrVar
	);

	//  Find the IRAddr* of varname. Return <IdType::Error,nullptr> if not found.
	[[nodiscard]] std::tuple<IdType, ircode::IRAddr *>
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