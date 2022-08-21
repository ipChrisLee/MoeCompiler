#include "RegisterAllocator.hpp"
#include "IR/IRModule.hpp"


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
	std::map<ircode::AddrPara *, backend::VRegS *> & _m_AddrArg_VRegS,
	ircode::IRFuncDef * _pFuncDef
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
	this->pFuncDef = _pFuncDef;
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
	if (pFuncDef->pAddrFun->uPtrReturnTypeInfo) {
		if (pFuncDef->pAddrFun->uPtrReturnTypeInfo->type == sup::Type::Int_t) {
			callerSaveRReg.insert(RId::r0);
		} else if (pFuncDef->pAddrFun->uPtrReturnTypeInfo->type == sup::Type::Float_t) {
			callerSaveSReg.insert(SId::s0);
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
		if (defVRegRAt.find(i) != defVRegRAt.end()) {
			auto & vec = defVRegRAt[i];
			for (auto * definingVRegR: vec) {
				if (freeRIds.empty()) {
					//  TODO: spill basing on some rate.
					//  Now: random choose
					auto newPos = chooseWhereToSpillRReg();
					auto * spilledVRegR = livingVRegR[newPos];
					if (fixedOpnd.count(spilledVRegR)) {
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
			}
		}
		if (defVRegSAt.find(i) != defVRegSAt.end()) {
			auto & vec = defVRegSAt[i];
			for (auto * definingVRegS: vec) {
				if (freeSIds.empty()) {
					auto newPos = chooseWhereToSpillSReg();
					auto * spilledVRegS = livingVRegS[newPos];
					if (fixedOpnd.count(spilledVRegS)) {
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
	}
}

int LinearScanAllocator::run() {
	prepare();
	linear_scan();
	return 0;
}

int LinearScanAllocator::rdSeed = 20001220;

LinearScanAllocator::LinearScanAllocator(OpndPool & opndPool) :
	RegisterAllocator(opndPool), _g(rdSeed) {
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
		if(!defineUseTimelineVRegR.count(pVRegR)||defineUseTimelineVRegR[pVRegR].empty()){
			pVRegR->rid=RId::stk;
			continue;
		} 
		auto  current=new vRegRSet();
		current->globalDefineUseTimeleline=defineUseTimelineVRegR[pVRegR];
		current->pvRegR=pVRegR;
		current->spillcost=pVRegR->expense;
		rcontent.push_back(current);
	}
	for(size_t i=0;i<rcontent.size();++i){
		for(size_t j=0;j<i;++j){
			if(live_At(rcontent[i],rcontent[j])) 
				conflictMatrixR.insert(std::make_pair(rcontent[i],rcontent[j]));
		}
	}
	for(auto * pVRegS: allVarVRegS) {
		if(!defineUseTimelineVRegS.count(pVRegS)||defineUseTimelineVRegS[pVRegS].empty()){
			pVRegS->sid=SId::stk;
			continue;
		} 
		auto current=new vRegSSet();
		current->globalDefineUseTimeleline=defineUseTimelineVRegS[pVRegS];
		current->pvRegS=pVRegS;
		current->spillcost=pVRegS->expense;
		s_rcontent.push_back(current);
	}
	for(size_t i=0;i<s_rcontent.size();++i){
		for(size_t j=0;j<i;++j){
			if(live_At(s_rcontent[i],s_rcontent[j])) 
				conflictMatrixS.insert(std::make_pair(s_rcontent[i],s_rcontent[j]));
		}
	}
}

void FigureShadingAllocator::prune_Graph(std::vector<backend::vRegRSet *> &rcontent){
	for(auto content:rcontent){
		for(auto content_p:rcontent){
			if(conflictMatrixR.count(std::make_pair(content,content_p))||
				conflictMatrixR.count(std::make_pair(content_p,content))
				){
					content->nints++;
					content->neighbors.push_back(content_p);
				}
		}
	}
	bool token;
	int adjnodes=rcontent.size();
	std::stack<vRegRSet *> adjStack;
	while(adjnodes!=0){
		token=false;
		for(auto content:rcontent){
			if(content->allocated) continue;
			if(content->nints<AVAILABLE_RREGR){
				token=true;
				--adjnodes;
				content->allocated=true;
				for(auto neighbor:content->neighbors){
					neighbor->nints--;
					neighbor->dn_neighbors.push_back(content);
				}
				adjStack.push(content);
			}
		}
		size_t lowest=0;
		vRegRSet * lowestadj;
		if(!token&&adjnodes!=0){
			for(auto content:rcontent){
				if(content->allocated) continue;
				if(lowest==0||content->spillcost<lowest){
					lowest=content->spillcost;
					lowestadj=content;
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
	for(auto content:rcontent) content->pvRegR->rid=content->rid;
	/*for(auto content :rcontent){
		std::cerr<<content->pvRegR->id<<":"<<(int)(content->pvRegR->rid)<<"\n";
		for(int tml:content->globalDefineUseTimeleline) std::cerr<<tml<<" ";
		std::cerr<<std::endl;
		std::cerr<<"--expense="<<content->spillcost<<"\n";
	}*/

}

void FigureShadingAllocator::prune_Graph(std::vector<backend::vRegSSet *> &rcontent){
		for(auto content:rcontent){
		for(auto content_p:rcontent){
			if(conflictMatrixS.count(std::make_pair(content,content_p))||
				conflictMatrixS.count(std::make_pair(content_p,content))
				){
					content->nints++;
					content->neighbors.push_back(content_p);
				}
		}
	}
	bool token;
	int adjnodes=rcontent.size();
	std::stack<vRegSSet *> adjStack;
	while(adjnodes!=0){
		token=false;
		for(auto content:rcontent){
			if(content->allocated) continue;
			if(content->nints<AVAILABLE_RREGS){
				token=true;
				--adjnodes;
				content->allocated=true;
				for(auto neighbor:content->neighbors){
					neighbor->nints--;
					neighbor->dn_neighbors.push_back(content);
				}
				adjStack.push(content);
			}
		}
		size_t lowest=0;
		vRegSSet * lowestadj;
		if(!token&&adjnodes!=0){
			for(auto content:rcontent){
				if(content->allocated) continue;
				if(lowest==0||content->spillcost<lowest){
					lowest=content->spillcost;
					lowestadj=content;
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
		content->pvRegS->sid=content->sid;
	}
	/*for(auto content :rcontent){
		std::cerr<<content->pvRegS->id;
		for(int tml:content->globalDefineUseTimeleline) std::cerr<<tml<<" ";
		std::cerr<<std::endl;
		std::cerr<<"--expense="<<content->spillcost<<"\n";
	}*/
}
int FigureShadingAllocator::run(){
	prepare_Matrixs();
	prune_Graph(rcontent);
	prune_Graph(s_rcontent);
	//for (auto * pVRegR: allVarVRegR) std::cerr<<pVRegR->id<<(int)(pVRegR->rid)<<' ';
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