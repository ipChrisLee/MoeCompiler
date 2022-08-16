#pragma once

#include <map>
#include <vector>
#include <moeconcept.hpp>
#include <stlpro.hpp>

#include "IR/IRAddr.hpp"
#include "pass/pass-common.hpp"
#include "IR/IRHeader.hpp"


namespace pass {


class Node {
  protected:
  public:
	//  JumpLabel of this basic block
	ircode::AddrJumpLabel * pIRLabel;
	//  Instrs in basic blocks. The last instr is br or ret.(term instr)
	std::list<ircode::IRInstr *> instrs;
	//  Information on CFG.
	std::set<Node *> succOnCFG, predOnCFG;
	//  phi instructions
	std::map<ircode::AddrLocalVariable *, ircode::InstrPhi *> phiInstrs;
	explicit Node(ircode::AddrJumpLabel * pIRLabel);

	void clear() {
		pIRLabel = nullptr;
		instrs.clear();
		succOnCFG.clear();
		predOnCFG.clear();
	}

	std::list<ircode::IRInstr *> toLLVMIRForm();
};

class CFG {
  protected:
	void buildCFG();
	void simplifyCFG();
	void calculateIDomAndDF();
	void mem2reg();
  public:
	ircode::IRModule & ir;
	moeconcept::Pool<Node> _nodePool;
	ircode::IRFuncDef * pFuncDefThis;

	//==Helper functions and property to help build CFG.
	using PNode = Node *;
	std::map<ircode::AddrJumpLabel *, Node *> _label2Node;
	Node * getNodeByLabel(ircode::AddrJumpLabel * pLabel);    //  exit prog if not find mapping
	void add_edge(Node * pNodeFrom, Node * pNodeTo);
	Node * new_node(ircode::AddrJumpLabel * pLabel);         //  maintain info automatically
	Node * try_merge_node(Node * pNodeFrom, PNode & pNodeTo); //  merge by <pNodeFrom, pNodeTo>

	//==Information and function of CFG.
	//  Entry node and Exit node
	Node * pEntryNode = nullptr;

	//==Information of Dominance. [Generate after building CFG.]
	//  Immediate dominator
	std::map<Node *, Node *> iDom;
	//  Dominance Frontier
	std::map<Node *, std::set<Node *>> domF;

	CFG(ircode::IRModule & ir, ircode::IRFuncDef * pFuncDefThis);
	CFG(CFG &&) = default;

	int opti();
	std::list<ircode::IRInstr *> toLLVMIRFrom();    //  keep llvm-ir form.
	std::list<ircode::IRInstr *> toDeSSAForm();     //  de-ssa ir. passed to backend.
};

class CFGIR {
  protected:
	class CFGPool : public moeconcept::Pool<CFG> {
	  public:
		explicit CFGPool(std::map<ircode::IRFuncDef *, CFG *> & mp);
	};

	ircode::IRModule & ir;
	std::map<ircode::IRFuncDef *, CFG *> funcDef2CFG;
	CFGPool cfgPool;
  public:
	explicit CFGIR(ircode::IRModule & ir);
	//  change ir basing on functions.
	int run();
	int opti();
	int genLLVMFormRes();                   //  change ir to optimized llvm-ir form
	int genDeSSAFormRes();                  //  change ir to de-ssa form, which is for backend
};

}
