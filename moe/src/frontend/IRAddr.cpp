#include <stack>

#include <submain.hpp>
#include <cprt.hpp>
#include "common.hpp"

#include "moeconcept.hpp"

#include "frontend/IRAddr.hpp"
#include "frontend/frontendHeader.hpp"
#include "stlextension.hpp"


/*

//  Implementation of `AddrPool` and `AddrPool::Scope` 
namespace ircode{

    int AddrPool::Scope::cnt=0;

    AddrPool::Scope::Scope():father(nullptr),id(++cnt){};
    
    AddrPool::Scope * AddrPool::Scope::addSonScope(){
        Scope * son=new Scope();
        son->father=this;sons.push_back(std::unique_ptr<Scope>(son));
        return son;
    }

    AddrPool::Scope * AddrPool::Scope::getFather() const {
        return father;
    }

    AddrPool::Scope * AddrPool::Scope::getThis() {
        return this;
    }

    void AddrPool::Scope::bindDominateVar(AddrOperand * addrvar){
        if(addrvar->getDominator() && addrvar->getDominator()!=this){
            com::Throw("Can NOT bind a addrvar who has had a dominator.",CODEPOS);
        }
        com::Assert(
            addrMap.count(addrvar->getVarName()) 
                    && addrMap[addrvar->getVarName()]->id!=addrvar->id,
            "Same name for different identifiers!",CODEPOS);
        addrMap[addrvar->getVarName()]=addrvar;
        addrvar->setDominator(this);
    }

    Addr * AddrPool::Scope::findVarInThisScope(const std::string & varname) const {
        return addrMap.count(varname)?(addrMap.find(varname)->second):nullptr;
    }

    std::string AddrPool::Scope::getIdChain() const {
        std::stack<int>idStack;
        const Scope * pScope=this;
        while(pScope){
            idStack.push(pScope->id);pScope=pScope->getFather();
        }
        std::string buf;
        while(idStack.size()){
            buf+="B"+to_string(idStack.top());idStack.pop();
        }
        return buf+"_";
    }

    AddrPool::AddrPool():pBlockRoot(std::make_unique<Scope>()){}

    AddrLocalVar * AddrPool::addAddrLocalVar(const AddrLocalVar & addrLocalVar,Scope * pScope){
        AddrLocalVar * pAddrLocalAddr=
            dynamic_cast<AddrLocalVar*>(pScope->findVarInThisScope(addrLocalVar.getVarName()));
        if(pAddrLocalAddr){
            if(addrLocalVar.id!=pAddrLocalAddr->id){
                com::Throw("Add a variable has same name in scope.",CODEPOS);
            }else{
                return pAddrLocalAddr;
            }
        }
        pool.emplace_back(addrLocalVar.getSameExceptScopePointerInstance());
        AddrOperand * pAddr=dynamic_cast<AddrOperand*>(pool.rbegin()->get());
        pScope->bindDominateVar(pAddr);
        pAddr->setDominator(pScope);
        return dynamic_cast<AddrLocalVar*>(pAddr);
    }

    AddrStaticVar * AddrPool::addAddrStaticVar(
        const AddrStaticVar & addrStaticVar,
        const StaticValue & staticValue
    ){
        AddrStaticVar * pAddrStaticVar=
            dynamic_cast<AddrStaticVar*>
                (pBlockRoot->findVarInThisScope(addrStaticVar.getVarName()));
        if(pAddrStaticVar){
            if(addrStaticVar.id!=pAddrStaticVar->id){
                com::Throw("Add a variable has same name in scope (in static filed).",CODEPOS);
            }else{
                return pAddrStaticVar;
            }
        }
        pool.emplace_back(addrStaticVar.getSameExceptScopePointerInstance());
        pAddrStaticVar=dynamic_cast<AddrStaticVar*>(pool.rbegin()->get());
        pBlockRoot->bindDominateVar(pAddrStaticVar);
        pAddrStaticVar->setDominator(pBlockRoot.get());
        pAddrStaticVar->setStaticValue(staticValue);
        return pAddrStaticVar;
    }

    Addr * AddrPool::addAddr(const Addr &){
        com::TODO("Havn't decide how to implement it.");
        return pool.rbegin()->get();
    }
    
    AddrPool::Scope * AddrPool::addScope(Scope * pFather){
        return pFather->addSonScope();
    }

    AddrPool::Scope * AddrPool::getRootScopePointer() {
        return pBlockRoot.get();
    }

    Addr * AddrPool::findAddrDownToRoot(
        const AddrPool::Scope * pFrom,
        const std::string & varname
    ){
        while(pFrom){
            Addr * pAddrOperand=pFrom->findVarInThisScope(varname);
            if(pAddrOperand) return pAddrOperand;
            pFrom=pFrom->getFather();
        }
        return nullptr;
    }
}

//  Implementation of Addr and TypeInfo
namespace ircode{

    std::unique_ptr<moeconcept::Cloneable> TypeInfo::__cloneToUniquePtr() const {
        com::Throw("Since `TypeInfo` is abstract, this method should not be called.",CODEPOS);
    }

    TypeInfo::TypeInfo(TypeInfo::Type type):type(type){
    }

    std::unique_ptr<moeconcept::Cloneable> IntType::__cloneToUniquePtr() const {
        return std::make_unique<IntType>(*this);
    }

    IntType::IntType():TypeInfo(TypeInfo::Type::Int_t){
    }

    std::string IntType::toLLVMIR() const{
        return "i32";
    }

    std::unique_ptr<moeconcept::Cloneable> FloatType::__cloneToUniquePtr() const {
        return std::make_unique<FloatType>(*this);
    }

    FloatType::FloatType():TypeInfo(TypeInfo::Type::Float_t){
    }

    std::string FloatType::toLLVMIR() const{
        return "float";
    }
    
    std::unique_ptr<moeconcept::Cloneable> IntArrayType::__cloneToUniquePtr() const {
        return std::make_unique<IntArrayType>(*this);
    }

    IntArrayType::IntArrayType(const std::vector<int>&shape)
        :TypeInfo(TypeInfo::Type::IntArray_t),shape(shape){
    }

    std::string IntArrayType::toLLVMIR() const {
        std::string buf;
        for(auto len:shape){
            buf+="["+to_string(len)+" x ";
        }
        buf+="i32";
        for(size_t i=0;i<shape.size();++i){
            buf+="]";
        }
        return buf;
    }

    std::unique_ptr<moeconcept::Cloneable> FloatArrayType::__cloneToUniquePtr() const {
        return std::make_unique<FloatArrayType>(*this);
    }

    FloatArrayType::FloatArrayType(const std::vector<int>&shape)
        :TypeInfo(TypeInfo::Type::FloatArray_t),shape(shape){
    }

    std::string FloatArrayType::toLLVMIR() const {
        std::string buf;
        for(auto len:shape){
            buf+="["+to_string(len)+" x ";
        }
        buf+="flaot";
        for(size_t i=0;i<shape.size();++i){
            buf+="]";
        }
        return buf;
    }

    std::unique_ptr<moeconcept::Cloneable> PointerType::__cloneToUniquePtr() const {
        return std::make_unique<PointerType>(*this,false);
    }

    PointerType::PointerType(const TypeInfo & typeInfo,bool newOne):TypeInfo(Type::Pointer_t){
        if(newOne){
            if(typeInfo.type==TypeInfo::Type::Pointer_t){
                const PointerType & pointerType=dynamic_cast<const PointerType &>(typeInfo);
                pointLevel=pointerType.pointLevel+1;
                pointTo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
                                            (pointerType.pointTo->cloneToUniquePtr());
            }else{
                pointLevel=1;
                pointTo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
                        (typeInfo.cloneToUniquePtr());
            }
        }else{ // just copy construct
            com::Assert(typeInfo.type==TypeInfo::Type::Pointer_t,
                    "`typeInfo` should be PointerType.",CODEPOS);
            const PointerType & pointerTypeInfo=dynamic_cast<const PointerType &>(typeInfo);
            com::Assert(pointerTypeInfo.pointTo->type!=Type::Pointer_t,
                    "`typeInfo.pointTo->type` should NOT be `Pointer_t`.",CODEPOS);
            pointTo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
                    (pointerTypeInfo.cloneToUniquePtr());
            pointLevel=pointerTypeInfo.pointLevel;
        }
    }
    
    std::string PointerType::toLLVMIR() const {
        std::string buf=pointTo->toLLVMIR();
        for(int i=0;i<pointLevel;++i){
            buf+="*";
        }
        return buf;
    }


    std::unique_ptr<moeconcept::Cloneable> BoolType::__cloneToUniquePtr() const {
        return std::make_unique<BoolType>(*this);
    }

    BoolType::BoolType():TypeInfo(TypeInfo::Type::Bool_t){
    }

    std::string BoolType::toLLVMIR() const{
        return "i1";
    }

}

static std::string shapeToLLVM(const std::vector<int>&shape,const std::string & bType){
    std::string typeName;
    for(int len:shape) typeName+="["+to_string(len)+" x ";
    typeName+=bType;
    for(size_t i=0;i<shape.size();++i) typeName+="]";
    return typeName;
}

//  Implementation of StaticValue
namespace ircode{
    
    std::unique_ptr<moeconcept::Cloneable> StaticValue::__cloneToUniquePtr() const {
        com::Throw("Since `TypeInfo` is abstract, this method should not be called.",CODEPOS);
    }

    StaticValue::StaticValue():uPtrInfo(nullptr){
    }

    StaticValue::StaticValue(const TypeInfo & typeInfo)
        :uPtrInfo(com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
                (typeInfo.cloneToUniquePtr())
               ){
        }


    StaticValue::StaticValue(const StaticValue & staticValue)
        :uPtrInfo(com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>(
                    staticValue.cloneToUniquePtr())){
        }

    std::unique_ptr<moeconcept::Cloneable> FloatStaticValue::__cloneToUniquePtr() const {
        return std::make_unique<FloatStaticValue>(*this);
    }

    FloatStaticValue::FloatStaticValue(const std::string & literal):value(std::stof(literal)){
    }

    std::string FloatStaticValue::toLLVMIR() const {
        return uPtrInfo->toLLVMIR()+" "+floatToString(value);
    }

    std::unique_ptr<moeconcept::Cloneable> IntStaticValue::__cloneToUniquePtr() const {
        return std::make_unique<IntStaticValue>(*this);
    }

    IntStaticValue::IntStaticValue(const std::string & literal):value(std::stoi(literal)){
    }
        
    std::string IntStaticValue::toLLVMIR() const {
        return uPtrInfo->toLLVMIR()+" "+intToString(value);
    }
 
    std::unique_ptr<moeconcept::Cloneable> FloatArrayStaticValue::__cloneToUniquePtr() const {
        return std::make_unique<FloatArrayStaticValue>(*this);
    }

    FloatArrayStaticValue::FloatArrayStaticValue(
        int len,
        const std::vector<FloatStaticValue> & vi
    ):shape({len}),value(vi){
        StaticValue::uPtrInfo=std::make_unique<FloatArrayType>(shape);
        com::Assert(len >= int(value.size()) && len,"Illegal `len`!",CODEPOS);
        stlextension::vector::PushBackByNumberAndInstance(value,len-int(value.size()),FloatStaticValue());
    }

    FloatArrayStaticValue::FloatArrayStaticValue(
        int len,
        const std::vector<int> & preShape,
        const std::vector<FloatArrayStaticValue> & vi
    ){
        com::Assert(len >= int(vi.size()) && len,"Illegal `len`!",CODEPOS);
        shape.push_back(len);shape.insert(shape.end(),preShape.begin(),preShape.end());
        for(auto & ar:vi){
            com::Assert(ar.shape==preShape,
                    "Subarray should have same shape as `preshape`!",CODEPOS);
            stlextension::vector::PushBackByIterators(value,ar.value.begin(),ar.value.end());
        }
        size_t S=1;for(int x:shape) S*=x;
        com::Assert(S>=value.size() && S,"Illegal `S`",CODEPOS);
        if(S > value.size()){
            stlextension::vector::PushBackByNumberAndInstance(value,S-value.size(),FloatStaticValue());
        }
        StaticValue::uPtrInfo=std::make_unique<FloatArrayType>(shape);
    }

    std::string FloatArrayStaticValue::toLLVMIR() const { 
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

    std::unique_ptr<moeconcept::Cloneable> IntArrayStaticValue::__cloneToUniquePtr() const {
        return std::make_unique<IntArrayStaticValue>(*this);
    }

    IntArrayStaticValue::IntArrayStaticValue(
        int len,
        const std::vector<IntStaticValue> & vi
    ):shape({len}),value(vi){
        StaticValue::uPtrInfo=std::make_unique<FloatArrayType>(shape);
        com::Assert(len >= int(value.size()) && len,"Illegal `len`!",CODEPOS);
        stlextension::vector::PushBackByNumberAndInstance(value,len-int(value.size()),IntStaticValue());
    }

    IntArrayStaticValue::IntArrayStaticValue(
        int len,
        const std::vector<int> & preShape,
        const std::vector<IntArrayStaticValue> & vi
    ){
        com::Assert(len >= int(vi.size()) && len,"Illegal `len`!",CODEPOS);
        shape.push_back(len);shape.insert(shape.end(),preShape.begin(),preShape.end());
        for(auto & ar:vi){
            com::Assert(ar.shape==preShape,
                    "Subarray should have same shape as `preshape`!",CODEPOS);
            
            stlextension::vector::PushBackByIterators(value,ar.value.begin(),ar.value.end());
        }
        size_t S=1;for(int x:shape) S*=x;
        com::Assert(S>=value.size() && S,"Illegal `S`",CODEPOS);
        if(S > value.size()){
            stlextension::vector::PushBackByNumberAndInstance(value,S-value.size(),IntStaticValue());
        }
        StaticValue::uPtrInfo=std::make_unique<IntArrayType>(shape);
    }

    std::string IntArrayStaticValue::toLLVMIR() const { 
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
}

//  Addr
namespace ircode{
    int Addr::cnt=0;

    Addr::Addr():id(++cnt){
    }
    
    std::unique_ptr<Addr> Addr::getSameExceptScopePointerInstance() const {
        com::Throw("This method should not be called.",CODEPOS);
    }


    AddrOperand::AddrOperand(
        const std::string & name,
        const TypeInfo & typeInfo,
        bool isConst
    ):dominator(nullptr),isConst(isConst),name(name){
        uPtrTypeInfo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
            (typeInfo.cloneToUniquePtr());
    }

    AddrPool::Scope * AddrOperand::getDominator() const {
        return dominator;
    }

    void AddrOperand::setDominator(AddrPool::Scope * dom){
        if(dominator && dom!=dominator){
            com::Throw("One addrvar can have only one dominator.");
        }
        dominator=dom;
    }

    bool AddrOperand::isconst() const {
        return isConst;
    }

    std::string AddrOperand::getVarName() const {
        return name;
    }

    AddrStaticVar::AddrStaticVar(
        const std::string & varname,
        const TypeInfo & typeInfo,
        bool isConst
    ):AddrOperand(varname,typeInfo,isConst),uPtrStaticValue(nullptr){
    }

    StaticValue * AddrStaticVar::setStaticValue(const StaticValue & staticValue){
        com::Assert(!uPtrStaticValue,"Setting set uPtrStaticValue of StaticVar.",CODEPOS);
        com::Assert(uPtrTypeInfo->type==staticValue.uPtrInfo->type,
                "Bind a mismatched type static value to a variable.",CODEPOS);
        uPtrStaticValue=
            com::dynamic_cast_unique_ptr<moeconcept::Cloneable,StaticValue>
                (staticValue.cloneToUniquePtr());
        return uPtrStaticValue.get();
    }

    const StaticValue * AddrStaticVar::getStaticValue() const {
        return uPtrStaticValue.get();
    }

    std::string AddrStaticVar::toLLVMIR() const {
        return "@"+getVarName();
    }

    std::unique_ptr<Addr> AddrStaticVar::getSameExceptScopePointerInstance() const {
        std::unique_ptr<AddrStaticVar>uPtrStaticVar=
            std::make_unique<AddrStaticVar>(name,*uPtrTypeInfo,isConst);
        uPtrStaticVar->setStaticValue(*uPtrStaticValue);
        return uPtrStaticVar;
    }

    AddrLocalVar::AddrLocalVar(
        const std::string & varname,
        const TypeInfo & typeInfo,
        bool isConst
    ):AddrOperand(varname,typeInfo,isConst){
    }

    std::string AddrLocalVar::toLLVMIR() const {
        return "%"+getDominator()->getIdChain()+getVarName();
    }

    std::unique_ptr<Addr> AddrLocalVar::getSameExceptScopePointerInstance() const {
        std::unique_ptr<AddrLocalVar>uPtrLocalVar=
            std::make_unique<AddrLocalVar>(name,*uPtrTypeInfo,isConst);
        return uPtrLocalVar;
    }

    std::string AddrTemp::toLLVMIR() const {
        return uPtrTypeInfo->toLLVMIR() + " %" + to_string(id);
    }

    std::unique_ptr<Addr> AddrTemp::getSameExceptScopePointerInstance() const {
        return std::make_unique<AddrTemp>(*uPtrTypeInfo);
    }

    AddrJumpLabel::AddrJumpLabel(const std::string & name):name(name){
    };

    std::string AddrJumpLabel::toLLVMIR() const {
        return "T"+to_string(id)+(name.size()?"_"+name:"");
    }

    std::unique_ptr<Addr> AddrJumpLabel::getSameExceptScopePointerInstance() const {
        return std::make_unique<AddrJumpLabel>(name);
    }

    AddrPara::AddrPara(const std::string name,const TypeInfo & typeInfo,int number)
        :number(number),name(name)
         ,uPtrTypeInfo(com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>(typeInfo.cloneToUniquePtr())){
        }

    std::string AddrPara::toLLVMIR() const {
        return "%P"+to_string(number)+"_"+name;
    }

    std::unique_ptr<Addr> AddrPara::getSameExceptScopePointerInstance() const {
        return std::make_unique<AddrPara>(name,*uPtrTypeInfo,number);
    }

    AddrFunction::AddrFunction(const std::string & name):name(name){
    }

    TypeInfo * AddrFunction::setReturnTypeInfo(const TypeInfo & retTypeInfo){
        com::Assert(!uPtrReturnTypeInfo,"Unique Pointer has been set.",CODEPOS);
        uPtrReturnTypeInfo=com::dynamic_cast_unique_ptr<moeconcept::Cloneable,TypeInfo>
            (retTypeInfo.cloneToUniquePtr());
        return uPtrReturnTypeInfo.get();
    }

    const TypeInfo * AddrFunction::getReturnTypeInfo() const {
        return uPtrReturnTypeInfo.get();
    }

    void AddrFunction::pushParameter(const AddrPara & addrPara){
        vecUPtrAddrPara.emplace_back(
            addrPara.getSameExceptScopePointerInstance()
        );
    }

    void AddrFunction::pushParameter(const std::string & paraName,const TypeInfo & paraTypeInfo){
        vecUPtrAddrPara.emplace_back(
            std::make_unique<AddrPara>(paraName,paraTypeInfo,vecUPtrAddrPara.size())
        );
    }

    const AddrPara * AddrFunction::getNumberThParameterTypeInfo(int number) const {
        com::Assert(number<int(vecUPtrAddrPara.size()),"Get unique_ptr of n-th para, but n is greater than the number of parameters.",CODEPOS);
        return vecUPtrAddrPara[number].get();
    }

    std::string AddrFunction::toLLVMIR() const {
        com::TODO("Just do NOT know what to do.",CODEPOS);
    }

    std::unique_ptr<Addr> AddrFunction::getSameExceptScopePointerInstance() const {
        std::unique_ptr<AddrFunction>uPtrAddrFunction=std::make_unique<AddrFunction>(name);
        if(uPtrReturnTypeInfo){
            uPtrAddrFunction->setReturnTypeInfo(*uPtrReturnTypeInfo);
        }
        for(auto & uPtrAddrPara:vecUPtrAddrPara){
            uPtrAddrFunction->pushParameter(*uPtrAddrPara);
        }
        return uPtrAddrFunction;
    }

    //AddSubMain(testAddrCompileConst,AddrCompileConst::testInSubMain);

    //int AddrCompileConst::testInSubMain(const std::vector<std::string>&){
        //IntInfo i0("0"),i1("1"),i2("2");
        //IntArrayInfo ar4(4,{i0,i1,i2});
        //IntArrayInfo ar3_4(3,{4},{ar4,ar4});
        //IntArrayInfo ar2_3_4(2,{3,4},{ar3_4,ar3_4});
        //AddrCompileConst acc(std::make_unique<IntArrayInfo>(ar2_3_4));
        //com::ccout.cprintLn(acc.toLLVMIR());
        //return 0;
    //}

    //AddSubMain(testAddrLocalVar,AddrLocalVar::testLocalVarDecl);

    //int AddrLocalVar::testLocalVarDecl(const std::vector<std::string>&){
        //AddrCompileConst::IntInfo i0("0"),i1("1"),i2("2");
        //AddrCompileConst::IntArrayInfo ar4(4,{i0,i1,i2});
        //AddrCompileConst::IntArrayInfo ar3_4(3,{4},{ar4,ar4});
        //AddrCompileConst::IntArrayInfo ar2_3_4(2,{3,4},{ar3_4,ar3_4});

        //AddrPool addrpool;
        //AddrPool::Scope * pRootScope=addrpool.getRootScopePointer();
        //AddrPool::Scope * pScopeMain=addrpool.addScope(pRootScope);
        //AddrPool::Scope * pScopeFun=addrpool.addScope(pRootScope);
        //AddrLocalVar * pLocalVarConst=addrpool.addAddrLocalVar(
                //std::make_unique<AddrLocalVar>("x",true),pScopeMain);
        //AddrLocalVar * pLocalVar=addrpool.addAddrLocalVar(
                //std::make_unique<AddrLocalVar>("x",false),pScopeFun);
        //com::ccout.cprintLn(pLocalVarConst->toLLVMIR());
        //com::ccout.cprintLn(pLocalVar->toLLVMIR());

        //return 0;
    //}

}

*/
