#include "optiPassOnPreSSA.hpp"
#include <stlpro.hpp>


int pass::FirstConstantPropagation::run() {
	auto retVal = 0;
	for (auto * pFuncDef: ir.funcPool) {
		retVal = run(pFuncDef);
	}
	return retVal;
}

int pass::FirstConstantPropagation::run(ircode::IRFuncDef * pFuncDef) {
	auto & instrs = pFuncDef->instrs;
	auto pNow = instrs.begin();
	while (pNow != instrs.end()) {
		auto mapPair = mapping(get(pNow));
		auto pNxt = std::next(pNow);
		useMapping(pFuncDef, mapPair);
		instrs.erase(pNow);
		pNow = pNxt;
	}
	return 0;
}

void pass::FirstConstantPropagation::useMapping(
	ircode::IRFuncDef * pFuncDef,
	std::pair<ircode::AddrVariable *, ircode::AddrStaticValue *> & mp
) {
	auto & instr = pFuncDef->instrs;
	auto pNow = instr.begin();
	while (pNow != instr.end()) {
		auto * pInstr = get(pNow);

		pNow = std::next(pNow);
	}
}

static std::string instrTypeToOp(ircode::InstrType instrType) {
	switch (instrType) {
		case ircode::InstrType::FAdd:
		case ircode::InstrType::Add: {
			return "+";
		}
		case ircode::InstrType::FSub:
		case ircode::InstrType::Sub: {
			return "-";
		}
		case ircode::InstrType::FDiv:
		case ircode::InstrType::SDiv: {
			return "/";
		}
		case ircode::InstrType::FMul:
		case ircode::InstrType::Mul: {
			return "*";
		}
		case ircode::InstrType::SRem: {
			return "%";
		}
		default:com::Throw("", CODEPOS);
	}

}

std::pair<ircode::AddrVariable *, ircode::AddrStaticValue *>
pass::FirstConstantPropagation::mapping(ircode::IRInstr * pInstr) {
	switch (pInstr->instrType) {
		case ircode::InstrType::FAdd:
		case ircode::InstrType::FSub:
		case ircode::InstrType::FMul:
		case ircode::InstrType::FDiv:
		case ircode::InstrType::Add:
		case ircode::InstrType::Sub:
		case ircode::InstrType::Mul:
		case ircode::InstrType::SDiv: {
			auto * pInstrBinaryOp = dynamic_cast<ircode::InstrBinaryOp *>(pInstr);
			auto * pLOp = pInstrBinaryOp->left;
			auto * pROp = pInstrBinaryOp->right;
			if (pLOp->addrType == ircode::AddrType::StaticValue &&
				pROp->addrType == ircode::AddrType::StaticValue) {
				const auto & lVal =
					dynamic_cast<ircode::AddrStaticValue *>(pLOp)->getStaticValue();
				const auto & rVal =
					dynamic_cast<ircode::AddrStaticValue *>(pROp)->getStaticValue();
				auto upSV = sup::calcOnSV(lVal, instrTypeToOp(pInstr->instrType), rVal);
				auto * pSVAddr = ir.addrPool.emplace_back(
					ircode::AddrStaticValue(std::move(upSV))
				);
				return {pInstrBinaryOp->res, pSVAddr};
			} else {
				return {pInstrBinaryOp->res, nullptr};
			}
		}
		default:return {nullptr, nullptr};
	}
}

