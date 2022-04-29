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
#include <moeconcept.hpp>
#include <stlextension.hpp>

#include "frontend/frontendHeader.hpp"


namespace ircode{
    class Addr;
    class AddrVar;
    class AddrLocalVar;
    class AddrStaticVar;
    class StaticValue;

    class AddrPool{
    public:
        class Scope{
        protected:
            std::map<std::string,Addr*>addrMap;
            Scope * father;
            std::vector<std::unique_ptr<Scope>>sons;
            const int id;
            static int cnt;
        public:
            Scope();
            Scope(const Scope &) = delete ;
            //  Add scope as son of this.
            Scope * addSonScope();
            //  Get father scope of this.
            Scope * getFather() const ;
            //  Get this.
            Scope * getThis() ;
            //  Bind AddrVar to this scope.
            void bindDominateVar(AddrVar * addrvar);
            //  
            /*  Find a AddrVar named `varname` in this scope (this method will not search 
             *  `varname` down to the root).
             *  Return nullptr if not find.
             * */
            Addr * findVarInThisScope(const std::string & varname) const ;
            std::string getIdChain() const ;
        };
    protected:
        std::vector<std::unique_ptr<Addr>>pool;
        std::unique_ptr<Scope>pBlockRoot;
    public:
        AddrPool();
        AddrPool(const AddrPool &) = delete ;
        //  Add AddrVar to some scope.
        AddrLocalVar * addAddrLocalVar(const AddrLocalVar &,Scope * pScope);
        AddrStaticVar * addAddrStaticVar(const AddrStaticVar &,const StaticValue &);
        //  Add an Addr to pool. This is the only way you can create addr;
        Addr * addAddr(const Addr &);
        //  Add a scope as sub of pFather. This is the only way you can create scope.
        Scope * addScope(Scope * pFather);
        //  Get the root scope.
        Scope * getRootScopePointer();
        //  Find var named `varname` from scopes. Search from `pFrom` up to scope root.
        Addr * findAddrDownToRoot(const Scope * pFrom,const std::string & varname);
    };

    class TypeInfo : public LLVMable, public moeconcept::Cloneable{ 
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        enum class Type{
            Float_t, Int_t, FloatArray_t, IntArray_t, Pointer_t, Unknown, Bool_t
        } type;
        explicit TypeInfo(Type type);
    };
    class IntType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        explicit IntType();  
        IntType(const IntType &) = default ;
        virtual std::string toLLVMIR() const override;
    };
    class FloatType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        explicit FloatType();  
        FloatType(const FloatType &) = default ;
        virtual std::string toLLVMIR() const override;
    };
    class IntArrayType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        std::vector<int>shape;
        explicit IntArrayType(const std::vector<int>&shape);
        IntArrayType(const IntArrayType &) = default ;
        virtual std::string toLLVMIR() const override;
    };
    class FloatArrayType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        std::vector<int>shape;
        explicit FloatArrayType(const std::vector<int>&shape);
        FloatArrayType(const FloatArrayType &) = default ;
        virtual std::string toLLVMIR() const override;
    };
    class PointerType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        std::unique_ptr<TypeInfo>pointTo; // This should not be a `PointerType`.
        int pointLevel;
        /*  newOne=false: copy `typeInfo` instance 
         *      typeInfo should be PointerType at this time
         *      At this time, typeInfo.pointTo->type should NOT be `Type::Pointer`
         *  newOne=true : create a new `PointerType`, and :
         *      if `typeInfo` is `PointerType`, 
         *          this->pointLevel=typeInfo.pointLevel+1
         *          *(this->pointTo)=*(typeInfo.pointTo)
         *      if `typeInfo` is not `PointerType`,
         *          this->pointLevel=1
         *          *(this->pointTo)=typeInfo
         * */
        explicit PointerType(const TypeInfo & typeInfo,bool newOne=false);
        virtual std::string toLLVMIR() const override;
    };
    class BoolType : public TypeInfo{
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        explicit BoolType();  
        BoolType(const BoolType &) = default ;
        virtual std::string toLLVMIR() const override;
    };

    class StaticValue : public LLVMable, public moeconcept::Cloneable {
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
        explicit StaticValue();
    public:
        std::unique_ptr<TypeInfo>uPtrInfo;
        explicit StaticValue(const TypeInfo &);
        explicit StaticValue(const StaticValue &);
        StaticValue & operator = (const StaticValue &);
        virtual ~StaticValue(){}
    };
    class FloatStaticValue : public StaticValue { 
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        float value;
        explicit FloatStaticValue(const std::string & literal="0");
        explicit FloatStaticValue(const FloatStaticValue &)=default;
        virtual std::string toLLVMIR() const override;
    };
    class IntStaticValue : public StaticValue { 
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        int value;
        explicit IntStaticValue(const std::string & literal="0");
        explicit IntStaticValue(const IntStaticValue &)=default;
        virtual std::string toLLVMIR() const override;
    };
    class FloatArrayStaticValue : public StaticValue {
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        std::vector<int>shape;
        std::vector<FloatStaticValue>value;
        //  Create new 1-d, `len` long float array.
        explicit FloatArrayStaticValue(int len,const std::vector<FloatStaticValue> & vi);

        /*  Create new array from arrays.
         *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
         *  is `(2,3,4)` array.
         *  This constructor will detect legality of arrays. (They should have same shape.)
         * */
        explicit FloatArrayStaticValue(
            int len,
            const std::vector<int> & preShape,
            const std::vector<FloatArrayStaticValue> & vi
        );
        explicit FloatArrayStaticValue(const FloatArrayStaticValue &)=default;
        virtual std::string toLLVMIR() const override;
    };
    class IntArrayStaticValue : public StaticValue {
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        std::vector<int>shape;
        std::vector<IntStaticValue>value;
        //  Create new 1-d, `len` long float array.
        explicit IntArrayStaticValue(int len,const std::vector<IntStaticValue> & vi);

        /*  Create new array from arrays.
         *  If arrays in `vi` has shape (3,4) and `len` equals 2, new array
         *  is `(2,3,4)` array.
         *  This constructor will detect legality of arrays. (They should have same shape.)
         * */
        explicit IntArrayStaticValue(
            int len,
            const std::vector<int> & preShape,
            const std::vector<IntArrayStaticValue> & vi
        );
        explicit IntArrayStaticValue(const IntArrayStaticValue &)=default;
        virtual std::string toLLVMIR() const override;
    };

    /*  First part of IR design: Address used in three-address-code.
     *  NOTICE: Copy constructor of Addr is deleted.
     * */
    class Addr : public LLVMable{
    public:
        static int cnt;
        const int id;
        Addr();
        Addr(const Addr &)=delete;
        virtual ~Addr(){}
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const = 0;
    };

    /*  Variable address. When construct with constructor, dominator is nullptr.
     *  You need to set dominator manually.
     * */
    class AddrVar : public Addr {
    protected:
        AddrPool::Scope * dominator;
        bool isConst;
        std::string name;
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
    public:
        explicit AddrVar() = delete;
        explicit AddrVar(const std::string & name, const TypeInfo &, bool isConst=false);
        AddrVar(const AddrVar &)=delete;
        virtual ~AddrVar() = default;
        AddrPool::Scope * getDominator() const ;
        void setDominator(AddrPool::Scope * dom);
        bool isconst() const ;
        std::string getVarName() const ;
    };

    /*  Static variable address. Static variable diff with local variable in initialzation.
     *  We have a instruction called `staticdef`, which is used to declare and define a 
     *  static viable. Every static variable has compile time initalization value, which 
     *  is pointed by `uPtrStaticValue`.
     *  When a new instance built, `uPtrStaticValue` is `nullptr`, you need to set it 
     *  manually.
     *  We need to save init value since there may be a case where 'a static is 
     *  initialized by a defined const static', like `const int a[2]={1,2};int x=a[0];`.
     * */
    class AddrStaticVar : public AddrVar{
    protected:
        std::unique_ptr<StaticValue>uPtrStaticValue;
    public:
        explicit AddrStaticVar(const std::string & varname, const TypeInfo & typeInfo, bool isConst=false);
        StaticValue * setStaticValue(const StaticValue &);
        const StaticValue * getStaticValue() const ;
        //  Only print variable name to LLVMIR.
        std::string toLLVMIR() const override;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
    };

    /*  Local variable address. Here is a GREAT difference between local and static variable:
     *  In sysy, static variable can NOT be a pointer, but local variable (and parameter) can be
     *  pointers.
     * */
    class AddrLocalVar : public AddrVar{
    public:
        explicit AddrLocalVar(const std::string & varname,const TypeInfo &,bool isConst=false);
        virtual std::string toLLVMIR() const override;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;

        static int testLocalVarDecl(const std::vector<std::string>&);
    };

    class AddrTemp : public Addr {
    protected:
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
    public:
        explicit AddrTemp() = delete;
        explicit AddrTemp(const TypeInfo &);
        explicit AddrTemp(const AddrTemp &) = delete;

        virtual std::string toLLVMIR() const override;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
    };

    class AddrJumpLabel : public Addr {
    protected:
        std::string name;
    public:
        explicit AddrJumpLabel(const std::string & name="");
        virtual std::string toLLVMIR() const override ;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
    };

    class AddrPara : public Addr {
    protected:
        int number; // number-th parameter
        std::string name;
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
    public:
        explicit AddrPara() = delete;
        explicit AddrPara(const std::string name,const TypeInfo &,int number);
        explicit AddrPara(const AddrPara &) = delete;
        virtual std::string toLLVMIR() const override;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
    };

    class AddrFunction : public Addr {
    protected:
        std::string name;
        std::unique_ptr<TypeInfo>uPtrReturnTypeInfo; // nullptr for void
        std::vector<std::unique_ptr<AddrPara>>vecUPtrAddrPara;
    public:
        explicit AddrFunction() = delete;
        explicit AddrFunction(const std::string & name);
        explicit AddrFunction(const AddrFunction &) = delete;

        TypeInfo * setReturnTypeInfo(const TypeInfo &);
        const TypeInfo * getReturnTypeInfo() const ;

        void pushParameter(const AddrPara &);
        void pushParameter(const std::string &,const TypeInfo &);
        const AddrPara * getNumberThParameterTypeInfo(int) const ;

        virtual std::string toLLVMIR() const override;
        virtual std::unique_ptr<Addr> getSameExceptScopePointerInstance() const override;
    };
}

