//
// Created by lee on 6/21/22.
//

#include "Module.hpp"
#include "Addr.hpp"


namespace mir {

AddrPool::AddrPool() : pool() {
}

std::string AddrPool::toLLVMIR() const {
	std::string res;
	for (auto & pAddr: globalVars) {
		res += pAddr->toDeclIR() + "\n";
	}
	return res + "\n";
}

const std::vector<AddrGlobalVariable *> & AddrPool::getGlobalVars() const {
	return globalVars;
}

std::string Module::toLLVMIR() const {
	auto res = SysY::llvmHeader + addrPool.toLLVMIR();
	for (auto * funcDecl: funcPool) {
		res += funcDecl->toLLVMIR();
	}
	res += llvmSyFunctionAttr;
	res += llvmSyLibFunctionAttr;
	return res;
}

void Module::finishLoading() {
	for (auto & func: funcPool) {
		func->finishLoading(*this);
	}
}

std::vector<AddrFunction *> Module::generateSysYDecl() {
	auto sysyFuncs = std::vector<AddrFunction *>();
	if (!sysyFuncAdded) {
		//  int getint()
		sysyFuncs.emplace_back(
			addrPool.emplace_back(
				AddrFunction("getint", sup::IntType())
			)
		);
		//  int getch()
		sysyFuncs.emplace_back(
			addrPool.emplace_back(
				AddrFunction("getch", sup::IntType())
			)
		);
		//  int getarray(int [])
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::PointerType(sup::IntType()), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("getarray", std::move(arg), sup::IntType())
				)
			);
		}
		//  float getfloat()
		sysyFuncs.emplace_back(
			addrPool.emplace_back(
				AddrFunction("getfloat", sup::FloatType())
			)
		);
		//  int getfarray(float [])
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::PointerType(sup::FloatType()), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("getfarray", std::move(arg), sup::IntType())
				)
			);
		}
		//  void putint(int a)
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("putint", std::move(arg))
				)
			);
		}
		//  void putch(int a)
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("putch", std::move(arg))
				)
			);
		}
		//  void putarray(int n,int a[])
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "n")
				)
			);
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::PointerType(sup::IntType()), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("putarray", std::move(arg))
				)
			);
		}
		//  void putfloat(float a)
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::FloatType(), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("putfloat", std::move(arg))
				)
			);
		}
		//  void putfarray(int n,float a[])
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "n")
				)
			);
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::PointerType(sup::FloatType()), "a")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("putfarray", std::move(arg))
				)
			);
		}
		//  _sysy_starttime(int lineno)
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "lineno")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("_sysy_starttime", std::move(arg))
				)
			);
		}
		//  _sysy_stoptime(int lineno)
		{
			auto arg = std::vector<AddrPara *>();
			arg.emplace_back(
				addrPool.emplace_back(
					AddrPara(sup::IntType(), "lineno")
				)
			);
			sysyFuncs.emplace_back(
				addrPool.emplace_back(
					AddrFunction("_sysy_stoptime", std::move(arg))
				)
			);
		}
		for (auto * p: sysyFuncs) {
			p->justDeclare = true;
			funcPool.emplace_back(FuncDef(p));
		}
		sysyFuncAdded = true;
	}
	return sysyFuncs;
}

void InstrPool::printAll(std::ostream & os) const {
	for (const auto & p: pool) {
		os << p->toLLVMIR() << std::endl;
	}
}

InstrPool::InstrPool() : pool() {
}

FuncBlock::FuncBlock() : instrs() {
}

std::string FuncBlock::toLLVMIR() const {
	com::TODO("", CODEPOS);
}


FuncDef::FuncDef(AddrFunction * pAddrFun)
	: pAddrFun(pAddrFun) {
}

std::string FuncDef::toLLVMIR() const {
	if (pAddrFun->justDeclare) {
		return pAddrFun->declLLVMIR() + "\n\n";
	}
	std::string res =
		pAddrFun->declLLVMIR() +
			"{\n";
	if (!loadFinished) {
		com::Throw(
			"You should call finishLoading before call other functions.", CODEPOS
		);
	} else {
		if (blocks.empty()) {
			for (const auto & pInstr: instrs) {
				res += pInstr->toLLVMIR() + "\n";
			}
		} else {
			for (const auto * block: blocks) { res += block->toLLVMIR() + "\n"; }
		}
	}
	res += "}\n\n";
	return res;
}

void FuncDef::finishLoading(Module & ir) {
	if (loadFinished) {
		com::Throw("Method `finishLoading` can be called only once.", CODEPOS);
	}
	rearrangeAlloca();
	addEntryLabelInstr(ir);
	loadFinished = true;
}

void FuncDef::rearrangeAlloca() {
	STLPro::list::move_all_to_front(
		instrs, [](mir::Instr * p) -> bool {
			return dynamic_cast<mir::InstrAlloca *>(p) != nullptr;
		}
	);
}

Instr * FuncDef::emplace_back(Instr * irInstr) {
	if (irInstr) { instrs.emplace_back(irInstr); }
	return irInstr;
}

AddrJumpLabel * FuncDef::addEntryLabelInstr(Module & ir) {
	auto pAddrLabel = ir.addrPool.emplace_back(mir::AddrJumpLabel("Entry"));
	instrs.emplace_front(
		ir.instrPool.emplace_back(mir::InstrLabel(pAddrLabel))
	);
	return pAddrLabel;
}

void FuncDef::emplace_back(std::list<Instr *> && appendList) {
	appendList.remove_if([](Instr * p) { return p == nullptr; });
	instrs.splice(instrs.end(), std::move(appendList));
}

FuncBlock * FuncDef::emplace_back(FuncBlock && irFuncBlock) {
	pool.emplace_back(std::make_unique<FuncBlock>(std::move(irFuncBlock)));
	return pool.rbegin()->get();
}

FuncDef * FuncDefPool::emplace_back(FuncDef && funcDef) {
	pool.emplace_back(std::make_unique<FuncDef>(std::move(funcDef)));
	funcDefs.emplace_back(pool.rbegin()->get());
	return pool.rbegin()->get();
}

}

using namespace mir;
namespace sup {

std::list<mir::Instr *> genBinaryOperationInstrs(
	mir::Module & ir, AddrOperand * opL, const std::string & op,
	AddrOperand * opR, mir::AddrVariable * opD
) {
	com::Assert(
		opL->getType() == opR->getType(), "Type of Operands should be same!", CODEPOS
	);
	auto typeLR = opL->getType().type;
	auto typeD = opD->getType().type;
	com::Assert(
		com::enum_fun::in(typeLR, {Type::Int_t, Type::Float_t, Type::Bool_t}),
		"Type of operands should be int/float/bool", CODEPOS
	);
	com::Assert(
		com::enum_fun::in(typeD, {Type::Int_t, Type::Float_t, Type::Bool_t}),
		"Type of dest should be int/float/bool", CODEPOS
	);
	com::Assert(
		typeD == Type::Bool_t || opL->getType() == opD->getType(),
		"If type of dest is not bool, type of operands should be same as dest.",
		CODEPOS
	);
	auto instrsRes = std::list<mir::Instr *>();
	switch (typeD) {
		case Type::Int_t: {
			if (op == "+") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrAdd(opL, opR, opD)
					)
				);
			} else if (op == "-") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrSub(opL, opR, opD)
					)
				);
			} else if (op == "*") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrMul(opL, opR, opD)
					)
				);
			} else if (op == "/") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrSDiv(opL, opR, opD)
					)
				);
			} else if (op == "%") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrSrem(opL, opR, opD)
					)
				);
			} else {
				com::Throw("Op should be one in {+,-,*,/,%}.", CODEPOS);
			}
			break;
		}
		case Type::Float_t: {
			if (op == "+") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFAdd(opL, opR, opD)
					)
				);
			} else if (op == "-") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFSub(opL, opR, opD)
					)
				);
			} else if (op == "*") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFMul(opL, opR, opD)
					)
				);
			} else if (op == "/") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFDiv(opL, opR, opD)
					)
				);
			} else {
				com::Throw("Op should be one in {+,-,*,/}.", CODEPOS);
			}
			break;
		}
		case Type::Bool_t: {
			switch (typeLR) {
				case Type::Int_t: {
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrICmp(opD, opL, strToICMP(op), opR)
						)
					);
					break;
				}
				case Type::Float_t: {
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrFCmp(opD, opL, strToFCMP(op), opR)
						)
					);
					break;
				}
				case Type::Bool_t: {
					com::Throw(
						"In C, comparison between bool is converted to comparison between int.",
						CODEPOS
					);
					break;
				}
				default:com::Throw("??", CODEPOS);
			}
			break;
		}
		default:
			com::Throw(
				"typeD of operand and result should be in {int float bool}.", CODEPOS
			);
	}
	return instrsRes;
}

std::list<mir::Instr *> genUnaryOperationInstrs(
	Module & ir, const std::string & op,
	AddrOperand * opR, mir::AddrVariable * opD
) {
	auto typeD = opD->getType().type;
	auto typeR = opR->getType().type;
	switch (typeD) {
		case Type::Bool_t: {
			auto instrsRes = std::list<mir::Instr *>();
			if (op == "!") {
				auto * pZero = ir.addrPool.emplace_back(
					mir::AddrStaticValue(opR->getType())
				);
				switch (opR->getType().type) {
					case Type::Bool_t:
					case Type::Int_t: {
						instrsRes.emplace_back(
							ir.instrPool.emplace_back(
								mir::InstrICmp(opD, opR, ICMP::EQ, pZero)
							)
						);
						break;
					}
					case Type::Float_t: {
						instrsRes.emplace_back(
							ir.instrPool.emplace_back(
								mir::InstrFCmp(opD, opR, FCMP::OEQ, pZero)
							)
						);
						break;
					}
					default: com::Throw("", CODEPOS);
				}
			} else {
				com::Throw("Unsupported OP", CODEPOS);
			}
			return instrsRes;
		}
		case Type::Int_t: {
			com::Assert(com::enum_fun::in(typeR, {Type::Int_t, Type::Bool_t}));
			auto [newOpR, _, instrs] = genAddrConversion(ir, opR, IntType());
			auto * pZeroAddr = ir.addrPool.emplace_back(
				mir::AddrStaticValue(IntStaticValue(0))
			);
			if (op == "+") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrAdd(pZeroAddr, newOpR, opD)
					)
				);
			} else if (op == "-") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrSub(pZeroAddr, newOpR, opD)
					)
				);
			} else {
				com::Throw("Unsupported op.", CODEPOS);
			}
			return instrs;
		}
		case Type::Float_t: {
			com::Assert(typeR == Type::Float_t, "", CODEPOS);
			auto * pZeroAddr = ir.addrPool.emplace_back(
				mir::AddrStaticValue(FloatStaticValue(0))
			);
			auto instrs = std::list<mir::Instr *>();
			if (op == "+") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFAdd(pZeroAddr, opR, opD)
					)
				);
			} else if (op == "-") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						mir::InstrFSub(pZeroAddr, opR, opD)
					)
				);
			} else {
				com::Throw("Unsupported op.", CODEPOS);
			}
			return instrs;
		}
		default: {
			com::Throw("Unsupported Type!", CODEPOS);
		}
	}
}

std::tuple<
	mir::AddrOperand *, mir::AddrOperand *, std::unique_ptr<TypeInfo>,
	std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir,
	AddrOperand * preOpL, AddrOperand * preOpR
) {
	auto error = false;
	auto typePreL = preOpL->getType().type, typePreR = preOpR->getType().type;
	auto typeNewL = typePreL, typeNewR = typePreR;
	switch (typePreL) {
		case Type::Int_t: {
			switch (typePreR) {
				case Type::Int_t: {
					// nothing to do
					break;
				}
				case Type::Float_t: {
					// opL is int but opR is float, convert opL to float.
					// opL : int -> float
					typeNewL = Type::Float_t;
					break;
				}
				case Type::Bool_t: {
					typeNewR = Type::Int_t;
					break;
				}
				default: {
					error = true;
					break;
				}
			}
			break;
		}
		case Type::Float_t: {
			switch (typePreR) {
				case Type::Int_t: {
					// opR is int but opL is float, convert opR to float.
					// opR : int -> float
					typeNewR = Type::Float_t;
					break;
				}
				case Type::Float_t: {
					// nothing to do
					break;
				}
				case Type::Bool_t: {
					typeNewR = Type::Float_t;
					break;
				}
				default: {
					error = true;
					break;
				}
			}
			break;
		}
		case Type::Bool_t: {
			switch (typePreR) {
				case Type::Bool_t: {
					typeNewL = Type::Int_t;
					typeNewR = Type::Int_t;
					break;
				}
				case Type::Int_t: {
					typeNewL = Type::Int_t;
					break;
				}
				case Type::Float_t: {
					typeNewL = Type::Float_t;
					break;
				}
				default: {
					error = true;
					break;
				}
			}
			break;
		}
		default: {
			error = true;
			break;
		}
	}
	if (error) {
		com::Throw("Error happened when conversion on binary operation.", CODEPOS);
	}
	auto [newOpL, _1, instrsL] = genAddrConversion(ir, preOpL, typeNewL);
	auto [newOpR, _2, instrsR] = genAddrConversion(ir, preOpR, typeNewR);
	auto instrsRes = std::list<mir::Instr *>();
	instrsRes.splice(instrsRes.end(), std::move(instrsL));
	instrsRes.splice(instrsRes.end(), std::move(instrsR));
	return {newOpL, newOpR, std::move(_1), std::move(instrsRes)};
}

std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir, mir::AddrOperand * preOp, sup::Type type
) {
	switch (type) {
		case Type::Bool_t: {
			return genAddrConversion(ir, preOp, BoolType());
		}
		case Type::Int_t: {
			return genAddrConversion(ir, preOp, IntType());
		}
		case Type::Float_t: {
			return genAddrConversion(ir, preOp, FloatType());
		}
		default: com::Throw("", CODEPOS);
	}
}

static std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> _genAddrConversion(
	mir::Module & ir,
	mir::AddrStaticValue * pSVAddr,
	const sup::TypeInfo & typeInfoD
) {
	auto newOp = static_cast<AddrOperand *>(nullptr);
	auto upResTypeInfo = std::unique_ptr<TypeInfo>();
	auto instrsRes = std::list<Instr *>();
	auto typeD = typeInfoD.type, typeOp = pSVAddr->getType().type;
	switch (typeD) {
		case Type::Int_t: {
			upResTypeInfo = std::make_unique<IntType>();
			break;
		}
		case Type::Float_t: {
			upResTypeInfo = std::make_unique<FloatType>();
			break;
		}
		case Type::Bool_t: {
			upResTypeInfo = std::make_unique<BoolType>();
			break;
		}
		case Type::Pointer_t: {
			newOp = pSVAddr;
			upResTypeInfo = com::dynamic_cast_uPtr<TypeInfo>(
				typeInfoD.cloneToUniquePtr()
			);
			break;
		}
		default: {
			com::Throw("Unsupported Type!", CODEPOS);
		}
	}
	if (!newOp) {
		newOp = ir.addrPool.emplace_back(
			mir::AddrStaticValue(
				convertOnSV(pSVAddr->getStaticValue(), *upResTypeInfo)
			)
		);
	}
	return {newOp, std::move(upResTypeInfo), std::move(instrsRes)};

}

static std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> _genAddrConversion(
	mir::Module & ir,
	mir::AddrVariable * pVarAddr,
	const sup::TypeInfo & typeInfoD
) {
	auto newOp = static_cast<AddrOperand *>(nullptr);
	auto upResTypeInfo = std::unique_ptr<TypeInfo>();
	auto instrsRes = std::list<Instr *>();
	auto typeD = typeInfoD.type, typeOp = pVarAddr->getType().type;
	switch (typeD) {
		case Type::Int_t: {
			upResTypeInfo = std::make_unique<IntType>();
			switch (typeOp) {
				case Type::Int_t: { // nothing to do
					newOp = pVarAddr;
					break;
				}
				case Type::Float_t: {
					// opD is int but opR is float, convert opR to int. float->int
					auto convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrFptosi(pVarAddr, convertedAddr)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Bool_t: {
					auto convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrZExt(pVarAddr, convertedAddr)
						)
					);
					newOp = convertedAddr;
					break;
				}
				default: { break; }
			}
			break;
		}
		case Type::Float_t: {
			upResTypeInfo = std::make_unique<FloatType>();
			switch (typeOp) {
				case Type::Int_t: {
					// opD is float but opR is int, convert opR to float. int->float
					auto convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrSitofp(pVarAddr, convertedAddr)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Float_t: {
					newOp = pVarAddr;
					break;
				}
				case Type::Bool_t: {
					auto midAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(IntType())
					);
					auto convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrZExt(pVarAddr, midAddr)
						)
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrSitofp(midAddr, convertedAddr)
						)
					);
					newOp = convertedAddr;
					break;
				}
				default: { break; }
			}
			break;
		}
		case Type::Pointer_t: {
			newOp = pVarAddr;
			upResTypeInfo = com::dynamic_cast_uPtr<TypeInfo>(
				typeInfoD.cloneToUniquePtr()
			);
			break;
		}
		case Type::Bool_t: {
			upResTypeInfo = std::make_unique<BoolType>();
			switch (typeOp) {
				case Type::Int_t: {
					auto * convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(BoolType())
					);
					auto * pZeroAddr = ir.addrPool.emplace_back(
						mir::AddrStaticValue(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrICmp(
								convertedAddr, pVarAddr, ICMP::NE, pZeroAddr
							)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Float_t: {
					auto * convertedAddr = ir.addrPool.emplace_back(
						mir::AddrVariable(BoolType())
					);
					auto * pZeroAddr = ir.addrPool.emplace_back(
						mir::AddrStaticValue(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							mir::InstrFCmp(
								convertedAddr, pVarAddr, FCMP::UNE, pZeroAddr
							)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Bool_t: {
					newOp = pVarAddr;
					break;
				}
				default :break;
			}
			break;
		}
		default: break;
	}
	com::Assert(newOp && upResTypeInfo, "??", CODEPOS);
	return {newOp, std::move(upResTypeInfo), std::move(instrsRes)};
}

std::tuple<
	mir::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<mir::Instr *>
> genAddrConversion(
	mir::Module & ir,
	mir::AddrOperand * preOp,
	const sup::TypeInfo & typeInfoD
) {
	auto * pSVAddr = dynamic_cast<AddrStaticValue *>(preOp);
	auto * pVarAddr = dynamic_cast<AddrVariable *>(preOp);
	if (pSVAddr) {
		return _genAddrConversion(ir, pSVAddr, typeInfoD);
	} else if (pVarAddr) {
		return _genAddrConversion(ir, pVarAddr, typeInfoD);
	} else {
		com::Throw("??", CODEPOS);
	}
}

std::list<mir::Instr *> genStoreInstrInFunction(
	mir::Module & ir, mir::AddrVariable * saveTo, mir::AddrVariable * from
) {
	com::Assert(
		saveTo->getType().type == Type::Pointer_t,
		"type of `saveTo` should be pointer.", CODEPOS
	);
	auto [newFrom, type, instrs] =
		genAddrConversion(
			ir, from, *dynamic_cast<const PointerType &>(saveTo->getType()).pointTo
		);
	instrs.emplace_back(
		ir.instrPool.emplace_back(
			mir::InstrStore(from, saveTo)
		)
	);
	return instrs;
}

mir::AddrVariable * genSuitableAddr(
	mir::Module & ir, const std::string & op, mir::AddrOperand * preOp
) {
	auto upTypeInfo = std::unique_ptr<sup::TypeInfo>();
	auto preType = preOp->getType().type;
	if (op == "!") {
		upTypeInfo = std::make_unique<sup::BoolType>();
	} else if (op == "+" || op == "-") {
		switch (preType) {
			case Type::Bool_t:
			case Type::Int_t: {
				upTypeInfo = std::make_unique<sup::IntType>();
				break;
			}
			case Type::Float_t: {
				upTypeInfo = std::make_unique<sup::FloatType>();
				break;
			}
			default: com::Throw("Unsupported Type.", CODEPOS);
		}
	}
	auto * destOp = ir.addrPool.emplace_back(mir::AddrVariable(*upTypeInfo));
	return destOp;
}
}
