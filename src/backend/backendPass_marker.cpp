
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
	allStkPtr.insert(pStkPtr);
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
	allUsedVRegR.insert(p);
	return p;
}

backend::VRegS * FuncInfo::convertFloatVariable(ircode::AddrVariable * pAddrVar) {
	com::Assert(pAddrVar->addrType == ircode::AddrType::Var, "", CODEPOS);
	com::Assert(pAddrVar->getType().type == sup::Type::Float_t, "", CODEPOS);
	if (m_AddrVar_VRegS.find(pAddrVar) != m_AddrVar_VRegS.end()) {
		return m_AddrVar_VRegS[pAddrVar];
	}
	auto * p = opndPool.emplace_back(
		backend::VRegS(backend::SId::unk)
	);
	m_AddrVar_VRegS[pAddrVar] = p;
	allUsedVRegS.insert(p);
	return p;
}

backend::VRegR * FuncInfo::convertThisIntArg(ircode::AddrPara * pAddrPara) {
	com::Assert(
		m_AddrArg_VRegR.find(pAddrPara) != m_AddrArg_VRegR.end(), "", CODEPOS
	);
	return m_AddrArg_VRegR[pAddrPara];
}

backend::VRegS * FuncInfo::convertThisFloatArg(ircode::AddrPara * pAddrPara) {
	com::Assert(
		m_AddrArg_VRegS.find(pAddrPara) != m_AddrArg_VRegS.end(), "", CODEPOS
	);
	return m_AddrArg_VRegS[pAddrPara];
}

backend::Label * FuncInfo::convertGlobalVar(ircode::AddrGlobalVariable * pAddrGVar) {
	com::Assert(
		m_AddrGVar_Label.find(pAddrGVar) != m_AddrGVar_Label.end(), "", CODEPOS
	);
	return m_AddrGVar_Label[pAddrGVar];
}

void FuncInfo::markOperand(ircode::AddrOperand * pAddrOperand) {
	switch (pAddrOperand->addrType) {
		case ircode::AddrType::Var: {
			auto * pAddrVar = dynamic_cast<ircode::AddrVariable *>(pAddrOperand);
			switch (pAddrOperand->getType().type) {
				case sup::Type::Pointer_t:
				case sup::Type::Int_t: {
					auto * pVRegR = convertIntVariable(pAddrVar);
					defineUseTimelineVRegR[pVRegR].emplace_back(tim);
					break;
				}
				case sup::Type::Float_t: {
					auto * pVRegS = convertFloatVariable(pAddrVar);
					defineUseTimelineVRegS[pVRegS].emplace_back(tim);
					break;
				}
				default:com::Throw("", CODEPOS);
			}
			break;
		}
		case ircode::AddrType::GlobalVar: {
			auto * pGVarAddr
				= dynamic_cast<ircode::AddrGlobalVariable *>(pAddrOperand);
			convertGlobalVar(pGVarAddr);
			break;
		}
		case ircode::AddrType::LocalVar: {
			auto * pLVarAddr
				= dynamic_cast<ircode::AddrLocalVariable *>(pAddrOperand);
			convertLocalVar(pLVarAddr);
			break;
		}
		case ircode::AddrType::StaticValue: {
			//  do nothing
			break;
		}
		case ircode::AddrType::ParaVar: {
			//  do nothing
			break;
		}
		default:com::Throw("", CODEPOS);
	}
}

}
