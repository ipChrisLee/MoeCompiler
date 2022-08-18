
#include "backendPass.hpp"


namespace pass {


backend::StkPtr *
FuncInfo::convertLocalVar(ircode::AddrLocalVariable * pAddrLocalVar) {
	if (m_AddrLocalVar_StkPtr.find(pAddrLocalVar) != m_AddrLocalVar_StkPtr.end()) {
		return m_AddrLocalVar_StkPtr[pAddrLocalVar];
	}
	auto pTypeInfoPtrTo = dynamic_cast<const sup::PointerType &>(
		pAddrLocalVar->getType()
	);
	auto sz = pTypeInfoPtrTo.pointTo->getSize();
	auto * pStkPtr = opndPool.emplace_back(backend::StkPtr(-1, sz));
	m_AddrLocalVar_StkPtr[pAddrLocalVar] = pStkPtr;
	allVarStkPtr.insert(pStkPtr);
	return pStkPtr;
}

backend::Label * FuncInfo::convertLabel(ircode::AddrJumpLabel * pAddrJumpLabel) {
	if (m_AddrJLabel_Label.find(pAddrJumpLabel) != m_AddrJLabel_Label.end()) {
		return m_AddrJLabel_Label[pAddrJumpLabel];
	}
	return m_AddrJLabel_Label[pAddrJumpLabel] = opndPool.emplace_back(
		backend::Label(pAddrJumpLabel)
	);
}

backend::VRegR * FuncInfo::convertIntVariable(ircode::AddrVariable * pAddrVar) {
	com::Assert(pAddrVar->addrType == ircode::AddrType::Var, "", CODEPOS);
	com::Assert(
		com::enum_fun::in(
			pAddrVar->getType().type, {sup::Type::Int_t, sup::Type::Pointer_t}
		), "", CODEPOS
	);
	if (m_AddrVar_VRegR.find(pAddrVar) != m_AddrVar_VRegR.end()) {
		return m_AddrVar_VRegR[pAddrVar];
	}
	auto * p = opndPool.emplace_back(
		backend::VRegR(backend::RId::unk)
	);
	m_AddrVar_VRegR[pAddrVar] = p;
	allVarVRegR.insert(p);
	return p;
}

backend::VRegS * FuncInfo::convertFloatVariable(ircode::AddrVariable * pAddrVar) {
	com::Assert(pAddrVar->addrType == ircode::AddrType::Var, "", CODEPOS);
	com::Assert(
		com::enum_fun::in(
			pAddrVar->getType().type, {sup::Type::Float_t}
		), "", CODEPOS
	);
	if (m_AddrVar_VRegS.find(pAddrVar) != m_AddrVar_VRegS.end()) {
		return m_AddrVar_VRegS[pAddrVar];
	}
	auto * p = opndPool.emplace_back(
		backend::VRegS(backend::SId::unk)
	);
	m_AddrVar_VRegS[pAddrVar] = p;
	allVarVRegS.insert(p);
	return p;
}

backend::VRegR * FuncInfo::convertThisIntArg(ircode::AddrPara * pAddrPara) {
	if (m_AddrArg_VRegR.find(pAddrPara) == m_AddrArg_VRegR.end()) {
		com::Throw("", CODEPOS);
//		com::Assert(
//			paramsInfoOnCallingThis.find(pAddrPara) != paramsInfoOnCallingThis.end(), "",
//			CODEPOS
//		);
//		auto * pVRegRPara = dynamic_cast<backend::VRegR *>(
//			paramsInfoOnCallingThis[pAddrPara]
//		);
//		auto * pVRegRArg = opndPool.emplace_back(backend::VRegR(*pVRegRPara));
//		m_AddrArg_VRegR[pAddrPara] = pVRegRArg;
	}
	return m_AddrArg_VRegR[pAddrPara];
}

backend::VRegS * FuncInfo::convertThisFloatArg(ircode::AddrPara * pAddrPara) {
	if (m_AddrArg_VRegS.find(pAddrPara) == m_AddrArg_VRegS.end()) {
		com::Throw("", CODEPOS);
//		com::Assert(
//			paramsInfoOnCallingThis.find(pAddrPara) != paramsInfoOnCallingThis.end(), "",
//			CODEPOS
//		);
//		auto * pVRegSPara = dynamic_cast<backend::VRegS *>(
//			paramsInfoOnCallingThis[pAddrPara]
//		);
//		auto * pVRegSArg = opndPool.emplace_back(backend::VRegS(*pVRegSPara));
//		m_AddrArg_VRegS[pAddrPara] = pVRegSArg;
	}
	return m_AddrArg_VRegS[pAddrPara];
}

backend::Label * FuncInfo::convertGlobalVar(ircode::AddrGlobalVariable * pAddrGVar) {
	com::Assert(
		m_AddrGVar_Label.find(pAddrGVar) != m_AddrGVar_Label.end(), "", CODEPOS
	);
	return m_AddrGVar_Label[pAddrGVar];
}

backend::Opnd * FuncInfo::markOperand(ircode::AddrOperand * pAddrOperand) {
	auto * res = static_cast<backend::Opnd *>(nullptr);
	switch (pAddrOperand->addrType) {
		case ircode::AddrType::Var: {
			auto * pAddrVar = dynamic_cast<ircode::AddrVariable *>(pAddrOperand);
			switch (pAddrOperand->getType().type) {
				case sup::Type::Pointer_t:
				case sup::Type::Int_t: {
					auto * pVRegR = convertIntVariable(pAddrVar);
					defineUseTimelineVRegR[pVRegR].emplace_back(tim);
					pVRegR->expense+=std::pow(10,loop_labels);
					res = pVRegR;
					break;
				}
				case sup::Type::Float_t: {
					auto * pVRegS = convertFloatVariable(pAddrVar);
					defineUseTimelineVRegS[pVRegS].emplace_back(tim);
					pVRegS->expense+=std::pow(10,loop_labels);
					res = pVRegS;
					break;
				}
				default:com::Throw("", CODEPOS);
			}
			break;
		}
		case ircode::AddrType::ParaVar: {
			auto * pAddrArgVar = dynamic_cast<ircode::AddrPara *>(pAddrOperand);
			switch (pAddrOperand->getType().type) {
				case sup::Type::Pointer_t:
				case sup::Type::Int_t: {
					auto * pVRegR = convertThisIntArg(pAddrArgVar);
					defineUseTimelineVRegR[pVRegR].emplace_back(tim);
					res = pVRegR;
					break;
				}
				case sup::Type::Float_t: {
					auto * pVRegS = convertThisFloatArg(pAddrArgVar);
					defineUseTimelineVRegS[pVRegS].emplace_back(tim);
					res = pVRegS;
					break;
				}
				default:com::Throw("", CODEPOS);
			}
			break;
		}
		case ircode::AddrType::GlobalVar: {
			auto * pGVarAddr
				= dynamic_cast<ircode::AddrGlobalVariable *>(pAddrOperand);
			res = convertGlobalVar(pGVarAddr);
			break;
		}
		case ircode::AddrType::LocalVar: {
			auto * pLVarAddr
				= dynamic_cast<ircode::AddrLocalVariable *>(pAddrOperand);
			res = convertLocalVar(pLVarAddr);
			break;
		}
		case ircode::AddrType::StaticValue: {
			//  do nothing
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

}
