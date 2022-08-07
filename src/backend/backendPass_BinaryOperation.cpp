#include "backendPass.hpp"


namespace pass {

int FuncInfo::run_Binary_Op_Int(ircode::InstrBinaryOp * pInstrBinaryOp) {
	auto * pLOp = pInstrBinaryOp->left;
	auto * pROp = pInstrBinaryOp->right;
	markOperand(pLOp);
	markOperand(pROp);
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
			rIdLOp = genASMLoadNumber(res, val, backend::RId::lhs);
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
			rIdROp = genASMLoadNumber(res, val, backend::RId::rhs);
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
			rIdLOp = genASMLoadNumber(res, val, backend::RId::lhs);
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
			rIdROp = genASMLoadNumber(res, val, backend::RId::rhs);
			break;
		}
		default:com::Throw("", CODEPOS);
	}
	switch (pDest->addrType) {
		case ircode::AddrType::Var: {
			genASMPushRegs(res, {backend::RId::r0});
			res += backend::toASM("sdiv", backend::RId::r0, rIdLOp, rIdROp);
			auto * pVRegRDest = convertIntVariable(pDest);
			if (backend::isGPR(pVRegRDest->rid)) {
				res += backend::toASM(
					"mls", pVRegRDest->rid, backend::RId::rhs, backend::RId::r0,
					backend::RId::lhs
				);
			} else if (pVRegRDest->rid == backend::RId::stk) {
				res += backend::toASM(
					"mls", backend::RId::r0, backend::RId::rhs, backend::RId::r0,
					backend::RId::lhs
				);
				genASMSaveFromRRegToOffset(
					res, backend::RId::r0, pVRegRDest->offset, backend::RId::lhs
				);
			} else { com::Throw("", CODEPOS); }
			genASMPopRegs(res, {backend::RId::r0});
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
			lOp = backend::to_asm(genASMLoadNumber(res, val, backend::RId::lhs));
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
				rOp = backend::to_asm(genASMLoadNumber(res, val, backend::RId::rhs));
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

}