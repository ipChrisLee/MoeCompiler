#include "backendPass.hpp"


namespace pass {

std::string FuncInfo::toASM(ircode::InstrCall * pInstrCall) {
	//  TODO: Check for correctness
	auto res = std::string();
	auto * pFuncInfoToCall = m_AddrFunc_FuncInfo[pInstrCall->func];
	auto rId2StkOffset = std::map<backend::RId, int>();
	auto sId2StkOffset = std::map<backend::SId, int>();
	//  back up caller saved registers; create mapping of caller-save rid and stk offset
	auto backUpStkSize = 0;
	for (auto rId: pFuncInfoToCall->callerSaveRReg) {
		rId2StkOffset[rId] = -(backUpStkSize + 1) * 4;
		genASMSaveFromRRegToOffset(res, rId, -(backUpStkSize + 1) * 4, rId);
		backUpStkSize += 4;
	}
	for (auto sId: pFuncInfoToCall->callerSaveSReg) {
		sId2StkOffset[sId] = -(backUpStkSize + 1) * 4;
		com::TODO("", CODEPOS);
		backUpStkSize += 4;
	}
	if (backUpStkSize % 8 != 0) { backUpStkSize += 4; }
	//  load to arguments
	auto iArg = 0;
	auto cntArg = (int) pFuncInfoToCall->pFuncDef->pAddrFun->vecPtrAddrPara.size();
	for (; iArg < cntArg; ++iArg) {
		auto * pPara = pFuncInfoToCall->pFuncDef->pAddrFun->vecPtrAddrPara[iArg];
		auto * pOperandArg = pInstrCall->paramsPassing[iArg];
		auto * pOpndPara = pFuncInfoToCall->argsOnCallingThis[pPara];
		switch (pOpndPara->getOpndType()) {
			case backend::OpndType::VRegS: {
				com::Throw("", CODEPOS);
				break;
			}
			case backend::OpndType::VRegR: {
				auto * pVRegRPara = dynamic_cast<backend::VRegR *>(pOpndPara);
				if (backend::isGPR(pVRegRPara->rid)) {
					auto ridToSave = pVRegRPara->rid;
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto valOn = genASMGetVRegRVal(
								res, convertIntVariable(pVarArg), ridToSave
							);
							if (valOn != ridToSave) {
								res += backend::toASM("mov", ridToSave, valOn);
							}
							break;
						}
						case ircode::AddrType::ParaVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::LocalVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::GlobalVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::StaticValue: {
							auto * pSVArg
								= dynamic_cast<ircode::AddrStaticValue *>(pOperandArg);
							auto val = dynamic_cast<const sup::IntStaticValue &>(
								pSVArg->getStaticValue()
							).value;
							genASMLoadNumber(res, val, ridToSave);
							break;
						}
						default:com::Throw("", CODEPOS);
					}
				} else if (pVRegRPara->rid == backend::RId::stk) {
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto valOn = genASMGetVRegRVal(
								res, convertIntVariable(pVarArg), backend::RId::lhs
							);
							genASMSaveFromRRegToOffset(
								res, valOn, pVRegRPara->offset, backend::RId::rhs
							);
							break;
						}
						case ircode::AddrType::ParaVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::LocalVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::GlobalVar: {
							com::TODO("", CODEPOS);
							break;
						}
						case ircode::AddrType::StaticValue: {
							auto * pSVArg
								= dynamic_cast<ircode::AddrStaticValue *>(pOperandArg);
							auto val = dynamic_cast<const sup::IntStaticValue &>(
								pSVArg->getType()
							).value;
							genASMLoadNumber(res, val, backend::RId::lhs);
							genASMSaveFromRRegToOffset(
								res, backend::RId::lhs, pVRegRPara->offset,
								backend::RId::rhs
							);
							break;
						}
						default:com::Throw("", CODEPOS);
					}
				} else { com::Throw("", CODEPOS); }
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	//  change sp NOW!
	if (backUpStkSize) {
		if (backend::Imm<backend::ImmType::Imm8m>::fitThis(backUpStkSize)) {
			res += backend::toASM(
				"sub", backend::RId::sp, backend::RId::sp, backUpStkSize
			);
		} else {
			genASMLoadNumber(res, backUpStkSize, backend::RId::rhs);
			res += backend::toASM(
				"sub", backend::RId::sp, backend::RId::sp, backend::RId::rhs
			);
		}
	}
	//  call
	res += backend::toASM("bl", pFuncInfoToCall->pFuncLabel->labelStr);
	if (pInstrCall->retAddr) {
		switch (pInstrCall->retAddr->addrType) {
			case ircode::AddrType::Var: {
				switch (pInstrCall->retAddr->getType().type) {
					case sup::Type::Int_t: {
						auto * pVRegR = convertIntVariable(pInstrCall->retAddr);
						genASMSaveFromRRegToVRegR(
							res, pVRegR, backend::RId::r0, backend::RId::lhs
						);
						break;
					}
					case sup::Type::Float_t: {
						com::TODO("", CODEPOS);
						break;
					}
					default:com::Throw("", CODEPOS);
				}
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	//  restore from back-up
	for (auto [rId, offset]: rId2StkOffset) {
		genASMDerefStkPtr(res, offset, rId);
	}
	for (auto [sId, offset]: sId2StkOffset) {
		com::TODO("", CODEPOS);
	}
	if (backUpStkSize) {
		if (backend::Imm<backend::ImmType::Imm8m>::fitThis(backUpStkSize)) {
			res += backend::toASM(
				"add", backend::RId::sp, backend::RId::sp, backUpStkSize
			);
		} else {
			genASMLoadNumber(res, backUpStkSize, backend::RId::rhs);
			res += backend::toASM(
				"add", backend::RId::sp, backend::RId::sp, backend::RId::rhs
			);
		}
	}
	return res;
}

}