
#include "backendPass.hpp"


namespace pass {


backend::RId
FuncInfo::genASMLoadInt(std::string & res, int32_t val, backend::RId to) {
	if (val < 0) { res += "@\t " + to_string(val) + " \n"; }
	auto [highVal, lowVal] = backend::splitNumber(val);
	res += backend::toASM("movw", to, lowVal);
	if (highVal) { res += backend::toASM("movt", to, highVal); }
	return to;
}

backend::SId
FuncInfo::genASMLoadFloat(
	std::string & res, float val, backend::SId to, backend::RId scratchRId
) {
	res += "@\t" + to_string(val) + "\n";
	auto [highVal, lowVal] = backend::splitNumber(val);
	res += backend::toASM("movw", scratchRId, lowVal);
	if (highVal) { res += backend::toASM("movt", scratchRId, highVal); }
	res += backend::toASM("vmov", to, scratchRId);
	return to;
}

backend::RId
FuncInfo::genASMLoadFloatToRReg(std::string & res, float val, backend::RId ridDest) {
	res += "@\t" + to_string(val) + "\n";
	auto [highVal, lowVal] = backend::splitNumber(val);
	res += backend::toASM("movw", ridDest, lowVal);
	if (highVal) { res += backend::toASM("movt", ridDest, highVal); }
	return ridDest;
}

backend::RId
FuncInfo::genASMLoadLabel(
	std::string & res, backend::Label * pLabel, backend::RId to
) {
	const auto & labelStr = pLabel->labelStr;
	res += backend::toASM("movw", to, ":lower16:" + labelStr);
	res += backend::toASM("movt", to, ":upper16:" + labelStr);
	return to;

}

void FuncInfo::genASMDerefStkPtr(
	std::string & res, int offset, backend::RId rIdDest
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	auto loadFrom = std::string();
	if (backend::Imm<backend::ImmType::ImmOffset>::fitThis(offset)) {
		loadFrom = "[sp, " + backend::to_asm(offset) + "]";
	} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
		res += backend::toASM(
			"add", rIdDest, backend::RId::sp, offset
		);
		loadFrom = "[" + to_asm(rIdDest) + ", " + backend::to_asm(0) + "]";
	} else {
		genASMLoadInt(res, offset, rIdDest);
		res += backend::toASM(
			"add", rIdDest, backend::RId::sp, rIdDest
		);
		loadFrom = "[" + to_asm(rIdDest) + ", " + backend::to_asm(0) + "]";
	}
	res += backend::toASM("ldr", rIdDest, loadFrom);
}

void FuncInfo::genASMDerefStkPtrToSReg(
	std::string & res, int offset, backend::SId sIdDest, backend::RId scratchRId
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	auto loadFrom = std::string();
	if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
		loadFrom = "[sp, " + backend::to_asm(offset) + "]";
	} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
		res += backend::toASM(
			"add", scratchRId, backend::RId::sp, offset
		);
		loadFrom = "[" + to_asm(scratchRId) + ", " + backend::to_asm(0) + "]";
	} else {
		genASMLoadInt(res, offset, scratchRId);
		res += backend::toASM(
			"add", scratchRId, backend::RId::sp, scratchRId
		);
		loadFrom = "[" + to_asm(scratchRId) + ", " + backend::to_asm(0) + "]";
	}
	res += backend::toASM("vldr", sIdDest, loadFrom);
}

std::string
FuncInfo::genASMPtrOffsetToOperand2(
	std::string & res, int offset, backend::RId rIdRest
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	auto ptrStk = std::string();    //  rhs as scratch reg
	if (backend::Imm<backend::ImmType::ImmOffset>::fitThis(offset)) {
		ptrStk = "[sp, " + backend::to_asm(offset) + "]";
	} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
		res += backend::toASM(
			"add", rIdRest, backend::RId::sp, offset
		);
		ptrStk = "[" + backend::to_asm(rIdRest) + ", " + backend::to_asm(0) + "]";
	} else {
		genASMLoadInt(res, offset, rIdRest);
		res += backend::toASM(
			"add", rIdRest, rIdRest, backend::RId::sp
		);
		ptrStk = "[" + backend::to_asm(rIdRest) + ", " + backend::to_asm(0) + "]";
	}
	return ptrStk;
}

std::string
FuncInfo::genASMPtrOffsetToFOperand2(
	std::string & res, int offset, backend::RId scratchRId
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	auto ptrStk = std::string();
	if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
		ptrStk = "[sp, " + backend::to_asm(offset) + "]";
	} else if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
		res += backend::toASM(
			"add", scratchRId, backend::RId::sp, offset
		);
		ptrStk = "[" + backend::to_asm(scratchRId) + ", " + backend::to_asm(0) + "]";
	} else {
		genASMLoadInt(res, offset, scratchRId);
		res += backend::toASM(
			"add", scratchRId, scratchRId, backend::RId::sp
		);
		ptrStk = "[" + backend::to_asm(scratchRId) + ", " + backend::to_asm(0) + "]";
	}
	return ptrStk;
}

backend::RId FuncInfo::genASMGetVRegRVal(
	std::string & res, backend::VRegR * pVRegR, backend::RId rIdIfInStk
) {
	if (pVRegR->rid == backend::RId::stk) {
		int offset = pVRegR->offset;
		genASMDerefStkPtr(res, offset, rIdIfInStk);
		return rIdIfInStk;
	} else if (backend::isGPR(pVRegR->rid)) {
		return pVRegR->rid;
	} else { com::Throw("", CODEPOS); }
}

backend::SId FuncInfo::genASMGetVRegSVal(
	std::string & res, backend::VRegS * pVRegS, backend::SId sIdIfInStk,
	backend::RId scratchRId
) {
	if (pVRegS->sid == backend::SId::stk) {
		int offset = pVRegS->offset;
		genASMDerefStkPtrToSReg(res, offset, sIdIfInStk, scratchRId);
		return sIdIfInStk;
	} else if (backend::isGPR(pVRegS->sid)) {
		return pVRegS->sid;
	} else { com::Throw("", CODEPOS); }
}


std::string FuncInfo::genASMCondName(ircode::ICMP icmp, bool reverse) {
	if (reverse) { icmp = ircode::getReverse(icmp); }
	switch (icmp) {
		case ircode::ICMP::NE: { return "ne"; }
		case ircode::ICMP::EQ: { return "eq"; }
		case ircode::ICMP::SGE: { return "ge"; }
		case ircode::ICMP::SLT: { return "lt"; }
		case ircode::ICMP::SGT: { return "gt"; }
		case ircode::ICMP::SLE: { return "le"; }
		default: { com::Throw("", CODEPOS); }
	}
}

std::string FuncInfo::genASMCondNameReverse(ircode::FCMP fcmp) {
	switch (fcmp) {
		case ircode::FCMP::UNE: { return "eq"; }
		case ircode::FCMP::OEQ: { return "ne"; }
		case ircode::FCMP::OLT: { return "pl"; }
		case ircode::FCMP::OLE: { return "hi"; }
		case ircode::FCMP::OGE: { return "lt"; }
		case ircode::FCMP::OGT: { return "le"; }
		default: { com::Throw("", CODEPOS); }
	}
}

void
FuncInfo::genASMPushRegs(std::string & res, const std::set<backend::RId> & list) {
	com::Assert(!list.empty(), "", CODEPOS);
	auto ins = std::string("push {");
	for (auto rid: list) {
		ins += backend::to_asm(rid) + ",";
	}
	ins.pop_back();
	ins += "}\n";
	res += ins;
}

void
FuncInfo::genASMPushRegs(std::string & res, const std::set<backend::SId> & list) {
	com::Assert(!list.empty(), "", CODEPOS);
	auto ins = std::string("vpush {");
	for (auto sid: list) {
		ins += backend::to_asm(sid) + ",";
	}
	ins.pop_back();
	ins += "}\n";
	res += ins;
}

void
FuncInfo::genASMPopRegs(std::string & res, const std::set<backend::RId> & list) {
	com::Assert(!list.empty(), "", CODEPOS);
	auto ins = std::string("pop {");
	for (auto rid: list) {
		ins += backend::to_asm(rid) + ",";
	}
	ins.pop_back();
	ins += "}\n";
	res += ins;
}

void
FuncInfo::genASMPopRegs(std::string & res, const std::set<backend::SId> & list) {
	com::Assert(!list.empty(), "", CODEPOS);
	auto ins = std::string("vpop {");
	for (auto sid: list) {
		ins += backend::to_asm(sid) + ",";
	}
	ins.pop_back();
	ins += "}\n";
	res += ins;
}

void
FuncInfo::genASMSaveFromRRegToOffset(
	std::string & res, backend::RId ridFrom, int offset, backend::RId scratchReg
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	if (backend::Imm<backend::ImmType::Imm8m>::fitThis(offset)) {
		auto to = "[sp, " + backend::to_asm(offset) + "]";
		res += backend::toASM("str", ridFrom, to);
	} else {
		genASMLoadInt(res, offset, scratchReg);
		res += backend::toASM("add", scratchReg, backend::RId::sp, scratchReg);
		auto to = "[" + backend::to_asm(scratchReg) + ", " + backend::to_asm(0) + "]";
		res += backend::toASM("str", ridFrom, to);
	}
}

void
FuncInfo::genASMSaveFromSRegToOffset(
	std::string & res, backend::SId sidFrom, int offset, backend::RId scratchReg
) {
	com::Assert(offset != INT_MIN, "", CODEPOS);
	if (backend::Imm<backend::ImmType::Immed>::fitThis(offset)) {
		auto to = "[sp, " + backend::to_asm(offset) + "]";
		res += backend::toASM("vstr", sidFrom, to);
	} else {
		genASMLoadInt(res, offset, scratchReg);
		res += backend::toASM("add", scratchReg, backend::RId::sp, scratchReg);
		auto to = "[" + backend::to_asm(scratchReg) + ", " + backend::to_asm(0) + "]";
		res += backend::toASM("vstr", sidFrom, to);
	}
}

void FuncInfo::genASMSaveFromRRegToVRegR(
	std::string & res, backend::VRegR * pVRegRTo, backend::RId rIdFrom,
	backend::RId scratchRId
) {
	if (pVRegRTo->rid == backend::RId::stk) {
		genASMSaveFromRRegToOffset(res, rIdFrom, pVRegRTo->offset, scratchRId);
	} else if (backend::isGPR(pVRegRTo->rid)) {
		res += backend::toASM("mov", pVRegRTo->rid, rIdFrom);
	} else { com::Throw("", CODEPOS); }
}

void FuncInfo::genASMSaveFromSRegToVRegS(
	std::string & res, backend::VRegS * pVRegSTo, backend::SId sIdFrom,
	backend::RId scratchRId
) {
	if (pVRegSTo->sid == backend::SId::stk) {
		genASMSaveFromSRegToOffset(res, sIdFrom, pVRegSTo->offset, scratchRId);
	} else if (backend::isGPR(pVRegSTo->sid)) {
		res += backend::toASM("mov", pVRegSTo->sid, sIdFrom);
	} else { com::Throw("", CODEPOS); }

}

void FuncInfo::genASMSaveFromVRegRToRReg(
	std::string & res, backend::VRegR * pVRegRFrom, backend::RId rIdTo
) {
	auto xRId = genASMGetVRegRVal(res, pVRegRFrom, rIdTo);
	if (xRId != rIdTo) {
		res += backend::toASM("mov", rIdTo, xRId);
	}
}

void FuncInfo::genASMSaveFromVRegSToSReg(
	std::string & res, backend::VRegS * pVRegSFrom, backend::SId sIdTo,
	backend::RId scratchRId
) {
	auto xSId = genASMGetVRegSVal(res, pVRegSFrom, sIdTo, scratchRId);
	if (xSId != sIdTo) {
		res += backend::toASM("mov", sIdTo, xSId);
	}
}

const char * ToASM::asmHeader = ".arch armv7ve\n.arm\n";

const char * ToASM::gVarHeader = ".section .data\n";

const char * ToASM::functionsHeader = ".global main \n.section .text\n";


std::string ToASM::declGVar(ircode::AddrGlobalVariable * pGVarAddr) {
	auto res = std::string(".align\n");
	auto gType = pGVarAddr->getStaticValue().getType().type;
	auto * pLabel = gVarToLabel[pGVarAddr];
	switch (gType) {
		case sup::Type::Int_t: {
			auto value = dynamic_cast<const sup::IntStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			res += "\t.long\t" + hexFormOf(value);
			break;
		}
		case sup::Type::Float_t: {
			auto value = dynamic_cast<const sup::FloatStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			res += "\t.long\t" + hexFormOf(value);
			break;
		}
		case sup::Type::IntArray_t: {
			const auto & viTypeInfo =
				dynamic_cast<const sup::IntArrayType &>(
					pGVarAddr->uPtrStaticValue->getType()
				);
			const auto & vecValue = dynamic_cast<const sup::IntArrayStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			auto posNow = 0;
			auto iPos = 0;
			for (auto valPair: vecValue) {
				posNow = sup::idxToPos(valPair.first, viTypeInfo.shape) * 4;
				if (posNow - iPos) {
					res += "\t.space\t" + hexFormOf(posNow - iPos) + "\n";
				}
				auto & value = valPair.second;
				res += "\t.long\t" + hexFormOf(value.value) + "\n";
				iPos = posNow + 4;
			}
			posNow = sup::idxToPos(viTypeInfo.shape, viTypeInfo.shape);
			if (posNow - iPos) {
				res += "\t.space\t" + hexFormOf(posNow - iPos) + "\n";
			}
			break;
		}
		case sup::Type::FloatArray_t: {
			const auto & vfTypeInfo =
				dynamic_cast<const sup::FloatArrayType &>(
					pGVarAddr->uPtrStaticValue->getType()
				);
			const auto & vecValue = dynamic_cast<const sup::FloatArrayStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			auto posNow = 0;
			auto iPos = 0;
			for (auto valPair: vecValue) {
				posNow = sup::idxToPos(valPair.first, vfTypeInfo.shape) * 4;
				if (posNow - iPos) {
					res += "\t.space\t" + hexFormOf(posNow - iPos) + "\n";
				}
				auto & value = valPair.second;
				res += "\t.long\t" + hexFormOf(value.value) + "\n";
				iPos = posNow + 4;
			}
			posNow = sup::idxToPos(vfTypeInfo.shape, vfTypeInfo.shape);
			if (posNow - iPos) {
				res += "\t.space\t" + hexFormOf(posNow - iPos) + "\n";
			}
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

std::string hexFormOf(int32_t val) {
	static char buf[50];
	sprintf(buf, "0x%x", val);
	return buf;
}

std::string hexFormOf(float val) {
	return hexFormOf(*reinterpret_cast<int32_t *>(&val));
}

}