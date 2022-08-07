#pragma once

#include <map>
#include <unordered_set>

#include "backend/Opnd.hpp"


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
	//  Stack size of arguments calling this function (alignment 8)
	int argsStkSizeOnCallingThis = 0;
  public:
	explicit RegisterAllocator(OpndPool & opndPool) : opndPool(opndPool) {}

	//  StkSize for spilled registers.
	int spilledStkSize = 0;
	//  backup of callee saved registers
	int backupStkSize = 0;
	std::set<RId> backupRReg;   //  always include lr, which is the return address
	std::set<RId> restoreRReg;  //  always include pc, which means `return`
	std::set<SId> backupAndRestoreSReg;
	//  caller saved registers may need to be stored in caller
	std::set<RId> callerSaveRReg;
	std::set<SId> callerSaveSReg;
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
		int _argsStkSizeOnPrev
	);
	int getRes();
	virtual ~RegisterAllocator() = default;
};

class AllOnStkAllocator : public RegisterAllocator {
  protected:
	int run() override;
  public:
	explicit AllOnStkAllocator(OpndPool & opndPool) : RegisterAllocator(opndPool) {}

};

}