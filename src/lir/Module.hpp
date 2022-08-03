#pragma once

#include <string>
#include <vector>

#include <common.hpp>
#include <moeconcept.hpp>
#include "mir/Module.hpp"
#include "support/Support.hpp"
#include "lir/Instr.hpp"


namespace lir {

class InstrPool : public moeconcept::Pool<lir::Instr> {
};

class Module : public LIRable {
  protected:

	mir::Module & mIR;
	lir::InstrPool instrPool;
	lir::OpndPool opndPool;
	std::list<lir::Instr *> instrs;

	struct Mapping {
	  protected:
		std::map<mir::AddrLocalVariable *, lir::StkPtr *> localVarToStkPtr;
		std::map<mir::AddrGlobalVariable *, lir::MemPtr *> globalVarToMemPtr;
		std::map<mir::AddrVariable *, lir::Opnd *> addrToOpnd;
		std::map<mir::AddrJumpLabel *, lir::Label *> labelToLabel;

	  public:
		struct FuncInfo {
			lir::Label * label;
			mir::Type retType;
			std::variant<RId, SId, std::monostate> retVal;
			std::map<int, RId> argRId;
			std::map<int, SId> argSId;
			std::set<RId> usedR;
			std::set<SId> usedS;
		};

		//  MIR::LocalVar -> LIR::StkPtr
		bool bind(mir::AddrLocalVariable * pAddrLocalVar, lir::StkPtr * pStkPtr);

		//  MIR::GlobalVar -> LIR::MemPtr
		bool bind(mir::AddrGlobalVariable * pAddrGlobalVar, lir::MemPtr * pMemPtr);

		lir::Opnd * getFromVariableToOpnd(mir::AddrVariable );
		//  MIR::TmpVar/MIR::Para -> LIR::VRegR()
		bool bind(mir::AddrVariable *, lir::Opnd *);

		//  MIR::JumpLabel -> LIR::Label
		bool bind(mir::AddrJumpLabel *, lir::Label *);

		std::map<const mir::AddrFunction *, FuncInfo> funcInfo;

		//  MIR::Func -> FuncInfo
		bool bind(const mir::AddrFunction * pAddrFunc, lir::Label * labelFunc);

		//  MIR::GlobalVar ->
		//  TODO:
	} mapping;

	struct Generator {
		std::list<lir::Instr *> genMemAddressOpnd(
			mir::AddrVariable * pAddrVar,
			lir::VRegR * genTo
		);
	} generator;

	std::list<lir::Instr *> fromMIRToLIR(mir::Instr * pInstr);

	std::list<lir::Instr *> fromMIRToLIR(mir::InstrAlloca * pAlloca);

	std::list<lir::Instr *> fromMIRToLIR(mir::InstrLabel * pLabel);

	std::list<lir::Instr *> fromMIRToLIR(mir::InstrStore * pStore);


  public:
	void main();

	explicit Module(mir::Module & mIr);

	[[nodiscard]] std::string toLIR() const override;


};

}