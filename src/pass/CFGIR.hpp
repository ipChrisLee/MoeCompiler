#pragma once

#include <map>
#include <vector>
#include <moeconcept.hpp>
#include <stlpro.hpp>

#include "IR/IRAddr.hpp"
#include "IR/IRInstr.hpp"
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
	//  mark instructions
	ircode::InstrMarkVars markInstr;
	explicit Node(ircode::AddrJumpLabel * pIRLabel);

	void insertBeforeTerm(ircode::IRInstr * pInstr);

	void clear() {
		pIRLabel = nullptr;
		instrs.clear();
		succOnCFG.clear();
		predOnCFG.clear();
	}

	std::list<ircode::IRInstr *> toLLVMIRForm();
};

class DUChain {
  public:
	using ItPIRInstr_t = typename std::list<ircode::IRInstr *>::iterator;
	using ItPPhi_t = typename std::map<ircode::AddrLocalVariable *, ircode::InstrPhi *>::iterator;
	enum class pos {
		def, use, phi
	};

	struct PosInfo {
		Node * pNode;
		ItPIRInstr_t itInstr;

		PosInfo(Node * pNode, ItPIRInstr_t it);
		//  `it` is the instruction of `pNode.instrs` which defines or uses a variable.
	};

	PosInfo def;
	std::vector<PosInfo> use;

	DUChain(Node * pNodeDef, ItPIRInstr_t instrDef);
	void insertUseInfo(
		Node * pNodeUse, ItPIRInstr_t instrUse
	); //  check if def.pNode dom pNodeUse by yourself.
	void insertPhiUse(Node * pNodeUse, ircode::AddrLocalVariable *);
};

class CFG {
  protected:
	//  builder
	void buildCFG();    //  called when initializing CFG.
	void simplifyCFG();
	//  analyzer. Information generated by analyzer will be maintained by analyzer and Optimizer
	void calculateIDomAndDFAndDom();
	void getDUChain();
	//  Resolver, which will NOT maintain information generated by analyzers anymore.
	//  Before resolve, we will call `calculateIDomAndDFAndDom` to get some information.
	//  On resolution, information of CFG Nodes will not be changed.
	void mem2reg();
	void resolvePhi();
	void addMarker();
	//  helper
	void collectInfoFromAllReachableNode(const std::function<void(const Node *)> & fun) const;
	void collectInfoFromAllReachableInstr(  //  include phi node
		const std::function<void(
			Node *, typename std::list<ircode::IRInstr *>::iterator
		)> & fun
	) const;
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

	//==Advanced Information. [Generated by analyzer]
	//  Immediate dominator [Generated by `calculateIDomAndDFAndDom` from CFG form of IR]
	std::map<Node *, Node *> iDom;
	//  Dominance Frontier [Generated by `calculateIDomAndDFAndDom` from CFG form of IR]
	std::map<Node *, std::set<Node *>> domF;
	//  Dom set [Generated by `calculateIDomAndDFAndDom` from CFG form of IR]
	std::map<Node *, std::set<Node *>> dom;
	//  DUChain of every variable. [Generated by `getDUChain` form CFG form of IR]
	std::map<ircode::AddrVariable *, DUChain> duChain;

	CFG(ircode::IRModule & ir, ircode::IRFuncDef * pFuncDefThis);
	CFG(CFG &&) = default;

	int opti();
	std::list<ircode::IRInstr *> toLLVMIRFrom();    //  keep llvm-ir form.
	std::list<ircode::IRInstr *> toDeSSAForm();     //  de-ssa ir. passed to backend.
};

class CFGIR {
  protected:
	//  functional pass
	void globalOpt();   //  for store-load form ir. global opti.

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
	int opti();
	int genLLVMFormRes();                   //  change ir to optimized llvm-ir form
	int genDeSSAFormRes();                  //  change ir to de-ssa form, which is for backend
};

}
