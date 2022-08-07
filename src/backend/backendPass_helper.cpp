
#include "backendPass.hpp"


namespace pass {


backend::RId
FuncInfo::genASMLoadNumber(std::string & res, int32_t val, backend::RId to) {
	if (val < 0) { res += "@\t " + to_string(val) + " \n"; }
	auto [highVal, lowVal] = backend::splitNumber(val);
	res += backend::toASM("movw", to, lowVal);
	if (highVal) { res += backend::toASM("movt", to, highVal); }
	return to;
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
		loadFrom = "[" + to_asm(rIdDest) + ", #0]";
	} else {
		genASMLoadNumber(res, offset, rIdDest);
		res += backend::toASM(
			"add", rIdDest, backend::RId::sp, rIdDest
		);
		loadFrom = "[" + to_asm(rIdDest) + ", #0]";
	}
	res += backend::toASM("ldr", rIdDest, loadFrom);
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
		ptrStk = "[" + backend::to_asm(rIdRest) + ", #0]";
	} else {
		genASMLoadNumber(res, offset, rIdRest);
		res += backend::toASM(
			"add", rIdRest, rIdRest, backend::RId::sp
		);
		ptrStk = "[" + backend::to_asm(rIdRest) + ", #0]";
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


std::string FuncInfo::genASMConditionalBranch(ircode::ICMP icmp, bool reverse) {
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
		genASMLoadNumber(res, offset, scratchReg);
		res += backend::toASM("add", scratchReg, backend::RId::sp, scratchReg);
		auto to = "[" + backend::to_asm(scratchReg) + ", #0]";
		res += backend::toASM("str", ridFrom, to);
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

void FuncInfo::genASMSaveFromVRegRToRReg(
	std::string & res, backend::VRegR * pVRegRFrom, backend::RId rIdTo
) {
	auto xRId = genASMGetVRegRVal(res, pVRegRFrom, rIdTo);
	if (xRId != rIdTo) {
		res += backend::toASM("mov", rIdTo, xRId);
	}
}

const char * ToASM::asmHeader = ".arch armv7ve\n.arm\n";

const char * ToASM::gVarHeader = ".section .bss\n.align\n";

const char * ToASM::functionsHeader = ".global main \n.section .text\n";


std::string ToASM::declGVar(ircode::AddrGlobalVariable * pGVarAddr) {
	auto res = std::string(gVarHeader);
	auto gType = pGVarAddr->getStaticValue().getType().type;
	auto * pLabel = gVarToLabel[pGVarAddr];
	switch (gType) {
		case sup::Type::Int_t: {
			auto value = dynamic_cast<const sup::IntStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			res += "\t.long\t" + to_string(value);
			break;
		}
		case sup::Type::Float_t: {
			com::TODO("", CODEPOS);
			break;
		}
		case sup::Type::IntArray_t: {
			auto vecValue = dynamic_cast<const sup::IntArrayStaticValue &>(
				pGVarAddr->getStaticValue()
			).value;
			res += pLabel->labelStr + ":\n";
			auto space = 0;
			for (auto intStaticValue: vecValue) {
				auto value = intStaticValue.value;
				if (value) {
					if (space) {
						res += "\t.space\t" + to_string(space) + "\n";
						space = 0;
					}
					res += "\t.long\t" + to_string(value) + "\n";
				} else {
					space += 4;
				}
			}
			if (space) {
				res += "\t.space\t" + to_string(space) + "\n";
			}
			break;
		}
		case sup::Type::FloatArray_t: {
			com::TODO("", CODEPOS);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

}