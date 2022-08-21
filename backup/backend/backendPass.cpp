#include "backendPass.hpp"
#define WHILE_ST "while_cond"
#define WHILE_ED "while_end"

namespace pass {


ToASM::ToASM(ircode::IRModule & ir, const std::string & name) :
	IRPass(ir, name) {
}

int ToASM::run() {
	for (auto * pGVarAddr: ir.addrPool.getGlobalVars()) {
		gVarToLabel[pGVarAddr] = opndPool.emplace_back(
			backend::Label(pGVarAddr)
		);
	}
	for (auto * pFuncDef: ir.funcPool.funcDefs) {
		auto * pFuncInfo = funcInfoPool.emplace_back(
			FuncInfo(
				opndPool, pFuncDef, addrToFuncInfo, gVarToLabel,
				std::make_unique<backend::AllOnStkAllocator>(opndPool)
			)
		);
		addrToFuncInfo[pFuncDef->pAddrFun] = pFuncInfo;
		if (pFuncDef->pAddrFun->justDeclare) {
			continue;
		}
		pFuncInfo->run();
	}
	return 0;
}


std::string ToASM::toASM() {
	auto res = std::string(pass::ToASM::asmHeader) + "\n\n";
	if (!ir.addrPool.getGlobalVars().empty()) {
		res += std::string(gVarHeader);
		for (auto * pGVarAddr: ir.addrPool.getGlobalVars()) {
			res += declGVar(pGVarAddr) + "\n";
		}
		res += "\n\n";
	}
	res += std::string(functionsHeader) + "\n\n";
	for (auto * pFuncInfo: funcInfoPool) {
		if (pFuncInfo->pFuncDef->pAddrFun->justDeclare) {
			continue;
		}
		res += pFuncInfo->toASM() + "\n";
	}
	return res;
}

FuncInfo::FuncInfo(
	backend::OpndPool & pool, ircode::IRFuncDef * pFuncDef,
	std::map<ircode::AddrFunction *, FuncInfo *> & addrFuncToFuncInfo,
	std::map<ircode::AddrGlobalVariable *, backend::Label *> & m_AddrGVar_Label,
	std::unique_ptr<backend::RegisterAllocator> regAllocator
) :
	opndPool(pool),
	m_AddrFunc_FuncInfo(addrFuncToFuncInfo), m_AddrGVar_Label(m_AddrGVar_Label),
	regAllocator(std::move(regAllocator)), pFuncDef(pFuncDef),
	pFuncLabel(opndPool.emplace_back(backend::Label(pFuncDef->pAddrFun))) {
	int rid = 0, sid = 0;
	for (auto * pAddrPara: pFuncDef->pAddrFun->vecPtrAddrPara) {
		switch (pAddrPara->getType().type) {
			case sup::Type::Pointer_t:
			case sup::Type::Int_t: {
				auto * opndPara = static_cast<backend::VRegR *>(nullptr);
				if (rid <= backend::mxRIdForParameters) {
					opndPara = pool.emplace_back(
						backend::VRegR(backend::RId(rid), INT_MIN)
					);
					++rid;
				} else {
					opndPara = pool.emplace_back(
						backend::VRegR(
							backend::RId::stk, -argsStkSizeOnCallingThis - 4
						)
					);
					argsStkSizeOnCallingThis += 4;
				}
				argsInfoOnCallingThis[pAddrPara] = opndPara;
				m_AddrArg_VRegR[pAddrPara] = opndPool.emplace_back(
					backend::VRegR(*opndPara)
				);
				break;
			}
			case sup::Type::Float_t: {
				auto * opndPara = static_cast<backend::VRegS *>(nullptr);
				if (sid <= backend::mxSIdForParameters) {
					opndPara = pool.emplace_back(
						backend::VRegS(backend::SId(sid), INT_MIN)
					);
					++sid;
				} else {
					opndPara = pool.emplace_back(
						backend::VRegS(
							backend::SId::stk, -argsStkSizeOnCallingThis - 4
						)
					);
					argsStkSizeOnCallingThis += 4;
				}
				argsInfoOnCallingThis[pAddrPara] = opndPara;
				m_AddrArg_VRegS[pAddrPara] = opndPool.emplace_back(
					backend::VRegS(*opndPara)
				);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	if (argsStkSizeOnCallingThis % 8 != 0) {
		argsStkSizeOnCallingThis += 4;
	}
}

std::string FuncInfo::toASM() {
	auto res = "@\t" + pFuncDef->pAddrFun->declLLVMIR() + " \n";
	res += pFuncLabel->labelStr + ":\n";
	if (!backupRReg.empty()) {
		genASMPushRegs(res, backupRReg);
	}
	if (!backupAndRestoreSReg.empty()) {
		genASMPushRegs(res, backupAndRestoreSReg);
	}
	if (backend::Imm<backend::ImmType::Imm8m>::fitThis(spilledStkSize)) {
		res += backend::toASM(
			"sub", backend::RId::sp, backend::RId::sp, spilledStkSize
		);
	} else {
		genASMLoadInt(res, spilledStkSize, backend::RId::rhs);
		res += backend::toASM(
			"sub", backend::RId::sp, backend::RId::sp, backend::RId::rhs
		);
	}
	for (auto * pInstr: pFuncDef->instrs) {
		res += toASM(pInstr);
	}
	if (backend::Imm<backend::ImmType::Imm8m>::fitThis(spilledStkSize)) {
		res += backend::toASM(
			"add", backend::RId::sp, backend::RId::sp, spilledStkSize
		);
	} else {
		genASMLoadInt(res, spilledStkSize, backend::RId::rhs);
		res += backend::toASM(
			"add", backend::RId::sp, backend::RId::sp, backend::RId::rhs
		);
	}
	if (!backupAndRestoreSReg.empty()) {
		genASMPopRegs(res, backupAndRestoreSReg);
	}
	if (!restoreRReg.empty()) {
		genASMPopRegs(res, restoreRReg);
	}
	return res;
}

int FuncInfo::run() {
	tim = 0;
	loop_labels=0;
	for (auto * pInstr: pFuncDef->instrs) {
		run(pInstr);
	}
	com::Assert(regAllocator.get(), "", CODEPOS);
	regAllocator->set(
		allUsedVRegR, allUsedVRegS, allStkPtr, defineUseTimelineVRegR,
		defineUseTimelineVRegS, tim, argsInfoOnCallingThis, argsStkSizeOnCallingThis,
		m_AddrArg_VRegR, m_AddrArg_VRegS
	);
	regAllocator->getRes();
	backupRReg = regAllocator->backupRReg;
	restoreRReg = regAllocator->restoreRReg;
	backupAndRestoreSReg = regAllocator->backupAndRestoreSReg;
	callerSaveRReg = regAllocator->callerSaveRReg;
	callerSaveSReg = regAllocator->callerSaveSReg;
	m_AddrArg_VRegR = regAllocator->m_AddrArg_VRegR;
	m_AddrArg_VRegS = regAllocator->m_AddrArg_VRegS;
	spilledStkSize = regAllocator->spilledStkSize;
	backupStkSize = regAllocator->backupStkSize;
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrAlloca * pInstrAlloca) {
	return "";
}

int FuncInfo::run(ircode::InstrAlloca * pInstrAlloca) {
	convertLocalVar(pInstrAlloca->allocaTo);
	return 0;
}

int FuncInfo::run(ircode::InstrBr * pBr) {
	if (pBr->pLabelTrue) {
		convertLabel(pBr->pLabelTrue);
	}
	if (pBr->pLabelFalse) {
		convertLabel(pBr->pLabelFalse);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrBr * pBr) {
	auto res = std::string();
	if (!pBr->pCond) {    //  unconditional jump
		com::Assert(pBr->pLabelTrue, "", CODEPOS);
		auto * pASMLabelTrue = convertLabel(pBr->pLabelTrue);
		res += backend::toASM("b", pASMLabelTrue->labelStr);
	} else if (pBr->pCond->addrType == ircode::AddrType::StaticValue) {
		auto * pSVCond = dynamic_cast<ircode::AddrStaticValue *>(pBr->pCond);
		auto b = dynamic_cast<const sup::BoolStaticValue &>(
			pSVCond->getStaticValue()
		).value;
		if (b) {
			if (pBr->pLabelTrue) {
				res += backend::toASM("b", convertLabel(pBr->pLabelTrue)->labelStr);
			}
		} else {
			if (pBr->pLabelFalse) {
				res += backend::toASM("b", convertLabel(pBr->pLabelFalse)->labelStr);
			}
		}
	} else {
		com::Assert(lastCondVarAddr == pBr->pCond, "", CODEPOS);
		if (pBr->pLabelTrue && pBr->pLabelFalse) {
			auto condStr = std::string();
			switch (cmpType) {
				case CmpType::I: {
					condStr = genASMCondName(lastICmp, false);
					res += backend::toASM(
						"b" + condStr, convertLabel(pBr->pLabelTrue)->labelStr
					);
					res += backend::toASM(
						"b", convertLabel(pBr->pLabelFalse)->labelStr
					);
					break;
				}
				case CmpType::F: {
					condStr = genASMCondNameReverse(lastFCmp);
					res += backend::toASM("vmrs", "APSR_nzcv", "fpscr");
					res += backend::toASM(
						"b" + condStr, convertLabel(pBr->pLabelFalse)->labelStr
					);
					res += backend::toASM("b", convertLabel(pBr->pLabelTrue)->labelStr);
					break;
				}
				default:com::Throw("", CODEPOS);
			}
		} else if (pBr->pLabelTrue && !pBr->pLabelFalse) {
			switch (cmpType) {
				case CmpType::I: {
					auto condStr = genASMCondName(lastICmp, false);
					res += backend::toASM(
						"b" + condStr, convertLabel(pBr->pLabelTrue)->labelStr
					);
					break;
				}
				default:com::Throw("", CODEPOS);
			}
		} else if (!pBr->pLabelTrue && pBr->pLabelFalse) {
			auto condStr = std::string();
			switch (cmpType) {
				case CmpType::I: {
					condStr = genASMCondName(lastICmp, true);
					res += backend::toASM(
						"b" + condStr, convertLabel(pBr->pLabelTrue)->labelStr
					);
					break;
				}
				case CmpType::F: {
					condStr = genASMCondNameReverse(lastFCmp);
					res += backend::toASM("vmrs");
					res += backend::toASM(
						"b" + condStr, convertLabel(pBr->pLabelFalse)->labelStr
					);
					break;
				}
				default:com::Throw("", CODEPOS);
			}
		} else {
			com::Throw("", CODEPOS);
		}
	}
	cmpType = CmpType::N;
	lastICmp = ircode::ICMP::ERR;
	lastFCmp = ircode::FCMP::ERR;
	lastCondVarAddr = nullptr;
	return res;
}

int FuncInfo::run(ircode::InstrRet * pRet) {
	com::Assert(
		!pRet->retAddr ||
			com::enum_fun::in(
				pRet->retAddr->addrType, {
					ircode::AddrType::Var, ircode::AddrType::ParaVar,
					ircode::AddrType::StaticValue
				}
			), "", CODEPOS
	);
	if (pRet->retAddr && pRet->retAddr->addrType == ircode::AddrType::Var) {
		auto * pAddrVar = dynamic_cast<ircode::AddrVariable *>(pRet->retAddr);
		switch (pRet->retAddr->getType().type) {
			case sup::Type::Int_t: {
				auto * pRetOpnd = convertIntVariable(pAddrVar);
				defineUseTimelineVRegR[pRetOpnd].emplace_back(tim);
				break;
			}
			case sup::Type::Float_t: {
				auto * pRetOpnd = convertFloatVariable(pAddrVar);
				defineUseTimelineVRegS[pRetOpnd].emplace_back(tim);
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrRet * pRet) {
	switch (pFuncDef->pAddrFun->getReturnTypeInfo().type) {
		case sup::Type::Int_t: {
			return toASM_Ret_Int(pRet);
		}
		case sup::Type::Float_t: {
			return toASM_Ret_Float(pRet);
		}
		case sup::Type::Void_t: {
			return { };
		}
		default:com::Throw("", CODEPOS);
	}
}

std::string FuncInfo::toASM_Ret_Int(ircode::InstrRet * pRet) {
	auto res = std::string();
	auto * pRetAddr = pRet->retAddr;
	switch (pRetAddr->addrType) {
		case ircode::AddrType::Var: {
			auto * pRetVarAddr = dynamic_cast<ircode::AddrVariable *>(pRetAddr);
			auto * pVRegRRet = convertIntVariable(pRetVarAddr);
			genASMGetVRegRVal(res, pVRegRRet, backend::RId::r0);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pRetSV = dynamic_cast<ircode::AddrStaticValue *>(pRetAddr);
			int value = dynamic_cast<const sup::IntStaticValue &>(
				pRetSV->getStaticValue()
			).value;
			genASMLoadInt(res, value, backend::RId::r0);
			break;
		}
		default: { com::Throw("", CODEPOS); }
	}
	return res;
}

std::string FuncInfo::toASM_Ret_Float(ircode::InstrRet * pRet) {
	auto res = std::string();
	auto * pRetAddr = pRet->retAddr;
	switch (pRetAddr->addrType) {
		case ircode::AddrType::Var: {
			auto * pRetVarAddr = dynamic_cast<ircode::AddrVariable *>(pRetAddr);
			auto * pVRegSRet = convertFloatVariable(pRetVarAddr);
			genASMGetVRegSVal(res, pVRegSRet, backend::SId::s0, backend::RId::lhs);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pRetSV = dynamic_cast<ircode::AddrStaticValue *>(pRetAddr);
			auto value = dynamic_cast<const sup::FloatStaticValue &>(
				pRetSV->getStaticValue()
			).value;
			genASMLoadFloat(res, value, backend::SId::s0, backend::RId::lhs);
			break;
		}
		default: { com::Throw("", CODEPOS); }
	}
	return res;

}

int FuncInfo::run(ircode::IRInstr * pInstr) {
	++tim;
	int retVal;
	switch (pInstr->instrType) {
		case ircode::InstrType::Alloca: {
			retVal = run(dynamic_cast<ircode::InstrAlloca *>(pInstr));
			break;
		}
		case ircode::InstrType::Br: {
			retVal = run(dynamic_cast<ircode::InstrBr *>(pInstr));
			break;
		}
		case ircode::InstrType::Load: {
			retVal = run(dynamic_cast<ircode::InstrLoad *>(pInstr));
			break;
		}
		case ircode::InstrType::Ret: {
			retVal = run(dynamic_cast<ircode::InstrRet *>(pInstr));
			break;
		}
		case ircode::InstrType::Label: {
			retVal = run(dynamic_cast<ircode::InstrLabel *>(pInstr));
			break;
		}
		case ircode::InstrType::Store: {
			retVal = run(dynamic_cast<ircode::InstrStore *>(pInstr));
			break;
		}
		case ircode::InstrType::Add:
		case ircode::InstrType::Sub:
		case ircode::InstrType::Mul:
		case ircode::InstrType::SDiv:
		case ircode::InstrType::SRem: {
			retVal = run_Binary_Op_Int(dynamic_cast<ircode::InstrBinaryOp *>(pInstr));
			break;
		}
		case ircode::InstrType::ICmp: {
			retVal = run(dynamic_cast<ircode::InstrICmp *>(pInstr));
			break;
		}
		case ircode::InstrType::FCmp: {
			retVal = run(dynamic_cast<ircode::InstrFCmp *>(pInstr));
			break;
		}
		case ircode::InstrType::Call: {
			retVal = run(dynamic_cast<ircode::InstrCall *>(pInstr));
			break;
		}
		case ircode::InstrType::Getelementptr: {
			retVal = run(dynamic_cast<ircode::InstrGetelementptr *>(pInstr));
			break;
		}
		case ircode::InstrType::ZExt: {
			retVal = run(dynamic_cast<ircode::InstrZExt *>(pInstr));
			break;
		}
		case ircode::InstrType::FAdd:
		case ircode::InstrType::FSub:
		case ircode::InstrType::FDiv:
		case ircode::InstrType::FMul: {
			retVal = run_Binary_Op_Float(dynamic_cast<ircode::InstrBinaryOp *>(pInstr));
			break;
		}
		case ircode::InstrType::Fptosi:
		case ircode::InstrType::Sitofp: {
			retVal = run(dynamic_cast<ircode::InstrConversionOp *>(pInstr));
			break;
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
	return retVal;
}

std::string FuncInfo::toASM(ircode::IRInstr * pInstr) {
	auto res = "@ " + pInstr->toLLVMIR() + " \n";
	switch (pInstr->instrType) {
		case ircode::InstrType::Alloca: {
			res += toASM(dynamic_cast<ircode::InstrAlloca *>(pInstr));
			break;
		}
		case ircode::InstrType::Br: {
			res += toASM(dynamic_cast<ircode::InstrBr *>(pInstr));
			break;
		}
		case ircode::InstrType::Load: {
			res += toASM(dynamic_cast<ircode::InstrLoad *>(pInstr));
			break;
		}
		case ircode::InstrType::Ret: {
			res += toASM(dynamic_cast<ircode::InstrRet *>(pInstr));
			break;
		}
		case ircode::InstrType::Label: {
			res += toASM(dynamic_cast<ircode::InstrLabel *>(pInstr));
			break;
		}
		case ircode::InstrType::Store: {
			res += toASM(dynamic_cast<ircode::InstrStore *>(pInstr));
			break;
		}
		case ircode::InstrType::Add:
		case ircode::InstrType::Sub:
		case ircode::InstrType::Mul:
		case ircode::InstrType::SDiv:
		case ircode::InstrType::SRem: {
			res += toASM_Binary_Op_Int(
				dynamic_cast<ircode::InstrBinaryOp *>(pInstr));
			break;
		}
		case ircode::InstrType::ICmp: {
			res += toASM(dynamic_cast<ircode::InstrICmp *>(pInstr));
			break;
		}
		case ircode::InstrType::FCmp: {
			res += toASM(dynamic_cast<ircode::InstrFCmp *>(pInstr));
			break;
		}
		case ircode::InstrType::Call: {
			res += toASM(dynamic_cast<ircode::InstrCall *>(pInstr));
			break;
		}
		case ircode::InstrType::Getelementptr: {
			res += toASM(dynamic_cast<ircode::InstrGetelementptr *>(pInstr));
			break;
		}
		case ircode::InstrType::ZExt: {
			res += toASM(dynamic_cast<ircode::InstrZExt *>(pInstr));
			break;
		}
		case ircode::InstrType::FAdd:
		case ircode::InstrType::FSub:
		case ircode::InstrType::FDiv:
		case ircode::InstrType::FMul: {
			res += toASM_Binary_Op_Float(dynamic_cast<ircode::InstrBinaryOp *>(pInstr));
			break;
		}
		case ircode::InstrType::Sitofp: {
			res += toASM(dynamic_cast<ircode::InstrSitofp *>(pInstr));
			break;
		}
		case ircode::InstrType::Fptosi: {
			res += toASM(dynamic_cast<ircode::InstrFptosi *>(pInstr));
			break;
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
	return res;
}

int FuncInfo::run(ircode::InstrLabel * pInstrLabel) {
	convertLabel(pInstrLabel->pAddrLabel);
	if(pInstrLabel->pAddrLabel->labelName==WHILE_ST) loop_labels++;
	else if(pInstrLabel->pAddrLabel->labelName==WHILE_ED) loop_labels--;
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrLabel * pInstrLabel) {
	auto * pASMLabel = convertLabel(pInstrLabel->pAddrLabel);
	auto res = pASMLabel->labelStr + ":\n";
	return res;
}

int FuncInfo::run(ircode::InstrICmp * pInstrICmp) {
	markOperand(pInstrICmp->leftOp);
	markOperand(pInstrICmp->rightOp);
	return 0;
}

int FuncInfo::run(ircode::InstrFCmp * pInstrFCmp) {
	markOperand(pInstrFCmp->leftOp);
	markOperand(pInstrFCmp->rightOp);
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrICmp * pInstrICmp) {
	auto res = std::string();
	auto lOp = std::string();
	auto * pOpndAddrL = pInstrICmp->leftOp;
	switch (pOpndAddrL->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOpndAddrL);
			lOp = backend::to_asm(
				genASMGetVRegRVal(
					res, convertIntVariable(pVarAddr), backend::RId::lhs
				)
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pOpndAddrL);
			auto val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			lOp = backend::to_asm(genASMLoadInt(res, val, backend::RId::lhs));
			break;
		}
		default: com::Throw("", CODEPOS);
	}
	auto rOp = std::string();
	auto * pOpndAddrR = pInstrICmp->rightOp;
	switch (pOpndAddrR->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOpndAddrR);
			rOp = backend::to_asm(
				genASMGetVRegRVal(
					res, convertIntVariable(pVarAddr), backend::RId::rhs
				)
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pOpndAddrR);
			auto val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			if (backend::Imm<backend::ImmType::Imm8m>::fitThis(val)) {
				rOp = backend::to_asm(val);
			} else {
				rOp = backend::to_asm(genASMLoadInt(res, val, backend::RId::rhs));
			}
			break;
		}
		default: com::Throw("", CODEPOS);
	}
	res += backend::toASM("cmp", lOp, rOp);
	cmpType = CmpType::I;
	lastICmp = pInstrICmp->icmp;
	lastCondVarAddr = pInstrICmp->dest;
	return res;
}

std::string FuncInfo::toASM(ircode::InstrFCmp * pInstrFCmp) {
	auto res = std::string();
	auto lOp = std::string();
	auto * pOpndAddrL = pInstrFCmp->leftOp;
	switch (pOpndAddrL->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOpndAddrL);
			lOp = backend::to_asm(
				genASMGetVRegSVal(
					res, convertFloatVariable(pVarAddr), backend::SId::lhs, backend::RId::lhs
				)
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pOpndAddrL);
			auto val = dynamic_cast<const sup::FloatStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			lOp = backend::to_asm(
				genASMLoadFloat(res, val, backend::SId::lhs, backend::RId::lhs)
			);
			break;
		}
		default: com::Throw("", CODEPOS);
	}
	auto rOp = std::string();
	auto * pOpndAddrR = pInstrFCmp->rightOp;
	switch (pOpndAddrR->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pOpndAddrR);
			rOp = backend::to_asm(
				genASMGetVRegSVal(
					res, convertFloatVariable(pVarAddr), backend::SId::rhs, backend::RId::rhs
				)
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pOpndAddrR);
			auto val = dynamic_cast<const sup::FloatStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rOp = backend::to_asm(
				genASMLoadFloat(res, val, backend::SId::rhs, backend::RId::rhs)
			);
			break;
		}
		default: com::Throw("", CODEPOS);
	}
	res += backend::toASM("vcmp.f32", lOp, rOp);
	cmpType = CmpType::F;
	lastFCmp = pInstrFCmp->fcmp;
	lastCondVarAddr = pInstrFCmp->dest;
	return res;

}

int FuncInfo::run(ircode::InstrCall * pInstrCall) {
	for (auto * pOperandAddr: pInstrCall->paramsPassing) {
		markOperand(pOperandAddr);
	}
	if (pInstrCall->retAddr) { markOperand(pInstrCall->retAddr); }
	return 0;
}

int FuncInfo::run(ircode::InstrGetelementptr * pInstrGetelementptr) {
	markOperand(pInstrGetelementptr->from);
	//if(pFrom->addrType==ircode::AddrType::)
	for (auto * p: pInstrGetelementptr->idxs) {
		markOperand(p);
	}
	markOperand(pInstrGetelementptr->to);
	auto * pRegTo=convertIntVariable(pInstrGetelementptr->to);
	if(pInstrGetelementptr->from->addrType==ircode::AddrType::LocalVar||
			pInstrGetelementptr->from->addrType==ircode::AddrType::GlobalVar){
		pRegTo->laddr=std::make_shared<backend::LocalAddr>(pInstrGetelementptr->from,false);
	}
	else {
		auto * pRegFrom=convertIntVariable(pInstrGetelementptr->from);
		pRegTo->laddr=std::make_shared<backend::LocalAddr>(*pRegFrom->laddr);
	}
	for(auto *p:pInstrGetelementptr->idxs){
		if(p->addrType==ircode::AddrType::StaticValue){
			auto pvalue=dynamic_cast<ircode::AddrStaticValue *>(p);
			auto static_value=&pvalue->getStaticValue();
			if(typeid(*static_value)==typeid(sup::IntStaticValue)){
				auto intv=dynamic_cast<const sup::IntStaticValue *>(static_value);
				pRegTo->laddr->idx.push_back(intv->value);
			}
		}
		else pRegTo->laddr->idx.push_back(p->id);
	}
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrGetelementptr * pInstrGetelementptr) {
	com::addRuntimeWarning(
		"Can be optimized: check if pVRegRTo is in R0-R11, if so, just accumulate to reg.",
		CODEPOS, com::addWarningOnlyOnce
	);
	auto res = std::string();
	auto * pVRegRTo = convertIntVariable(pInstrGetelementptr->to);
	//  generate information of index
	auto biasInt = 0;
	auto pTypeInfoNow = com::dynamic_cast_uPtr<sup::TypeInfo>(
		pInstrGetelementptr->from->getType().cloneToUniquePtr()
	);
	auto biasCntOnStk = 0;
	for (auto * pOperandIdx: pInstrGetelementptr->idxs) {
		pTypeInfoNow = sup::typeDeduceForBackend(*pTypeInfoNow, 1);
		if (pOperandIdx->addrType == ircode::AddrType::StaticValue) {
			auto * pSVIdx = dynamic_cast<ircode::AddrStaticValue *>(pOperandIdx);
			auto i = dynamic_cast<const sup::IntStaticValue &>(
				pSVIdx->getStaticValue()
			).value;
			biasInt += i * pTypeInfoNow->getSize();
		} else if (pOperandIdx->addrType == ircode::AddrType::Var) {
			auto * pVarIdx = dynamic_cast<ircode::AddrVariable *>(pOperandIdx);
			auto * pVRegRIdx = convertIntVariable(pVarIdx);
			genASMSaveFromVRegRToRReg(res, pVRegRIdx, backend::RId::lhs);
			genASMLoadInt(res, pTypeInfoNow->getSize(), backend::RId::rhs);
			res += backend::toASM(
				"mul", backend::RId::lhs, backend::RId::lhs, backend::RId::rhs
			);
			genASMSaveFromRRegToOffset(
				res, backend::RId::lhs, -(biasCntOnStk + 1) * 4, backend::RId::rhs
			);
			biasCntOnStk += 1;
		} else { com::Throw("", CODEPOS); }
	}
	genASMLoadInt(res, biasInt, backend::RId::lhs);  //  biasInt
	while (biasCntOnStk > 0) {  //  \sum size[i]*idx[i]
		genASMDerefStkPtr(res, -biasCntOnStk * 4, backend::RId::rhs);
		res += backend::toASM(
			"add", backend::RId::lhs, backend::RId::lhs, backend::RId::rhs
		);
		--biasCntOnStk;
	}
	//  base ptr
	switch (pInstrGetelementptr->from->addrType) {
		case ircode::AddrType::Var: {
			auto rIdBase = genASMGetVRegRVal(
				res, convertIntVariable(pInstrGetelementptr->from), backend::RId::rhs
			);
			res += backend::toASM(
				"add", backend::RId::lhs, backend::RId::lhs, rIdBase
			);
			break;
		}
		case ircode::AddrType::GlobalVar: {
			auto * pGVarAddrFrom = dynamic_cast<ircode::AddrGlobalVariable *>(
				pInstrGetelementptr->from
			);
			auto rIdBase = genASMLoadLabel(
				res, convertGlobalVar(pGVarAddrFrom), backend::RId::rhs
			);
			res += backend::toASM(
				"add", backend::RId::lhs, backend::RId::lhs, rIdBase
			);
			break;
		}
		case ircode::AddrType::LocalVar: {
			auto * pLVarAddrFrom = dynamic_cast<ircode::AddrLocalVariable *>(
				pInstrGetelementptr->from
			);
			auto offset = convertLocalVar(pLVarAddrFrom)->offset;
			com::Assert(offset != INT_MIN, "", CODEPOS);
			if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
				res += backend::toASM(
					"add", backend::RId::rhs, backend::RId::sp, offset
				);
			} else {
				genASMLoadInt(res, offset, backend::RId::rhs);
				res += backend::toASM(
					"add", backend::RId::rhs, backend::RId::rhs, backend::RId::sp
				);
			}
			res += backend::toASM(
				"add", backend::RId::lhs, backend::RId::lhs, backend::RId::rhs
			);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	//  res is in lhs, save to `to`
	if (backend::isGPR(pVRegRTo->rid)) {
		res += backend::toASM("mov", pVRegRTo->rid, backend::RId::lhs);
	} else if (pVRegRTo->rid == backend::RId::stk) {
		genASMSaveFromRRegToOffset(
			res, backend::RId::lhs, pVRegRTo->offset, backend::RId::rhs
		);
	} else { com::Throw("", CODEPOS); }
	return res;
}

int FuncInfo::run(ircode::InstrZExt * pInstrZExt) {
	markOperand(pInstrZExt->to);
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrZExt * pInstrZExt) {
	//  TODO: can be optimized
	auto res = std::string();
	auto * pOpndFrom = pInstrZExt->from;
	com::Assert(pInstrZExt->from->getType().type == sup::Type::Bool_t, "", CODEPOS);
	auto extValRId = backend::RId::lhs;
	switch (pOpndFrom->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarAddrFrom = dynamic_cast<ircode::AddrVariable *>(
				pInstrZExt->from
			);
			com::Assert(lastCondVarAddr == pVarAddrFrom, "", CODEPOS);
			switch (cmpType) {
				case CmpType::I: {
					res += backend::toASM("eor", extValRId, extValRId, extValRId);
					res += backend::toASM("movw" + genASMCondName(lastICmp), extValRId, 1);
					break;
				}
				case CmpType::F: {
					res += backend::toASM("movw", extValRId, 1);
					res += backend::toASM(
						"movw" + genASMCondNameReverse(lastFCmp), extValRId, 0
					);
					break;
				}
				default:com::Throw("", CODEPOS);
			}
			lastCondVarAddr = nullptr;
			lastICmp = ircode::ICMP::ERR;
			lastFCmp = ircode::FCMP::ERR;
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddrFrom = dynamic_cast<ircode::AddrStaticValue * >(
				pInstrZExt->from
			);
			auto val = dynamic_cast<const sup::BoolStaticValue &>(
				pSVAddrFrom->getStaticValue()
			).value;
			if (val) {
				res += backend::toASM("movw", extValRId, 1);
			} else {
				res += backend::toASM("eor", extValRId, extValRId, extValRId);
			}
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto * pVarTo = pInstrZExt->to;
	auto * pVRegTo = convertIntVariable(pVarTo);
	genASMSaveFromRRegToVRegR(res, pVRegTo, extValRId, backend::RId::rhs);
	return res;
}

int FuncInfo::run(ircode::InstrConversionOp * pInstrConversionOp) {
	markOperand(pInstrConversionOp->from);
	markOperand(pInstrConversionOp->to);
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrSitofp * pInstrSitofp) {
	auto res = std::string();
	auto valIntFrom = backend::RId::err;
	auto * pOperandFrom = pInstrSitofp->from;
	auto * pVarTo = pInstrSitofp->to;
	switch (pOperandFrom->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pOperandFrom);
			auto * pVRegRFrom = convertIntVariable(pVarFrom);
			valIntFrom = genASMGetVRegRVal(res, pVRegRFrom, backend::RId::lhs);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVFrom = dynamic_cast<ircode::AddrStaticValue *>(pOperandFrom);
			auto staticVal = dynamic_cast<const sup::IntStaticValue &>(
				pSVFrom->getStaticValue()
			).value;
			valIntFrom = genASMLoadInt(res, staticVal, backend::RId::lhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	com::Assert(pVarTo->addrType == ircode::AddrType::Var, "", CODEPOS);
	auto * pVRegSTo = convertFloatVariable(pVarTo);
	if (backend::isGPR(pVRegSTo->sid)) {
		res += backend::toASM("vmov", pVRegSTo->sid, valIntFrom);
		res += backend::toASM("vcvt.f32.s32", pVRegSTo->sid, pVRegSTo->sid);
	} else if (pVRegSTo->sid == backend::SId::stk) {
		res += backend::toASM("vmov", backend::SId::lhs, valIntFrom);
		res += backend::toASM("vcvt.f32.s32", backend::SId::lhs, backend::SId::lhs);
		auto offset = pVRegSTo->offset;
		auto strTo = std::string();
		if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
			strTo = "[sp, " + backend::to_asm(offset) + "]";
		} else if (backend::Imm<backend::ImmType::ImmOffset>::fitThis(offset)) {
			res += backend::toASM("add", backend::RId::rhs, backend::RId::rhs, offset);
			strTo = "[" + backend::to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		} else {
			genASMLoadInt(res, offset, backend::RId::rhs);
			res += backend::toASM(
				"add", backend::RId::rhs, backend::RId::sp, backend::RId::rhs
			);
			strTo = "[" + backend::to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		}
		res += backend::toASM("vstr", backend::SId::lhs, strTo);
	} else { com::Throw("", CODEPOS); }
	return res;
}

std::string FuncInfo::toASM(ircode::InstrFptosi * pInstrFptosi) {
	auto res = std::string();
	auto * pOperandFrom = pInstrFptosi->from;
	auto * pVarTo = pInstrFptosi->to;
	switch (pOperandFrom->addrType) {
		case ircode::AddrType::Var: {
			auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pOperandFrom);
			auto * pVRegSFrom = convertFloatVariable(pVarFrom);
			genASMSaveFromVRegSToSReg(res, pVRegSFrom, backend::SId::lhs, backend::RId::lhs);
			res += backend::toASM("vcvt.s32.f32", backend::SId::lhs, backend::SId::lhs);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVFrom = dynamic_cast<ircode::AddrStaticValue *>(pOperandFrom);
			auto staticVal = dynamic_cast<const sup::FloatStaticValue &>(
				pSVFrom->getStaticValue()
			).value;
			genASMLoadFloat(res, staticVal, backend::SId::lhs, backend::RId::lhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	com::Assert(pVarTo->addrType == ircode::AddrType::Var, "", CODEPOS);
	auto * pVRegRTo = convertIntVariable(pVarTo);
	if (backend::isGPR(pVRegRTo->rid)) {
		res += backend::toASM("vmov", pVRegRTo->rid, backend::SId::lhs);
	} else if (pVRegRTo->rid == backend::RId::stk) {
		auto offset = pVRegRTo->offset;
		auto strTo = std::string();
		if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
			strTo = "[sp, " + backend::to_asm(offset) + "]";
		} else if (backend::Imm<backend::ImmType::ImmOffset>::fitThis(offset)) {
			res += backend::toASM("add", backend::RId::rhs, backend::RId::rhs, offset);
			strTo = "[" + backend::to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		} else {
			genASMLoadInt(res, offset, backend::RId::rhs);
			res += backend::toASM(
				"add", backend::RId::rhs, backend::RId::sp, backend::RId::rhs
			);
			strTo = "[" + backend::to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		}
		res += backend::toASM("vstr", backend::SId::lhs, strTo);
	} else { com::Throw("", CODEPOS); }
	return res;
}
}