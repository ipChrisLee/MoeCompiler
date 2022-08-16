#include<unordered_map>
#include<vector>
#include<cmath>

#include "backendPass.hpp"
#include "support/TypeInfo.hpp"
#define WHILE_ST "while_cond"
#define WHILE_ED "while_end"

namespace pass {
using namespace ircode;
using namespace sup;
bool Live_at(symreg &s1,symreg &s2){
    auto var1=s1.var;
    auto var2=s2.var;
   size_t ld,up;
   if(var1->def_in.empty()&&var1->use_in.empty()) return false;
   else if(var1->def_in.empty()) {ld=var1->use_in[0]; up=var1->use_in[var1->use_in.size()-1];}
   else if(var1->use_in.empty()) {ld=var1->def_in[0]; up=var1->def_in[var1->def_in.size()-1];}
   else {
        ld=std::min(var1->def_in[0],var1->use_in[0]);
        up=std::max(var1->use_in[var1->use_in.size()-1],var1->def_in[var1->def_in.size()-1]);
   }
   size_t ld1,up1;  
   if(var2->def_in.empty()&&var2->use_in.empty()) return false;
   else if(var2->def_in.empty()) {ld1=var2->use_in[0]; up1=var2->use_in[var2->use_in.size()-1];}
   else if(var2->use_in.empty()) {ld1=var2->def_in[0]; up1=var2->def_in[var2->def_in.size()-1];}
   else {
        ld1=std::min(var2->def_in[0],var2->use_in[0]);
        up1=std::max(var2->use_in[var2->use_in.size()-1],var2->def_in[var2->def_in.size()-1]);
   }
   if(up<=ld1||up1<=ld) return false;
   else return true;
}

int RegisterAssign::run() {
	for (auto * pFuncDef: ir.funcPool) {
		run(pFuncDef->instrs);
	}
	ir.data_seg+="\t.data\n";
    	set_dataSeg();
    	std::cerr<<ir.data_seg;
	return 0;
}

void RegisterAssign::set_dataSeg(){
    std::vector<AddrGlobalVariable *> globalvars=ir.addrPool.getGlobalVars();
    //std::stringstream is;
    for(auto glob:globalvars){
        std::string full_nm=glob->getName()+"."+to_string(glob->id)+":\n";
        if(glob->getType().type==Type::Int_t||glob->getType().type==Type::Float_t){
        std::cerr<<"in\n";
            full_nm+="\t.word\n";
        }
        else if(glob->getType().type==Type::FloatArray_t||glob->getType().type==Type::IntArray_t){
            full_nm+="\t.space ";
            if(glob->getType().type==Type::FloatArray_t){
                auto arrayp=(FloatArrayType *)(&glob->getType());
                size_t m=1;
                for(int index:arrayp->shape) m*=index;
                full_nm+=to_string(m);
               
            }
            else if(glob->getType().type==Type::IntArray_t){
                auto arrayp=(IntArrayType *)(&glob->getType());
                size_t m=1;
                std::string temp;
                for(int index:arrayp->shape) m*=index;
                full_nm+=to_string(m);
            }
            full_nm+="\n";
        }
        ir.data_seg+=full_nm;
        full_nm.clear();
    }
}

bool ofNoUse(ud_chain * c1,size_t pos){
    size_t d1=c1->def_in.empty()?0:(c1->def_in[c1->def_in.size()-1]);
    //size_t u1=c1->use_in.empty()?0:(c1->use_in[c1->use_in.size()-1]);
    return (pos>d1)?true:false;
}

void mergeUdchain(ud_chain *c1,ud_chain *c2,size_t pos){
    for(size_t d:c2->def_in) c1->def_in.push_back(d);
    std::sort(c1->def_in.begin(),c1->def_in.end());
    for(size_t u:c2->use_in) c1->use_in.push_back(u);
    std::sort(c1->use_in.begin(),c1->use_in.end());
}
RegisterAssign::RegisterAssign(ircode::IRModule & ir, std::string name) :
	IRPass(ir, std::move(name)) {
}

void RegisterAssign::merge_Registers(){
    for(auto mv:mvs){
        if(typeid(*(mv.first.first))==typeid(AddrStaticValue)){
            if(chains.count(mv.first.second)){           
                chains[mv.first.second]->insert(true,mv.second);
                std::sort(chains[mv.first.second]->def_in.begin(),chains[mv.first.second]->def_in.end());
            }
            continue;

        }
        std::pair<AddrOperand *,AddrVariable *> ins=mv.first;
        //if(!chains.count((AddrVariable *)ins.first)||!chains.count(ins.second)) continue;
        size_t pos=mv.second;
        ud_chain *c1,*c2;
        c1=c2=nullptr;
        if(!chains.count((AddrVariable *)ins.first)||!chains.count(ins.second)){
            if(!chains.count((AddrVariable *)ins.first)&&replace.count((AddrVariable*)ins.first))
                c1=chains[replace[(AddrVariable*)ins.first]];
            if(!chains.count(ins.second)&&replace.count(ins.second))
                c2=chains[replace[ins.second]];
            if((!replace.count((AddrVariable*)ins.first))&&(!replace.count(ins.second)))
		   continue;
        }
        if(!c1)    c1=chains[(AddrVariable *)ins.first];
        if(!c2)    c2=chains[ins.second];
        symReg s1(c1);
        symReg s2(c2);
        if(ofNoUse(c1,pos)&&ofNoUse(c2,pos)&&!Live_at(s1,s2)){
            std::cerr<<c2->name->getName()<<" "<<c1->name->getName()<<std::endl;
            mergeUdchain(c2,c1,pos);
            replace.insert(std::make_pair(c1->name,c2->name));
            chains.erase(c1->name);
        }
        else{
            c1->use_in.push_back(pos);
            std::sort(c1->use_in.begin(),c1->use_in.end());
            c2->def_in.push_back(pos);
            std::sort(c2->def_in.begin(),c2->def_in.end());
        }
    }
}

void RegisterAssign::caculate_Spillcost(){
    for(auto sym:symRegs){
        ud_chain *c1=sym->var;
        for(size_t df:c1->def_in){
            for(size_t i=0;i<loop_depth.size();i++){
                if(loop_depth[i].count(df)){
                    //std::cerr<<df<<":"<<i<<std::endl;
                    sym->spill_cost+=std::pow(10,i);
                    break;
                }
            }
        }
        for(size_t us:c1->use_in){
            for(size_t i=0;i<loop_depth.size();i++){
                if(loop_depth[i].count(us)){
                    //std::cerr<<us<<":"<<i<<std::endl;
                    sym->spill_cost+=std::pow(10,i);
                    break;
                }
            }
        }
     //std::cerr<<c1->name->getName()<<": cost="<<sym->spill_cost<<std::endl;   
    }
}

void RegisterAssign::prune_Graph(){
    bool token;
    size_t nodes=adjList.size();
    while(nodes!=0){
        token=false;
        for(auto adj:adjList){
            if(adj->assigned) continue;
            if(adj->nints<REG_NUM){
                //std::cerr<<adj->sy->var->name->getName()<<adj->nints<<std::endl;
                token=true;
                nodes--;
                adj->assigned=true;
                for(size_t j:adj->neighbors){
                    adjList[j]->nints--;
                    adjList[j]->rmvadj.push_back(adj);
                }
                adjStack.push(adj);
            }
        }
        size_t lowest=0;
        adjnds *lowestadj;
        if(!token&&nodes!=0){
            for(auto adj:adjList){
                if(adj->assigned) continue;
                if(lowest==0||adj->sy->spill_cost<lowest){
                    lowestadj=adj;
                    lowest=lowestadj->sy->spill_cost;
                }
            }
            nodes--;
            lowestadj->assigned=true;
            //std::cerr<<lowestadj->sy->var->name->getName()<<": spill"<<std::endl;
            for(size_t j:lowestadj->neighbors){
                adjList[j]->nints--;
                adjList[j]->rmvadj.push_back(lowestadj);
            }
            adjStack.push(lowestadj);
        }
    }
}

void RegisterAssign::assign_Color(){
    bool no_need_to_spill;
    while(!adjStack.empty()){
        auto node=adjStack.top();
        adjStack.pop();
        no_need_to_spill=false;
        for(int j=0;j<REG_NUM;j++){
            if(node->available[j]){
                no_need_to_spill=true;
                node->sy->color=j+1;
                for(auto rmnode:node->rmvadj){
                    rmnode->available[j]=false;
                }
                break;
            }
        }
        if(!no_need_to_spill){
            node->sy->spill=true;
        }
    }
    for(auto sym:symRegs){
        std::cerr<<sym->var->name->getName()<<"."<<sym->var->name->id<<":";
        if(sym->spill) std::cerr<<"spill -spill_cost="<<sym->spill_cost<<std::endl;
        else std::cerr<<" r"<<sym->color<<std::endl;
    }

}

void RegisterAssign::assignReg(){
    //寄存器合并
    merge_Registers();
    /*for(auto chain=chains.begin();chain!=chains.end();chain++){
            chain->second->print();
    }*/
    //计算溢出代价
    for(auto chain=chains.begin();chain!=chains.end();chain++){
            symRegs.push_back(new symreg(chain->second));
    }
    caculate_Spillcost();
    
    //冲突矩阵
    for(auto sym:symRegs) std::cerr<<sym->var->name->getName()<<"."<<sym->var->name->id<<"\n";
    conflictMatrix=new bool *[symRegs.size()];
    for(size_t cr=0;cr<symRegs.size();cr++) conflictMatrix[cr]=new bool[symRegs.size()];
    for(size_t i=0;i<symRegs.size();i++){
        for(size_t j=0;j<i;j++){
            if(Live_at(*symRegs[i],*symRegs[j])) conflictMatrix[i][j]=true;
            else conflictMatrix[i][j]=false;
        }
    }
    for(size_t i=0;i<symRegs.size();i++){
        for(size_t j=0;j<i;j++){
            std::cerr<<conflictMatrix[i][j]<<", ";
        }
        std::cerr<<"\n";
    }
    //邻接表
    for(size_t cr=0;cr<symRegs.size();cr++){
        auto adjnd=new adjnds();
        adjnd->sy=symRegs[cr];
        for(size_t j=0;j<cr;j++){
            if(conflictMatrix[cr][j]){
                adjnd->nints++;
                adjnd->neighbors.push_back(j);
            }
        }
        for(size_t j=cr+1;j<symRegs.size();j++){
            if(conflictMatrix[j][cr]){
                adjnd->nints++;
                adjnd->neighbors.push_back(j);
            }
        }
        adjList.push_back(adjnd);
    }
    //修剪冲突图
    prune_Graph();
    //分配
    assign_Color();

}

    

int RegisterAssign::run(std::list<ircode::IRInstr *> & instrs) {
	auto itPInstrNow = instrs.begin();
    size_t count=0;
    size_t loopd=1;
    //获取use-def链
    while(itPInstrNow!=instrs.end()){
        if(g(itPInstrNow)->instrType==InstrType::Label){
            auto lbInstr=(InstrLabel *)(g(itPInstrNow));
            std::string tp=lbInstr->pAddrLabel->getLabelName();
            if(tp==WHILE_ST) loopd++;
            else if(tp==WHILE_ED) loopd--;
        }
        if(loopd>loop_depth.size()){
            std::set<size_t> member={count};
            loop_depth.push_back(member);
        }
        else loop_depth[loopd-1].insert(count);
        if(g(itPInstrNow)->instrType == InstrType::Load){
            auto ldInstr=(InstrLoad *)(g(itPInstrNow));
            local_content.insert(std::make_pair(ldInstr->to,ldInstr->from));   
        }
        else if(g(itPInstrNow)->instrType == InstrType::Store){
            auto stInstr=(InstrStore *)(g(itPInstrNow));
            if(typeid(*(stInstr->from))==typeid(AddrStaticValue)){
            	//std::cerr<<"mov"<<std::endl;
                mvs.insert(std::make_pair(std::make_pair(stInstr->from,stInstr->to),count));
            }
            else if(local_content.count((AddrVariable *)stInstr->from)){
            	if(mvs.count(std::make_pair(local_content[(AddrVariable *)stInstr->from],stInstr->to)))
                   mvs.insert(std::make_pair(std::make_pair(local_content[(AddrVariable *)stInstr->from],stInstr->to),count));
                else mvs[std::make_pair(local_content[(AddrVariable *)stInstr->from],stInstr->to)]=count;
            }
            local_content.insert(std::make_pair((AddrVariable *)stInstr->from,stInstr->to));
        }
        /*else if(g(itPInstrNow)->instrType == InstrType::Getelementptr){
            auto geInstr=(InstrGetelementptr *)(g(itPInstrNow));
            local_content.insert(std::make_pair(geInstr->to,geInstr->from));
        }*/
        ++itPInstrNow;
   	count++;
    }
    count=0;
    itPInstrNow = instrs.begin();
    while(itPInstrNow!=instrs.end()){
        if(g(itPInstrNow)->instrType == InstrType::ICmp){
            auto cpInstr=(InstrCompare *)(g(itPInstrNow));
            AddrVariable *l=(AddrVariable *)cpInstr->leftOp;
            if(local_content.count(l)){
                l=local_content[l];
                if(!chains.count(l))  chains[l]=new ud_chain(l);
                 chains[l]->insert(false,count);
            }
            AddrVariable *r=(AddrVariable *)cpInstr->rightOp;
            if(local_content.count(r)){
                r=local_content[r];
                if(!chains.count(r))  chains[r]=new ud_chain(r);
                 chains[r]->insert(false,count);
            }
        }
        else if(g(itPInstrNow)->instrType == InstrType::FCmp){
            auto fcpInstr=(InstrCompare *)(g(itPInstrNow));
            AddrVariable *l=(AddrVariable *)fcpInstr->leftOp;
            if(local_content.count(l)){
                l=local_content[l];
                if(!chains.count(l))  chains[l]=new ud_chain(l);
                 chains[l]->insert(false,count);
            }
            AddrVariable *r=(AddrVariable *)fcpInstr->rightOp;
             if(local_content.count(r)){
                r=local_content[r];
                if(!chains.count(r))  chains[r]=new ud_chain(r);
                 chains[r]->insert(false,count);
            }
            //lst-def.insert(std::make_pair(ldInstr->from,count));
        }             
        else if(g(itPInstrNow)->instrType == InstrType::BinaryOp){
            auto biInstr=(InstrBinaryOp *)(g(itPInstrNow));
            AddrVariable *re=(AddrVariable *)biInstr->res;
            if(local_content.count(re)){
                re=local_content[re];
                if(!chains.count(re))  chains[re]=new ud_chain(re);
                 chains[re]->insert(true,count);
            }
            AddrVariable *l=(AddrVariable *)biInstr->left;
            if(local_content.count(l)){
                l=local_content[l];
                if(!chains.count(l))  chains[l]=new ud_chain(l);
                 chains[l]->insert(false,count);
            }
            AddrVariable *r=(AddrVariable *)biInstr->right;
            if(local_content.count(r)){
                r=local_content[r];
                if(!chains.count(r))  chains[r]=new ud_chain(r);
                 chains[r]->insert(false,count);
            }
        }
        else if(g(itPInstrNow)->instrType == InstrType::Ret){
            auto reInstr=(InstrRet *)(g(itPInstrNow));
            AddrVariable * r=(AddrVariable *)reInstr->retAddr;
            if(local_content.count(r)){
                r=local_content[r];
                if(!chains.count(r))  chains[r]=new ud_chain(r);
                 chains[r]->insert(false,count);
            }
        }
        count++;
        ++itPInstrNow;
    }
    //寄存器分配pass
    assignReg();
  
    return 0;
}
}
