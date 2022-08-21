#pragma once

#include <unordered_set>
#include <map>
#include <set>

#include "IR/IRAddr.hpp"
#include "pass/pass-common.hpp"
#include "backend/Opnd.hpp"
#include "backend/RegisterAllocator.hpp"
#include "backend/to_asm.hpp"

#define WHILE_ST "while_cond"
#define WHILE_ED "while_end"
namespace pass {
std::string hexFormOf(int32_t val);
std::string hexFormOf(float val);

class FuncInfo {
  protected:
	//  OpndPool for this func
	backend::OpndPool & opndPool;

	//  mir::AddrFunction           ->      FuncInfo(include Label)
	std::map<ircode::AddrFunction *, FuncInfo *> & m_AddrFunc_FuncInfo;
	//  mir::AddrGlobalVariable     ->      Label
	std::map<ircode::AddrGlobalVariable *, backend::Label *> & m_AddrGVar_Label;
	//  mir::AddrLocalVariable      ->      StkPtr
	std::map<ircode::AddrLocalVariable *, backend::StkPtr *> m_AddrLocalVar_StkPtr;
	//  mir::AddrJumpLabel          ->      Label
	std::map<ircode::AddrJumpLabel *, backend::Label *> m_AddrJLabel_Label;
	//  mir::AddrVariable(tmp int)  ->      VRegR(unk)  ->  VRegR(stk)/VRegR(rx)
	std::map<ircode::AddrVariable *, backend::VRegR *> m_AddrVar_VRegR;
	//  mir::AddrVariable(tmp float)->      VRegS(unk)  ->  VRegS(stk)/VRegS(sx)
	std::map<ircode::AddrVariable *, backend::VRegS *> m_AddrVar_VRegS;
	//  mir::AddrPara(int/pointer)  ='run(), copy from `paramsInfoOnCallingThis`'=>
	//  VRegR(rx)/StkPtr(-stk)      ='retAllocator'=>
	//  VRegR(rx)/StkPtr(+stk)
	std::map<ircode::AddrPara *, backend::VRegR *> m_AddrArg_VRegR;
	//  mir::AddrPara(float)        ='copy from `paramsInfoOnCallingThis`'=>
	//  VRegS(sx)/StkPtr(-stk)      ='retAllocator'=>
	//  VRegS(sx)/StkPtr(+stk)
	std::map<ircode::AddrPara *, backend::VRegS *> m_AddrArg_VRegS;

	//  all VRegR and VRegS and StkPtr used in func, EXCLUDE para.
	std::unordered_set<backend::VRegR *> allVarVRegR;  //  all VRegR used in func
	std::unordered_set<backend::VRegS *> allVarVRegS;  //  all VRegS used in func
	std::unordered_set<backend::StkPtr *> allVarStkPtr;    //  all StkPtr used in func
	//  VRegR(unk) and VRegS(unk) define-use timeline. INCLUDE para.
	std::map<backend::VRegR *, std::vector<int>> defineUseTimelineVRegR;
	std::map<backend::VRegS *, std::vector<int>> defineUseTimelineVRegS;
	//  time counter
	int tim = 0;
	std::unique_ptr<backend::RegisterAllocator> regAllocator;
	//  StkSize for spilled registers.
	int spilledStkSize = 0;
	//  backup of callee saved registers
	int backupStkSizeWhenCallingThis = 0;
	//  backup of callee saved registers. Alignment is in consideration.
	std::set<backend::RId> backupRReg;   //  include lr, which is the return addr
	std::set<backend::RId> restoreRReg;  //  include pc, which means `return addr`
	std::set<backend::SId> backupAndRestoreSReg;

	//  Fixed opnd, like arg and return value, can not change its position.
	std::set<backend::Opnd *> fixedOpnd;

	backend::Opnd * markOperand(ircode::AddrOperand * pAddrOperand);
  public:
	int loop_label;
	//  IRFuncDef of this function
	ircode::IRFuncDef * pFuncDef;
	//  backend::Label of this function
	backend::Label * pFuncLabel = nullptr;
	//  caller saved registers may need to be stored in caller when calling this.
	std::set<backend::RId> callerSaveRReg;
	std::set<backend::SId> callerSaveSReg;
	//  AddrPara information when calling this.
	//  backendOpnd * : VRegR(rx), VRegR(stk, -bias), VRegS(sx), VRegS(stk, -bias)
	std::map<ircode::AddrPara *, backend::Opnd *> paramsInfoOnCallingThis;
	//  Stack size of arguments calling this function
	int argsStkSizeOnCallingThis = 0;
	FuncInfo(
		backend::OpndPool & pool, ircode::IRFuncDef * pFuncDef,
		std::map<ircode::AddrFunction *, FuncInfo *> & addrFuncToFuncInfo,
		std::map<ircode::AddrGlobalVariable *, backend::Label *> & m_AddrGVar_Label,
		std::unique_ptr<backend::RegisterAllocator> regAllocator
	);

	backend::StkPtr * convertLocalVar(ircode::AddrLocalVariable * pAddrLocalVar);
	backend::Label * convertLabel(ircode::AddrJumpLabel * pAddrJumpLabel);
	backend::VRegR * convertIntVariable(ircode::AddrVariable * pAddrVar);
	backend::VRegS * convertFloatVariable(ircode::AddrVariable * pAddrVar);
	backend::VRegR * convertThisIntArg(ircode::AddrPara * pAddrPara);
	backend::VRegS * convertThisFloatArg(ircode::AddrPara * pAddrPara);
	backend::Label * convertGlobalVar(ircode::AddrGlobalVariable * pAddrGVar);

	//  IRAddr markd => Register allocation =>
	int run();

	std::string toASM();

  protected:
	enum class CmpType {
		I, F, N
	} cmpType = CmpType::N;
	ircode::ICMP lastICmp = ircode::ICMP::ERR;
	ircode::FCMP lastFCmp = ircode::FCMP::ERR;
	ircode::AddrVariable * lastCondVarAddr = nullptr;

	static void
	genASMSaveFromRRegToVRegR(
		std::string & res, backend::VRegR * pVRegRTo, backend::RId rIdFrom,
		backend::RId scratchRId
	);

	static void
	genASMSaveFromSRegToVRegS(
		std::string & res, backend::VRegS * pVRegSTo, backend::SId sIdFrom,
		backend::RId scratchRId
	);

	static void
	genASMSaveFromVRegRToRReg(
		std::string & res, backend::VRegR * pVRegRFrom, backend::RId rIdTo
	);

	static void
	genASMSaveFromVRegSToSReg(
		std::string & res, backend::VRegS * pVRegSFrom, backend::SId sIdTo,
		backend::RId scratchRId
	);

	//  automatically sorted by id
	static void
	genASMPushRegs(std::string & res, const std::set<backend::RId> & list);

	static void
	genASMPushRegs(std::string & res, const std::set<backend::SId> & list);

	static void
	genASMPopRegs(std::string & res, const std::set<backend::RId> & list);

	static void
	genASMPopRegs(std::string & res, const std::set<backend::SId> & list);

	static backend::RId
	genASMLoadInt(std::string & res, int32_t val, backend::RId to);

	static backend::SId
	genASMLoadFloat(std::string & res, float val, backend::SId to, backend::RId scratchRId);

	static backend::RId
	genASMLoadFloatToRReg(std::string & res, float val, backend::RId destId);

	static backend::RId
	genASMLoadLabel(std::string & res, backend::Label * pLabel, backend::RId to);

	static void
	genASMDerefStkPtr(std::string & res, int offset, backend::RId rIdDest);

	static void
	genASMDerefStkPtrToSReg(
		std::string & res, int offset, backend::SId sIdDest, backend::RId scratchRId
	);

	[[nodiscard]] static std::string
	genASMPtrOffsetToOperand2(std::string & res, int offset, backend::RId rIdRest);

	[[nodiscard]] static std::string
	genASMPtrOffsetToFOperand2(std::string & res, int offset, backend::RId scratchRId);

	/**
	 * @brief Get value stored in VRegR.
	 * @param rIdIfInStk If pVRegR->rid==stk, saving value on this.
	 * @return The value is storing in where.
	 */
	static backend::RId genASMGetVRegRVal(
		std::string & res, backend::VRegR * pVRegR, backend::RId rIdIfInStk
	);

	static backend::SId genASMGetVRegSVal(
		std::string & res, backend::VRegS * pVRegS, backend::SId sIdIfInStk,
		backend::RId scratchRId
	);

	static void
	genASMSaveFromRRegToOffset(
		std::string & res, backend::RId ridFrom, int offset, backend::RId scratchReg
	);

	static void
	genASMSaveFromSRegToOffset(
		std::string & res, backend::SId sidFrom, int offset, backend::RId scratchReg
	);

	static void
	genASMSaveFromVRegRToVRegR(
		std::string & res, backend::VRegR * pVRegRFrom, backend::VRegR * pVRegRTo,
		backend::RId scratchRId, backend::RId scratchRId1
	);

	static void
	genASMSaveFromVRegSToVRegS(
		std::string & res, backend::VRegS * pVRegSFrom, backend::VRegS * pVRegSTo,
		backend::RId scratchRId, backend::RId scratchRId1
	);

	[[nodiscard]] static std::string
	genASMCondName(ircode::ICMP icmp, bool reverse = false);

	[[nodiscard]] static std::string
	genASMCondNameReverse(ircode::FCMP fcmp);

	[[nodiscard]] static std::string
	genASMBranchInstrs(
		const std::string & cond, backend::Label * pLabelTo, backend::RId scratchRId
	);

	//  Generate mapping of defined var in instruction
	//  Complete timeline of VRegs
	int run(ircode::IRInstr * pInstr);
	std::string toASM(ircode::IRInstr * pInstr);

	int run(ircode::InstrAlloca * pInstrAlloca);
	std::string toASM(ircode::InstrAlloca * pInstrAlloca);

	int run(ircode::InstrBr * pBr);
	std::string toASM(ircode::InstrBr * pBr);

	int run(ircode::InstrLoad * pInstrLoad);
	int run_Load_Int(ircode::InstrLoad * pInstrLoad);
	int run_Load_Float(ircode::InstrLoad * pInstrLoad);
	std::string toASM(ircode::InstrLoad * pInstrLoad);
	std::string toASM_Load_Int(ircode::InstrLoad * pInstrLoad);
	std::string toASM_Load_Float(ircode::InstrLoad * pInstrLoad);

	int run(ircode::InstrRet * pRet);
	std::string toASM(ircode::InstrRet * pRet);
	std::string toASM_Ret_Int(ircode::InstrRet * pRet);
	std::string toASM_Ret_Float(ircode::InstrRet * pRet);

	int run(ircode::InstrLabel * pInstrLabel);
	std::string toASM(ircode::InstrLabel * pInstrLabel);

	int run(ircode::InstrStore * pInstrStore);
	int run_Store_Int(ircode::InstrStore * pInstrStore);
	int run_Store_Float(ircode::InstrStore * pInstrStore);
	std::string toASM(ircode::InstrStore * pInstrStore);
	std::string toASM_Store_Int(ircode::InstrStore * pInstrStore);
	std::string toASM_Store_IntArray(ircode::InstrStore * pInstrStore);
	std::string toASM_Store_Float(ircode::InstrStore * pInstrStore);
	std::string toASM_Store_FloatArray(ircode::InstrStore * pInstrStore);

	int run_Binary_Op_Int(ircode::InstrBinaryOp * pInstrBinaryOp);
	int run_Binary_Op_Float(ircode::InstrBinaryOp * pInstrBinaryOp);
	std::string toASM_Binary_Op_Int(ircode::InstrBinaryOp * pInstrBinaryOp);
	std::string toASM_ADD_SUB_RSB(
		ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
		ircode::AddrVariable * pDest, const std::string & instr
	);
	std::string toASM_MUL_SDIV(
		ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
		ircode::AddrVariable * pDest, const std::string & instr
	);
	std::string toASM_SRem(
		ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
		ircode::AddrVariable * pDest
	);
	std::string toASM_Binary_Op_Float(ircode::InstrBinaryOp * pInstrBinaryOp);

	int run(ircode::InstrICmp * pInstrICmp);
	std::string toASM(ircode::InstrICmp * pInstrICmp);

	int run(ircode::InstrFCmp * pInstrFCmp);
	std::string toASM(ircode::InstrFCmp * pInstrFCmp);

	int run(ircode::InstrCall * pInstrCall);
	std::string toASM(ircode::InstrCall * pInstrCall);

	int run(ircode::InstrGetelementptr * pInstrGetelementptr);
	std::string toASM(ircode::InstrGetelementptr * pInstrGetelementptr);

	int run(ircode::InstrZExt * pInstrZExt);
	std::string toASM(ircode::InstrZExt * pInstrZExt);

	int run(ircode::InstrConversionOp * pInstrConversionOp);
	std::string toASM(ircode::InstrSitofp * pInstrSitofp);
	std::string toASM(ircode::InstrFptosi * pInstrFptosi);

	int run(ircode::InstrParaMov * pInstrParaMov);
	std::string toASM(ircode::InstrParaMov * pInstrParaMov);

	int run(ircode::InstrParallelCopy * pInstrCopy);
	std::string toASM(ircode::InstrParallelCopy * pInstrCopy);
	std::string toASM_Copy_Int(ircode::InstrParallelCopy * pInstrCopy);
	std::string toASM_Copy_Float(ircode::InstrParallelCopy * pInstrCopy);

	int run(ircode::InstrMarkVars * pInstrMark);
	std::string toASM(ircode::InstrMarkVars * pInstrMark);
};

class ToASM : public IRPass {
  protected:
	moeconcept::Pool<FuncInfo> funcInfoPool;
	backend::OpndPool opndPool;

	std::map<ircode::AddrFunction *, FuncInfo *> addrToFuncInfo;
	std::map<ircode::AddrGlobalVariable *, backend::Label *> gVarToLabel;
	static const char * asmHeader;
	static const char * gVarHeaderDataSection;
	static const char * functionsHeader;
	static const char * gVarHeaderBssSection;

	std::string declGVar(ircode::AddrGlobalVariable * pGVarAddr);
  public:
	explicit ToASM(ircode::IRModule & ir, const std::string & name = "");

	int run() override;

	[[nodiscard]] std::string toASM();
};

}