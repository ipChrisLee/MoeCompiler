#include "backendPass.hpp"


namespace pass {

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
	auto * pVRegRTo = convertIntVariable(pInstrLoad->to);
	defineUseTimelineVRegR[pVRegRTo].emplace_back(tim);
	if (pInstrLoad->from->addrType == ircode::AddrType::Var) {
		auto * pVRegRFrom = convertIntVariable(pInstrLoad->from);
		defineUseTimelineVRegR[pVRegRFrom].emplace_back(tim);
	} else if (pInstrLoad->from->addrType == ircode::AddrType::ParaVar) {
		com::Throw("May should not occur?", CODEPOS);
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
	auto * pVRegSTo = convertFloatVariable(pInstrLoad->to);
	defineUseTimelineVRegS[pVRegSTo].emplace_back(tim);
	if (pInstrLoad->from->addrType == ircode::AddrType::Var) {
		auto * pVRegRFrom = convertIntVariable(pInstrLoad->from);
		defineUseTimelineVRegR[pVRegRFrom].emplace_back(tim);
	} else if (pInstrLoad->from->addrType == ircode::AddrType::ParaVar) {
		com::Throw("May should not occur?", CODEPOS);
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
					auto valFrom = "[" + backend::to_asm(rIdFrom) + ", " + backend::to_asm(0) +
						"]";
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
	auto res = std::string();
	auto * pTo = pInstrLoad->to;
	if (pTo->addrType == ircode::AddrType::Var) {
		auto * pVRegSTo = convertFloatVariable(pTo);
		if (pVRegSTo->sid == backend::SId::stk) {
			auto loadTo = genASMPtrOffsetToFOperand2(
				res, pVRegSTo->offset, backend::RId::rhs
			);
			auto * pFrom = pInstrLoad->from;
			switch (pFrom->addrType) {
				case ircode::AddrType::Var: {
					auto rIdFrom = genASMGetVRegRVal(
						res, convertIntVariable(pFrom), backend::RId::lhs
					);
					auto valFrom = "[" + backend::to_asm(rIdFrom) + ", " + backend::to_asm(0) +
						"]";
					res += backend::toASM("vldr", backend::SId::lhs, valFrom);
					res += backend::toASM("vstr", backend::SId::lhs, loadTo);
					break;
				}
				case ircode::AddrType::LocalVar: {
					//  from stack to stack
					auto * pLVarAddr
						= dynamic_cast<ircode::AddrLocalVariable *>(pFrom);
					auto * pStkPtrLVal = convertLocalVar(pLVarAddr);
					com::Assert(pStkPtrLVal->offset != INT_MIN, "", CODEPOS);
					genASMDerefStkPtrToSReg(
						res, pStkPtrLVal->offset, backend::SId::lhs, backend::RId::lhs
					);
					res += backend::toASM("vstr", backend::SId::lhs, loadTo);
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
					res += backend::toASM("vldr", backend::SId::lhs, loadFrom);
					res += backend::toASM("vstr", backend::SId::lhs, loadTo);
					break;
				}
				default : {
					com::Throw("", CODEPOS);
				}
			}
		} else if (backend::isGPR(pVRegSTo->sid)) {
			com::TODO("", CODEPOS);
		} else {
			com::Throw("", CODEPOS);
		}
	} else {
		com::Throw("", CODEPOS);
	}
	return res;
}

int FuncInfo::run(ircode::InstrStore * pInstrStore) {
	switch (pInstrStore->from->getType().type) {
		case sup::Type::IntArray_t:
		case sup::Type::Pointer_t:
		case sup::Type::Int_t: {
			return run_Store_Int(pInstrStore);
		}
		case sup::Type::FloatArray_t:
		case sup::Type::Float_t: {
			return run_Store_Float(pInstrStore);
		}
		default:com::Throw("", CODEPOS);
	}
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

int FuncInfo::run_Store_Float(ircode::InstrStore * pInstrStore) {
	auto * pFrom = pInstrStore->from;
	if (pFrom->addrType == ircode::AddrType::Var) {
		auto * pVarFrom = dynamic_cast<ircode::AddrVariable *>(pFrom);
		auto * pVRegS = convertFloatVariable(pVarFrom);
		defineUseTimelineVRegS[pVRegS].emplace_back(tim);
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
		case sup::Type::IntArray_t: {
			return toASM_Store_IntArray(pInstrStore);
		}
		case sup::Type::FloatArray_t: {
			return toASM_Store_FloatArray(pInstrStore);
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
			", " + backend::to_asm(0) + "]";
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
			storeTo = "[" + to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		} else {
			storeTo = "[" +
				to_asm(genASMLoadInt(res, offset, backend::RId::rhs)) + ", " +
				backend::to_asm(0) + "]";
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
			genASMLoadInt(res, pSV.value, backend::RId::lhs));
	} else {
		com::Throw("", CODEPOS);
	}
	res += backend::toASM("str", storeFrom, storeTo);
	return res;
}

std::string FuncInfo::toASM_Store_IntArray(ircode::InstrStore * pInstrStore) {
	auto res = std::string();
	auto * pSVAddrFrom = dynamic_cast<ircode::AddrStaticValue *>(pInstrStore->from);
	com::Assert(pSVAddrFrom, "", CODEPOS);
	auto * pVarAddrTo = dynamic_cast<ircode::AddrLocalVariable *>(pInstrStore->to);
	//  ?
	com::Assert(pVarAddrTo, "", CODEPOS);
	auto * pStkPtrTo = convertLocalVar(pVarAddrTo);
	if (backend::Imm<backend::ImmType::Imm8m>::fitThis(pStkPtrTo->offset)) {
		res += backend::toASM(
			"add", backend::RId::lhs, backend::RId::sp, pStkPtrTo->offset
		);
	} else {
		genASMLoadInt(res, pStkPtrTo->offset, backend::RId::lhs);
		res += backend::toASM(
			"add", backend::RId::lhs, backend::RId::sp, backend::RId::lhs
		);
	}
	auto & shape = dynamic_cast<const sup::IntArrayStaticValue &>(
		pSVAddrFrom->getStaticValue()
	).shape;
	const auto & vecStaticValue = dynamic_cast<const sup::IntArrayStaticValue &>(
		pSVAddrFrom->getStaticValue()
	).value;
	auto step = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
	for (auto i = 0; i < step; ++i) {
		auto value = 0;
		auto p = vecStaticValue.find(sup::posToIdx(i, shape));
		if (p != vecStaticValue.end()) {
			value = p->second.value;
		}
		genASMLoadInt(res, value, backend::RId::rhs);
		auto saveTo = "[" + backend::to_asm(backend::RId::lhs) + ", " +
			backend::to_asm(0) + "]";
		res += backend::toASM("str", backend::RId::rhs, saveTo);
		res += backend::toASM("add", backend::RId::lhs, backend::RId::lhs, 4);
	}
	return res;
}

std::string FuncInfo::toASM_Store_Float(ircode::InstrStore * pInstrStore) {
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
			", " + backend::to_asm(0) + "]";
	} else if (pTo->addrType == ircode::AddrType::LocalVar) {
		auto * pLVarTo = dynamic_cast<ircode::AddrLocalVariable *>(pTo);
		com::Assert(
			pLVarTo->getType().type == sup::Type::Pointer_t, "", CODEPOS
		);
		auto * pStkPtr = convertLocalVar(pLVarTo);
		int offset = pStkPtr->offset;
		com::Assert(offset != INT_MIN, "", CODEPOS);
		if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
			storeTo = "[sp, " + backend::to_asm(offset) + "]";
		} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
			res += backend::toASM(
				"add", backend::RId::rhs, backend::RId::sp, offset
			);
			storeTo = "[" + to_asm(backend::RId::rhs) + ", " + backend::to_asm(0) + "]";
		} else {
			storeTo = "[" +
				to_asm(genASMLoadInt(res, offset, backend::RId::rhs)) + ", " +
				backend::to_asm(0) + "]";
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
		auto * pVRegS = convertFloatVariable(pVarFrom);
		auto rIdVal = genASMGetVRegSVal(res, pVRegS, backend::SId::lhs, backend::RId::lhs);
		storeFrom = backend::to_asm(rIdVal);
	} else if (pFrom->addrType == ircode::AddrType::ParaVar) {
		auto * pVarFrom = dynamic_cast<ircode::AddrPara *>(pFrom);
		auto * pVRegS = convertThisFloatArg(pVarFrom);
		auto sIdVal = genASMGetVRegSVal(res, pVRegS, backend::SId::lhs, backend::RId::lhs);
		storeFrom = backend::to_asm(sIdVal);
	} else if (pFrom->addrType == ircode::AddrType::StaticValue) {
		auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pFrom);
		auto & pSV = dynamic_cast<const sup::FloatStaticValue &>(
			pSVAddr->getStaticValue()
		);
		storeFrom = backend::to_asm(
			genASMLoadFloat(res, pSV.value, backend::SId::lhs, backend::RId::lhs));
	} else {
		com::Throw("", CODEPOS);
	}
	res += backend::toASM("vstr", storeFrom, storeTo);
	return res;
}

std::string FuncInfo::toASM_Store_FloatArray(ircode::InstrStore * pInstrStore) {
	auto res = std::string();
	auto * pSVAddrFrom = dynamic_cast<ircode::AddrStaticValue *>(pInstrStore->from);
	com::Assert(pSVAddrFrom, "", CODEPOS);
	auto * pVarAddrTo = dynamic_cast<ircode::AddrLocalVariable *>(pInstrStore->to);
	//  ?
	com::Assert(pVarAddrTo, "", CODEPOS);
	auto * pStkPtrTo = convertLocalVar(pVarAddrTo);
	if (backend::Imm<backend::ImmType::Imm8m>::fitThis(pStkPtrTo->offset)) {
		res += backend::toASM(
			"add", backend::RId::lhs, backend::RId::sp, pStkPtrTo->offset
		);
	} else {
		genASMLoadInt(res, pStkPtrTo->offset, backend::RId::lhs);
		res += backend::toASM(
			"add", backend::RId::lhs, backend::RId::sp, backend::RId::lhs
		);
	}
	auto & shape = dynamic_cast<const sup::FloatArrayStaticValue &>(
		pSVAddrFrom->getStaticValue()
	).shape;
	const auto & vecStaticValue = dynamic_cast<const sup::FloatArrayStaticValue &>(
		pSVAddrFrom->getStaticValue()
	).value;
	auto step = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<>());
	for (auto i = 0; i < step; ++i) {
		auto value = 0.0f;
		auto p = vecStaticValue.find(sup::posToIdx(i, shape));
		if (p != vecStaticValue.end()) {
			value = p->second.value;
		}
		genASMLoadFloatToRReg(res, value, backend::RId::rhs);
		auto saveTo = "[" + backend::to_asm(backend::RId::lhs) + ", " +
			backend::to_asm(0) + "]";
		res += backend::toASM("str", backend::RId::rhs, saveTo);
		res += backend::toASM("add", backend::RId::lhs, backend::RId::lhs, 4);

	}
	return res;
}


}
