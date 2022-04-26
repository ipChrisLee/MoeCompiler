#include "frontend/IRAddr.hpp"

#include <stack>

#include <submain.hpp>
#include <cprt.hpp>

int ircode::Addr::cnt=0;


namespace ircode{
    AddrPool::AddrPool():pBlockRoot(std::unique_ptr<Scope>(new Scope())){}
    
    AddrPool::Scope * AddrPool::Scope::addSonScope(){
        Scope * son=new Scope();
        son->father=this;sons.push_back(std::unique_ptr<Scope>(son));
        return son;
    }
    void AddrPool::Scope::bindDominateVar(AddrVar * addrvar){
        if(addrvar->getDominator() && addrvar->getDominator()!=this){
            com::Throw("Can NOT bind a addrvar who has dominator.");
        }
        for(auto pVar:vars){
            if(pVar->getVarName() == addrvar->getVarName() && pVar!=addrvar){
                com::Throw("Same name for different identifiers!");
            }
        }
        vars.push_back(addrvar);
    }

    AddrLocalVar * AddrPool::addAddrLocalVar(std::unique_ptr<AddrLocalVar>&&upAddr,Scope * pScope){
        AddrLocalVar * pAddr=upAddr.get();
        pool.emplace_back(std::move(upAddr));
        pScope->bindDominateVar(pAddr);
        pAddr->setDominator(pScope);
        return pAddr;
    }

    AddrStaticVar * AddrPool::addAddrStaticVar(std::unique_ptr<AddrStaticVar>&&upAddr){
        AddrStaticVar * pAddr=upAddr.get();
        pool.emplace_back(std::move(upAddr));
        pBlockRoot->bindDominateVar(pAddr);
        pAddr->setDominator(pBlockRoot.get());
        return pAddr;
    }

    Addr * AddrPool::addAddr(std::unique_ptr<Addr>&&upAddr){
        Addr * pAddr=upAddr.get();
        pool.emplace_back(std::move(upAddr));
        return pAddr;
    }
    
    AddrPool::Scope * AddrPool::addScope(Scope * pFather){
        return pFather->addSonScope();
    }

    AddrPool::Scope * AddrPool::getRootScopePointer() {
        return pBlockRoot.get();
    }

    AddrVar * AddrPool::Scope::findVar(const std::string & varname) const {
        for(auto & pAddr:vars){
            if(pAddr->getVarName()==varname){
                return pAddr;
            }
        }
        return nullptr;
    }

    AddrVar * AddrPool::findAddrFrom(AddrPool::Scope * pFrom,const std::string & varname){
        while(pFrom){
            AddrVar * pAddrVar=pFrom->findVar(varname);
            if(pAddrVar) return pAddrVar;
        }
        return nullptr;
    }
    void AddrVar::setDominator(AddrPool::Scope * dom){
        if(dominator && dom!=dominator){
            com::Throw("One addrvar can have only one dominator.");
        }
        dominator=dom;
    }

    int AddrPool::Scope::cnt=0;

    AddrCompileConst::AddrCompileConst(
        std::unique_ptr<AddrCompileConst::Info> && pinfo
    ):pInfo(std::move(pinfo)){
        if(dynamic_cast<FloatInfo*>(pInfo.get())){
            type=Type::Float_t;
        }else if(dynamic_cast<IntInfo*>(pInfo.get())){
            type=Type::Int_t;
        }else if(dynamic_cast<FloatArrayInfo*>(pInfo.get())){
            type=Type::FloatArray_t;
        }else if(dynamic_cast<IntArrayInfo*>(pInfo.get())){
            type=Type::IntArray_t;
        }else{
            com::Throw("Unknown type of Info pointer!");
        }
    }

    std::string AddrCompileConst::toLLVMIR() const {
        return pInfo->toLLVMIR() + ", align 4";
    }

    static std::string shapeToLLVM(const std::vector<int>&shape,const std::string & bType){
        std::string typeName;
        for(int len:shape) typeName+="["+to_string(len)+" x ";
        typeName+=bType;
        for(size_t i=0;i<shape.size();++i) typeName+="]";
        return typeName;
    }

    std::string AddrCompileConst::FloatInfo::toLLVMIR() const { return "float "+origin; }
    std::string AddrCompileConst::IntInfo::toLLVMIR() const { return "i32 "+origin; }
    std::string AddrCompileConst::FloatArrayInfo::toLLVMIR() const { 
        std::function<std::string(const std::vector<int>&,int,int)>fun;
        fun=[&](const std::vector<int> & shapeNow,int idxBegin,int idxEnd)->std::string{
            if(!shapeNow.size()){
                return value[idxBegin].toLLVMIR();
            }
            std::string strVal=shapeToLLVM(shapeNow,"float")+" [";
            std::vector<int>shapeNext(shapeNow.begin()+1,shapeNow.end());
            int slide=1;
            for(int x:shapeNext) slide*=x;
            for(int i=idxBegin;i<idxEnd;i+=slide){
                strVal+=fun(shapeNext,i,i+slide)+",";
            }
            strVal.pop_back();
            strVal.push_back(']');
            return strVal;
        };
        return fun(shape,0,value.size());
    }
    std::string AddrCompileConst::IntArrayInfo::toLLVMIR() const { 
        std::function<std::string(const std::vector<int>&,int,int)>fun;
        fun=[&](const std::vector<int> & shapeNow,int idxBegin,int idxEnd)->std::string{
            if(!shapeNow.size()){
                return value[idxBegin].toLLVMIR();
            }
            std::string strVal=shapeToLLVM(shapeNow,"i32")+" [";
            std::vector<int>shapeNext(shapeNow.begin()+1,shapeNow.end());
            int slide=1;
            for(int x:shapeNext) slide*=x;
            for(int i=idxBegin;i<idxEnd;i+=slide){
                strVal+=fun(shapeNext,i,i+slide)+",";
            }
            strVal.pop_back();
            strVal.push_back(']');
            return strVal;
        };
        return fun(shape,0,value.size());
    }


    AddrCompileConst * AddrStaticVar::setCompileConst(AddrCompileConst * pAddrCConst){
        if(pInitValue && pInitValue!=pAddrCConst){
            com::Throw("Rebind pinitvalue to a different pointer!");
        }
        return pInitValue=pAddrCConst;
    }


    std::string AddrStaticVar::toLLVMIR() const {
        return "@"+getVarName();
    }

    std::string AddrLocalVar::toLLVMIR() const {
        return "%"+getDominator()->getIdChain()+getVarName();
    }

    std::string AddrPool::Scope::getIdChain() const {
        std::stack<std::string>stk;
        const Scope * pScope=this;
        while(pScope){
            stk.push(to_string(pScope->id));pScope=pScope->getFather();
        }
        std::string buf;
        while(stk.size()){
            buf+="B"+stk.top();stk.pop();
        }
        return buf+"_";
    }

    AddrPool::Scope * AddrPool::Scope::getFather() const {
        return father;
    }

    AddSubMain(testAddrCompileConst,AddrCompileConst::testInSubMain);

    int AddrCompileConst::testInSubMain(const std::vector<std::string>&){
        IntInfo i0("0"),i1("1"),i2("2");
        IntArrayInfo ar4(4,{i0,i1,i2});
        IntArrayInfo ar3_4(3,{4},{ar4,ar4});
        IntArrayInfo ar2_3_4(2,{3,4},{ar3_4,ar3_4});
        AddrCompileConst acc(std::make_unique<IntArrayInfo>(ar2_3_4));
        com::ccout.cprintLn(acc.toLLVMIR());
        return 0;
    }

    AddSubMain(testAddrLocalVar,AddrLocalVar::testLocalVarDecl);

    int AddrLocalVar::testLocalVarDecl(const std::vector<std::string>&){
        AddrCompileConst::IntInfo i0("0"),i1("1"),i2("2");
        AddrCompileConst::IntArrayInfo ar4(4,{i0,i1,i2});
        AddrCompileConst::IntArrayInfo ar3_4(3,{4},{ar4,ar4});
        AddrCompileConst::IntArrayInfo ar2_3_4(2,{3,4},{ar3_4,ar3_4});

        AddrPool addrpool;
        AddrPool::Scope * pRootScope=addrpool.getRootScopePointer();
        AddrPool::Scope * pScopeMain=addrpool.addScope(pRootScope);
        AddrPool::Scope * pScopeFun=addrpool.addScope(pRootScope);
        AddrLocalVar * pLocalVarConst=addrpool.addAddrLocalVar(
                std::make_unique<AddrLocalVar>("x",true),pScopeMain);
        AddrLocalVar * pLocalVar=addrpool.addAddrLocalVar(
                std::make_unique<AddrLocalVar>("x",false),pScopeFun);
        com::ccout.cprintLn(pLocalVarConst->toLLVMIR());
        com::ccout.cprintLn(pLocalVar->toLLVMIR());

        return 0;
    }

}
