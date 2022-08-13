#include "backendPass.hpp"


namespace pass {

int FuncInfo::run_Binary_Op_Int(ircode::InstrBinaryOp * pInstrBinaryOp) {
	auto * pLOp = pInstrBinaryOp->left;
	auto * pROp = pInstrBinaryOp->right;
	auto * pDest = pInstrBinaryOp->res;
	markOperand(pLOp);
	markOperand(pROp);
	markOperand(pDest);
	return 0;
}

int FuncInfo::run_Binary_Op_Float(ircode::InstrBinaryOp * pInstrBinaryOp) {
	auto * pLOp = pInstrBinaryOp->left;
	auto * pROp = pInstrBinaryOp->right;
	auto * pDest = pInstrBinaryOp->res;
	markOperand(pLOp);
	markOperand(pROp);
	markOperand(pDest);
	return 0;
}

std::string FuncInfo::toASM_Binary_Op_Int(ircode::InstrBinaryOp * pInstrBinaryOp) {
	auto * pLOp = pInstrBinaryOp->left;
	auto * pROp = pInstrBinaryOp->right;
	auto * pDest = pInstrBinaryOp->res;
	switch (pInstrBinaryOp->instrType) {
		case ircode::InstrType::Add: {
			return toASM_ADD_SUB_RSB(
				pInstrBinaryOp->left, pInstrBinaryOp->right, pDest, "add"
			);
		}
		case ircode::InstrType::Sub: {
			if (pLOp->addrType == ircode::AddrType::StaticValue &&
				pROp->addrType != ircode::AddrType::StaticValue) {
				return toASM_ADD_SUB_RSB(
					pROp, pLOp, pDest, "rsb"
				);
			} else {
				return toASM_ADD_SUB_RSB(pLOp, pROp, pDest, "sub");
			}
		}
		case ircode::InstrType::Mul: {
			return toASM_MUL_SDIV(
				pInstrBinaryOp->left, pInstrBinaryOp->right, pDest, "mul"
			);
		}
		case ircode::InstrType::SDiv: {
			return toASM_MUL_SDIV(
				pInstrBinaryOp->left, pInstrBinaryOp->right, pDest, "sdiv"
			);
		}
		case ircode::InstrType::SRem: {
			return toASM_SRem(pLOp, pROp, pDest);
		}
		default:com::Throw("", CODEPOS);
	}
}

std::string FuncInfo::toASM_MUL_SDIV(
	ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
	ircode::AddrVariable * pDest, const std::string & instr
) {
	auto res = std::string();
	auto rIdLOp = backend::RId::err;
	switch (pLOp->addrType) {
		case ircode::AddrType::Var: {
			auto pVarAddr = dynamic_cast<ircode::AddrVariable *>(pLOp);
			rIdLOp = genASMGetVRegRVal(
				res, convertIntVariable(pVarAddr), backend::RId::lhs
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pLOp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rIdLOp = genASMLoadInt(res, val, backend::RId::lhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto rIdROp = backend::RId::err;
	switch (pROp->addrType) {
		case ircode::AddrType::Var: {
			auto pVarAddr = dynamic_cast<ircode::AddrVariable *>(pROp);
			rIdROp = genASMGetVRegRVal(
				res, convertIntVariable(pVarAddr), backend::RId::rhs
			);
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pROp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rIdROp = genASMLoadInt(res, val, backend::RId::rhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	res += backend::toASM(instr, backend::RId::lhs, rIdLOp, rIdROp);
	auto * pVRegRDest = convertIntVariable(pDest);
	if (backend::isGPR(pVRegRDest->rid)) {
		res += backend::toASM("mov", pVRegRDest->rid, backend::RId::lhs);
	} else if (pVRegRDest->rid == backend::RId::stk) {
		auto strTo = genASMPtrOffsetToOperand2(
			res, pVRegRDest->offset, backend::RId::rhs
		);
		res += backend::toASM("str", backend::RId::lhs, strTo);
	}
	return res;
}

std::string FuncInfo::toASM_SRem(
	ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
	ircode::AddrVariable * pDest
) {
	auto res = std::string();
	auto rIdLOp = backend::RId::err;
	switch (pLOp->addrType) {
		case ircode::AddrType::Var: {
			auto pVarAddr = dynamic_cast<ircode::AddrVariable *>(pLOp);
			genASMSaveFromVRegRToRReg(
				res, convertIntVariable(pVarAddr), backend::RId::lhs
			);
			rIdLOp = backend::RId::lhs;
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pLOp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rIdLOp = genASMLoadInt(res, val, backend::RId::lhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto rIdROp = backend::RId::err;
	switch (pROp->addrType) {
		case ircode::AddrType::Var: {
			auto pVarAddr = dynamic_cast<ircode::AddrVariable *>(pROp);
			genASMSaveFromVRegRToRReg(
				res, convertIntVariable(pVarAddr), backend::RId::rhs
			);
			rIdROp = backend::RId::rhs;
			break;
		}
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pROp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rIdROp = genASMLoadInt(res, val, backend::RId::rhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	switch (pDest->addrType) {
		case ircode::AddrType::Var: {
			auto * pVRegRDest = convertIntVariable(pDest);
			//  TODO: can be optimized [check what register is freeing]
			if (backend::isGPR(pVRegRDest->rid)) {
				res += backend::toASM("sdiv", pVRegRDest->rid, rIdLOp, rIdROp);
				res += backend::toASM(
					"mls", pVRegRDest->rid, rIdROp, pVRegRDest->rid, rIdLOp
				);
			} else if (pVRegRDest->rid == backend::RId::stk) {
				genASMPushRegs(res, {backend::RId::r0});
				res += backend::toASM("sdiv", backend::RId::r0, rIdLOp, rIdROp);
				res += backend::toASM(
					"mls", backend::RId::lhs, rIdROp, backend::RId::r0, rIdLOp
				);
				genASMPopRegs(res, {backend::RId::r0});
				genASMSaveFromRRegToVRegR(
					res, pVRegRDest, backend::RId::lhs, backend::RId::rhs
				);
			} else { com::Throw("", CODEPOS); }
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	return res;
}

std::string FuncInfo::toASM_ADD_SUB_RSB(
	ircode::AddrOperand * pLOp, ircode::AddrOperand * pROp,
	ircode::AddrVariable * pDest, const std::string & instr
) {
	auto res = std::string();
	auto lOp = std::string();
	switch (pLOp->addrType) {
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pLOp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			lOp = backend::to_asm(genASMLoadInt(res, val, backend::RId::lhs));
			break;
		}
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pLOp);
			auto lRId = genASMGetVRegRVal(
				res, convertIntVariable(pVarAddr), backend::RId::lhs
			);
			lOp = backend::to_asm(lRId);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto rOp = std::string();
	switch (pROp->addrType) {
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pROp);
			int val = dynamic_cast<const sup::IntStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			if (backend::Imm<backend::ImmType::Imm8m>::fitThis(val)) {
				rOp = backend::to_asm(val);
			} else {
				rOp = backend::to_asm(genASMLoadInt(res, val, backend::RId::rhs));
			}
			break;
		}
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pROp);
			auto rRId = genASMGetVRegRVal(
				res, convertIntVariable(pVarAddr), backend::RId::rhs
			);
			rOp = backend::to_asm(rRId);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto * pVRegRDest = convertIntVariable(pDest);
	if (backend::isGPR(pVRegRDest->rid)) {
		res += backend::toASM(instr, pVRegRDest->rid, lOp, rOp);
	} else if (pVRegRDest->rid == backend::RId::stk) {
		res += backend::toASM(instr, backend::RId::lhs, lOp, rOp);
		auto offset = pVRegRDest->offset;
		auto ptrToDest = genASMPtrOffsetToOperand2(res, offset, backend::RId::rhs);
		res += backend::toASM("str", backend::RId::lhs, ptrToDest);
	} else { com::Throw("", CODEPOS); }
	return res;
}


std::string FuncInfo::toASM_Binary_Op_Float(ircode::InstrBinaryOp * pInstrBinaryOp) {
	auto pLOp = pInstrBinaryOp->left;
	auto pROp = pInstrBinaryOp->right;
	auto pDest = pInstrBinaryOp->res;
	auto instr = [pInstrBinaryOp]() -> std::string {
		auto ins = std::string();
		switch (pInstrBinaryOp->instrType) {
			case ircode::InstrType::FAdd: ins = "vadd";
				break;
			case ircode::InstrType::FSub: ins = "vsub";
				break;
			case ircode::InstrType::FMul: ins = "vmul";
				break;
			case ircode::InstrType::FDiv: ins = "vdiv";
				break;
			default:com::Throw("", CODEPOS);
		}
		return ins;
	}() + ".f32";

	auto res = std::string();
	auto lOp = std::string();
	switch (pLOp->addrType) {
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pLOp);
			auto val = dynamic_cast<const sup::FloatStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			lOp = backend::to_asm(
				genASMLoadFloat(res, val, backend::SId::lhs, backend::RId::lhs)
			);
			break;
		}
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pLOp);
			auto lSId = genASMGetVRegSVal(
				res, convertFloatVariable(pVarAddr), backend::SId::lhs, backend::RId::lhs
			);
			lOp = backend::to_asm(lSId);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto rOp = std::string();
	switch (pROp->addrType) {
		case ircode::AddrType::StaticValue: {
			auto * pSVAddr = dynamic_cast<ircode::AddrStaticValue *>(pROp);
			float val = dynamic_cast<const sup::FloatStaticValue &>(
				pSVAddr->getStaticValue()
			).value;
			rOp = backend::to_asm(
				genASMLoadFloat(res, val, backend::SId::rhs, backend::RId::rhs)
			);
			break;
		}
		case ircode::AddrType::Var: {
			auto * pVarAddr = dynamic_cast<ircode::AddrVariable *>(pROp);
			auto rSId = genASMGetVRegSVal(
				res, convertFloatVariable(pVarAddr), backend::SId::rhs, backend::RId::rhs
			);
			rOp = backend::to_asm(rSId);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	auto * pVRegSDest = convertFloatVariable(pDest);
	if (backend::isGPR(pVRegSDest->sid)) {
		res += backend::toASM(instr, pVRegSDest->sid, lOp, rOp);
	} else if (pVRegSDest->sid == backend::SId::stk) {
		res += backend::toASM(instr, backend::SId::lhs, lOp, rOp);
		auto offset = pVRegSDest->offset;
		auto ptrToDest = genASMPtrOffsetToFOperand2(res, offset, backend::RId::rhs);
		res += backend::toASM("vstr", backend::SId::lhs, ptrToDest);
	} else { com::Throw("", CODEPOS); }
	return res;
}

}