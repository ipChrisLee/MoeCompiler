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
	int _argsStkSizeOnPrev,
	std::map<ircode::AddrPara *, backend::VRegR *> & _m_AddrArg_VRegR,
	std::map<ircode::AddrPara *, backend::VRegS *> & _m_AddrArg_VRegS
) {
	this->allUsedVRegR = _allUsedVRegR;
	this->allUsedVRegS = _allUsedVRegS;
	this->allStkPtr = _allStkPtr;
	this->defineUseTimelineVRegR = _defineUseTimelineVRegR;
	this->defineUseTimelineVRegS = _defineUseTimelineVRegS;
	this->totalTim = _totalTim;
	this->argsOnCallingThis = _argsOnPrev;
	this->argsStkSizeOnCallingThis = _argsStkSizeOnPrev;
	this->m_AddrArg_VRegR = _m_AddrArg_VRegR;
	this->m_AddrArg_VRegS = _m_AddrArg_VRegS;
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
	if ((backupStkSize + spilledStkSize) % 8 != 0) {  //  need alignment
		spilledStkSize += 4;
	}
	//  change vreg of para after register allocation
	for (auto [pParaAddr, pOpndArg]: argsOnCallingThis) {
		switch (pOpndArg->getOpndType()) {
			case OpndType::VRegR: {
				auto * pVRegRArg = m_AddrArg_VRegR[pParaAddr];
				if (pVRegRArg->rid == RId::stk) {
					pVRegRArg->offset = argsStkSizeOnCallingThis +
						pVRegRArg->offset + spilledStkSize + backupStkSize;
				} else if (isGPR(pVRegRArg->rid)) {
					//  do nothing
				} else { com::Throw("", CODEPOS); }
				break;
			}
			case OpndType::VRegS: {
				auto * pVRegSArg = m_AddrArg_VRegS[pParaAddr];
				if (pVRegSArg->sid == SId::stk) {
					pVRegSArg->offset = argsStkSizeOnCallingThis +
						pVRegSArg->offset + spilledStkSize + backupStkSize;
				} else if (isGPR(pVRegSArg->sid)) {
					//  do nothing
				} else { com::Throw("", CODEPOS); }
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	return 0;
}

void AllOnStkAllocator::prune_Graph(std::unordered_map<LocalAddr , backend::vRegRSet *,Hashfunc> &rcontent){
	for(auto content:rcontent){
		for(auto content_p:rcontent){
			if(conflictMatrixR.count(std::make_pair(content.second,content_p.second))||
				conflictMatrixR.count(std::make_pair(content_p.second,content.second))
				){
					content.second->nints++;
					content.second->neighbors.push_back(content_p.second);
				}
		}
	}
	bool token;
	int adjnodes=rcontent.size();
	std::stack<vRegRSet *> adjStack;
	while(adjnodes!=0){
		token=false;
		for(auto content:rcontent){
			if(content.second->allocated) continue;
			if(content.second->nints<AVAILABLE_RREGR){
				token=true;
				--adjnodes;
				content.second->allocated=true;
				for(auto neighbor:content.second->neighbors){
					neighbor->nints--;
					neighbor->dn_neighbors.push_back(content.second);
				}
				adjStack.push(content.second);
			}
		}
		size_t lowest=0;
		vRegRSet * lowestadj;
		if(!token&&adjnodes!=0){
			for(auto content:rcontent){
				if(content.second->allocated) continue;
				if(lowest==0||content.second->spillcost<lowest){
					lowest=content.second->spillcost;
					lowestadj=content.second;
				}
			}
			--adjnodes;
			lowestadj->allocated=true;
			for(auto neighbor:lowestadj->neighbors){
				neighbor->nints--;
				neighbor->dn_neighbors.push_back(lowestadj);
			}
			adjStack.push(lowestadj);
		}
	}
	RId baseline=RId::r4;
	while(!adjStack.empty()){
		auto node=adjStack.top();
		adjStack.pop();
		int j;
		for(j=0;j<AVAILABLE_RREGR;j++){
			if(node->available[j]) break;
		}
		if(j<AVAILABLE_RREGR){
			node->rid=(RId)((int)baseline+j);
			for(auto neighbor:node->dn_neighbors){
				neighbor->available[j]=false;
			}
		}
		else node->rid=RId::stk;
	}
	for(auto content:rcontent){
		for(auto pvReg:content.second->pvRegRs) pvReg->rid=content.second->rid;
	}
	for(auto content :rcontent){
		std::cerr<<content.first.local_v->getName()<<"."<<content.first.local_v->id<<":"
			<<(int)(content.second->rid)<<std::endl;
		for(int tml:content.second->globalDefineUseTimeleline) std::cerr<<tml<<" ";
		std::cerr<<std::endl;
		std::cerr<<"--expense="<<content.second->spillcost<<"\n";
	}

}

bool AllOnStkAllocator::live_At(const vRegRSet *s1,const vRegRSet *s2){
	if(s1->globalDefineUseTimeleline.back()<s2->globalDefineUseTimeleline.front()
		||(s1->globalDefineUseTimeleline.front()>s2->globalDefineUseTimeleline.back()))
		return false;
	else return true;	
}

int AllOnStkAllocator::run() {
	std::unordered_map<LocalAddr , backend::vRegRSet *,Hashfunc> rcontent;
	//std::map<ircode::AddrPara * ,RId> callers; 
	for (auto * pVRegR: allUsedVRegR) {
					pVRegR->rid=RId::stk;
		if(!pVRegR->laddr){
			pVRegR->rid=RId::stk;
			continue;
		}
		if(!pVRegR->laddr->valorarray_t){
			std::cerr<<"%"<<pVRegR->laddr->local_v->getName()<<":";
			for(int id:pVRegR->laddr->idx) std::cerr<<id<<' ';
			std::cerr<<'\n';
			for(int l:defineUseTimelineVRegR[pVRegR]) std::cerr<<l<<' ';
			std::cerr<<'\n';
		}
		//std::cerr<<pVRegR->id;
		//pVRegR->prt();
		//backend::vRegRSet *set;
		if(!rcontent.count(*(pVRegR->laddr))) rcontent[*(pVRegR->laddr)]=new vRegRSet();
		auto set=rcontent[*(pVRegR->laddr)];
		set->pvRegRs.push_back(pVRegR);
		set->spillcost+=pVRegR->expense;
		std::vector<int> current=defineUseTimelineVRegR[pVRegR];
		set->globalDefineUseTimeleline.insert(set->globalDefineUseTimeleline.end(),
			current.begin(),current.end());
	}
	std::unordered_map<LocalAddr,backend::vRegRSet *,Hashfunc>::iterator it1,it2;
	for(it1=rcontent.begin();it1!=rcontent.end();++it1){
		std::sort(it1->second->globalDefineUseTimeleline.begin(),
			it1->second->globalDefineUseTimeleline.end());
		for(it2=rcontent.begin();it2!=it1;++it2){
			if(live_At(it1->second,it2->second)) 
				conflictMatrixR.insert(std::make_pair(it1->second,it2->second));
		}
	}
	//prune_Graph(rcontent);
	conflictMatrixR.clear();
	for (auto * pVRegS: allUsedVRegS) {
		pVRegS->sid = SId::stk;
		pVRegS->prt();
		//std::cerr<<pVRegS->id;
	}
	return 0;
}
}
