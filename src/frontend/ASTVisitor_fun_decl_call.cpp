#include "ASTVisitor.hpp"


using namespace sup;

namespace frontend {
antlrcpp::Any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext * ctx) {
	// funcDef -> funcType Identifier '(' (funcFParams)? ')' block
	//  Set info, get funcType and Identifier.
	setWithAutoRestorer(info.stat.inGlobal, false);
	ctx->funcType()->accept(this);
	auto funcType = retVal.restore<FuncType>();
	auto funName = ctx->Identifier()->getText();
	//  Visit funcFParams to get paramsInfoOnCallingThis information.
	auto funcDeclScope = symbolTable.pScopeNow;
	setWithAutoRestorer(symbolTable.pScopeNow, symbolTable.pScopeNow->addSonScope());
	std::vector<ircode::AddrPara *> params;
	if (ctx->funcFParams()) {
		ctx->funcFParams()->accept(this);
		params = retVal.restore<std::vector<ircode::AddrPara *>>();
	}
	//  Create Addr of function and retval and ret bock label.
	ircode::AddrFunction * pAddrFun = nullptr;
	ircode::AddrLocalVariable * pRetvalMem = nullptr;
	setWithAutoRestorer(info.func.pRetBlockLabel,
	                    ir.addrPool.emplace_back(ircode::AddrJumpLabel("return")));
	switch (funcType) {
		case FuncType::Void: {
			pAddrFun = ir.addrPool.emplace_back(
				ircode::AddrFunction(funName, params)
			);
			pRetvalMem = nullptr;
			break;
		}
		case FuncType::Float: {
			pAddrFun = ir.addrPool.emplace_back(
				ircode::AddrFunction(funName, params, FloatType())
			);
			pRetvalMem = ir.addrPool.emplace_back(
				ircode::AddrLocalVariable(
					FloatType(), "retval", false
				)
			);
			break;
		}
		case FuncType::Int: {
			pAddrFun = ir.addrPool.emplace_back(
				ircode::AddrFunction(funName, params, IntType())
			);
			pRetvalMem = ir.addrPool.emplace_back(
				ircode::AddrLocalVariable(
					IntType(), "retval", false
				)
			);
			break;
		}
		default: {
			com::Throw("Unknown Type!", CODEPOS);
		}
	}
	setWithAutoRestorer(info.func.pRetvalMem, std::move(pRetvalMem));//NOLINT
	//  Add to symbol table
	funcDeclScope->bindDominateVar(funName, IdType::FunctionName, pAddrFun);
	//  Create default funcDef variable.
	auto funcDef = ircode::IRFuncDef(pAddrFun);
	for (const auto & pAddrPara: params) {
		auto pParaMemAddr = ir.addrPool.emplace_back(
			ircode::AddrLocalVariable(pAddrPara->getType(), pAddrPara->getName(), false)
		);
		symbolTable.pScopeNow->bindDominateVar(
			pAddrPara->getName(), IdType::ParameterName, pParaMemAddr
		);
		funcDef.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrAlloca(pParaMemAddr, pAddrPara->getType())
			)
		);
		funcDef.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrStore(pAddrPara, pParaMemAddr)
			)
		);
	}
	setWithAutoRestorer(info.func.pFuncDef, &funcDef);
	//  Create instrs list.
	auto instrsRes = std::list<ircode::IRInstr *>();
	//  for return
	if (info.func.pRetvalMem) {
		instrsRes.emplace_back(
			ir.instrPool.emplace_back(
				ircode::InstrAlloca(info.func.pRetvalMem)
			)
		);
	}
	//  Translate block items.
	ctx->block()->accept(this);
	auto instrsBlock = retInstrs.restore<std::list<ircode::IRInstr *>>();
	instrsRes.splice(instrsRes.end(), std::move(instrsBlock));
	//  add `return` label
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrLabel(info.func.pRetBlockLabel)
		)
	);
	//  store from retval
	ircode::AddrVariable * pRetval = nullptr;
	switch (funcType) {
		case FuncType::Int: {
			pRetval = ir.addrPool.emplace_back(
				ircode::AddrVariable(IntType())
			);
			instrsRes.emplace_back(
				ir.instrPool.emplace_back(
					ircode::InstrLoad(
						info.func.pRetvalMem, pRetval
					)
				)
			);
			break;
		}
		case FuncType::Float: {
			pRetval = ir.addrPool.emplace_back(
				ircode::AddrVariable(FloatType())
			);
			instrsRes.emplace_back(
				ir.instrPool.emplace_back(
					ircode::InstrLoad(
						info.func.pRetvalMem, pRetval
					)
				)
			);
			break;
		}
		case FuncType::Void: {
			pRetval = nullptr;
			break;
		}
		default: {
			com::Throw("", CODEPOS);
		}
	}
	//  `ret` instruction
	instrsRes.emplace_back(
		ir.instrPool.emplace_back(
			ircode::InstrRet(
				pRetval
			)
		)
	);
	//  Add instrs to funcDef and add funcDef to irModule.
	funcDef.emplace_back(std::move(instrsRes));
	ir.funcPool.emplace_back(std::move(funcDef));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitFuncFParams(SysYParser::FuncFParamsContext * ctx) {
	// funcFParams -> funcFParam (',' funcFParam)*;
	/**
	 * @brief The move constructor of std::vector will just move pointer to memory.
	 * @ref https://stackoverflow.com/a/53879096/17924585
	 */
	std::vector<ircode::AddrPara *> vecPara;
	for (auto p: ctx->funcFParam()) {
		p->accept(this);
		vecPara.emplace_back(retVal.restore<ircode::AddrPara *>());
	}
	retVal.save(std::move(vecPara));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitFuncFParam(SysYParser::FuncFParamContext * ctx) {
	// funcFParam -> bType Identifier ('[' ']' ('[' constExp ']'])* )?
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.var.btype, retVal.restore<BType>());
	auto varname = ctx->Identifier()->getText();
	bool isPointer = !ctx->Lbrkt().empty();
	auto uPtrTypeInfo = std::unique_ptr<TypeInfo>(nullptr);
	if (isPointer) {
		std::vector<int> shape;
		for (auto p: ctx->constExp()) {
			p->accept(this);
			auto len = retVal.restore<std::unique_ptr<StaticValue>>();
			shape.emplace_back(
				com::dynamic_cast_uPtr_get<IntStaticValue>(len)->value
			);
		}
		auto uPtrTypePointTo = bTypeToTypeInfoUPtr(info.var.btype, shape);
		uPtrTypeInfo =
			std::make_unique<PointerType>(
				PointerType(*uPtrTypePointTo));
	} else {
		uPtrTypeInfo = bTypeToTypeInfoUPtr(info.var.btype);
	}
	ircode::AddrPara * pAddrPara =
		ir.addrPool.emplace_back(ircode::AddrPara(*uPtrTypeInfo, varname));
	retVal.save( // some exception... but not important.
		std::move(pAddrPara));/*NOLINT*/
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitFuncRParams(SysYParser::FuncRParamsContext * ctx) {
	// funcRParams -> funcRParam (',' funcRParam)*
	auto instrsRes = std::list<ircode::IRInstr *>();
	auto params = std::vector<ircode::AddrOperand *>();
	com::Assert(
		info.var.pUsingFunc, "funcAddr should not be nullptr when getting rParams.",
		CODEPOS
	);
	com::Assert(
		int(ctx->funcRParam().size()) == info.var.pUsingFunc->getNumberOfParameter(),
		"Number of parameter should match!", CODEPOS
	);
	int nParams = info.var.pUsingFunc->getNumberOfParameter();
	for (int i = 0; i < nParams; ++i) {
		ctx->funcRParam(i)->accept(this);
		auto * pRParaAddr = retVal.restore<ircode::AddrOperand *>();
		auto instrs = retInstrs.restore<std::list<ircode::IRInstr *>>();
		com::addRuntimeWarning(
			"Pointer as param?", CODEPOS, com::addWarningOnlyOnce
		);
		auto [pConvertedRParaAddr, upTypeInfo, convertedInstrs] = genAddrConversion(
			ir, pRParaAddr, info.var.pUsingFunc->getNumberThParameterTypeInfo(i)
		);
		instrs.splice(instrs.end(), std::move(convertedInstrs));
		params.emplace_back(pConvertedRParaAddr);
		instrsRes.splice(instrsRes.end(), std::move(instrs));
	}
	retVal.save(std::move(params));
	retInstrs.save(std::move(instrsRes));
	return nullptr;
}

antlrcpp::Any ASTVisitor::visitExpAsRParam(SysYParser::ExpAsRParamContext * ctx) {
	// funcRParam -> exp # expAsRParam
	return ctx->exp()->accept(this);
}

antlrcpp::Any
ASTVisitor::visitStringAsRParam(SysYParser::StringAsRParamContext * ctx) {
	// funcRParam -> STRING # stringAsRParam
	com::TODO("", CODEPOS);
}


}