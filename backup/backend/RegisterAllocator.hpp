#pragma once

#include <map>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include "backend/Opnd.hpp"
#define AVAILABLE_RREGR 3
#define AVAILABLE_RREGS 16

namespace backend {


class RegisterAllocator {
  protected:
	virtual int run() = 0;
	backend::OpndPool & opndPool;

	using pii = std::pair<int, int>;
	//  all VRegR and VRegS used in func
	std::unordered_set<backend::VRegR *> allUsedVRegR;  //  all VRegR used in func
	std::unordered_set<backend::VRegS *> allUsedVRegS;  //  all VRegS used in func
	std::unordered_set<backend::StkPtr *> allStkPtr;    //  all StkPtr used in func
	//  VRegR(unk) and VRegS(unk) define-use timeline
	std::map<backend::VRegR *, std::vector<int>> defineUseTimelineVRegR;
	std::map<backend::VRegS *, std::vector<int>> defineUseTimelineVRegS;

	//  Interval of regs
	std::map<backend::VRegR *, pii> intervalOfVRegR;
	std::map<backend::VRegS *, pii> intervalOfVRegS;
	//  time counter
	int totalTim = 0;
	//  AddrPara information when calling this function.
	//  backendOpnd * : VRegR(rx), VRegR(stk, -bias), VRegS(sx), VRegS(stk, -bias)
	std::map<ircode::AddrPara *, backend::Opnd *> argsOnCallingThis;
  public:
	explicit RegisterAllocator(OpndPool & opndPool) : opndPool(opndPool) {}

	//  StkSize for spilled registers.
	int spilledStkSize = 0;
	//  backup of callee saved registers. Alignment is in consideration.
	int backupStkSize = 0;
	std::set<RId> backupRReg;   //  always include lr, which is the return address
	std::set<RId> restoreRReg;  //  always include pc, which means `return`
	std::set<SId> backupAndRestoreSReg;
	//  caller saved registers may need to be stored in caller
	std::set<RId> callerSaveRReg;
	std::set<SId> callerSaveSReg;
	//  Stack size of arguments calling this function (alignment 8)
	int argsStkSizeOnCallingThis = 0;
	//  get mapping of arguments to call this and bias of sp@function-runtime
	std::map<ircode::AddrPara *, backend::VRegR *> m_AddrArg_VRegR;
	std::map<ircode::AddrPara *, backend::VRegS *> m_AddrArg_VRegS;
	void set(
		std::unordered_set<backend::VRegR *> & _allUsedVRegR,
		std::unordered_set<backend::VRegS *> & _allUsedVRegS,
		std::unordered_set<backend::StkPtr *> & _allStkPtr,
		std::map<backend::VRegR *, std::vector<int>> & _defineUseTimelineVRegR,
		std::map<backend::VRegS *, std::vector<int>> & _defineUseTimelineVRegS,
		int _totalTim,
		std::map<ircode::AddrPara *, backend::Opnd *> & _argsOnPrev,
		int _argsStkSizeOnPrev,
		std::map<ircode::AddrPara *, backend::VRegR *> & m_AddrArg_VRegR,
		std::map<ircode::AddrPara *, backend::VRegS *> & m_AddrArg_VRegS
	);
	int getRes();
	virtual ~RegisterAllocator() = default;
};

typedef struct vRegRSet{
	std::vector<VRegR *> pvRegRs;
	RId rid;
	std::vector<int> globalDefineUseTimeleline;
	size_t spillcost;
	bool available[AVAILABLE_RREGR];
	bool allocated;
	int nints;
	std::vector<vRegRSet *> neighbors;
	std::vector<vRegRSet *> dn_neighbors;
	vRegRSet(){
		spillcost=0;
		nints=0;
		allocated=false;
		for(int j=0;j<AVAILABLE_RREGR;++j) available[j]=true;
	}
} vRegRSet;

struct Hashfunc{
	size_t operator()(const backend::LocalAddr & addr) const{
		return std::hash<int>()(addr.local_v->id);
	}
};

class AllOnStkAllocator : public RegisterAllocator {
  protected:
	int run() override;
	bool live_At(const vRegRSet *s1,const vRegRSet *s2);
	void prune_Graph(std::unordered_map<LocalAddr , backend::vRegRSet *,Hashfunc> &rcontent);
	std::set<std::pair<vRegRSet *,vRegRSet *>> conflictMatrixR;
  public:
	explicit AllOnStkAllocator(OpndPool & opndPool) : RegisterAllocator(opndPool) {}

};

}
