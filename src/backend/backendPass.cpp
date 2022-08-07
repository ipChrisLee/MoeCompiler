#include "backendPass.hpp"


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
	for (auto * pGVarAddr: ir.addrPool.getGlobalVars()) {
		res += declGVar(pGVarAddr) + "\n";
	}
	res += "\n\n";
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
		argsStkSizeOnCallingThis += pAddrPara->getType().getSize();
		switch (pAddrPara->getType().type) {
			case sup::Type::Pointer_t:
			case sup::Type::Int_t: {
				auto * opndPara = pool.emplace_back(
					backend::VRegR(
						rid < 4 ? backend::RId(rid) : backend::RId::stk,
						rid < 4 ? INT_MIN : (rid - 4 + 1) * -4
					)
				);
				++rid;
				argsOnCallingThis[pAddrPara] = opndPara;
				break;
			}
			case sup::Type::Float_t: {
				auto * opndPara = pool.emplace_back(
					backend::VRegS(
						sid < backend::mxSIdForParameters ? backend::SId(sid)
						                                  : backend::SId::stk,
						sid < backend::mxSIdForParameters ? INT_MIN :
						(sid - backend::mxSIdForParameters + 1) * -4
					)
				);
				++sid;
				argsOnCallingThis[pAddrPara] = opndPara;
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
		genASMLoadNumber(res, spilledStkSize, backend::RId::rhs);
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
		genASMLoadNumber(res, spilledStkSize, backend::RId::rhs);
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
	for (auto * pInstr: pFuncDef->instrs) {
		run(pInstr);
	}
	com::Assert(regAllocator.get(), "", CODEPOS);
	regAllocator->set(
		allUsedVRegR, allUsedVRegS, allStkPtr, defineUseTimelineVRegR,
		defineUseTimelineVRegS, tim, argsOnCallingThis, argsStkSizeOnCallingThis
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
	com::Assert(pBr->pLabelTrue, "", CODEPOS);
	if (!pBr->pCond) {    //  unconditional jump
		auto * pASMLabelTrue = convertLabel(pBr->pLabelTrue);
		res += backend::toASM("b", pASMLabelTrue->labelStr);
	} else {
		com::Assert(lastCondVarAddr == pBr->pCond, "", CODEPOS);
		if (pBr->pLabelTrue && pBr->pLabelFalse) {
			auto condStr = genASMConditionalBranch(lastICmp, false);
			res += backend::toASM(
				"b" + condStr, convertLabel(pBr->pLabelTrue)->labelStr
			);
			res += backend::toASM(
				"b", convertLabel(pBr->pLabelFalse)->labelStr
			);
		} else if (pBr->pLabelTrue && !pBr->pLabelFalse) {
			auto condStr = genASMConditionalBranch(lastICmp, false);
			res += backend::toASM(
				"b" + condStr, convertLabel(pBr->pLabelTrue)->labelStr
			);
		} else if (!pBr->pLabelTrue && pBr->pLabelFalse) {
			auto condStr = genASMConditionalBranch(lastICmp, true);
			res += backend::toASM(
				"b" + condStr, convertLabel(pBr->pLabelFalse)->labelStr
			);
		} else {
			com::Throw("", CODEPOS);
		}
	}
	lastICmp = ircode::ICMP::ERR;
	lastCondVarAddr = nullptr;
	return res;
}

int FuncInfo::run(ircode::InstrLoad * pInstrLoad) {
	switch (pInstrLoad->to->getType().type) {
		case sup::Type::Pointer_t:
		case sup::Type::Int_t: {
			run_Load_Int(pInstrLoad);
			break;
		}
		case sup::Type::Float_t: {
			run_Load_Float(pInstrLoad);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return 0;
}

int FuncInfo::run_Load_Int(ircode::InstrLoad * pInstrLoad) {
	auto * pOpnd = convertIntVariable(pInstrLoad->to);
	defineUseTimelineVRegR[pOpnd].emplace_back(tim);
	if (pInstrLoad->from->addrType == ircode::AddrType::Var) {
		auto * pVarVRegR = convertIntVariable(pInstrLoad->from);
		defineUseTimelineVRegR[pVarVRegR].emplace_back(tim);
	} else if (pInstrLoad->from->addrType == ircode::AddrType::ParaVar) {
		com::TODO("", CODEPOS);
	} else if (pInstrLoad->from->addrType == ircode::AddrType::LocalVar) {
		auto * pLVarAddr = dynamic_cast<ircode::AddrLocalVariable *>(
			pInstrLoad->from
		);
		convertLocalVar(pLVarAddr);
	} else if (pInstrLoad->from->addrType == ircode::AddrType::GlobalVar) {
		auto * pGVarAddr = dynamic_cast<ircode::AddrGlobalVariable *>(
			pInstrLoad->from
		);
		convertGlobalVar(pGVarAddr);
	} else {
		com::Throw("", CODEPOS);
	}
	return 0;
}

int FuncInfo::run_Load_Float(ircode::InstrLoad * pInstrLoad) {
	com::Throw("", CODEPOS);
}

std::string FuncInfo::toASM(ircode::InstrLoad * pInstrLoad) {
	auto res = std::string();
	switch (pInstrLoad->to->getType().type) {
		case sup::Type::Pointer_t:
		case sup::Type::Int_t: {
			res = toASM_Load_Int(pInstrLoad);
			break;
		}
		case sup::Type::Float_t: {
			res = toASM_Load_Float(pInstrLoad);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

std::string FuncInfo::toASM_Load_Int(ircode::InstrLoad * pInstrLoad) {
	auto res = std::string();
	auto * pTo = pInstrLoad->to;
	if (pTo->addrType == ircode::AddrType::Var) {
		auto * pVRegRTo = convertIntVariable(pTo);
		if (pVRegRTo->rid == backend::RId::stk) {
			auto loadTo = genASMPtrOffsetToOperand2(
				res, pVRegRTo->offset, backend::RId::rhs
			);
			auto * pFrom = pInstrLoad->from;
			switch (pFrom->addrType) {
				case ircode::AddrType::Var: {
					auto rIdFrom = genASMGetVRegRVal(
						res, convertIntVariable(pFrom), backend::RId::lhs
					);
					auto valFrom = "[" + backend::to_asm(rIdFrom) + ", #0]";
					res += backend::toASM("ldr", backend::RId::lhs, valFrom);
					res += backend::toASM("str", backend::RId::lhs, loadTo);
					break;
				}
				case ircode::AddrType::LocalVar: {
					//  from stack to stack
					auto * pLVarAddr
						= dynamic_cast<ircode::AddrLocalVariable *>(pFrom);
					auto * pStkPtrLVal = convertLocalVar(pLVarAddr);
					com::Assert(pStkPtrLVal->offset != INT_MIN, "", CODEPOS);
					genASMDerefStkPtr(res, pStkPtrLVal->offset, backend::RId::lhs);
					res += backend::toASM("str", backend::RId::lhs, loadTo);
					break;
				}
				case ircode::AddrType::GlobalVar: {
					auto * pGVarAddr
						= dynamic_cast<ircode::AddrGlobalVariable *>(pFrom);
					genASMLoadLabel(
						res, convertGlobalVar(pGVarAddr), backend::RId::lhs
					);
					auto loadFrom = "[" + backend::to_asm(backend::RId::lhs) + ", " +
						backend::to_asm(0) + "]";
					res += backend::toASM("ldr", backend::RId::lhs, loadFrom);
					res += backend::toASM("str", backend::RId::lhs, loadTo);
					break;
				}
				default : {
					com::Throw("", CODEPOS);
				}
			}
		} else if (backend::isGPR(pVRegRTo->rid)) {
			com::TODO("", CODEPOS);
		} else {
			com::Throw("", CODEPOS);
		}
	} else {
		com::Throw("", CODEPOS);
	}
	return res;
}

std::string FuncInfo::toASM_Load_Float(ircode::InstrLoad * pInstrLoad) {
	com::TODO("", CODEPOS);
}


int FuncInfo::run(ircode::InstrRet * pRet) {
	com::Assert(
		com::enum_fun::in(
			pRet->retAddr->addrType, {
				ircode::AddrType::Var, ircode::AddrType::ParaVar,
				ircode::AddrType::StaticValue
			}
		), "", CODEPOS
	);
	if (pRet->retAddr->addrType == ircode::AddrType::Var) {
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
			com::TODO("", CODEPOS);
			break;
		}
		case sup::Type::Void_t: {
			com::TODO("", CODEPOS);
			break;
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
		case ircode::AddrType::ParaVar: {
			com::TODO("", CODEPOS);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pRetSV = dynamic_cast<ircode::AddrStaticValue *>(pRetAddr);
			int value = dynamic_cast<const sup::IntStaticValue &>(
				pRetSV->getStaticValue()
			).value;
			genASMLoadNumber(res, value, backend::RId::r0);
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
			retVal = run_Binary_Op_Int(
				dynamic_cast<ircode::InstrBinaryOp *>(pInstr));
			break;
		}
		case ircode::InstrType::ICmp: {
			retVal = run(dynamic_cast<ircode::InstrICmp *>(pInstr));
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
		case ircode::InstrType::Call: {
			res += toASM(dynamic_cast<ircode::InstrCall *>(pInstr));
			break;
		}
		case ircode::InstrType::Getelementptr: {
			res += toASM(dynamic_cast<ircode::InstrGetelementptr *>(pInstr));
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
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrLabel * pInstrLabel) {
	auto * pASMLabel = convertLabel(pInstrLabel->pAddrLabel);
	auto res = pASMLabel->labelStr + ":\n";
	return res;
}

int FuncInfo::run_Store_Int(ircode::InstrStore * pInstrStore) {
	auto * pFrom = pInstrStore->from;
	if (pFrom->addrType == ircode::AddrType::Var) {
		auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
		auto * pVRegR = convertIntVariable(pVarFrom);
		defineUseTimelineVRegR[pVRegR].emplace_back(tim);
	} else if (pFrom->addrType == ircode::AddrType::ParaVar) {
		//  do nothing
	} else if (pFrom->addrType == ircode::AddrType::StaticValue) {
		// do nothing
	} else {
		com::Throw("", CODEPOS);
	}
	auto * pTo = pInstrStore->to;
	if (pTo->addrType == ircode::AddrType::Var) {
		auto * pVarTo = dynamic_cast<ircode::AddrVariable *>(pTo);
		com::Assert(pVarTo->getType().type == sup::Type::Pointer_t, "", CODEPOS);
		auto * pVRegR = convertIntVariable(pVarTo);
		defineUseTimelineVRegR[pVRegR].emplace_back(tim);
	} else if (pTo->addrType == ircode::AddrType::LocalVar) {
		auto * pLVarTo = dynamic_cast<ircode::AddrLocalVariable *>(pTo);
		convertLocalVar(pLVarTo);
	} else if (pTo->addrType == ircode::AddrType::GlobalVar) {
		auto * pGVarTo = dynamic_cast<ircode::AddrGlobalVariable *>(pTo);
		convertGlobalVar(pGVarTo);
	} else { com::Throw("", CODEPOS); }
	return 0;
}

std::string FuncInfo::toASM(ircode::InstrStore * pInstrStore) {
	switch (pInstrStore->from->getType().type) {
		case sup::Type::Pointer_t:
		case sup::Type::Int_t: {
			return toASM_Store_Int(pInstrStore);
		}
		case sup::Type::Float_t: {
			return toASM_Store_Float(pInstrStore);
		}
		default:com::Throw("", CODEPOS);
	}
}

std::string FuncInfo::toASM_Store_Int(ircode::InstrStore * pInstrStore) {
	auto res = std::string();
	auto storeTo = std::string();
	auto storeFrom = std::string();
	auto * pTo = pInstrStore->to;
	if (pTo->addrType == ircode::AddrType::Var) {
		auto * pVarTo = dynamic_cast<ircode::AddrVariable *>(pTo);
		com::Assert(pVarTo->getType().type == sup::Type::Pointer_t, "", CODEPOS);
		auto * pVRegR = convertIntVariable(pVarTo);
		storeTo = "[" +
			backend::to_asm(genASMGetVRegRVal(res, pVRegR, backend::RId::rhs)) +
			", #0]";
	} else if (pTo->addrType == ircode::AddrType::LocalVar) {
		auto * pLVarTo = dynamic_cast<ircode::AddrLocalVariable *>(pTo);
		com::Assert(
			pLVarTo->getType().type == sup::Type::Pointer_t, "", CODEPOS
		);
		auto * pStkPtr = convertLocalVar(pLVarTo);
		int offset = pStkPtr->offset;
		com::Assert(offset != INT_MIN, "", CODEPOS);
		if (backend::Imm<backend::ImmType::ImmOffset>::fitThis(offset)) {
			storeTo = "[sp, " + backend::to_asm(offset) + "]";
		} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
			res += backend::toASM(
				"add", backend::RId::rhs, backend::RId::sp, offset
			);
			storeTo = "[" + to_asm(backend::RId::rhs) + ", #0]";
		} else {
			storeTo = "[" +
				to_asm(genASMLoadNumber(res, offset, backend::RId::rhs)) + ", #0]";
		}
	} else if (pTo->addrType == ircode::AddrType::GlobalVar) {
		auto * pGVarAddrTo = dynamic_cast<ircode::AddrGlobalVariable *>(pTo);
		auto storeRId = genASMLoadLabel(
			res, convertGlobalVar(pGVarAddrTo), backend::RId::rhs
		);
		storeTo = "[" + backend::to_asm(storeRId) + ", " + backend::to_asm(0) + "]";
	} else { com::Throw("", CODEPOS); }

	auto * pFrom = pInstrStore->from;
	if (pFrom->addrType == ircode::AddrType::Var) {
		auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
		auto * pVRegR = convertIntVariable(pVarFrom);
		auto rIdVal = genASMGetVRegRVal(res, pVRegR, backend::RId::lhs);
		storeFrom = backend::to_asm(rIdVal);
	} else if (pFrom->addrType == ircode::AddrType::ParaVar) {
		auto * pVarFrom = dynamic_cast<ircode::AddrPara *>(pFrom);
		auto * pVRegR = convertThisIntArg(pVarFrom);
		auto rIdVal = genASMGetVRegRVal(res, pVRegR, backend::RId::lhs);
		storeFrom = backend::to_asm(rIdVal);
	} else if (pFrom->addrType == ircode::AddrType::StaticValue) {
		auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
		auto & pSV = dynamic_cast<const sup::IntStaticValue &>(
			pSVAddr->getStaticValue()
		);
		storeFrom = backend::to_asm(
			genASMLoadNumber(res, pSV.value, backend::RId::lhs));
	} else {
		com::Throw("", CODEPOS);
	}
	res += backend::toASM("str", storeFrom, storeTo);
	return res;
}

int FuncInfo::run(ircode::InstrStore * pInstrStore) {
	switch (pInstrStore->from->getType().type) {
		case sup::Type::Pointer_t:
		case sup::Type::Int_t: {
			return run_Store_Int(pInstrStore);
		}
		case sup::Type::Float_t: {
			return run_Store_Float(pInstrStore);
		}
		default:com::Throw("", CODEPOS);
	}
}

int FuncInfo::run_Store_Float(ircode::InstrStore * pInstrStore) {
	com::TODO("", CODEPOS);
}

std::string FuncInfo::toASM_Store_Float(ircode::InstrStore * pInstrStore) {
	com::TODO("", CODEPOS);
}

int FuncInfo::run(ircode::InstrICmp * pInstrICmp) {
	markOperand(pInstrICmp->leftOp);
	markOperand(pInstrICmp->rightOp);
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
			lOp = backend::to_asm(genASMLoadNumber(res, val, backend::RId::lhs));
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
				rOp = backend::to_asm(genASMLoadNumber(res, val, backend::RId::rhs));
			}
			break;
		}
		default: com::Throw("", CODEPOS);
	}
	res += backend::toASM("cmp", lOp, rOp);
	lastICmp = pInstrICmp->icmp;
	lastCondVarAddr = pInstrICmp->dest;
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
	for (auto * p: pInstrGetelementptr->idxs) {
		markOperand(p);
	}
	markOperand(pInstrGetelementptr->to);
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
		pTypeInfoNow = sup::typeDeduce(*pTypeInfoNow, 1);
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
			genASMLoadNumber(res, pTypeInfoNow->getSize(), backend::RId::rhs);
			res += backend::toASM(
				"mul", backend::RId::lhs, backend::RId::lhs, backend::RId::rhs
			);
			genASMSaveFromRRegToOffset(
				res, backend::RId::lhs, -(biasCntOnStk + 1) * 4, backend::RId::rhs
			);
			biasCntOnStk += 1;
		} else { com::Throw("", CODEPOS); }
	}
	genASMLoadNumber(res, biasInt, backend::RId::lhs);  //  biasInt
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
				genASMLoadNumber(res, offset, backend::RId::rhs);
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


}
