#pragma once

#include <string>
#include <vector>
#include <list>
#include <functional>

#include <common.hpp>
#include <moeconcept.hpp>

#include "frontend/IRAddr.hpp"
#include "frontend/frontendHeader.hpp"


#ifdef CODING
namespace ircode{

    class IRInstr;
    
    class BasicBlock : public LLVMable{
    protected:
        std::list<IRInstr*>instrList;
    public:
        explicit BasicBlock() = default;
        explicit BasicBlock(const BasicBlock &) = delete;

        virtual std::string toLLVMIR() const override;
    };

    class FuncField : public LLVMable {
    protected:
        std::list<IRInstr*>instrList;
        std::list<BasicBlock*>blockInstrList;
        std::vector<AddrPara*>args;
        bool blocked;
        bool isStatic;
        
    public:
        explicit FuncField(bool isStatic);
        explicit FuncField(const FuncField *)=delete;

        IRInstr * pushInstrOnBack(IRInstr *);

        bool rangeAsBlocked() const ;

        virtual std::string toLLVMIR() const override;
    };

    class IRInstrPool : public LLVMable {
    public:
    protected:
        std::vector<std::unique_ptr<IRInstr>>instrPool;
        std::vector<std::unique_ptr<FuncField>>funcPool;
        std::vector<std::unique_ptr<BasicBlock>>blockPool;
    public:
        explicit IRInstrPool();
        explicit IRInstrPool(const IRInstrPool &)=delete;

        FuncField * addEmptyFuncField() ;
        FuncField * getStaticFuncFieldPtr() const ;

        IRInstr * addIRInstr(const IRInstr &,FuncField *);

        virtual std::string toLLVMIR() const override;
    };

    /*  class for IR instruction.
     *  Notice that, `IRInstr` has no control of pointers it saves, which means you should
     *  always save addresses in `AddrPool`, and then get Addr* from `AddrPool`.
     * */
    class IRInstr : public LLVMable, public moeconcept::Cloneable {
    protected:
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    protected:
        int id;
        static int cnt;
    public:
        explicit IRInstr():id(++cnt){}
    };

    
    /*  `RET` instruction.
     *  LLVM-IR Format :
     *      ret <type> <value>
     *      ret void
     *  NOTICE : 
     *      For void return, retAddr=nullptr.
     * */
    class IRInstr_RET : public IRInstr{
    protected:
        Addr * retAddr;
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        explicit IRInstr_RET(Addr * pAddrRet=nullptr);
        
        virtual std::string toLLVMIR() const override;
        
    };

    /*  `CONDBR` instruction. (conditional br)
     *  LLVM-IR Format :
     *      br i1 <cond>, label <iftrue>, label <iffalse>
     *  NOTICE : 
     *      For unconditional jump, use `br i1 1, label L, label L` instead.
     *      <cond> should be `i1` (i.e. `Bool_t` in `TypeInfo::Type`).
     * */
    class IRInstr_CONDBR : public IRInstr {
    protected:
        Addr * condAddr;
        AddrJumpLabel * labelTrue,labelFalse;
        virtual std::unique_ptr<moeconcept::Cloneable> __cloneToUniquePtr() const override;
    public:
        explicit IRInstr_CONDBR(Addr * condAddr,AddrJumpLabel * labelTrue,AddrJumpLabel * labelFalse);

        virtual std::string toLLVMIR() const override;
    };

    /*  `BINARY OP` instruction.
     * */
    class IRInstr_BOP : public IRInstr{
    protected:
        AddrOperand * leftOperand, * rightOperand;
    public:
        class BOP : public LLVMable {
        public:
            using CalFun_t=std::function<StaticValue(const StaticValue &,const StaticValue &)>;
        protected:
            std::string instrName;
            CalFun_t calFun;
        public:
            explicit BOP(const std::string &, const CalFun_t &);
            virtual std::string toLLVMIR() const override;
        };
    };

    /*  `ALLOCA` instruction.
     *  LLVM-IR Format : 
     *      <result> = alloca <type> , align <alignment>
     * */
    class IRInstr_ALLOC : public IRInstr {
    protected:
        AddrMemVar * result;
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
    public:
        virtual std::string toLLVMIR() const override;
    };

    /*  `LOAD` instruction.
     *  LLVM-IR Format : 
     *      <result> = load <type>, <type> * <pointer>, align <alignment>
     * */
    class IRInstr_LOAD : public IRInstr {
    protected:
        AddrOperand * result;
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
    };

    /*  `STORE` instruction.
     *  LLVM-IR Format : 
     *      store <type> <value>, <type> * <dest>
     * */
    class IRInstr_STORE : public IRInstr {
    protected:
        std::unique_ptr<TypeInfo>uPtrTypeInfo;
        AddrOperand * pValue;
        AddrMemVar * pDest;
    };

    /*  `CONVERSION` instruction.
     *  LLVM-IR Format : 
     *      <result> = <conversion_fun> <typeSrc> <value> to <typeDest>
     * */
    class IRInstr_CONVERSION : public IRInstr {
    protected:
        std::unique_ptr<TypeInfo>uPtrTypeSrc,uPtrTypeDest;
        AddrOperand * pValue, * pResult;
        
    };

    /*  `COMPARISION` instruction.
     *  LLVM-IR Format : 
     *      <result> = <"icmp"|"fcmp"> <cond> <type> <op1>, <op2>
     * */
    class IRInstr_COMP : public IRInstr {
    protected:
        AddrOperand * result, * opLeft, * opRight;
        std::unique_ptr<TypeInfo>uPtrType;
    };

    /*  `CALL` instruction.
     *  LLVM-IR Format : 
     *      <result> = <ty> <function> (<function args>)
     * */
    class IRInstr_CALL : public IRInstr {
    protected:
        AddrOperand * result;
        std::unique_ptr<TypeInfo>uPtrReturnType;
        AddrFunction * functionPointer;
        std::vector<AddrOperand*>args;
    };

}
#endif
