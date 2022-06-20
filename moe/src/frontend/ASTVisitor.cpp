#ifndef IGNORE_WHEN_TESTING

#include "ASTVisitor.hpp"


namespace frontend {

ASTVisitor::ASTVisitor() : addrPool(), info(), retVal() {
	pScopeNow = addrPool.getRootScope();
}

std::any ASTVisitor::visitChildren(antlr4::tree::ParseTree * node) {
	for (auto son : node->children) {
		son->accept(this);
	}
	return nullptr;
}

std::any ASTVisitor::visitCompUnit(SysYParser::CompUnitContext * ctx) {
	return visitChildren(ctx);
}

std::any ASTVisitor::visitDecl(SysYParser::DeclContext * ctx) {
	//  Decl -> ConstDecl | VarDecl
	return visitChildren(ctx);
}

std::any ASTVisitor::visitConstDecl(SysYParser::ConstDeclContext * ctx) {
	//  ConstDecl -> "const" BType ConstDef {"," ConstDef};
	setWithAutoRestorer(info.isConst, true);
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.btype, retVal.restore<BType>());
	for (auto son : ctx->constDef()) {
		son->accept(this);
	}
	return nullptr;
}

std::any ASTVisitor::visitConstDef(SysYParser::ConstDefContext * ctx) {
	//  ConstDef -> Ident {"[" ConstExp "]"}} "=" ConstInitVal
	std::string varname = ctx->Identifier()->getText();
	if (!ctx->constExp().empty()) {   //    Defining an array.
		std::vector<int> shape;
		for (auto son : ctx->constExp()) {
			son->accept(this);
			auto len = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
			shape.push_back(
				com::dynamic_cast_uPtr_get<ircode::IntStaticValue>(len)->value
			);
		}
		setWithAutoRestorer(info.shape, shape);
		ctx->constInitVal()->accept(this);
		auto uType = bTypeToTypeInfoUPtr(info.btype, info.shape);
		auto constVal = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ircode::Addr * pAddr = nullptr;
		if (info.inGlobal) {
			pAddr = addrPool.addAddrToScope(
				ircode::AddrGlobalVariable(*uType, varname, *constVal),
				pScopeNow, ircode::IdType::GlobalVarName, varname
			);
			instrPool.addInstrToPool(
				ircode::instr::DeclGlobal(
					dynamic_cast<ircode::AddrGlobalVariable *>(pAddr)
				));
		} else {
			com::TODO("", CODEPOS);
		}
		return nullptr;
	} else { // Defining a variable
		ctx->constInitVal()->accept(this);
		std::unique_ptr<ircode::TypeInfo> uType = bTypeToTypeInfoUPtr(info.btype);
		auto constVal = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		constVal = constVal->convertTo(*uType);
		ircode::Addr * pAddr = nullptr;
		if (info.inGlobal) {
			pAddr = addrPool.addAddrToScope(
				ircode::AddrGlobalVariable(*uType, varname, *constVal),
				pScopeNow, ircode::IdType::GlobalVarName, varname
			);
			instrPool.addInstrToPool(
				ircode::instr::DeclGlobal(
					dynamic_cast<ircode::AddrGlobalVariable *>(pAddr)));
		} else {
			pAddr = addrPool.addAddrToScope(
				ircode::AddrVariable(*uType, varname, *constVal),
				pScopeNow, ircode::IdType::GlobalVarName, varname
			);
			com::TODO("", CODEPOS);
		}
		return nullptr;
	}
}

std::any ASTVisitor::visitConstExp(SysYParser::ConstExpContext * ctx) {
	//  ConstExp -> AddExp
	setWithAutoRestorer(info.visitingConst, true);
	ctx->addExp()->accept(this);
	auto ret = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
	retVal.save(std::move(ret));
	return nullptr;
}

std::any ASTVisitor::visitFuncDef(SysYParser::FuncDefContext * ctx) {
	setWithAutoRestorer(info.inGlobal, false);
	setWithAutoRestorer(info.funcType, strToFuncType(ctx->funcType()->getText()));
	auto varname = ctx->Identifier()->getText();
	auto pScopeSon = pScopeNow->addSonScope();
	setWithAutoRestorer(pScopeNow, pScopeSon);
	ctx->funcFParams()->accept(this);
	com::TODO("Get the funcFParams.", CODEPOS);
	ctx->block()->accept(this);
	return nullptr;
}

std::any ASTVisitor::visitVarDecl(SysYParser::VarDeclContext * ctx) {
	// varDecl -> bType varDef (',' varDef)* ';'
	setWithAutoRestorer(info.isConst, false);
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.btype, retVal.restore<BType>());
	for (auto son : ctx->varDef()) {
		son->accept(this);
	}
	return nullptr;
}

std::any ASTVisitor::visitBType(SysYParser::BTypeContext * ctx) {
	retVal.save<BType>(strToBType(ctx->getText()));
	return nullptr;
}

std::any ASTVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext * ctx) {
	// constInitVal -> constExp # scalarConstInitVal
	return visitChildren(ctx);
}

std::any ASTVisitor::visitListInitval(SysYParser::ListInitvalContext * ctx) {
	if (info.inGlobal) { //  Just same as visitListConstInitVal...
		//  Copy from visitListConstInitVal.
		setWithAutoRestorer(info.visitingConst, true);
		com::Assert(
			!info.shape.empty(),
			"When visitListInitVal in global, shape should not be empty", CODEPOS
		);
		auto nowShape = info.shape;
		int len = nowShape[0];
		setWithAutoRestorer(info.shape,
			std::vector(info.shape.begin() + 1, info.shape.end()));
		if (nowShape.size() == 1) {
			switch (info.btype) {
				case BType::Float: {
					std::vector<ircode::FloatStaticValue> arr;
					for (auto p : ctx->initVal()) {
						p->accept(this);
						auto psv = com::dynamic_cast_uPtr<ircode::FloatStaticValue>(
							retVal.restore<std::unique_ptr<ircode::StaticValue>>()
								->convertTo(ircode::FloatType())
						);
						arr.emplace_back(*psv);
					}
					retVal.save(
						com::dynamic_cast_uPtr<ircode::StaticValue>(
							std::make_unique<ircode::FloatArrayStaticValue>(len, arr)
						));
					break;
				}
				case BType::Int: {
					std::vector<ircode::IntStaticValue> arr;
					for (auto p : ctx->initVal()) {
						p->accept(this);
						auto psv = com::dynamic_cast_uPtr<ircode::IntStaticValue>(
							retVal.restore<std::unique_ptr<ircode::StaticValue>>()
								->convertTo(ircode::IntType())
						);
						arr.emplace_back(*psv);
					}
					retVal.save(
						com::dynamic_cast_uPtr<ircode::StaticValue>(
							std::make_unique<ircode::IntArrayStaticValue>(len, arr)
						));
					break;
				}
				default: {
					com::Throw("Error BType.", CODEPOS);
				}
			}
		} else {
			switch (info.btype) {
				case BType::Float: {
					std::vector<ircode::FloatArrayStaticValue> arrOfArr;
					auto sons = ctx->initVal();
					auto itP = sons.begin();
					//  Generate {...} arrays first.
					for (; itP != sons.end(); ++itP) {
						auto p = *itP;
						if (dynamic_cast<SysYParser::ScalarInitValContext *>(p)) {
							break;
						}
						p->accept(this);
						auto psv =
							com::dynamic_cast_uPtr<ircode::FloatArrayStaticValue>(
								retVal.restore<
									std::unique_ptr<ircode::StaticValue>>());
						arrOfArr.emplace_back(*psv);
					}
					//  Generate blank value.
					std::vector<ircode::FloatStaticValue> rest;
					for (; itP != sons.end(); ++itP) {
						auto p = *itP;
						com::Assert(
							dynamic_cast<SysYParser::ScalarInitValContext *>(p),
							"p should be scalar initval at this time.", CODEPOS
						);
						p->accept(this);
						auto psv = com::dynamic_cast_uPtr<ircode::FloatStaticValue>(
							retVal.restore<std::unique_ptr<ircode::StaticValue>>()
						);
						rest.emplace_back(*psv);
					}
					retVal.save(
						com::dynamic_cast_uPtr<ircode::StaticValue>(
							std::make_unique<ircode::FloatArrayStaticValue>(
								len, info.shape, arrOfArr, rest
							)
						));
					break;
				}
				case BType::Int: {
					std::vector<ircode::IntArrayStaticValue> arrOfArr;
					auto sons = ctx->initVal();
					auto itP = sons.begin();
					//  Generate {...} arrays first.
					for (; itP != sons.end(); ++itP) {
						auto p = *itP;
						if (dynamic_cast<SysYParser::ScalarInitValContext *>(p)) {
							//  p is scalarConstInitVal
							break;
						}
						p->accept(this);
						auto psv =
							com::dynamic_cast_uPtr<ircode::IntArrayStaticValue>(
								retVal.restore<
									std::unique_ptr<ircode::StaticValue>>());
						arrOfArr.emplace_back(*psv);
					}
					//  Generate blank value.
					std::vector<ircode::IntStaticValue> rest;
					for (; itP != sons.end(); ++itP) {
						auto p = *itP;
						com::Assert(
							dynamic_cast<SysYParser::ScalarInitValContext *>(p),
							"p should be scalar const initval at this time.", CODEPOS
						);
						p->accept(this);
						auto psv = com::dynamic_cast_uPtr<ircode::IntStaticValue>(
							retVal.restore<std::unique_ptr<ircode::StaticValue>>()
						);
						rest.emplace_back(*psv);
					}
					retVal.save(
						com::dynamic_cast_uPtr<ircode::StaticValue>(
							std::make_unique<ircode::IntArrayStaticValue>(
								len, info.shape, arrOfArr, rest
							)
						));
					break;
				}
				default: {
					com::Throw("Error BType!", CODEPOS);
				}
			}
		}
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitFuncType(SysYParser::FuncTypeContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitInitVarDef(SysYParser::InitVarDefContext * ctx) {
	// varDef -> Identifier ('[' constExp ']')* '=' initVal # initVarDef
	std::string varname = ctx->Identifier()->getText();
	if (info.inGlobal) {
		std::vector<int> shape;
		for (auto p : ctx->constExp()) {
			p->accept(this);
			auto pSV = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
			shape.push_back(
				com::dynamic_cast_uPtr_get<ircode::IntStaticValue>(pSV)->value
			);
		}
		setWithAutoRestorer(info.shape, shape);
		setWithAutoRestorer(info.visitingConst, true);
		ctx->initVal()->accept(this);
		auto uType = bTypeToTypeInfoUPtr(info.btype, info.shape);
		auto constVal = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		auto pAddr = addrPool.addAddrToScope(
			ircode::AddrGlobalVariable(*uType, varname, *constVal, false),
			pScopeNow, ircode::IdType::GlobalVarName, varname
		);
		instrPool.addInstrToPool(
			ircode::instr::DeclGlobal(
				dynamic_cast<ircode::AddrGlobalVariable *>(pAddr)
			));
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext * ctx) {
	//  varDef -> Identifier ('[' constExp ']')* # uninitVarDef
	std::string varname = ctx->Identifier()->getText();
	std::vector<int> shape;
	for (auto p : ctx->constExp()) {
		p->accept(this);
		auto psv = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		shape.push_back(
			com::dynamic_cast_uPtr_get<ircode::IntStaticValue>(psv)->value
		);
	}
	if (info.inGlobal) {
		auto uType = bTypeToTypeInfoUPtr(info.btype, info.shape);
		auto pAddr = addrPool.addAddrToScope(
			ircode::AddrGlobalVariable(
				*uType,
				varname
			),
			pScopeNow, ircode::IdType::GlobalVarName, varname
		);
		instrPool.addInstrToPool(
			ircode::instr::DeclGlobal(
				dynamic_cast<ircode::AddrGlobalVariable *>(pAddr)
			));
	} else {
		com::TODO("", CODEPOS);
	}
	return nullptr;
}

std::any ASTVisitor::visitFuncFParams(SysYParser::FuncFParamsContext * ctx) {
	// funcFParams -> funcFParam (',' funcFParam)*;
	/**
	 * @brief The move constructor of std::vector will just move pointer to memory.
	 * @ref https://stackoverflow.com/a/53879096/17924585
	 */
	std::vector<ircode::AddrPara> vecPara;
	for (auto p : ctx->funcFParam()) {
		p->accept(this);
		vecPara.emplace_back(retVal.restore<ircode::AddrPara>());
	}
	retVal.save(std::move(vecPara));
	return nullptr;
}

std::any
ASTVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext * ctx) {
	// constInitVal -> '{' (constInitVal (',' constInitVal)* )? '}' # listConstInitVal
	//  Copy to visitListInitVal. If you change code here, you should change code there.
	com::Assert(
		!info.shape.empty(),
		"When visitListConstInitVal, shape should not be empty", CODEPOS
	);
	auto nowShape = info.shape;
	int len = nowShape[0];
	setWithAutoRestorer(info.shape,
		std::vector(info.shape.begin() + 1, info.shape.end()));
	if (nowShape.size() == 1) {
		switch (info.btype) {
			case BType::Float: {
				std::vector<ircode::FloatStaticValue> arr;
				for (auto p : ctx->constInitVal()) {
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::FloatStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>()
							->convertTo(ircode::FloatType())
					);
					arr.emplace_back(*psv);
				}
				retVal.save(
					com::dynamic_cast_uPtr<ircode::StaticValue>(
						std::make_unique<ircode::FloatArrayStaticValue>(len, arr)
					));
				break;
			}
			case BType::Int: {
				std::vector<ircode::IntStaticValue> arr;
				for (auto p : ctx->constInitVal()) {
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::IntStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>()
							->convertTo(ircode::IntType())
					);
					arr.emplace_back(*psv);
				}
				retVal.save(
					com::dynamic_cast_uPtr<ircode::StaticValue>(
						std::make_unique<ircode::IntArrayStaticValue>(len, arr)
					));
				break;
			}
			default: {
				com::Throw("Error BType.", CODEPOS);
			}
		}
	} else {
		switch (info.btype) {
			case BType::Float: {
				std::vector<ircode::FloatArrayStaticValue> arrOfArr;
				auto sons = ctx->constInitVal();
				auto itP = sons.begin();
				//  Generate {...} arrays first.
				for (; itP != sons.end(); ++itP) {
					auto p = *itP;
					if (dynamic_cast<SysYParser::ScalarConstInitValContext *>(p)) {
						//  p is scalarConstInitVal
						break;
					}
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::FloatArrayStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>());
					arrOfArr.emplace_back(*psv);
				}
				//  Generate blank value.
				std::vector<ircode::FloatStaticValue> rest;
				for (; itP != sons.end(); ++itP) {
					auto p = *itP;
					com::Assert(
						dynamic_cast<SysYParser::ScalarConstInitValContext *>(p),
						"p should be scalar const initval at this time.", CODEPOS
					);
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::FloatStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>()
					);
					rest.emplace_back(*psv);
				}
				retVal.save(
					com::dynamic_cast_uPtr<ircode::StaticValue>(
						std::make_unique<ircode::FloatArrayStaticValue>(
							len, info.shape, arrOfArr, rest
						)
					));
				break;
			}
			case BType::Int: {
				std::vector<ircode::IntArrayStaticValue> arrOfArr;
				auto sons = ctx->constInitVal();
				auto itP = sons.begin();
				//  Generate {...} arrays first.
				for (; itP != sons.end(); ++itP) {
					auto p = *itP;
					if (dynamic_cast<SysYParser::ScalarConstInitValContext *>(p)) {
						//  p is scalarConstInitVal
						break;
					}
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::IntArrayStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>());
					arrOfArr.emplace_back(*psv);
				}
				//  Generate blank value.
				std::vector<ircode::IntStaticValue> rest;
				for (; itP != sons.end(); ++itP) {
					auto p = *itP;
					com::Assert(
						dynamic_cast<SysYParser::ScalarConstInitValContext *>(p),
						"p should be scalar const initval at this time.", CODEPOS
					);
					p->accept(this);
					auto psv = com::dynamic_cast_uPtr<ircode::IntStaticValue>(
						retVal.restore<std::unique_ptr<ircode::StaticValue>>()
					);
					rest.emplace_back(*psv);
				}
				retVal.save(
					com::dynamic_cast_uPtr<ircode::StaticValue>(
						std::make_unique<ircode::IntArrayStaticValue>(
							len, info.shape, arrOfArr, rest
						)
					));
				break;
			}
			default: {
				com::Throw("Error BType!", CODEPOS);
			}
		}
	}
	return nullptr;
}

std::any
ASTVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext * ctx) {
	// constInitVal -> constExp # scalarConstInitVal
	return visitChildren(ctx);
}

std::any ASTVisitor::visitFuncFParam(SysYParser::FuncFParamContext * ctx) {
	ctx->bType()->accept(this);
	setWithAutoRestorer(info.btype, retVal.restore<BType>());
	auto varname = ctx->Identifier()->getText();
	bool isPointer = !ctx->Lbrkt().empty();
	std::vector<int> shape;
	for (auto p : ctx->constExp()) {
		p->accept(this);
		auto len = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		shape.emplace_back(
			com::dynamic_cast_uPtr_get<ircode::IntStaticValue>(len)->value
		);
	}
	com::TODO("Decide the type of function, create AddrPara.", CODEPOS);
}

std::any ASTVisitor::visitBlock(SysYParser::BlockContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBlockItem(SysYParser::BlockItemContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitAssignment(SysYParser::AssignmentContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExpStmt(SysYParser::ExpStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBlockStmt(SysYParser::BlockStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitIfStmt1(SysYParser::IfStmt1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitIfStmt2(SysYParser::IfStmt2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitWhileStmt(SysYParser::WhileStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitBreakStmt(SysYParser::BreakStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitContinueStmt(SysYParser::ContinueStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitReturnStmt(SysYParser::ReturnStmtContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExp(SysYParser::ExpContext * ctx) {
	// exp -> addExp
	return visitChildren(ctx);
}

std::any ASTVisitor::visitCond(SysYParser::CondContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLVal(SysYParser::LValContext * ctx) {
	// lVal -> Identifier ('[' exp ']')*
	std::string varname = ctx->Identifier()->getText();
	if (info.visitingConst) {
		std::vector<int> idxs;
		for (auto p : ctx->exp()) {
			p->accept(this);
			auto idx = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
			switch (idx->getTypeInfo().type) {
				case ircode::TypeInfo::Type::Int_t: {
					idxs.push_back(
						com::dynamic_cast_uPtr_get<ircode::IntStaticValue>(
							idx
						)->value
					);
					break;
				}
				case ircode::TypeInfo::Type::Bool_t: {
					idxs.push_back(
						com::dynamic_cast_uPtr_get<ircode::BoolStaticValue>(
							idx
						)->value
					);
					break;
				}
				default: {
					com::Throw("Const value of index should be int/bool.", CODEPOS);
				}
			}
		}
		auto [idType, pAddr] = pScopeNow->findIdInThisScope(varname);
		com::Assert(
			pAddr, "Get nullptr from scope. varname=[" + varname + "].",
			CODEPOS
		);
		switch (idType) {
			case ircode::IdType::GlobalVarName: {
				auto globalVarAddr =
					dynamic_cast<ircode::AddrGlobalVariable *>(pAddr);
				retVal.save(globalVarAddr->uPtrStaticValue->getValue(idxs));
				break;
			}
			case ircode::IdType::LocalVarName: {
				auto localVarAddr = dynamic_cast<ircode::AddrVariable *>(pAddr);
				retVal.save(localVarAddr->uPtrStaticValue->getValue(idxs));
				break;
			}
			default: {
				com::Throw("Error idType!", CODEPOS);
			}
		}
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context * ctx) {
	// primaryExp -> '(' exp ')' # primaryExp1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context * ctx) {
	// primaryExp -> lVal # primaryExp2
	return visitChildren(ctx);
}

std::any ASTVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context * ctx) {
	// primaryExp -> number # primaryExp3
	return visitChildren(ctx);
}

std::any ASTVisitor::visitNumber(SysYParser::NumberContext * ctx) {
	// number -> IntLiteral | FloatLiteral
	if (ctx->FloatLiteral()) {
		std::unique_ptr<ircode::StaticValue> v =
			std::make_unique<ircode::FloatStaticValue>(ctx->getText());
		retVal.save(std::move(v));
		return nullptr;
	} else if (ctx->IntLiteral()) {
		std::unique_ptr<ircode::StaticValue> v =
			std::make_unique<ircode::IntStaticValue>(ctx->getText());
		retVal.save(std::move(v));
		return nullptr;
	} else {
		com::Throw("ctx should be float/int literal.", CODEPOS);
	}
}

std::any ASTVisitor::visitUnary1(SysYParser::Unary1Context * ctx) {
	// unaryExp -> primaryExp # unary1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitUnary2(SysYParser::Unary2Context * ctx) {
	// unaryExp -> Identifier '(' (funcRParams)? ')' # unary2
	if (info.visitingConst) {
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
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitUnary3(SysYParser::Unary3Context * ctx) {
	// unaryExp -> unaryOp unaryExp # unary3
	ctx->unaryOp()->accept(this);
	auto op = retVal.restore<std::string>();
	if (info.visitingConst) {
		ctx->unaryExp()->accept(this);
		auto p = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		retVal.save(p->calc(op));
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitUnaryOp(SysYParser::UnaryOpContext * ctx) {
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

std::any ASTVisitor::visitFuncRParams(SysYParser::FuncRParamsContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitExpAsRParam(SysYParser::ExpAsRParamContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitStringAsRParam(SysYParser::StringAsRParamContext * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitMul2(SysYParser::Mul2Context * ctx) {
	// mulExp -> mulExp ('*' | '/' | '%') unaryExp # mul2
	if (info.visitingConst) {
		ctx->mulExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ctx->unaryExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		std::string op;
		if (ctx->Division()) {
			op = "/";
		} else if (ctx->Modulo()) {
			op = "%";
		} else if (ctx->Multiplication()) {
			op = "*";
		} else {
			com::Throw("ctx should contain '/' or '%' or '*'.", CODEPOS);
		}
		retVal.save(resL->calc(*resR, op));
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitMul1(SysYParser::Mul1Context * ctx) {
	// mulExp -> unaryExp # mul1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitAdd2(SysYParser::Add2Context * ctx) {
	//  addExp -> addExp ('+'|'-') mulExp # add 2
	if (info.visitingConst) {
		ctx->addExp()->accept(this);
		auto resL = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		ctx->mulExp()->accept(this);
		auto resR = retVal.restore<std::unique_ptr<ircode::StaticValue>>();
		if (ctx->Minus()) {
			retVal.save(resL->calc(*resR, "-"));
		} else if (ctx->Addition()) {
			retVal.save(resL->calc(*resR, "+"));
		} else {
			com::Throw("ctx should contain '+' or '-'.", CODEPOS);
		}
		return nullptr;
	} else {
		com::TODO("", CODEPOS);
	}
}

std::any ASTVisitor::visitAdd1(SysYParser::Add1Context * ctx) {
	//  addExp -> mulExp # add1
	return visitChildren(ctx);
}

std::any ASTVisitor::visitRel2(SysYParser::Rel2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitRel1(SysYParser::Rel1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitEq1(SysYParser::Eq1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitEq2(SysYParser::Eq2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLAnd2(SysYParser::LAnd2Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLAnd1(SysYParser::LAnd1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLOr1(SysYParser::LOr1Context * ctx) {
	com::TODO("", CODEPOS);
}

std::any ASTVisitor::visitLOr2(SysYParser::LOr2Context * ctx) {
	com::TODO("", CODEPOS);
}


}
#endif