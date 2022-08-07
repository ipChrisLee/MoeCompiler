#include "RegisterAllocator.hpp"


namespace backend {

void RegisterAllocator::set(
	std::unordered_set<backend::VRegR *> & _allUsedVRegR,
	std::unordered_set<backend::VRegS *> & _allUsedVRegS,
	std::unordered_set<backend::StkPtr *> & _allStkPtr,
	std::map<backend::VRegR *, std::vector<int>> & _defineUseTimelineVRegR,
	std::map<backend::VRegS *, std::vector<int>> & _defineUseTimelineVRegS,
	int _totalTim,
	std::map<ircode::AddrPara *, backend::Opnd *> & _argsOnPrev,
	int _argsStkSizeOnPrev
) {
	this->allUsedVRegR = _allUsedVRegR;
	this->allUsedVRegS = _allUsedVRegS;
	this->allStkPtr = _allStkPtr;
	this->defineUseTimelineVRegR = _defineUseTimelineVRegR;
	this->defineUseTimelineVRegS = _defineUseTimelineVRegS;
	this->totalTim = _totalTim;
	this->argsOnCallingThis = _argsOnPrev;
	this->argsStkSizeOnCallingThis = _argsStkSizeOnPrev;
	com::Assert(argsStkSizeOnCallingThis % 8 == 0, "", CODEPOS);
}

int RegisterAllocator::getRes() {
	run();
	//  Local VReg, include spilled registers and value alloca on stack.
	for (auto * pVRegR: allUsedVRegR) {
		if (pVRegR->rid == RId::stk) {
			pVRegR->offset = spilledStkSize;
			spilledStkSize += 4;
		} else if (isCalleeSave(pVRegR->rid)) {
			backupRReg.insert(pVRegR->rid);
			backupStkSize += 4;
		} else if (isCallerSave(pVRegR->rid)) {
			callerSaveRReg.insert(pVRegR->rid);
		} else { com::Throw("", CODEPOS); }
	}
	for (auto * pVRegS: allUsedVRegS) {
		if (pVRegS->sid == SId::stk) {
			pVRegS->offset = spilledStkSize;
			spilledStkSize += 4;
		} else if (isCalleeSave(pVRegS->sid)) {
			backupAndRestoreSReg.insert(pVRegS->sid);
			backupStkSize += 4;
		} else if (isCallerSave(pVRegS->sid)) {
			callerSaveSReg.insert(pVRegS->sid);
		} else { com::Throw("", CODEPOS); }
	}
	for (auto * pStkPtr: allStkPtr) {
		pStkPtr->offset = spilledStkSize;
		spilledStkSize += pStkPtr->sz;
	}
	restoreRReg = backupRReg;
	backupRReg.insert(RId::lr);
	restoreRReg.insert(RId::pc);
	backupStkSize += 4;
	if (backupStkSize + spilledStkSize % 8 != 0) {  //  need alignment
		spilledStkSize += 4;
	}
	for (auto [pParaAddr, pOpndArg]: argsOnCallingThis) {
		switch (pOpndArg->getOpndType()) {
			case OpndType::VRegR: {
				auto * pVRegRArg = dynamic_cast<VRegR *>(pOpndArg);
				if (pVRegRArg->rid == RId::stk) {
					pVRegRArg->offset = argsStkSizeOnCallingThis +
						pVRegRArg->offset + spilledStkSize + backupStkSize;
				} else if (isGPR(pVRegRArg->rid)) {
					//  do nothing
				} else { com::Throw("", CODEPOS); }
				m_AddrArg_VRegR[pParaAddr] = pVRegRArg;
				break;
			}
			case OpndType::VRegS: {
				auto * pVRegSArg = dynamic_cast<VRegS *>(pOpndArg);
				if (pVRegSArg->sid == SId::stk) {
					pVRegSArg->offset = argsStkSizeOnCallingThis +
						pVRegSArg->offset + spilledStkSize + backupStkSize;
				} else if (isGPR(pVRegSArg->sid)) {
					//  do nothing
				} else { com::Throw("", CODEPOS); }
				m_AddrArg_VRegS[pParaAddr] = pVRegSArg;
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	return 0;
}

int AllOnStkAllocator::run() {
	for (auto * pVRegR: allUsedVRegR) {
		pVRegR->rid = RId::stk;
	}
	for (auto * pVRegS: allUsedVRegS) {
		pVRegS->sid = SId::stk;
	}
	return 0;
}
}