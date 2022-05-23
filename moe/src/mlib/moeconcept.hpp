#pragma once 

#include <fstream>
#include <string>
#include <functional>
#include <memory>

#include <common.hpp>

struct EmptyStruct{ //  Empty class for good thing.
    virtual ~EmptyStruct(){}    // To hold poly.
};   

namespace moeconcept{

    struct Stringable{
        virtual std::string toString() const = 0;
    };

    /*  Interface of storing object to string.
     *  e.g. : 
     *      struct S : public storable{
     *          int x[2];
     *          S(){ x[0]=rand();x[1]=rand(); }
     *          virtual std::string defaultGetContent() override {
     *              return std::to_string(x[0])+","+std::to_string(x[1]);
     *          }
     *          static int Main(){ S s;s.store("x.log"); }
     *      };
     * */
    struct Storable{
        virtual bool store(
            const std::string & filePath,
            std::function<std::string(Storable&)>getContent=&Storable::defaultGetContent
        ) final {
            std::ofstream ofs(filePath);
            if(!ofs.is_open()) return false;
            ofs<<getContent(*this);
            ofs.close();
            return ofs.good();
        }
        virtual std::string defaultGetContent(){
            return "Storable::defaultGetContent\n";
        }
    };


    /*  Cloneable concept.
     *  It is like `clone()` method in Java, but has some difference:
     *  1.  Assume that `DerivedA` is derived from `Base` and `Base` is derived from `Cloneable`.
     *      Assume we have `unique_ptr<Base>pb=make_unique<DerivedA>()`, to call 
     *      `pb->cloneToUniquePtr()` without error, you should have implemented
     *      `DerivedA::__cloneToUniquePtr()`. So this `Cloneable` ensures that, the generated
     *      instance has same type of copied one.
     *  2.  Since `cloneToUniquePtr()` returns `unique_ptr<Cloneable>`, you should convert it
     *      to ohter types manually by calling `com::dynamic_cast_unique_ptr`. (See common.hpp)
     *  3.  If you use `make_unique(*this)` as implementation method, notice to define copy
     *      constructor (since in our project, `std::unique_ptr` is always a member, which does NOT
     *      have a copy constructor, and will delete the default copy constructor of you class).
     * */
    class Cloneable {
    protected:
        virtual std::unique_ptr<Cloneable> __cloneToUniquePtr() const = 0;
    public:
        virtual std::unique_ptr<Cloneable> cloneToUniquePtr() const final {
            std::unique_ptr<Cloneable> clonedUniquePtr=__cloneToUniquePtr();
            auto clonedPtr=clonedUniquePtr.get(); // For `-Wpotentially-evaluated-expression`.
            if(typeid(*clonedPtr)!=typeid(*this)) {
                com::Throw(com::concatToString({
                    "Calling `__cloneToUniquePtr`method from type [",typeid(*clonedPtr).name(),
                    "], which is different to the type of `this` [",typeid(*this).name(),
                    "]. Check if you have implemented `__cloneToUniquePtr` method of type",
                    typeid(*this).name()," first."}));
            }
            return clonedUniquePtr;
        }
        virtual ~Cloneable(){}
    };



    /*  Ref: https://stackoverflow.com/a/55076727/17924585
     *  This is deleted since it is too difficult to use.
     *  This is a interface you can use when need to cast a truely cloned instance from another one.
     *  For example : 
     *    Decl : 
     *      struct Base : public Clonable<Base,EmptyStruct> {
     *          //  just normal fields. NO need for decl of `cloneToUniquePtr` method.
     *          virtual ~Base(){}   //  For polymorphism.
     *      };
     *      struct DerivedA : virtual public Base, public Clonable<DerivedA,Base> { 
     *          //  DerivedA is derived from Base via `Clonable<DerivedA,Base>`.
     *          //  just normal fields.
     *      };
     *      struct DDerivedA : virtual public DerivedA, public Clonable<DDerivedA,Base> {
     *          // just normal fileds.
     *      };
     *      struct DerivedB : virtual public Base, public Clonable<DerivedB,Base> {
     *          // just normal fields.
     *      };
     *    Usage : 
     *      void fun(const Base & base){
     *          std::unique_ptr<Base>pb(pb.cloneToSharedPtr());
     *              //  Move constructor of unique_ptr.
     *              //  And it will deep copy, which means `pb` will head to proper class
     *              //  according to the type `base` refer to.
     *      }
     *  Notice :  
     *      Copy constructor of is needed for ALL classes.
     *      Default constructor (i.e. Constructor with no parameter) of Base class is visable to
     *      `Clonable`.
     *          This is because, if `Base` class has no default constructor, the constructor of 
     *          `Clonable` will be deleted, and make it more complicated to write code.
     *          But there will be no memory waste, since you use `virtual public` inherit.
     *      For inheritance chain B<-A<-C, you should make C inheriting from `Clonable<C,A>`,
     *      and A should inherite from `Clonable<B,A>`.
     * */
    /*  Deleted.
    template<typename Derived,typename Base>
    struct Clonable : virtual public Base {
        virtual Base * __do_copy() const {
            return new Derived(*static_cast<const Derived*>(this));
        }
        std::unique_ptr<Derived> cloneToUniquePtr() const {
            std::unique_ptr<Derived>pDerived(dynamic_cast<Derived*>(__do_copy()));
            assert(pDerived);
            return std::move(pDerived);
        }
        std::shared_ptr<Derived> cloneToSharedPtr() const {
            std::shared_ptr<Derived>pDerived(dynamic_cast<Derived*>(__do_copy()));
            assert(pDerived);
            return std::move(pDerived);
        }
        using Base::Base;
    };
    */

}
