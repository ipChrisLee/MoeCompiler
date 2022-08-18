#include "RegisterAllocator.hpp"


namespace backend {

void RegisterAllocator::set(
	std::unordered_set<backend::VRegR *> & _allVarVRegR,
	std::unordered_set<backend::VRegS *> & _allVarVRegS,
	std::unordered_set<backend::StkPtr *> & _allVarStkPtr,
	std::map<backend::VRegR *, std::vector<int>> & _defineUseTimelineVRegR,
	std::map<backend::VRegS *, std::vector<int>> & _defineUseTimelineVRegS,
	int _totalTim,
	std::map<ircode::AddrPara *, backend::Opnd *> & _argsOnPrev,
	int _argsStkSizeOnPrev,
	std::map<ircode::AddrPara *, backend::VRegR *> & _m_AddrArg_VRegR,
	std::map<ircode::AddrPara *, backend::VRegS *> & _m_AddrArg_VRegS
) {
	this->allVarVRegR = _allVarVRegR;
	this->allVarVRegS = _allVarVRegS;
	this->allVarStkPtr = _allVarStkPtr;
	this->defineUseTimelineVRegR = _defineUseTimelineVRegR;
	this->defineUseTimelineVRegS = _defineUseTimelineVRegS;
	this->totalTim = _totalTim;
	this->paramsOnCallingThis = _argsOnPrev;
	this->argsStkSizeOnCallingThis = _argsStkSizeOnPrev;
	this->m_AddrArg_VRegR = _m_AddrArg_VRegR;
	this->m_AddrArg_VRegS = _m_AddrArg_VRegS;
	for (auto [_, p]: this->m_AddrArg_VRegR) {
		fixedOpnd.emplace(p);
	}
	for (auto [_, p]: this->m_AddrArg_VRegS) {
		fixedOpnd.emplace(p);
	}
	com::Assert(argsStkSizeOnCallingThis % 8 == 0, "", CODEPOS);
}

int RegisterAllocator::getRes() {
	run();
	//  TODO: Add support for time-register-VRegRS
	//  Local VReg, include spilled registers and value alloca on stack.
	for (auto * pVRegR: allVarVRegR) {
		if (pVRegR->rid == RId::stk) {
			pVRegR->offset = spilledStkSize;
			spilledStkSize += 4;
		} else if (isCalleeSave(pVRegR->rid)) {
			if (backupRReg.find(pVRegR->rid) == backupRReg.end()) {
				backupRReg.insert(pVRegR->rid);
				backupStkSizeWhenCallingThis += 4;
			}
		} else if (isCallerSave(pVRegR->rid)) {
			callerSaveRReg.insert(pVRegR->rid);
		} else { com::Throw("", CODEPOS); }
	}
	for (auto * pVRegS: allVarVRegS) {
		if (pVRegS->sid == SId::stk) {
			pVRegS->offset = spilledStkSize;
			spilledStkSize += 4;
		} else if (isCalleeSave(pVRegS->sid)) {
			if (backupAndRestoreSReg.find(pVRegS->sid) == backupAndRestoreSReg.end()) {
				backupAndRestoreSReg.insert(pVRegS->sid);
				backupStkSizeWhenCallingThis += 4;
			}
		} else if (isCallerSave(pVRegS->sid)) {
			callerSaveSReg.insert(pVRegS->sid);
		} else { com::Throw("", CODEPOS); }
	}
	for (auto [pPara, pOpnd]: paramsOnCallingThis) {
		switch (pOpnd->getOpndType()) {
			case OpndType::VRegR: {
				auto * pVRegR = dynamic_cast<VRegR *>(pOpnd);
				if (isCallerSave(pVRegR->rid)) {
					callerSaveRReg.insert(pVRegR->rid);
				}
				break;
			}
			case OpndType::VRegS: {
				auto * pVRegS = dynamic_cast<VRegS *>(pOpnd);
				if (isCallerSave(pVRegS->sid)) {
					callerSaveSReg.insert(pVRegS->sid);
				}
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
	for (auto * pStkPtr: allVarStkPtr) {
		pStkPtr->offset = spilledStkSize;
		spilledStkSize += pStkPtr->sz;
	}
	restoreRReg = backupRReg;
	backupRReg.insert(RId::lr);
	restoreRReg.insert(RId::pc);
	backupStkSizeWhenCallingThis += 4;
	if ((backupStkSizeWhenCallingThis + spilledStkSize) % 8 != 0) {  //  need alignment
		spilledStkSize += 4;
	}
	//  change vreg of para after register allocation
	for (auto [pParaAddr, pOpndArg]: paramsOnCallingThis) {
		switch (pOpndArg->getOpndType()) {
			case OpndType::VRegR: {
				auto * pVRegRArg = m_AddrArg_VRegR[pParaAddr];
				if (pVRegRArg->rid == RId::stk) {
					pVRegRArg->offset = argsStkSizeOnCallingThis +
						pVRegRArg->offset + spilledStkSize + backupStkSizeWhenCallingThis;
				} else if (isGPR(pVRegRArg->rid)) {
					//  do nothing
				} else { com::Throw("", CODEPOS); }
				break;
			}
			case OpndType::VRegS: {
				auto * pVRegSArg = m_AddrArg_VRegS[pParaAddr];
				if (pVRegSArg->sid == SId::stk) {
					pVRegSArg->offset = argsStkSizeOnCallingThis +
						pVRegSArg->offset + spilledStkSize + backupStkSizeWhenCallingThis;
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

int AllOnStkAllocator::run() {
	for (auto * pVRegR: allVarVRegR) {
		pVRegR->rid = RId::stk;
	}
	for (auto * pVRegS: allVarVRegS) {
		pVRegS->sid = SId::stk;
	}
	return 0;
}

void LinearScanAllocator::prepare() {
	for (const auto & [pVRegRVar, timeVec]: defineUseTimelineVRegR) {
		if (intervalOfVRegR.find(pVRegRVar) == intervalOfVRegR.end()) {
			auto mn = totalTim + 1, mx = -1;
			std::for_each(
				timeVec.begin(), timeVec.end(), [&mn, &mx](int x) {
					mn = std::min(mn, x);
					mx = std::max(mx, x);
				}
			);
			intervalOfVRegR.emplace(pVRegRVar, std::make_pair(mn, mx));
			defVRegRAt[mn].emplace_back(pVRegRVar);
			lstUseVRegRAt[mx].emplace_back(pVRegRVar);
		}
	}
	for (const auto & [pVRegSVar, timeVec]: defineUseTimelineVRegS) {
		if (intervalOfVRegS.find(pVRegSVar) == intervalOfVRegS.end()) {
			auto mn = totalTim + 1, mx = -1;
			std::for_each(
				timeVec.begin(), timeVec.end(), [&mn, &mx](int x) {
					mn = std::min(mn, x);
					mx = std::max(mx, x);
				}
			);
			intervalOfVRegS.emplace(pVRegSVar, std::make_pair(mn, mx));
			defVRegSAt[mn].emplace_back(pVRegSVar);
			lstUseVRegSAt[mx].emplace_back(pVRegSVar);
		}
	}
	for (int i = 0; i < ridCnt; ++i) {
		if (isGPR(RId(i))) {
			freeRIds.emplace(RId(i));
		}
	}
	for (int i = 0; i < sidCnt; ++i) {
		if (isGPR(SId(i))) {
			freeSIds.emplace(SId(i));
		}
	}
	for (auto [pAddrPara, pOpndPara]: paramsOnCallingThis) {
		switch (pOpndPara->getOpndType()) {
			case OpndType::VRegR: {
				auto * pVRegR = dynamic_cast<backend::VRegR *>(pOpndPara);
				freeRIds.erase(pVRegR->rid);
				if (backend::isGPR(pVRegR->rid)) {
					livingVRegR[pVRegR->rid] = pVRegR;
				}
				break;
			}
			case OpndType::VRegS: {
				auto * pVRegS = dynamic_cast<backend::VRegS *>(pOpndPara);
				freeSIds.erase(pVRegS->sid);
				if (backend::isGPR(pVRegS->sid)) {
					livingVRegS[pVRegS->sid] = pVRegS;
				}
				break;
			}
			default:com::Throw("", CODEPOS);
		}
	}
}

RId LinearScanAllocator::chooseWhereToSpillRReg() {
	static std::vector<RId> vec = []() {
		auto res = std::vector<RId>();
		for (int i = 0; i < ridCnt; ++i) {
			if (backend::isGPR(RId(i))) {
				res.emplace_back(RId(i));
			}
		}
		return res;
	}();
	static auto idx = 0;
	if (idx == int(vec.size())) {
		idx = 0;
		std::shuffle(vec.begin(), vec.end(), _g);
	}
	RId r = vec[idx];
	++idx;
	return r;
}

SId LinearScanAllocator::chooseWhereToSpillSReg() {
	static std::vector<SId> vec = []() {
		auto res = std::vector<SId>();
		for (int i = 0; i < sidCnt; ++i) {
			if (backend::isGPR(SId(i))) {
				res.emplace_back(SId(i));
			}
		}
		return res;
	}();
	static auto idx = 0;
	if (idx == int(vec.size())) {
		idx = 0;
		std::shuffle(vec.begin(), vec.end(), _g);
	}
	SId r = vec[idx];
	++idx;
	return r;
}

void LinearScanAllocator::linear_scan() {
	for (int i = 1; i <= totalTim; ++i) {
		com::Assert(
			defVRegRAt.find(i) == defVRegRAt.end() || defVRegSAt.find(i) == defVRegSAt.end(),
			"", CODEPOS
		);
		if (lstUseVRegRAt.find(i) != lstUseVRegRAt.end()) {
			auto & vec = lstUseVRegRAt[i];
			for (auto * pVRegRLstUse: vec) {
				if (isGPR(pVRegRLstUse->rid)) {
					freeRIds.emplace(pVRegRLstUse->rid);
					livingVRegR.erase(pVRegRLstUse->rid);
				}
			}
		}
		if (lstUseVRegSAt.find(i) != lstUseVRegSAt.end()) {
			auto & vec = lstUseVRegSAt[i];
			for (auto * pVRegSLstUse: vec) {
				if (isGPR(pVRegSLstUse->sid)) {
					freeSIds.emplace(pVRegSLstUse->sid);
					livingVRegS.erase(pVRegSLstUse->sid);
				}
			}
		}
		if (defVRegRAt.find(i) != defVRegRAt.end()) {
			auto & vec = defVRegRAt[i];
			com::Assert(vec.size() == 1, "", CODEPOS);
			auto * definingVRegR = vec[0];
			if (freeRIds.empty()) {
				//  TODO: spill basing on some rate.
				//  Now: random choose
				auto newPos = chooseWhereToSpillRReg();
				auto * spilledVRegR = livingVRegR[newPos];
				if (fixedOpnd.find(spilledVRegR) != fixedOpnd.end()) {
					definingVRegR->rid = backend::RId::stk;
				} else {
					spilledVRegR->rid = backend::RId::stk;
					definingVRegR->rid = newPos;
					livingVRegR[newPos] = definingVRegR;
				}
			} else {
				auto newPos = *freeRIds.rbegin();
				freeRIds.erase(newPos);
				definingVRegR->rid = newPos;
				livingVRegR[newPos] = definingVRegR;
			}
		} else if (defVRegSAt.find(i) != defVRegSAt.end()) {
			auto & vec = defVRegSAt[i];
			com::Assert(vec.size() == 1, "", CODEPOS);
			auto * definingVRegS = vec[0];
			if (freeSIds.empty()) {
				auto newPos = chooseWhereToSpillSReg();
				auto * spilledVRegS = livingVRegS[newPos];
				if (fixedOpnd.find(spilledVRegS) != fixedOpnd.end()) {
					definingVRegS->sid = backend::SId::stk;
				} else {
					spilledVRegS->sid = backend::SId::stk;
					definingVRegS->sid = newPos;
					livingVRegS[newPos] = definingVRegS;
				}
			} else {
				auto newPos = *freeSIds.rbegin();
				freeSIds.erase(newPos);
				definingVRegS->sid = newPos;
				livingVRegS[newPos] = definingVRegS;
			}
		}
	}
}

int LinearScanAllocator::run() {
	prepare();
	linear_scan();
	return 0;
}

void FigureShadingAllocator::prune_Graph(std::unordered_map<LocalAddr , backend::vRegRSet *,Hashfunc> &rcontent){
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

void FigureShadingAllocator::prune_Graph(std::unordered_map<LocalAddr , backend::vRegSSet *,Hashfunc> &rcontent){
		for(auto content:rcontent){
		for(auto content_p:rcontent){
			if(conflictMatrixS.count(std::make_pair(content.second,content_p.second))||
				conflictMatrixS.count(std::make_pair(content_p.second,content.second))
				){
					content.second->nints++;
					content.second->neighbors.push_back(content_p.second);
				}
		}
	}
	bool token;
	int adjnodes=rcontent.size();
	std::stack<vRegSSet *> adjStack;
	while(adjnodes!=0){
		token=false;
		for(auto content:rcontent){
			if(content.second->allocated) continue;
			if(content.second->nints<AVAILABLE_RREGS){
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
		vRegSSet * lowestadj;
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
	SId baseline=SId::s16;
	while(!adjStack.empty()){
		auto node=adjStack.top();
		adjStack.pop();
		int j;
		for(j=0;j<AVAILABLE_RREGS;j++){
			if(node->available[j]) break;
		}
		if(j<AVAILABLE_RREGS){
			node->sid=(SId)((int)baseline+j);
			for(auto neighbor:node->dn_neighbors){
				neighbor->available[j]=false;
			}
		}
		else node->sid=SId::stk;
	}
	for(auto content:rcontent){
		for(auto pvReg:content.second->pvRegSs) pvReg->sid=content.second->sid;
	}
	for(auto content :rcontent){
		std::cerr<<content.first.local_v->getName()<<"."<<content.first.local_v->id<<":"
			<<(int)(content.second->sid)<<std::endl;
		for(int tml:content.second->globalDefineUseTimeleline) std::cerr<<tml<<" ";
		std::cerr<<std::endl;
		std::cerr<<"--expense="<<content.second->spillcost<<"\n";
	}
}

bool FigureShadingAllocator::live_At(const vRegSet *s1,const vRegSet *s2){
		if(s1->globalDefineUseTimeleline.back()<s2->globalDefineUseTimeleline.front()
		||(s1->globalDefineUseTimeleline.front()>s2->globalDefineUseTimeleline.back()))
		return false;
	else return true;	
}

void FigureShadingAllocator::prepare_Matrixs(){
	//std::map<ircode::AddrPara * ,RId> callers; 
	for (auto * pVRegR: allVarVRegR) {
		if(!pVRegR->laddr){
			pVRegR->rid=RId::stk;
			continue;
		}
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
	for(auto * pVRegS: allVarVRegS) {
		if(!pVRegS->laddr){
			pVRegS->sid=SId::stk;
			continue;
		}
		if(!s_rcontent.count(*(pVRegS->laddr))) s_rcontent[*(pVRegS->laddr)]=new vRegSSet();
		auto set=s_rcontent[*(pVRegS->laddr)];
		set->pvRegSs.push_back(pVRegS);
		set->spillcost+=pVRegS->expense;
		std::vector<int> current=defineUseTimelineVRegS[pVRegS];
		set->globalDefineUseTimeleline.insert(set->globalDefineUseTimeleline.end(),
			current.begin(),current.end());	
	}
	std::unordered_map<LocalAddr,backend::vRegSSet *,Hashfunc>::iterator its1,its2;
	for(its1=s_rcontent.begin();its1!=s_rcontent.end();++its1){
		std::sort(its1->second->globalDefineUseTimeleline.begin(),
			its1->second->globalDefineUseTimeleline.end());
		for(its2=s_rcontent.begin();its2!=its1;++its2){
			if(live_At(its1->second,its2->second)) 
				conflictMatrixS.insert(std::make_pair(its1->second,its2->second));
		}
	}
}

int FigureShadingAllocator::run(){
	prepare_Matrixs();
	prune_Graph(rcontent);
	prune_Graph(s_rcontent);
	conflictMatrixR.clear();
	conflictMatrixS.clear();
	/*for (auto * pVRegS: allVarVRegS) {
		pVRegS->sid = SId::stk;
		//pVRegS->prt();
		//std::cerr<<pVRegS->id;
	}*/

	return 0;
}

}
		/*if(!pVRegR->laddr->valorarray_t){
			std::cerr<<"%"<<pVRegR->laddr->local_v->getName()<<":";
			for(int id:pVRegR->laddr->idx) std::cerr<<id<<' ';
			std::cerr<<'\n';
			for(int l:defineUseTimelineVRegR[pVRegR]) std::cerr<<l<<' ';
			std::cerr<<'\n';
		}*/
		//std::cerr<<pVRegR->id;
		//pVRegR->prt();
		//backend::vRegRSet *set;