//
// Created by lee on 6/22/22.
//

#include <stack>
#include <tuple>

#include "common.hpp"

#include "SymbolTableAndScope.hpp"
#include "IR/IRAddr.hpp"


namespace frontend {

int Scope::cnt = 0;

Scope * Scope::addSonScope() {
	sons.emplace_back(std::make_unique<Scope>(this));
	return sons.rbegin()->get();
}

Scope::Scope(Scope * pFather) : father(pFather), id(++cnt) {
}

Scope * Scope::getFather() const {
	return father;
}

void Scope::bindDominateVar(
	const std::string & str, IdType idType, ircode::IRAddr * addrVar
) {
	if (addrMap.count(str)) {
		com::Throw("Same name!", CODEPOS);
	}
	switch (idType) {
		case IdType::LocalVarName: {
			auto pAddr = dynamic_cast<ircode::AddrLocalVariable *>(addrVar);
			com::Assert(
				pAddr && pAddr->getType().type == sup::Type::Pointer_t,
				"addr should be `AddrVariable *` if `idType` is local var "\
                "and type of var should be pointer.",
				CODEPOS
			);
			break;
		}
		case IdType::GlobalVarName: {
			auto pAddr = dynamic_cast<ircode::AddrGlobalVariable *>(addrVar);
			com::Assert(
				pAddr && pAddr->getType().type == sup::Type::Pointer_t,
				"addr should be `AddrGlobalVariable` if `idType` is global var "\
                "and type of var should be pointer.",
				CODEPOS
			);
			break;
		}
		case IdType::ParameterName: {
			auto pAddr = dynamic_cast<ircode::AddrLocalVariable *>(addrVar);
			com::Assert(
				pAddr && pAddr->getType().type == sup::Type::Pointer_t,
				"addr should be `AddrGlobalVariable` if `idType` is para var,"\
                " and type of this addr should be pointer.",
				CODEPOS
			);
			break;
		}
		case IdType::FunctionName:
		case IdType::BuiltInFunction: {
			auto pAddr = dynamic_cast<ircode::AddrFunction *>(addrVar);
			com::addRuntimeWarning(
				"Consider checking name of function for builtin functions.", CODEPOS,
				true
			);
			com::Assert(
				pAddr, "addr should be `AddrFunction` if `idType` is function.",
				CODEPOS
			);
			break;
		}
		default: {
			com::Throw("Not finished or Error!", CODEPOS);
		}
	}
	addrMap[str] = std::make_tuple(idType, addrVar);
}


std::tuple<IdType, ircode::IRAddr *>
Scope::findIdDownToRoot(const std::string & name) const { // NOLINT
	if (!addrMap.count(name)) {
		if (father == nullptr) {
			return std::make_tuple(IdType::Error, nullptr);
		} else {
			return father->findIdDownToRoot(name);
		}
	} else {
		return addrMap.find(name)->second;
	}
}


SymbolTable::SymbolTable() : root(std::make_unique<Scope>()), pScopeNow(root.get()) {
}
}
