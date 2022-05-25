#pragma once

#include <fstream>
#include <string>
#include <functional>
#include <memory>

#include <common.hpp>

struct EmptyStruct { //  Empty class for good thing.
	virtual ~EmptyStruct() = default;    // To hold poly.
};

namespace moeconcept {

struct Stringable {
	[[nodiscard]] virtual std::string toString() const = 0;
};

/*  Interface of storing object to string.
 *  e.g. :
 *      struct S : public storable{
 *          int x[2];
 *          S(){ x[0]=rand();x[1]=rand(); }
 *          virtual std::string defaultGetContent() override {
 *              return std::to_string(x[0])+","+std::to_string(x[1]);
 *          }
 *          static int Main(){ S s;s.store("x.log");s.store("y.log",&S::defaultContent); }
 *      };
 * */
struct Storable {
	virtual bool store(
			const std::string & filePath,
			std::function<std::string(Storable &)> getContent
	) final {
		std::ofstream ofs(filePath);
		if (!ofs.is_open()) return false;
		ofs << getContent(*this);
		ofs.close();
		return ofs.good();
	}
	
	virtual bool store(
			const std::string & filePath
	) final {
		std::ofstream ofs(filePath);
		if (!ofs.is_open()) return false;
		ofs << defaultGetContent();
		ofs.close();
		return ofs.good();
	}
	
	virtual std::string defaultGetContent() {
		return "Storable::defaultGetContent\n";
	}
};


/*  Cloneable concept.
 *  It is like `clone()` method in Java, but has some difference:
 *  1.  Assume that `DerivedA` is derived from `Base` and `Base` is derived from `Cloneable`.
 *      Assume we have `unique_ptr<Base>pb=make_unique<DerivedA>()`, to call
 *      `pb->cloneToUniquePtr()` without error, you should have implemented
 *      `DerivedA::_cloneToUniquePtr()`. So this `Cloneable` ensures that, the generated
 *      instance has same type of copied one.
 *  2.  Since `cloneToUniquePtr()` returns `unique_ptr<Cloneable>`, you should convert it
 *      to other types manually by calling `com::dynamic_cast_unique_ptr`. (See common.hpp)
 *  3.  If you use `make_unique(*this)` as implementation method, notice to define copy
 *      constructor (since in our project, `std::unique_ptr` is always a member, which does NOT
 *      have a copy constructor, and will delete the default copy constructor of you class).
 * */
class Cloneable {
  protected:
	[[nodiscard]] virtual std::unique_ptr<Cloneable> _cloneToUniquePtr() const = 0;
  
  public:
	[[nodiscard]] virtual std::unique_ptr<Cloneable> cloneToUniquePtr() const final {
		std::unique_ptr<Cloneable> clonedUniquePtr = _cloneToUniquePtr();
		Cloneable * clonedPtr = clonedUniquePtr.get(); /* NOLINT */ // For `-Wpotentially-evaluated-expression`.
		if (typeid(*clonedPtr) != typeid(*this)) {
			com::Throw(com::concatToString({
					                               "Calling `__cloneToUniquePtr`method from type [",
					                               typeid(*clonedPtr).name(),
					                               "], which is different to the type of `this` [",
					                               typeid(*this).name(),
					                               "]. Check if you have implemented `__cloneToUniquePtr` method of type",
					                               typeid(*this).name(), " first."}));
		}
		return clonedUniquePtr;
	}
	
	virtual ~Cloneable() = default;
};
}
