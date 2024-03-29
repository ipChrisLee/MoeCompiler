#pragma once

#include <map>
#include <unordered_set>
#include <queue>
#include <random>

#include "backend/Opnd.hpp"
#include "IR/IRModule.hpp"


namespace backend {

using pii = std::pair<int, int>;

class RegisterAllocator {
  protected:
	virtual int run() = 0;
	backend::OpndPool & opndPool;

	//==Information got from backendPass
	//  all VRegR and VRegS and StkPtr used in func. EXCLUDE para.
	std::unordered_set<backend::VRegR *> allVarVRegR;
	std::unordered_set<backend::VRegS *> allVarVRegS;
	std::unordered_set<backend::StkPtr *> allVarStkPtr;
	//  VRegR(unk) and VRegS(unk) define-use timeline. INCLUDE para.
	std::map<backend::VRegR *, std::vector<int>> defineUseTimelineVRegR;
	std::map<backend::VRegS *, std::vector<int>> defineUseTimelineVRegS;
	//  time counter. From 1 to totalTim.
	int totalTim = 0;
	//  AddrPara information when calling this function.
	//  backendOpnd * : VRegR(rx), VRegR(stk, -bias), VRegS(sx), VRegS(stk, -bias)
	std::map<ircode::AddrPara *, backend::Opnd *> paramsOnCallingThis;

	//  Fixed opnd, like arg, can not change its position.
	std::set<backend::Opnd *> fixedOpnd;

  public:
	explicit RegisterAllocator(OpndPool & opndPool) : opndPool(opndPool) {}

	//==Information generated after allocation.
	//  StkSize for spilled registers.
	int spilledStkSize = 0;
	//  backup of callee saved registers. Alignment is in consideration.
	int backupStkSizeWhenCallingThis = 0;
	std::set<RId> backupRReg;   //  always include lr, which is the return address
	std::set<RId> restoreRReg;  //  always include pc, which means `return`
	std::set<SId> backupAndRestoreSReg;
	//  caller saved registers may need to be stored in caller
	std::set<RId> callerSaveRReg;
	std::set<SId> callerSaveSReg;
	//  Stack size of arguments calling this function (alignment 8)
	int argsStkSizeOnCallingThis = 0;

	//==Changed from backendPass.
	//  get mapping of arguments to call this and bias of sp@function-runtime
	std::map<ircode::AddrPara *, backend::VRegR *> m_AddrArg_VRegR;
	std::map<ircode::AddrPara *, backend::VRegS *> m_AddrArg_VRegS;

	ircode::IRFuncDef * pFuncDef=nullptr;
	void set(
		std::unordered_set<backend::VRegR *> & _allVarVRegR,
		std::unordered_set<backend::VRegS *> & _allVarVRegS,
		std::unordered_set<backend::StkPtr *> & _allVarStkPtr,
		std::map<backend::VRegR *, std::vector<int>> & _defineUseTimelineVRegR,
		std::map<backend::VRegS *, std::vector<int>> & _defineUseTimelineVRegS,
		int _totalTim,
		std::map<ircode::AddrPara *, backend::Opnd *> & _argsOnPrev,
		int _argsStkSizeOnPrev,
		std::map<ircode::AddrPara *, backend::VRegR *> & m_AddrArg_VRegR,
		std::map<ircode::AddrPara *, backend::VRegS *> & m_AddrArg_VRegS,
		ircode::IRFuncDef * pFuncDef
	);

	//  Call run() first, do allocate, and analyze result.
	//  Then generate result on public properties.
	int getRes();
	virtual ~RegisterAllocator() = default;
};

class AllOnStkAllocator : public RegisterAllocator {
  protected:
	int run() override;
  public:
	explicit AllOnStkAllocator(OpndPool & opndPool) : RegisterAllocator(opndPool) {}
};

class LinearScanAllocator : public RegisterAllocator {
  protected:
	//==Information generated and used by Allocator
	//  Interval of regs. [def-time, last-use-time]
	//  def-time is equal to last-use-time if defined but not used.
	std::map<backend::VRegR *, pii> intervalOfVRegR;
	std::map<backend::VRegS *, pii> intervalOfVRegS;
	std::map<int, std::vector<backend::VRegR *>> defVRegRAt;
	std::map<int, std::vector<backend::VRegS *>> defVRegSAt;
	std::map<int, std::vector<backend::VRegR *>> lstUseVRegRAt;
	std::map<int, std::vector<backend::VRegS *>> lstUseVRegSAt;
	std::set<backend::RId, std::greater<>> freeRIds;
	std::set<backend::SId, std::greater<>> freeSIds;
	//  rid -> VRegR // sid -> VRegS | rid and sid is GPR
	std::map<backend::RId, backend::VRegR *, std::greater<>> livingVRegR;   //  include param
	std::map<backend::SId, backend::VRegS *, std::greater<>> livingVRegS;   //  include param

	RId chooseWhereToSpillRReg();
	SId chooseWhereToSpillSReg();

	void prepare();
	void linear_scan();

	int run() override;

	static int rdSeed;
	std::mt19937 _g;
  public:
	explicit LinearScanAllocator(OpndPool & opndPool);

};

}