//
// Created by lee on 6/21/22.
//

#include "IRModule.hpp"
#include "IR/IRAddr.hpp"


namespace ircode {

IRAddrPool::IRAddrPool() : pool() {
	afterEmplace = [this](IRAddr * pAddr) {
		if (auto * pp = dynamic_cast<AddrGlobalVariable *>(pAddr)) {
			globalVars.emplace_back(pp);
		}
	};
}

std::string IRAddrPool::toLLVMIR() const {
	std::string res;
	for (auto & pAddr: globalVars) {
		res += pAddr->toDeclIR() + "\n";
	}
	return res + "\n";
}

const std::vector<AddrGlobalVariable *> & IRAddrPool::getGlobalVars() const {
	return globalVars;
}

std::string IRModule::toLLVMIR() const {
	auto res = SysY::llvmHeader + addrPool.toLLVMIR();
	for (auto * funcDecl: funcPool) {
		res += funcDecl->toLLVMIR();
	}
	res += llvmSyFunctionAttr;
	res += llvmSyLibFunctionAttr;
	return res;
}

void IRModule::finishLoading() {
	for (auto & func: funcPool) {
		func->finishLoading(*this);
	}
}

std::vector<AddrFunction *> IRModule::generateSysYDecl() {
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
			funcPool.emplace_back(IRFuncDef(p));
		}
		sysyFuncAdded = true;
	}
	return sysyFuncs;
}

void IRInstrPool::printAll(std::ostream & os) const {
	for (const auto & p: pool) {
		os << p->toLLVMIR() << std::endl;
	}
}


IRFuncBlock::IRFuncBlock() : instrs() {
}

std::string IRFuncBlock::toLLVMIR() const {
	com::TODO("", CODEPOS);
}


IRFuncDef::IRFuncDef(AddrFunction * pAddrFun)
	: pAddrFun(pAddrFun) {
}

std::string IRFuncDef::toLLVMIR() const {
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

void IRFuncDef::finishLoading(IRModule & ir) {
	if (loadFinished) {
		com::Throw("Method `finishLoading` can be called only once.", CODEPOS);
	}
	rearrangeAlloca();
	addEntryLabelInstr(ir);
	loadFinished = true;
}

void IRFuncDef::rearrangeAlloca() {
	STLPro::list::move_all_to_front(
		instrs, [](ircode::IRInstr * p) -> bool {
			return dynamic_cast<ircode::InstrAlloca *>(p) != nullptr;
		}
	);
}

IRInstr * IRFuncDef::emplace_back(IRInstr * irInstr) {
	if (irInstr) { instrs.emplace_back(irInstr); }
	return irInstr;
}

AddrJumpLabel * IRFuncDef::addEntryLabelInstr(IRModule & ir) {
	auto pAddrLabel = ir.addrPool.emplace_back(ircode::AddrJumpLabel("Entry"));
	instrs.emplace_front(
		ir.instrPool.emplace_back(ircode::InstrLabel(pAddrLabel))
	);
	return pAddrLabel;
}

void IRFuncDef::emplace_back(std::list<IRInstr *> && appendList) {
	appendList.remove_if([](IRInstr * p) { return p == nullptr; });
	instrs.splice(instrs.end(), std::move(appendList));
}

IRFuncBlock * IRFuncDef::emplace_back(IRFuncBlock && irFuncBlock) {
	pool.emplace_back(std::make_unique<IRFuncBlock>(std::move(irFuncBlock)));
	return pool.rbegin()->get();
}

IRFuncDefPool::IRFuncDefPool() {
	afterEmplace = [this](IRFuncDef * p) {
		funcDefs.emplace_back(p);
	};
}
}

using namespace ircode;
namespace sup {

std::list<ircode::IRInstr *> genBinaryOperationInstrs(
	ircode::IRModule & ir, AddrOperand * opL, const std::string & op,
	AddrOperand * opR, ircode::AddrVariable * opD
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
	auto instrsRes = std::list<ircode::IRInstr *>();
	switch (typeD) {
		case Type::Int_t: {
			if (op == "+") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrAdd(opL, opR, opD)
					)
				);
			} else if (op == "-") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrSub(opL, opR, opD)
					)
				);
			} else if (op == "*") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrMul(opL, opR, opD)
					)
				);
			} else if (op == "/") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrSDiv(opL, opR, opD)
					)
				);
			} else if (op == "%") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrSrem(opL, opR, opD)
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
						ircode::InstrFAdd(opL, opR, opD)
					)
				);
			} else if (op == "-") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrFSub(opL, opR, opD)
					)
				);
			} else if (op == "*") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrFMul(opL, opR, opD)
					)
				);
			} else if (op == "/") {
				instrsRes.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrFDiv(opL, opR, opD)
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
							ircode::InstrICmp(opD, opL, strToICMP(op), opR)
						)
					);
					break;
				}
				case Type::Float_t: {
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrFCmp(opD, opL, strToFCMP(op), opR)
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

std::list<ircode::IRInstr *> genUnaryOperationInstrs(
	IRModule & ir, const std::string & op,
	AddrOperand * opR, ircode::AddrVariable * opD
) {
	auto typeD = opD->getType().type;
	auto typeR = opR->getType().type;
	switch (typeD) {
		case Type::Bool_t: {
			auto instrsRes = std::list<ircode::IRInstr *>();
			if (op == "!") {
				auto * pZero = ir.addrPool.emplace_back(
					ircode::AddrStaticValue(opR->getType())
				);
				switch (opR->getType().type) {
					case Type::Bool_t:
					case Type::Int_t: {
						instrsRes.emplace_back(
							ir.instrPool.emplace_back(
								ircode::InstrICmp(opD, opR, ICMP::EQ, pZero)
							)
						);
						break;
					}
					case Type::Float_t: {
						instrsRes.emplace_back(
							ir.instrPool.emplace_back(
								ircode::InstrFCmp(opD, opR, FCMP::OEQ, pZero)
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
				ircode::AddrStaticValue(IntStaticValue(0))
			);
			if (op == "+") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrAdd(pZeroAddr, newOpR, opD)
					)
				);
			} else if (op == "-") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrSub(pZeroAddr, newOpR, opD)
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
				ircode::AddrStaticValue(FloatStaticValue(0))
			);
			auto instrs = std::list<ircode::IRInstr *>();
			if (op == "+") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrFAdd(pZeroAddr, opR, opD)
					)
				);
			} else if (op == "-") {
				instrs.emplace_back(
					ir.instrPool.emplace_back(
						ircode::InstrFSub(pZeroAddr, opR, opD)
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
	ircode::AddrOperand *, ircode::AddrOperand *, std::unique_ptr<TypeInfo>,
	std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir,
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
	auto instrsRes = std::list<ircode::IRInstr *>();
	instrsRes.splice(instrsRes.end(), std::move(instrsL));
	instrsRes.splice(instrsRes.end(), std::move(instrsR));
	return {newOpL, newOpR, std::move(_1), std::move(instrsRes)};
}

std::tuple<
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir, ircode::AddrOperand * preOp, sup::Type type
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
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> _genAddrConversion(
	ircode::IRModule & ir,
	ircode::AddrStaticValue * pSVAddr,
	const sup::TypeInfo & typeInfoD
) {
	auto newOp = static_cast<AddrOperand *>(nullptr);
	auto upResTypeInfo = std::unique_ptr<TypeInfo>();
	auto instrsRes = std::list<IRInstr *>();
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
			ircode::AddrStaticValue(
				convertOnSV(pSVAddr->getStaticValue(), *upResTypeInfo)
			)
		);
	}
	return {newOp, std::move(upResTypeInfo), std::move(instrsRes)};

}

static std::tuple<
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> _genAddrConversion(
	ircode::IRModule & ir,
	ircode::AddrVariable * pVarAddr,
	const sup::TypeInfo & typeInfoD
) {
	auto newOp = static_cast<AddrOperand *>(nullptr);
	auto upResTypeInfo = std::unique_ptr<TypeInfo>();
	auto instrsRes = std::list<IRInstr *>();
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
						ircode::AddrVariable(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrFptosi(pVarAddr, convertedAddr)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Bool_t: {
					auto convertedAddr = ir.addrPool.emplace_back(
						ircode::AddrVariable(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrZExt(pVarAddr, convertedAddr)
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
						ircode::AddrVariable(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrSitofp(pVarAddr, convertedAddr)
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
						ircode::AddrVariable(IntType())
					);
					auto convertedAddr = ir.addrPool.emplace_back(
						ircode::AddrVariable(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrZExt(pVarAddr, midAddr)
						)
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrSitofp(midAddr, convertedAddr)
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
						ircode::AddrVariable(BoolType())
					);
					auto * pZeroAddr = ir.addrPool.emplace_back(
						ircode::AddrStaticValue(IntType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrICmp(
								convertedAddr, pVarAddr, ICMP::NE, pZeroAddr
							)
						)
					);
					newOp = convertedAddr;
					break;
				}
				case Type::Float_t: {
					auto * convertedAddr = ir.addrPool.emplace_back(
						ircode::AddrVariable(BoolType())
					);
					auto * pZeroAddr = ir.addrPool.emplace_back(
						ircode::AddrStaticValue(FloatType())
					);
					instrsRes.emplace_back(
						ir.instrPool.emplace_back(
							ircode::InstrFCmp(
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
	ircode::AddrOperand *, std::unique_ptr<sup::TypeInfo>, std::list<ircode::IRInstr *>
> genAddrConversion(
	ircode::IRModule & ir,
	ircode::AddrOperand * preOp,
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

std::list<ircode::IRInstr *> genStoreInstrInFunction(
	ircode::IRModule & ir, ircode::AddrVariable * saveTo, ircode::AddrVariable * from
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
			ircode::InstrStore(from, saveTo)
		)
	);
	return instrs;
}

ircode::AddrVariable * genSuitableAddr(
	ircode::IRModule & ir, const std::string & op, ircode::AddrOperand * preOp
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
	auto * destOp = ir.addrPool.emplace_back(ircode::AddrVariable(*upTypeInfo));
	return destOp;
}
}
