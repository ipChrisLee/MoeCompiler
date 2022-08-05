#pragma once

#include <unordered_map>
#include <algorithm>
#include <stack>
#include <list>
#include "pass/pass-common.hpp"
#include "IR/IRInstr.hpp"

#define REG_NUM 2
namespace pass {
using namespace ircode;
typedef struct ud_Chain{
	AddrVariable *name;
	std::vector<size_t> def_in;
	std::vector<size_t> use_in;
	ud_Chain(AddrVariable *n){
		name=n;
		def_in.clear();
		use_in.clear();
	}
	void insert(bool du,size_t d){
		if(du) def_in.push_back(d);
		else use_in.push_back(d);
	}
	void print(){	
		std::cerr<<name->getName()<<"."<<name->id<<": ";
		std::cerr<<" -d";
		for(size_t def:def_in) std::cerr<<def<<" ";
		std::cerr<<" -u";
		for(size_t use:use_in) std::cerr<<use<<" ";
		std::cerr<<"\n";
	}
}ud_chain;

typedef struct symReg{
	ud_chain *var;
	bool spill;
	size_t spill_cost;
	size_t color;
	symReg(ud_chain * var){
		this->var=var;
		spill=false;
		spill_cost=0;
	}
}symreg;

typedef struct adjNds{
	symReg * sy;
	size_t nints;
	bool assigned;
	bool available[REG_NUM];
	std::list<size_t> neighbors;
	std::list<adjNds *> rmvadj;
	adjNds(){
		nints=0;
		assigned=false;
		for(int j=0;j<REG_NUM;j++){
			available[j]=true;
		}
	}
}adjnds;

class RegisterAssign : public IRPass {
  protected:
	int run(std::list<ircode::IRInstr *> & instrs);
	std::unordered_map<AddrVariable *,AddrVariable *> local_content;
    	std::unordered_map<AddrVariable*,ud_chain *> chains;
    	std::map<std::pair<AddrOperand *,AddrVariable *>,size_t>  mvs;
    	std::unordered_map<AddrVariable *,AddrVariable*> replace;
	std::vector<std::set<size_t>> loop_depth;
	std::vector<symreg *> symRegs;
	std::vector<adjnds *> adjList;
	std::stack<adjnds *> adjStack;
	bool **conflictMatrix;
	void merge_Registers();
	void caculate_Spillcost();
	void prune_Graph();
	void assign_Color();
  public:
  	void assignReg();
	int run() override;

	explicit RegisterAssign(ircode::IRModule & ir, std::string name = "");
};


}
