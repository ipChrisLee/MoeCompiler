#include "ASTVisitor.hpp"


using namespace mir;

namespace frontend {

antlrcpp::Any ASTVisitor::visitExp(SysYParser::ExpContext * ctx) {
	// exp -> addExp
	return ctx->addExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitAdd1(SysYParser::Add1Context * ctx) {
	//  addExp -> mulExp # add1
	return ctx->mulExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitAdd2(SysYParser::Add2Context * ctx) {
	//  addExp -> addExp ('+'|'-') mulExp # add 2
	auto op = std::string();
	if (ctx->Minus()) {
		op = "-";
	} else if (ctx->Addition()) {
		op = "+";
	} else {
		com::Throw("ctx should contain '+' or '-'.", CODEPOS);
	}
	if (info.stat.calculatingStaticValue) {
		ctx->addExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<StaticValue>>();
		ctx->mulExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<StaticValue>>();
		retVal.save(calcOnSV(*resL, op, *resR));
		return nullptr;
	} else {
		auto instrsRes = std::list<mir::Instr *>();
		ctx->addExp()->accept(this);
		auto resL = retVal.restore<mir::AddrOperand *>();
		auto instrsL = retInstrs.restore<std::list<mir::Instr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrsL));
		ctx->mulExp()->accept(this);
		auto resR = retVal.restore<mir::AddrOperand *>();
		auto instrsR = retInstrs.restore<std::list<mir::Instr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrsR));
		auto [opL, opR, type, instrsConversion] =
			genAddrConversion(ir, resL, resR);
		auto opD = ir.addrPool.emplace_back(
			mir::AddrVariable(*type)
		);
		instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
		instrsRes.splice(
			instrsRes.end(), genBinaryOperationInstrs(ir, opL, op, opR, opD)
		);
		retVal.save(static_cast<mir::AddrOperand *>(opD));
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitMul1(SysYParser::Mul1Context * ctx) {
	// mulExp -> unaryExp # mul1
	return ctx->unaryExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitMul2(SysYParser::Mul2Context * ctx) {
	// mulExp -> mulExp ('*' | '/' | '%') unaryExp # mul2
	auto op = std::string();
	if (ctx->Division()) {
		op = "/";
	} else if (ctx->Modulo()) {
		op = "%";
	} else if (ctx->Multiplication()) {
		op = "*";
	} else {
		com::Throw("ctx should contain '/' or '%' or '*'.", CODEPOS);
	}
	if (info.stat.calculatingStaticValue) {
		ctx->mulExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<StaticValue>>();
		ctx->unaryExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<StaticValue>>();
		retVal.save(calcOnSV(*resL, op, *resR));
		return nullptr;
	} else {
		auto instrsRes = std::list<mir::Instr *>();
		ctx->mulExp()->accept(this);
		auto * resL = retVal.restore<mir::AddrOperand *>();
		auto instrsL = retInstrs.restore<std::list<mir::Instr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrsL));
		ctx->unaryExp()->accept(this);
		auto * resR = retVal.restore<mir::AddrOperand *>();
		auto instrsR = retInstrs.restore<std::list<mir::Instr *>>();
		instrsRes.splice(instrsRes.end(), std::move(instrsR));
		auto [opL, opR, pType, instrsConversion] =
			genAddrConversion(ir, resL, resR);
		instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
		auto opD = ir.addrPool.emplace_back(
			mir::AddrVariable(*pType)
		);
		instrsRes.splice(
			instrsRes.end(), genBinaryOperationInstrs(ir, opL, op, opR, opD)
		);
		retVal.save(static_cast<mir::AddrOperand *>(opD));
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitUnary1(SysYParser::Unary1Context * ctx) {
	// unaryExp -> primaryExp # unary1
	return ctx->primaryExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitUnary2(SysYParser::Unary2Context * ctx) {
	// unaryExp -> Identifier '(' (funcRParams)? ')' # unary2
	//  For function call.
	auto varName = ctx->Identifier()->getText();
	if (info.stat.calculatingStaticValue) {
		com::Throw(
			com::concatToString(
				{
					"Const variable should be initialized with compile-time value, ",
					"but value calculated by function call is not compile time value ",
					"according to SysY semantic definition."
				}
			), CODEPOS
		);
	} else {
		auto instrsRes = std::list<mir::Instr *>();
		auto [idType, _addrFunc] = symbolTable.pScopeNow->findIdDownToRoot(varName);
		com::Assert(
			idType == IdType::FunctionName, "idType should be function name.",
			CODEPOS
		);
		auto pAddrFunc = dynamic_cast<mir::AddrFunction *>(_addrFunc);
		setWithAutoRestorer(info.var.pUsingFunc, std::move(pAddrFunc));//NOLINT
		auto paramsList = std::vector<mir::AddrOperand *>();
		if (ctx->funcRParams()) {
			ctx->funcRParams()->accept(this);
			paramsList = retVal.restore<std::vector<mir::AddrOperand * >>();
			instrsRes.splice(
				instrsRes.end(),
				retInstrs.restore<std::list<mir::Instr * >>()
			);
		}
		auto * pRetValAddr = static_cast<mir::AddrVariable *>(nullptr);
		if (info.var.pUsingFunc->getReturnTypeInfo().type != Type::Void_t) {
			pRetValAddr = ir.addrPool.emplace_back(
				mir::AddrVariable(info.var.pUsingFunc->getReturnTypeInfo())
			);
		}
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				mir::InstrCall(info.var.pUsingFunc, paramsList, pRetValAddr)
			)
		);
		retVal.save(static_cast<mir::AddrOperand *>(pRetValAddr));
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitUnaryOp(SysYParser::UnaryOpContext * ctx) {
	// unaryOp -> '+' | '-' | '!'
	if (auto p1 = ctx->Addition()) {
		retVal.save(p1->getText());
	} else if (auto p2 = ctx->Minus()) {
		retVal.save(p2->getText());
	} else if (auto p3 = ctx->Exclamation()) {
		retVal.save(p3->getText());
	} else {
		com::Throw("Unary op should be one of '+','-' or '!'.", CODEPOS);
	}
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitUnary3(SysYParser::Unary3Context * ctx) {
	// unaryExp -> unaryOp unaryExp # unary3
	ctx->unaryOp()->accept(this);
	auto op = retVal.restore<std::string>();
	ctx->unaryExp()->accept(this);
	if (info.stat.calculatingStaticValue) {
		auto p = retVal.restore<std::unique_ptr<StaticValue>>();
		retVal.save(calcOnSV(op, *p));
		return nullptr;
	} else {
		auto * opR = retVal.restore<mir::AddrOperand *>();
		auto instrsR = retInstrs.restore<std::list<mir::Instr *>>();
		auto * opD = genSuitableAddr(ir, op, opR);
		instrsR.splice(
			instrsR.end(), genUnaryOperationInstrs(ir, op, opR, opD)
		);
		retVal.save(static_cast<mir::AddrOperand *>(opD));//NOLINT
		retInstrs.save(std::move(instrsR));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context * ctx) {
	// primaryExp -> '(' exp ')' # primaryExp1
	return ctx->exp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context * ctx) {
	// primaryExp -> lVal # primaryExp2
	return ctx->lVal()->accept(this);
}

antlrcpp::Any ASTVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context * ctx) {
	// primaryExp -> number # primaryExp3
	return ctx->number()->accept(this);
}

antlrcpp::Any ASTVisitor::visitLVal(SysYParser::LValContext * ctx) {
	// lVal -> Identifier ('[' exp ']')*
	auto varName = ctx->Identifier()->getText();
	auto [identifierType, pIDVarAddr] = [
		this, &varName
	]() -> std::pair<IdType, mir::AddrVariable *> {
		auto [idStrType, pIDAddr] = symbolTable.pScopeNow->findIdDownToRoot(varName);
		com::Assert(
			com::enum_fun::in(
				idStrType,
				{IdType::LocalVarName, IdType::GlobalVarName, IdType::ParameterName}
			) && pIDAddr,
			"Error IdType or Error addr.", CODEPOS
		);
		com::Assert(pIDAddr->getType().type == Type::Pointer_t, "", CODEPOS);
		auto * _pIDVarAddr = dynamic_cast<mir::AddrVariable *>(pIDAddr);
		com::Assert(_pIDVarAddr, "", CODEPOS);
		return {idStrType, _pIDVarAddr};
	}();
	const auto & idTypeInfo = pIDVarAddr->getType();
	const auto & idPointToTypeInfo
		= *(dynamic_cast<const PointerType &>(pIDVarAddr->getType()).pointTo);

	if (info.stat.calculatingStaticValue) {
		auto idxs = [ctx, this]() -> std::vector<int> {
			auto idxs = std::vector<int>();
			setWithAutoRestorer(info.stat.calculatingStaticValue, true);
			for (auto p: ctx->exp()) {
				p->accept(this);
				auto idx = retVal.restore<std::unique_ptr<StaticValue>>();
				idx = convertOnSV(*idx, IntType());
				idxs.emplace_back(dynamic_cast<IntStaticValue &>(*idx).value);
			}
			return idxs;
		}();
		switch (identifierType) {
			case IdType::GlobalVarName: {
				auto * pAddr
					= dynamic_cast<mir::AddrGlobalVariable *>(pIDVarAddr);
				com::Assert(
					pAddr->isConstVar(), "Addr of var should be const.", CODEPOS
				);
				retVal.save(pAddr->getStaticValue().getValue(idxs));
				break;
			}
			case IdType::LocalVarName: {
				auto * pAddr = dynamic_cast<mir::AddrLocalVariable *>(pIDVarAddr);
				com::Assert(
					pAddr->isConstVar(), "Addr of var should be const.", CODEPOS
				);
				retVal.save(pAddr->getStaticValue().getValue(idxs));
				break;
			}
			default: com::Throw("Unsupported identifierType.", CODEPOS);
		}
		return nullptr;
	}

	auto instrsRes = std::list<mir::Instr *>();
	if (com::enum_fun::in(idPointToTypeInfo.type, {Type::Float_t, Type::Int_t})) {
		if (info.stat.visitingAssignmentLeft) { // as left side of assignment
			retVal.save(dynamic_cast<mir::AddrVariable *>(pIDVarAddr));
			retInstrs.save(std::move(instrsRes));
			return nullptr;
		} else { // use its value
			auto * pValAddr = ir.addrPool.emplace_back(
				mir::AddrVariable(idPointToTypeInfo)
			);
			instrsRes.emplace_back(
				ir.instrPool.emplace_back(
					mir::InstrLoad(pIDVarAddr, pValAddr)
				)
			);
			retVal.save(static_cast<mir::AddrOperand *>(pValAddr));
			retInstrs.save(std::move(instrsRes));
			return nullptr;
		}
	}

	//  get number of exp, and idxs
	auto nExp = ctx->exp().size();
	auto idxs = [
		this, &instrsRes, ctx, nExp
	]() -> std::vector<mir::AddrOperand *> {
		auto idxs = std::vector<mir::AddrOperand *>();
		for (auto i = size_t(0); i < nExp; ++i) {
			setWithAutoRestorer(info.stat.visitingAssignmentLeft, false);
			ctx->exp(i)->accept(this);
			auto * pIdxAddr = retVal.restore<mir::AddrOperand *>();
			auto instrsIdx = retInstrs.restore<std::list<mir::Instr *>>();
			instrsRes.splice(instrsRes.begin(), std::move(instrsIdx));
			idxs.emplace_back(pIdxAddr);
		}
		return idxs;
	}();

	auto * pDerefOnceAddr = [
		this, pIDVarAddr = pIDVarAddr, idType = identifierType, &instrsRes,
		&idTypeInfo
	]() -> mir::AddrVariable * {
		auto * pDerefOnceAddr = ir.addrPool.emplace_back(
			mir::AddrVariable(*typeDeduce(idTypeInfo, 1))
		);
		if (idType == IdType::ParameterName) {
			instrsRes.emplace_back(
				ir.instrPool.emplace_back(
					mir::InstrLoad(pIDVarAddr, pDerefOnceAddr)
				)
			);
		} else {
			auto idxs = std::vector<mir::AddrOperand *>(
				{
					ir.addrPool.emplace_back(
						mir::AddrStaticValue(IntStaticValue(0))
					),
					ir.addrPool.emplace_back(
						mir::AddrStaticValue(IntStaticValue(0))
					)
				}
			);
			instrsRes.emplace_back(
				ir.instrPool.emplace_back(
					mir::InstrGetelementptr(
						pDerefOnceAddr, pIDVarAddr, std::move(idxs)
					)
				)
			);
		}
		return pDerefOnceAddr;
	}();

	if (info.stat.visitingAssignmentLeft) { // as left side of assignment
		//  get val mem addr
		auto * pLValMemAddr = ir.addrPool.emplace_back(
			mir::AddrVariable(
				PointerType(*typeDeduce(pDerefOnceAddr->getType(), nExp))
			)
		);
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				mir::InstrGetelementptr(
					pLValMemAddr, pDerefOnceAddr, std::move(idxs)
				)
			)
		);
		retVal.save(std::move(pLValMemAddr));//NOLINT
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	} else { // use its value
		com::addRuntimeWarning(
			"Consider pass for load lVal.", CODEPOS, com::addWarningOnlyOnce
		);
		//  addr for val
		auto pValAddr = ir.addrPool.emplace_back(
			mir::AddrVariable(*typeDeduce(pDerefOnceAddr->getType(), nExp))
		);
		switch (pValAddr->getType().type) {
			case Type::Pointer_t: {
				idxs.emplace_back(
					ir.addrPool.emplace_back(
						mir::AddrStaticValue(IntStaticValue(0))
					)
				);
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrGetelementptr(
							pValAddr, pDerefOnceAddr, std::move(idxs)
						)
					)
				);
				break;
			}
			case Type::Int_t: {
				auto pValMemAddr = ir.addrPool.emplace_back(
					mir::AddrVariable(PointerType(IntType()))
				);
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrGetelementptr(
							pValMemAddr, pDerefOnceAddr, std::move(idxs)
						)
					)
				);
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrLoad(pValMemAddr, pValAddr)
					)
				);
				break;
			}
			case Type::Float_t: {
				auto pValMemAddr = ir.addrPool.emplace_back(
					mir::AddrVariable(PointerType(FloatType()))
				);
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrGetelementptr(
							pValMemAddr, pDerefOnceAddr, std::move(idxs)
						)
					)
				);
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrLoad(pValMemAddr, pValAddr)
					)
				);
				break;
			}
			default: {
				com::Throw(
					"Type deduced from another type should be int, float or pointer.",
					CODEPOS
				);
			}
		}
		retVal.save(static_cast<mir::AddrOperand *>(pValAddr));
		retInstrs.save(std::move(instrsRes));
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitNumber(SysYParser::NumberContext * ctx) {
	// number -> IntLiteral | FloatLiteral
	if (info.stat.calculatingStaticValue) {
		if (ctx->FloatLiteral()) {
			retVal.save(
				com::dynamic_cast_uPtr<StaticValue>(
					std::make_unique<FloatStaticValue>(ctx->getText())
				)
			);
			return nullptr;
		} else if (ctx->IntLiteral()) {
			retVal.save(
				com::dynamic_cast_uPtr<StaticValue>(
					std::make_unique<IntStaticValue>(ctx->getText())
				)
			);
			return nullptr;
		} else {
			com::Throw("ctx should be float/int literal.", CODEPOS);
		}
	} else {
		auto pAddr = static_cast<mir::AddrOperand *>(nullptr);
		if (ctx->FloatLiteral()) {
			pAddr = ir.addrPool.emplace_back(
				mir::AddrStaticValue(FloatStaticValue(ctx->getText()))
			);
		} else if (ctx->IntLiteral()) {
			pAddr = ir.addrPool.emplace_back(
				mir::AddrStaticValue(IntStaticValue(ctx->getText()))
			);
		} else {
			com::Throw("ctx should be float/int literal.", CODEPOS);
		}
		retVal.save(std::move(pAddr));//NOLINT
		retInstrs.save(std::list<mir::Instr *>());
		return nullptr;
	}
}

antlrcpp::Any ASTVisitor::visitLOr1(SysYParser::LOr1Context * ctx) {
	// lOrExp -> lAndExp # lOr1
	return ctx->lAndExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitLOr2(SysYParser::LOr2Context * ctx) {
	// lOrExp -> lOrExp '||' lAndExp # lOr2
	auto instrsRes = std::list<mir::Instr *>();
	auto * pLabelLeftFalse = ir.addrPool.emplace_back(
		mir::AddrJumpLabel("lor.lhs.false")
	);
	{
		setWithAutoRestorer(info.cond.jumpLabelFalse,
		                    std::move(pLabelLeftFalse));//NOLINT
		ctx->lOrExp()->accept(this);
		instrsRes.splice(
			instrsRes.end(), retInstrs.restore<std::list<mir::Instr *>>()
		);
	}
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(mir::InstrLabel(pLabelLeftFalse))//NOLINT
	);
	ctx->lAndExp()->accept(this);
	instrsRes.splice(
		instrsRes.end(), retInstrs.restore<std::list<mir::Instr *>>()
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitLAnd1(SysYParser::LAnd1Context * ctx) {
	// lAndExp -> eqExp # lAnd1
	auto instrsRes = std::list<mir::Instr *>();
	//  get val from expression
	ctx->eqExp()->accept(this);
	auto * pOpAddr = retVal.restore<mir::AddrOperand *>();
	auto instrsOperand = retInstrs.restore<std::list<mir::Instr *>>();
	//  Convert to bool
	auto [pCondAddr, _, instrsConversion]
		= genAddrConversion(ir, pOpAddr, Type::Bool_t);
	instrsRes.splice(instrsRes.end(), std::move(instrsOperand));
	instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			mir::InstrBr(
				pCondAddr, info.cond.jumpLabelTrue, info.cond.jumpLabelFalse
			)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitLAnd2(SysYParser::LAnd2Context * ctx) {
	// lAndExp -> lAndExp '&&' eqExp # lAnd2
	auto instrsRes = std::list<mir::Instr *>();
	auto * pLabelLeftTrue = ir.addrPool.emplace_back(
		mir::AddrJumpLabel("land.lhs.true")
	);
	{
		setWithAutoRestorer(info.cond.jumpLabelTrue,
		                    std::move(pLabelLeftTrue));//NOLINT
		ctx->lAndExp()->accept(this);
		instrsRes.splice(
			instrsRes.end(), retInstrs.restore<std::list<mir::Instr *>>()
		);
	}
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(mir::InstrLabel(pLabelLeftTrue))//NOLINT
	);
	ctx->eqExp()->accept(this);
	auto * pExpResAddr = retVal.restore<mir::AddrOperand *>();
	instrsRes.splice(
		instrsRes.end(), retInstrs.restore<std::list<mir::Instr *>>()
	);
	auto [pCondAddr, _, instrsCond]
		= genAddrConversion(ir, pExpResAddr, Type::Bool_t);
	instrsRes.splice(instrsRes.end(), std::move(instrsCond));
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			mir::InstrBr(
				pCondAddr, info.cond.jumpLabelTrue, info.cond.jumpLabelFalse
			)
		)
	);
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}


antlrcpp::Any ASTVisitor::visitEq1(SysYParser::Eq1Context * ctx) {
	// eqExp -> relExp # eq1
	return ctx->relExp()->accept(this);
}

antlrcpp::Any ASTVisitor::visitEq2(SysYParser::Eq2Context * ctx) {
	// eqExp -> eqExp ('==' | '!=') relExp # eq2
	auto instrsRes = std::list<mir::Instr *>();
	auto op = std::string(ctx->EQ() ? "==" : (ctx->NEQ() ? "!=" : ""));
	//  visit left and right operand
	com::Assert(!op.empty(), "??", CODEPOS);
	ctx->eqExp()->accept(this);
	auto * pLeftResAddr = retVal.restore<mir::AddrOperand *>();
	auto instrsLeft = retInstrs.restore<std::list<mir::Instr *>>();
	ctx->relExp()->accept(this);
	auto * pRightResAddr = retVal.restore<mir::AddrOperand *>();
	auto instrsRight = retInstrs.restore<std::list<mir::Instr *>>();
	//  Conversion
	auto [pLeftOpAddr, pRightOpAddr, _typeLR, instrsConversion]
		= genAddrConversion(ir, pLeftResAddr, pRightResAddr);
	//  calc
	auto * pResAddr = ir.addrPool.emplace_back(
		mir::AddrVariable(BoolType())
	);
	auto instrsCalc = genBinaryOperationInstrs(
		ir, pLeftOpAddr, op, pRightOpAddr, pResAddr
	);
	instrsRes.splice(instrsRes.end(), std::move(instrsLeft));
	instrsRes.splice(instrsRes.end(), std::move(instrsRight));
	instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
	instrsRes.splice(instrsRes.end(), std::move(instrsCalc));
	retVal.save(static_cast<mir::AddrOperand *>(pResAddr));
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitRel1(SysYParser::Rel1Context * ctx) {
	// relExp -> addExp # rel1
	ctx->addExp()->accept(this);
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitRel2(SysYParser::Rel2Context * ctx) {
	// relExp -> relExp ('<' | '>' | '<=' | '>=') addExp # rel2
	auto instrsRes = std::list<mir::Instr *>();
	auto op = [ctx]() -> std::string {
		if (ctx->LT()) { return "<"; }
		else if (ctx->GT()) { return ">"; }
		else if (ctx->LE()) { return "<="; }
		else if (ctx->GE()) { return ">="; }
		else { return ""; }
	}();
	//  visit left and right operand
	com::Assert(!op.empty(), "??", CODEPOS);
	ctx->relExp()->accept(this);
	auto * pLeftResAddr = retVal.restore<mir::AddrOperand *>();
	auto instrsLeft = retInstrs.restore<std::list<mir::Instr *>>();
	ctx->addExp()->accept(this);
	auto * pRightResAddr = retVal.restore<mir::AddrOperand *>();
	auto instrsRight = retInstrs.restore<std::list<mir::Instr *>>();
	//  Conversion
	auto [pLeftOpAddr, pRightOpAddr, _typeLR, instrsConversion]
		= genAddrConversion(ir, pLeftResAddr, pRightResAddr);
	//  calc
	auto * pResAddr = ir.addrPool.emplace_back(
		mir::AddrVariable(BoolType())
	);
	auto instrsCalc = genBinaryOperationInstrs(
		ir, pLeftOpAddr, op, pRightOpAddr, pResAddr
	);
	instrsRes.splice(instrsRes.end(), std::move(instrsLeft));
	instrsRes.splice(instrsRes.end(), std::move(instrsRight));
	instrsRes.splice(instrsRes.end(), std::move(instrsConversion));
	instrsRes.splice(instrsRes.end(), std::move(instrsCalc));
	retVal.save(static_cast<mir::AddrOperand *>(pResAddr));
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}


}
