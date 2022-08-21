//
// Created by lee on 7/18/22.
//

#include "preprocessPass.hpp"


namespace pass {
using namespace ircode;


int AddBrToNextBB::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) { continue; }
		run(pFuncDef->instrs);
	}
	return 0;
}

AddBrToNextBB::AddBrToNextBB(ircode::IRModule & ir, std::string name) :
	IRPass(ir, std::move(name)) {
}

int AddBrToNextBB::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstrNow = instrs.begin();
	while (itPInstrNow != instrs.end()) {
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != instrs.end() &&
			!isTerminalInstr(get(itPInstrNow)->instrType) &&
			get(itPInstrNxt)->instrType == InstrType::Label) {
			instrs.emplace(
				itPInstrNxt, ir.instrPool.emplace_back(
					ircode::InstrBr(
						dynamic_cast<InstrLabel *>(get(itPInstrNxt))->pAddrLabel
					)
				)
			);
		}
		itPInstrNow = std::next(itPInstrNow);
	}
//	itPInstrNow = instrs.begin();
//	while (itPInstrNow != instrs.end()) {
//		auto itPInstrNxt = std::next(itPInstrNow);
//		if (itPInstrNxt != instrs.end() &&
//			isTerminalInstr(get(itPInstrNow)->instrType) &&
//			get(itPInstrNxt)->instrType != InstrType::Label) {
//			auto * pLabelAddr = ir.addrPool.emplace_back(
//				ircode::AddrJumpLabel()
//			);
//			instrs.emplace(
//				itPInstrNxt, ir.instrPool.emplace_back(
//					ircode::InstrLabel(pLabelAddr)
//				)
//			);
//		}
//		itPInstrNow = std::next(itPInstrNow);
//	}
	return 0;
}

int EliminateBrAfterBr::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) { continue; }
		run(pFuncDef->instrs);
	}
	return 0;
}


EliminateBrAfterBr::EliminateBrAfterBr(IRModule & ir, const std::string & name) :
	IRPass(ir, name) {
}

int EliminateBrAfterBr::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstrNow = instrs.begin();
	while (itPInstrNow != instrs.end()) {
		auto itPInstrNxt = std::next(itPInstrNow);
		if (itPInstrNxt != instrs.end()) {
			if (get(itPInstrNow)->instrType == ircode::InstrType::Br &&
				get(itPInstrNxt)->instrType != ircode::InstrType::Label) {
				auto * pBrNow = dynamic_cast<ircode::InstrBr *>(get(itPInstrNow));
				com::Assert(
					(pBrNow->pLabelTrue && pBrNow->pLabelFalse && !pBrNow->pCond) ||
						(pBrNow->pLabelTrue && !pBrNow->pLabelFalse && !pBrNow->pCond), "",
					CODEPOS
				);
				instrs.erase(itPInstrNxt);
				continue;
			}
		}
		itPInstrNow = itPInstrNxt;
	}
	return 0;
}

AddParaMovInstr::AddParaMovInstr(IRModule & ir, const std::string & name) : IRPass(ir, name) {
}

int AddParaMovInstr::run() {
	for (auto * pFuncDef: ir.funcPool) {
		if (pFuncDef->pAddrFun->justDeclare) { continue; }
		run(pFuncDef->instrs);
	}
	return 0;
}

int AddParaMovInstr::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstr = instrs.begin();
	while (itPInstr != instrs.end()) {
		if (get(itPInstr)->instrType == ircode::InstrType::Store) {
			auto * pStore = dynamic_cast<ircode::InstrStore *>(get(itPInstr));
			if (pStore->from->addrType == ircode::AddrType::ParaVar) {
				auto * pPara = dynamic_cast<ircode::AddrPara *>(pStore->from);
				auto * mid = ir.addrPool.emplace_back(
					ircode::AddrVariable(pStore->from->getType())
				);
				auto * pInstrMov = ir.instrPool.emplace_back(
					ircode::InstrParaMov(mid, pPara, ir)
				);
				pStore->from = mid;
				instrs.emplace(itPInstr, pInstrMov);
			}
		}
		itPInstr = std::next(itPInstr);
	}
	return 0;
}

}
