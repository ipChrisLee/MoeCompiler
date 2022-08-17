#include "backend/backendPass.hpp"


namespace pass {

int FuncInfo::run(ircode::InstrParallelCopy * pInstrCopy) {
	for (auto [from, to, _]: pInstrCopy->copies) {
		markOperand(from);
		markOperand(to);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrParallelCopy * pInstrCopy) {
	auto res = std::string();
	for (auto [pFrom, pTo, _]: pInstrCopy->copies) {
		switch (pTo->getType().type) {
			case sup::Type::Float_t: {
				res += toASM_Copy_Float(pFrom, pTo);
				break;
			}
			case sup::Type::Int_t: {
				res += toASM_Copy_Int(pFrom, pTo);
				break;
			}
			default:com::Throw("", CODEPOS);
		}

	}
	return res;
}

std::string
FuncInfo::toASM_Copy_Float(ircode::AddrOperand * pFrom, ircode::AddrVariable * pTo) {
	auto res = std::string();
	auto * pVRegSTo = convertFloatVariable(pTo);
	if (pVRegSTo->sid == backend::SId::stk) {
		switch (pFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
				auto value = dynamic_cast<const sup::FloatStaticValue &>(
					pSVAddr->getStaticValue()
				).value;
				genASMLoadFloatToRReg(res, value, backend::RId::lhs);
				genASMSaveFromRRegToOffset(
					res, backend::RId::lhs, pVRegSTo->offset, backend::RId::rhs
				);
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarAddrFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
				auto * pVRegSFrom = convertFloatVariable(pVarAddrFrom);
				genASMSaveFromVRegSToVRegS(
					res, pVRegSFrom, pVRegSTo, backend::RId::lhs, backend::RId::rhs
				);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	} else if (backend::isGPR(pVRegSTo->sid)) {
		switch (pFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
				auto value = dynamic_cast<const sup::FloatStaticValue &>(
					pSVAddr->getStaticValue()
				).value;
				genASMLoadFloat(res, value, pVRegSTo->sid, backend::RId::lhs);
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarAddrFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
				auto * pVRegSFrom = convertFloatVariable(pVarAddrFrom);
				genASMSaveFromVRegSToSReg(res, pVRegSFrom, pVRegSTo->sid, backend::RId::lhs);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	} else { com::Throw("", CODEPOS); }
	return res;
}

std::string FuncInfo::toASM_Copy_Int(ircode::AddrOperand * pFrom, ircode::AddrVariable * pTo) {
	auto res = std::string();
	auto * pVRegRTo = convertIntVariable(pTo);
	if (pVRegRTo->rid == backend::RId::stk) {
		switch (pFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
				auto value = dynamic_cast<const sup::IntStaticValue &>(
					pSVAddr->getStaticValue()
				).value;
				genASMLoadInt(res, value, backend::RId::lhs);
				genASMSaveFromRRegToOffset(
					res, backend::RId::lhs, pVRegRTo->offset, backend::RId::rhs
				);
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarAddrFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
				auto * pVRegRFrom = convertIntVariable(pVarAddrFrom);
				genASMSaveFromVRegRToVRegR(
					res, pVRegRFrom, pVRegRTo, backend::RId::lhs, backend::RId::rhs
				);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	} else if (backend::isGPR(pVRegRTo->rid)) {
		switch (pFrom->addrType) {
			case ircode::AddrType::StaticValue: {
				auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
				auto value = dynamic_cast<const sup::IntStaticValue &>(
					pSVAddr->getStaticValue()
				).value;
				genASMLoadInt(res, value, pVRegRTo->rid);
				break;
			}
			case ircode::AddrType::Var: {
				auto * pVarAddrFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
				auto * pVRegRFrom = convertIntVariable(pVarAddrFrom);
				genASMSaveFromVRegRToRReg(res, pVRegRFrom, pVRegRTo->rid);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	} else { com::Throw("", CODEPOS); }
	return res;
}

int FuncInfo::run(ircode::InstrParaMov * pInstrParaMov) {
	markOperand(pInstrParaMov->from);
	markOperand(pInstrParaMov->to);
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrParaMov * pInstrParaMov) {
	auto res = std::string();
	auto * pFrom = pInstrParaMov->from;
	auto * pTo = pInstrParaMov->to;
	switch (pTo->getType().type) {
		case sup::Type::Int_t:
		case sup::Type::Pointer_t: {
			auto * pVRegRFrom = convertThisIntArg(pFrom);
			auto * pVRegRTo = convertIntVariable(pTo);
			genASMSaveFromVRegRToVRegR(
				res, pVRegRFrom, pVRegRTo, backend::RId::lhs, backend::RId::rhs
			);
			break;
		}
		case sup::Type::Float_t: {
			auto * pVRegSFrom = convertThisFloatArg(pFrom);
			auto * pVRegSTo = convertFloatVariable(pTo);
			genASMSaveFromVRegSToVRegS(
				res, pVRegSFrom, pVRegSTo, backend::RId::lhs, backend::RId::rhs
			);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

int FuncInfo::run(ircode::InstrMarkVars * pInstrMark) {
	for (auto * p: pInstrMark->vecPVar) {
		markOperand(p);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrMarkVars * pInstrMark) {
	return "";
}

}