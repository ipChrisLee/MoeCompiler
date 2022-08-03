#include "Module.hpp"


namespace lir {

Module::Module(mir::Module & mIr) : mIR(mIr) {
}

std::string Module::toLIR() const {
	auto res = std::string();
	for (auto * pInstr: instrs) {
		res += pInstr->toLIR();
	}
	return res;
}

void Module::main() {
	for (auto * pFunc: mIR.funcPool) {
		auto * labelFunc = opndPool.emplace_back(
			Label(pFunc->getFuncAddrPtr()->getName())
		);
		mapping.bind(pFunc->getFuncAddrPtr(), labelFunc);
		instrs.emplace_back(
			instrPool.emplace_back(
				InstrLABEL(labelFunc)
			)
		);
		for (auto * mirInstr: pFunc->instrs) {
			instrs.splice(instrs.end(), fromMIRToLIR(mirInstr));
		}
	}
}

std::list<lir::Instr *> Module::fromMIRToLIR(mir::Instr * pInstr) {
	auto res = std::list<lir::Instr *>();
	switch (pInstr->instrType) {
		case mir::InstrType::Alloca: {
			auto * p = dynamic_cast<mir::InstrAlloca *>(pInstr);
			res = fromMIRToLIR(p);
			break;
		}
		case mir::InstrType::Label: {
			auto * p = dynamic_cast<mir::InstrLabel *>(pInstr);
			res = fromMIRToLIR(p);
			break;
		}
		default: {
			com::TODO("", CODEPOS);
		}
	}
	return res;
}

std::list<lir::Instr *> Module::fromMIRToLIR(mir::InstrAlloca * pAlloca) {
	auto * pPtrAddr = pAlloca->allocaTo;
	auto * pOpnd = opndPool.emplace_back(lir::StkPtr(-1, pPtrAddr->getSize()));
	mapping.bind(pPtrAddr, pOpnd);
	return { };
}

std::list<lir::Instr *> Module::fromMIRToLIR(mir::InstrLabel * pLabel) {
	auto * pAddrLabel = pLabel->pAddrLabel;
	auto * pOpnd = opndPool.emplace_back(lir::Label(pAddrLabel->labelName));
	mapping.bind(pAddrLabel, pOpnd);
	auto res = std::list<lir::Instr *>();
	res.emplace_back(
		instrPool.emplace_back(
			lir::InstrLABEL(pOpnd)
		)
	);
	return res;
}

std::list<lir::Instr *> Module::fromMIRToLIR(mir::InstrStore * pStore) {
	auto res = std::list<lir::Instr *>();
	auto * pAddrOperand = pStore->from;
	auto * pAddrVar = pStore->to;
	auto * pOpndPtr = static_cast<Ptr *>(nullptr);
	switch (pAddrVar->addrType) {
		case mir::AddrType::LocalVariable: {
			pOpndPtr = mapping.localVarToStkPtr[
				dynamic_cast<mir::AddrLocalVariable *>(pAddrVar)
			];
			break;
		}
		case mir::AddrType::GlobalVariable: {
			pOpndPtr = mapping.globalVarToMemPtr[
				dynamic_cast<mir::AddrGlobalVariable *>(pAddrVar)
			];
			break;
		}
		default: com::Throw();
	}
	switch (pAddrOperand->getType().type) {
		case mir::Type::Int_t: {
			auto * pRd = opndPool.emplace_back(VRegR());
			if (pAddrOperand->addrType == mir::AddrType::StaticValue) {
				auto * pAddrSV = dynamic_cast<mir::AddrStaticValue *>(pAddrOperand);
				auto * imm = opndPool.emplace_back(
					Imm<ImmType::Unk>(
						dynamic_cast<const mir::IntStaticValue &>(
							pAddrSV->getStaticValue()
						).value
					)
				);
				res.emplace_back(
					instrPool.emplace_back(
						InstrMOVImm(pRd, imm)
					)
				);
			} else {

			}
			res.emplace_back(
				instrPool.emplace_back(
					InstrSTR(pRd, pOpndPtr)
				)
			);
			break;
		}
		case mir::Type::Float_t: {
			com::TODO();
			break;
		}
		default:com::Throw();
	}
	return res;
}

bool Module::Mapping::bind(mir::AddrVariable * pAddr, lir::Opnd * opnd) {
	com::Assert(pAddr->addrType == mir::AddrType::Variable, "", CODEPOS);
	if (addrToOpnd.find(pAddr) != addrToOpnd.end()) {
		return false;
	}
	addrToOpnd[pAddr] = opnd;
	return true;
}

bool Module::Mapping::bind(mir::AddrJumpLabel * pAddr, lir::Label * opnd) {
	if (labelToLabel.find(pAddr) != labelToLabel.end()) {
		return false;
	}
	labelToLabel[pAddr] = opnd;
	return true;
}

bool Module::Mapping::bind(
	const mir::AddrFunction * pAddrFunc, lir::Label * labelFunc
) {
	if (funcInfo.find(pAddrFunc) != funcInfo.end()) {
		com::Throw("?", CODEPOS);
	}
	auto & info = funcInfo[pAddrFunc];
	info.label = labelFunc;
	info.retType = pAddrFunc->getReturnTypeInfo().type;
	switch (info.retType) {
		case mir::Type::Int_t: {
			info.retVal.emplace<RId>(RId::r0);
			break;
		}
		case mir::Type::Float_t: {
			info.retVal.emplace<SId>(SId::s0);
			break;
		}
		case mir::Type::Void_t: {
			info.retVal.emplace<std::monostate>();
			break;
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
	for (int i = 0, rid = 0, sid = 0; i < pAddrFunc->getNumberOfParameter(); ++i) {
		switch (pAddrFunc->getNumberThParameterTypeInfo(i).type) {
			case mir::Type::Pointer_t:
			case mir::Type::Int_t: {
				if (rid < 4) {
					info.argRId[i] = RId(rid);
					++rid;
				} else {
					info.argRId[i] = RId::stk;
				}
				break;
			}
			case mir::Type::Float_t: {
				if (sid < 32) {
					info.argSId[i] = SId(sid);
					++sid;
				} else {
					info.argSId[i] = SId::stk;
				}
				break;
			}
			default: {
				com::Throw("", CODEPOS);
			}
		}
	}
	return true;
}

bool Module::Mapping::bind(
	mir::AddrLocalVariable * pAddrLocalVar, lir::StkPtr * pStkPtr
) {
	if (localVarToStkPtr.find(pAddrLocalVar) != localVarToStkPtr.end()) {
		return false;
	}
	localVarToStkPtr[pAddrLocalVar] = pStkPtr;
	return true;
}

bool Module::Mapping::bind(
	mir::AddrGlobalVariable * pAddrGlobalVar, lir::MemPtr * pMemPtr
) {
	if (globalVarToMemPtr.find(pAddrGlobalVar) != globalVarToMemPtr.end()) {
		return false;
	}
	globalVarToMemPtr[pAddrGlobalVar] = pMemPtr;
	return true;
}

std::list<lir::Instr *> Module::Generator::genMemAddressOpnd(
	mir::AddrVariable * pAddrVar, lir::VRegR * genTo
) {
	auto res = std::list<lir::Instr *>();
	switch (pAddrVar->addrType) {
		case mir::AddrType::LocalVariable: {
			break;
		}
		case mir::AddrType::GlobalVariable: {
			break;
		}
		default: {
			com::Throw("Unsupported AddrType.", CODEPOS);
		}
	}
	return res;
}
}

