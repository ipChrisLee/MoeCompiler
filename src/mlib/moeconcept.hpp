#pragma once

#include <fstream>
#include <string>
#include <functional>
#include <memory>
#include <type_traits>

#include "common.hpp"


namespace moeconcept {
class Cutable;
}

namespace com {
[[nodiscard]] std::unique_ptr<moeconcept::Cutable>
cutToUniquePtr(moeconcept::Cutable && o);
}

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
		if (!ofs.is_open()) { return false; }
		ofs << getContent(*this);
		ofs.close();
		return ofs.good();
	}

	virtual bool store(
		const std::string & filePath
	) final {
		std::ofstream ofs(filePath);
		if (!ofs.is_open()) { return false; }
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
 *      UPD: for now, calling `com::cloneable_cast_uPtr` is also OK.
 *  3.  If you use `make_unique(*this)` as implementation method, notice to define copy
 *      constructor (since in our project, `std::unique_ptr` is always a member, which does NOT
 *      have a copy constructor, and will delete the default copy constructor of you class).
 * */
class Cloneable {
  protected:
	[[nodiscard]] virtual std::unique_ptr<Cloneable>
	_cloneToUniquePtr() const = 0;

  public:
	[[nodiscard]] virtual std::unique_ptr<Cloneable>
	cloneToUniquePtr() const final {
		std::unique_ptr<Cloneable> clonedUniquePtr = _cloneToUniquePtr();
		// For `-Wpotentially-evaluated-expression`.
		Cloneable * clonedPtr = clonedUniquePtr.get();//NOLINT
		if (typeid(*clonedPtr) != typeid(*this)) {
			com::Throw(
				com::concatToString(
					{
						"Calling `_cloneToUniquePtr`method from type [",
						typeid(*clonedPtr).name(),
						"], which is different to the type of `this` [",
						typeid(*this).name(),
						"]. Check if you have implemented `_cloneToUniquePtr` method of type",
						typeid(*this).name(),
						" first."
					}
				));
		}
		return clonedUniquePtr;
	}

	virtual ~Cloneable() = default;
};

class Cutable {
  protected:
	[[nodiscard]] virtual std::unique_ptr<Cutable> _cutToUniquePtr() = 0;

  public:
	virtual ~Cutable() = default;

	friend std::unique_ptr<Cutable> com::cutToUniquePtr(Cutable && o);
};

#define CUTABLE_DEFAULT_IMPLEMENT { return std::make_unique<std::remove_reference<decltype(*this)>::type>(std::move(*this));}
#define CLONEABLE_DEFAULT_IMPLEMENT { return std::make_unique<std::remove_const<std::remove_reference<decltype(*this)>::type>::type>(*this);}

template<typename Base>
class Pool {
  protected:
	std::vector<std::unique_ptr<Base>> pool;
	std::function<void(Base *)> afterAdd;
  public:
	explicit Pool(
		std::function<void(Base *)> afterAdd = std::function<void(Base *)>()
	) : afterAdd(afterAdd) {}

	Pool(Pool<Base> &) = delete;

	virtual ~Pool() = default;

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<Base, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(std::unique_ptr<T> && instance) {
		pool.template emplace_back(std::move(instance));
		auto * p = pool.rbegin()->get();
		if (afterAdd) { afterAdd(p); }
		return dynamic_cast<T *>(p);
	}

	template<
		typename T,
		class = typename std::enable_if<
			!std::is_lvalue_reference<T>::value && std::is_base_of<Base, T>::value
		>::type
	>
	[[nodiscard]] T * emplace_back(T && instance) {
		pool.template emplace_back(
			std::make_unique<T>(std::forward<T>(instance))
		);
		auto * p = pool.rbegin()->get();
		if (afterAdd) { afterAdd(p); }
		return dynamic_cast<T *>(p);
	}

};

}

namespace com {

/**
 * @brief
 * Dynamic_cast for @c unique_ptr from @c moeconcept::Cloneable.
 * @deprecated
 * Not @c com::dynamic_cast_uPtr can do the same thing, but is more general.
 */
template<typename To>
[[deprecated("Use com::dynamic_cast_uPtr instead.")]]
std::unique_ptr<To>
cloneable_cast_uPtr(std::unique_ptr<moeconcept::Cloneable> && fromP) {
	//  return nullptr if source pointer is nullptr.
	if (!fromP) { return std::unique_ptr<To>(nullptr); }
	To * p = dynamic_cast<To *>(fromP.release());
	Assert(
		p, concatToString(
			{
				"dynamic_cast_uPtr failed. From [moeconcept::Cloneable] to [",
				typeid(To).name(), "*]."
			}
		));
	return std::unique_ptr<To>(p);
}

}