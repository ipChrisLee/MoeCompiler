#include "backendPass.hpp"


namespace pass {

std::string FuncInfo::toASM(ircode::InstrCall * pInstrCall) {
	auto res = std::string();
	auto * pFuncInfoToCall = m_AddrFunc_FuncInfo[pInstrCall->func];
	auto rId2StkOffset = std::map<backend::RId, int>();
	auto sId2StkOffset = std::map<backend::SId, int>();
	//  back up caller saved registers; create mapping of caller-save rid and stk offset
	auto backUpStkSize = 0;
	for (auto rId: pFuncInfoToCall->callerSaveRReg) {
		rId2StkOffset[rId] = -backUpStkSize - 4;
		genASMSaveFromRRegToOffset(res, rId, -backUpStkSize - 4, backend::RId::lhs);
		backUpStkSize += 4;
	}
	for (auto sId: pFuncInfoToCall->callerSaveSReg) {
		sId2StkOffset[sId] = -backUpStkSize * 4;
		genASMSaveFromSRegToOffset(res, sId, -backUpStkSize - 4, backend::RId::lhs);
		backUpStkSize += 4;
	}
	if (backUpStkSize % 8 != 0) { backUpStkSize += 4; }
	//  load to arguments
	auto iArg = 0;
	auto cntArg = (int) pFuncInfoToCall->pFuncDef->pAddrFun->vecPtrAddrPara.size();
	for (; iArg < cntArg; ++iArg) {
		auto * pParaAddr = pFuncInfoToCall->pFuncDef->pAddrFun->vecPtrAddrPara[iArg];
		auto * pOpndPara = pFuncInfoToCall->paramsInfoOnCallingThis[pParaAddr];
		auto * pOperandArg = pInstrCall->paramsPassing[iArg];
		switch (pOpndPara->getOpndType()) {
			case backend::OpndType::VRegS: {
				auto * pVRegSPara = dynamic_cast<backend::VRegS *>(pOpndPara);
				if (backend::isGPR(pVRegSPara->sid)) {
					//  passing this parameter by sreg
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto * pVRegArg = convertFloatVariable(pVarArg);
							if (sId2StkOffset.find(pVRegArg->sid) != sId2StkOffset.end()) {
								//  the value of this arg is saving on stk, not sreg
								//  since sreg may have been covered.
								genASMDerefStkPtrToSReg(
									res, sId2StkOffset[pVRegArg->sid], pVRegSPara->sid,
									backend::RId::lhs
								);
							} else if (backend::isGPR(pVRegArg->sid)) {
								//  the value of this arg is saving on sreg which does not
								//  pass parameter.
								res += backend::toASM("vmov", pVRegSPara->sid, pVRegArg->sid);
							} else if (pVRegArg->sid == backend::SId::stk) {
								//  the value of this arg is saving on stk.
								genASMDerefStkPtrToSReg(
									res, pVRegArg->offset, pVRegSPara->sid, backend::RId::lhs
								);
							} else { com::Throw("", CODEPOS); };
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
							auto val = dynamic_cast<const sup::FloatStaticValue &>(
								pSVArg->getStaticValue()
							).value;
							genASMLoadFloat(res, val, pVRegSPara->sid, backend::RId::lhs);
							break;
						}
						default:com::Throw("", CODEPOS);
					}
				} else if (pVRegSPara->sid == backend::SId::stk) {
					//  passing this parameter by stack
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto * pVRegArg = convertFloatVariable(pVarArg);
							if (sId2StkOffset.find(pVRegArg->sid) != sId2StkOffset.end()) {
								//  the value of this arg is saving on stk, not sreg
								//  since sreg may have been covered.
								genASMDerefStkPtrToSReg(
									res, sId2StkOffset[pVRegArg->sid], backend::SId::lhs,
									backend::RId::lhs
								);
								genASMSaveFromSRegToOffset(
									res, backend::SId::lhs,
									-backUpStkSize + pVRegSPara->offset, backend::RId::rhs
								);
							} else if (backend::isGPR(pVRegArg->sid)) {
								//  the value of this arg is saving on sreg which does not
								//  pass parameter.
								genASMSaveFromSRegToOffset(
									res, pVRegArg->sid, -backUpStkSize + pVRegSPara->offset,
									backend::RId::rhs
								);
							} else if (pVRegArg->sid == backend::SId::stk) {
								//  the value of this arg is saving on stk.
								genASMDerefStkPtrToSReg(
									res, pVRegArg->offset, backend::SId::lhs, backend::RId::lhs
								);
								genASMSaveFromSRegToOffset(
									res, backend::SId::lhs,
									-backUpStkSize + pVRegSPara->offset, backend::RId::rhs
								);
							} else { com::Throw("", CODEPOS); };
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
							auto val = dynamic_cast<const sup::FloatStaticValue &>(
								pSVArg->getStaticValue()
							).value;
							genASMLoadFloat(res, val, backend::SId::lhs, backend::RId::lhs);
							genASMSaveFromSRegToOffset(
								res, backend::SId::lhs, -backUpStkSize + pVRegSPara->offset,
								backend::RId::rhs
							);
							break;
						}
						default:com::Throw("", CODEPOS);
					}
				} else { com::Throw("", CODEPOS); }
				break;
			}
			case backend::OpndType::VRegR: {
				auto * pVRegRPara = dynamic_cast<backend::VRegR *>(pOpndPara);
				if (backend::isGPR(pVRegRPara->rid)) {
					//  passing this parameter by rreg
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto * pVRegArg = convertIntVariable(pVarArg);
							if (rId2StkOffset.find(pVRegArg->rid) != rId2StkOffset.end()) {
								//  the value of this arg is saving on stk, not rreg
								//  since rreg may have been covered.
								genASMDerefStkPtr(
									res, rId2StkOffset[pVRegArg->rid], pVRegRPara->rid
								);
							} else if (backend::isGPR(pVRegArg->rid)) {
								//  the value of this arg is saving on rreg which does not
								//  pass parameter.
								res += backend::toASM("mov", pVRegRPara->rid, pVRegArg->rid);
							} else if (pVRegArg->rid == backend::RId::stk) {
								//  the value of this arg is saving on stk.
								genASMDerefStkPtr(res, pVRegArg->offset, pVRegRPara->rid);
							} else { com::Throw("", CODEPOS); };
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
							genASMLoadInt(res, val, pVRegRPara->rid);
							break;
						}
						default:com::Throw("", CODEPOS);
					}
				} else if (pVRegRPara->rid == backend::RId::stk) {
					//  passing this parameter by stack
					switch (pOperandArg->addrType) {
						case ircode::AddrType::Var: {
							auto * pVarArg
								= dynamic_cast<ircode::AddrVariable *>(pOperandArg);
							auto * pVRegArg = convertIntVariable(pVarArg);
							if (rId2StkOffset.find(pVRegArg->rid) != rId2StkOffset.end()) {
								//  the value of this arg is saving on stk, not rreg
								//  since rreg may have been covered.
								genASMDerefStkPtr(
									res, rId2StkOffset[pVRegArg->rid], backend::RId::lhs
								);
								genASMSaveFromRRegToOffset(
									res, backend::RId::lhs,
									-backUpStkSize + pVRegRPara->offset, backend::RId::rhs
								);
							} else if (backend::isGPR(pVRegArg->rid)) {
								//  the value of this arg is saving on rreg which does not
								//  pass parameter.
								genASMSaveFromRRegToOffset(
									res, pVRegArg->rid, -backUpStkSize + pVRegRPara->offset,
									backend::RId::rhs
								);
							} else if (pVRegArg->rid == backend::RId::stk) {
								//  the value of this arg is saving on stk.
								genASMDerefStkPtr(
									res, pVRegArg->offset, backend::RId::lhs
								);
								genASMSaveFromRRegToOffset(
									res, backend::RId::lhs,
									-backUpStkSize + pVRegRPara->offset, backend::RId::rhs
								);
							} else { com::Throw("", CODEPOS); };
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
							genASMLoadInt(res, val, backend::RId::lhs);
							genASMSaveFromRRegToOffset(
								res, backend::RId::lhs, -backUpStkSize + pVRegRPara->offset,
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
	//  delete arguments field on calling next function
	if (pFuncInfoToCall->argsStkSizeOnCallingThis + backUpStkSize) {
		auto stkSizeArgs = pFuncInfoToCall->argsStkSizeOnCallingThis + backUpStkSize;
		if (backend::Imm<backend::ImmType::Imm8m>::fitThis(stkSizeArgs)) {
			res += backend::toASM(
				"sub", backend::RId::sp, backend::RId::sp, stkSizeArgs
			);
		} else {
			genASMLoadInt(res, stkSizeArgs, backend::RId::lhs);
			res += backend::toASM(
				"sub", backend::RId::sp, backend::RId::sp, backend::RId::lhs
			);
		}
	}
	//  call
	if (pFuncInfoToCall->pFuncLabel->lineNum == INT_MIN || backend::inLabelRange(
		backend::instrCnt, pFuncInfoToCall->pFuncLabel->lineNum
	)) {
		res += backend::toASM("bl", pFuncInfoToCall->pFuncLabel->labelStr);
	} else {
		genASMLoadLabel(res, pFuncInfoToCall->pFuncLabel, backend::RId::lhs);
		res += backend::toASM("blx", backend::RId::lhs);
	}
	//  restore arguments field on calling next function
	if (pFuncInfoToCall->argsStkSizeOnCallingThis + backUpStkSize) {
		auto stkSizeArgs = pFuncInfoToCall->argsStkSizeOnCallingThis + backUpStkSize;
		if (backend::Imm<backend::ImmType::Imm8m>::fitThis(stkSizeArgs)) {
			res += backend::toASM(
				"add", backend::RId::sp, backend::RId::sp, stkSizeArgs
			);
		} else {
			genASMLoadInt(res, stkSizeArgs, backend::RId::lhs);
			res += backend::toASM(
				"add", backend::RId::sp, backend::RId::sp, backend::RId::lhs
			);
		}
	}
	auto ridRet = backend::RId::err;
	auto sidRet = backend::SId::err;
	if (pInstrCall->retAddr) {
		switch (pInstrCall->retAddr->addrType) {
			case ircode::AddrType::Var: {
				switch (pInstrCall->retAddr->getType().type) {
					case sup::Type::Int_t: {
						auto * pVRegRRet = convertIntVariable(pInstrCall->retAddr);
						genASMSaveFromRRegToVRegR(
							res, pVRegRRet, backend::RId::r0, backend::RId::rhs
						);
						ridRet = pVRegRRet->rid;
						break;
					}
					case sup::Type::Float_t: {
						auto * pVRegSRet = convertFloatVariable(pInstrCall->retAddr);
						genASMSaveFromSRegToVRegS(
							res, pVRegSRet, backend::SId::s0, backend::RId::rhs
						);
						sidRet = pVRegSRet->sid;
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
		if (rId != ridRet) {
			genASMDerefStkPtr(res, offset, rId);
		}
	}
	for (auto [sId, offset]: sId2StkOffset) {
		if (sId != sidRet) {
			genASMDerefStkPtrToSReg(res, offset, sId, backend::RId::rhs);
		}
	}
	return res;
}

}