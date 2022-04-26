/*  Intro:
 *      `Addr` is a base class of address.
 *          `AddrCompileConst` is address for compile time const. This inclue a info structure
 *          which provides information basing on type of const variable(`int`,`float`,`intArray` 
 *          and `floatArray`)
 *              >Notice:
 *                  Legal expression of operator, const BASIC-TYPE value and literals is
 *                  compile time const.
 *          `AddrVar` is address for variable declared in source program.
 *              `AddrStaticVar` is a address for static variable (since sysy does not have
 *              key word like `static`, this is for global variable.).
 *              `AddrLocalVar` is a address for local variable.
 *      `AddrPool` is like a memory pool of Addr. It also contains a dominator tree where 
 *      every tree node is a scope in source code.
 *          >Notice:
 *              AddrPool has a class member variable `loop` in type `vector<unique_ptr<Addr>>`.
 *              This `pool` is in control of every `Addr`
 *      `AddrPool::Scope` is a scope descriptor.
 * */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <mdb.hpp>
#include <common.hpp>
#include <stlextension.hpp>

#include "frontend/frontendHeader.hpp"


namespace ircode{
    class Addr;
    class AddrVar;
    class AddrLocalVar;
    class AddrStaticVar;
    class AddrPool{
    public:
        class Scope{
        protected:
            std::vector<AddrVar*>vars;
            Scope * father;
            std::vector<std::unique_ptr<Scope>>sons;
            int id;
            static int cnt;
        public:
            Scope():father(nullptr),id(++cnt){};
            //  Add scope as son of this.
            Scope * addSonScope();
            //  Get father scope of this.
            Scope * getFather() const ;
            //  Get this.
            Scope * getThis() ;
            //  Bind AddrVar to this scope.
            void bindDominateVar(AddrVar * addrvar);
            //  Find a AddrVar named `varname`. Return nullptr if not find.
            AddrVar * findVar(const std::string & varname) const ;
            std::string getIdChain() const ;
        };
    protected:
        std::vector<std::unique_ptr<Addr>>pool;
        std::unique_ptr<Scope>pBlockRoot;
    public:
        AddrPool();
        //  Add AddrVar to some scope.
        AddrLocalVar * addAddrLocalVar(std::unique_ptr<AddrLocalVar>&&,Scope * pScope);
        AddrStaticVar * addAddrStaticVar(std::unique_ptr<AddrStaticVar>&&);
        //  Add an Addr to pool. This is the only way you can create addr;
        Addr * addAddr(std::unique_ptr<Addr>&&);
        //  Add a scope as sub of pFather. This is the only way you can create scope.
        Scope * addScope(Scope * pFather);
        //  Get the root scope.
        Scope * getRootScopePointer();
        //  Find var named `varname` from scopes. Search from `pFrom` up to scope root.
        AddrVar * findAddrFrom(Scope * pFrom,const std::string & varname);
    };

    /*  First part of IR design: Address used in three-address-code.
     * */
    class Addr : public LLVMable{
    public:
        static int cnt;
        int id;
        Addr():id(++cnt){}
        virtual ~Addr(){}
    };

    /*  Const Addr. Notice that, legal expression of operator, const BASIC-TYPE value and 
     *  literals is compile time constant.
     *  Four type of constant: float, int, array of int, array of float.
     *  (I will add code inside this class if more type of constant need to be added.)
     * */
    class AddrCompileConst : public Addr{
    public:
        enum class Type{Float_t,Int_t,FloatArray_t,IntArray_t}type;
        struct Info : public LLVMable{ 
            virtual ~Info()=default; 
        };
        struct FloatInfo : public Info{ 
            float value;const std::string origin;
            //  Create from a origin string.
            explicit FloatInfo(const std::string & origin)
                :value(std::stof(origin)),origin(origin){}
            //  Create zero value.
            FloatInfo():value(0),origin("0.0"){}
            virtual std::string toLLVMIR() const override;
        };
        struct IntInfo : public Info{ 
            int value;const std::string origin;
            //  Create from a origin string.
            explicit IntInfo(const std::string & origin)
                :value(std::stoi(origin)),origin(origin){}
            //  Create zero value.
            IntInfo():value(0),origin("0"){}
            virtual std::string toLLVMIR() const override;
        };
        struct FloatArrayInfo : public Info{
            std::vector<int>shape;
            std::vector<FloatInfo>value;
            //  Create new 1-d, `len` long float array.
            explicit FloatArrayInfo(int len,const std::vector<FloatInfo> & vi)
                :shape({len}),value(vi)
            {
                if(len > int(value.size())){
                    stlextension::vector::InsertByNumberAndInstance(
                        value,len-int(value.size()),FloatInfo()
                    );
                }else if(len < int(value.size()) || !len){
                    com::Throw("Illegal `len`.");
                }
            }
            /*  Create new array from arrays.
             *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
             *  is `(2,3,4)` array.
             *  This constructor will detect legality of arrays. (They should have same shape.)
             * */
            explicit FloatArrayInfo(
                int len,
                const std::vector<int> & preShape,
                const std::vector<FloatArrayInfo> & vi
            ){
                if(len < int(vi.size()) || !len) com::Throw("Illegal `len`.");
                shape.push_back(len);shape.insert(shape.end(),preShape.begin(),preShape.end());
                for(auto & ar:vi){
                    if(ar.shape!=preShape){
                        com::Throw("Subarray should have same shape as `preShape`!");
                    }
                    stlextension::vector::InsertByIterator(value,ar.value.begin(),ar.value.end());
                }
                size_t S=1;for(int x:shape) S*=x;
                if(S > value.size()){
                    stlextension::vector::InsertByNumberAndInstance(
                        value,S-int(value.size()),FloatInfo()
                    );
                }else if(S < value.size() || !S) com::Throw("Illegal `S`.");
            }
            virtual std::string toLLVMIR() const override;
        };
        struct IntArrayInfo : public Info{
            std::vector<int>shape;
            std::vector<IntInfo>value;
            //  Create new 1-d, `len` long int array.
            explicit IntArrayInfo(int len,const std::vector<IntInfo> & vi)
                :shape({len}),value(vi)
            {
                if(len > int(value.size())){
                    stlextension::vector::InsertByNumberAndInstance(
                        value,len-int(value.size()),IntInfo()
                    );
                }else if(len < int(value.size()) || !len){
                    com::Throw("Illegal `len`.");
                }
            }
            /*  Create new array from arrays.
             *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
             *  is `(2,3,4)` array.
             *  This constructor will detect legality of arrays. (They should have same shape.)
             * */
            explicit IntArrayInfo(
                int len,
                const std::vector<int> & preShape,
                const std::vector<IntArrayInfo> & vi
            ){
                if(len < int(vi.size()) || !len) com::Throw("Illegal `len`.");
                shape.push_back(len);shape.insert(shape.end(),preShape.begin(),preShape.end());
                for(auto & ar:vi){
                    if(ar.shape!=preShape){
                        com::Throw("Subarray should have same shape as `preShape`!");
                    }
                    for(auto it:ar.value){
                        value.emplace_back(it);
                    }
                }
                size_t S=1;for(int x:shape) S*=x;
                if(S > value.size()){
                    stlextension::vector::InsertByNumberAndInstance(
                        value,S-int(value.size()),IntInfo()
                    );
                }else if(S < value.size() || !S) com::Throw("Illegal `S`.");
            }
            virtual std::string toLLVMIR() const override;
        };
        std::unique_ptr<Info>pInfo;
        explicit AddrCompileConst(std::unique_ptr<Info> && pInfo);
        std::string toLLVMIR() const override;
        static int testInSubMain(const std::vector<std::string>&);
    };

    /*  Variable address. When construct with constructor, dominator is nullptr.
     *  You should set dominator manually.
     * */
    class AddrVar : public Addr{
    protected:
        AddrPool::Scope * dominator;
        bool isConst;
        std::string name;
    public:
        virtual ~AddrVar() = default;
        explicit AddrVar(const std::string & name,bool isConst=false)
            :dominator(nullptr),isConst(isConst),name(name){}
        AddrPool::Scope * getDominator() const { return dominator; }
        void setDominator(AddrPool::Scope * dom);
        bool isconst() const { return isConst; }
        std::string getVarName() const { return name;}
    };

    /*  Static variable address. Static variable diff with local variable in initialzation.
     *  We have a instruction called `staticdef`, which is used to declare and define a 
     *  static viable. Every static variable has compile time initalization value, which 
     *  is pointed by `pInitValue`.
     *  This class has a pointer to init value since there may be a case where 'a static is 
     *  initialized by a defined const static', like `const int a[2]={1,2};int x=a[0];`.
     *  We need to preserve init value.
     * */
    class AddrStaticVar : public AddrVar{
    protected:
        AddrCompileConst * pInitValue;
    public:
        AddrStaticVar(
            const std::string & varname,
            AddrCompileConst * pInitValue,
            bool isConst=false
        ):AddrVar(varname,isConst),pInitValue(pInitValue){}
        const AddrCompileConst * getConstPointerInitValue() const { return pInitValue; }
        AddrCompileConst * setCompileConst(AddrCompileConst *);
        std::string toLLVMIR() const override;
    };

    class AddrLocalVar : public AddrVar{
    public:
        AddrLocalVar(const std::string & varname,bool isConst):AddrVar(varname,isConst){}
        std::string toLLVMIR() const ;
        static int testLocalVarDecl(const std::vector<std::string>&);
    };

}

