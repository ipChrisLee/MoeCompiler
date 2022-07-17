//
// Created by lee on 6/27/22.
//

#ifdef TESTCODE

#include "gtest/gtest.h"

#include <moeconcept.hpp>

class Base : public moeconcept::Cutable {
  protected:
	[[nodiscard]] std::unique_ptr<Cutable> _cutToUniquePtr() override {
		return std::make_unique<Base>(std::move(*this));
	}
  
  public:
	std::unique_ptr<int> pi;
	
	Base() = default;
	
	explicit Base(int x) : pi(std::make_unique<int>(x)) {
	}
	
	Base(Base &&) = default;
	
	virtual std::string toString() const {
		std::string res;
		if (pi) {
			res = "Base{ pi->" + std::to_string(*pi) + " }";
		} else {
			res = "Base{ pi=nullptr }";
		}
		return res;
	}
	
	friend std::ostream & operator <<(std::ostream & os, const Base & ba) {
		return os << ba.toString();
	}
};

class DerivedA : public Base {
  protected:
	[[nodiscard]] std::unique_ptr<Cutable> _cutToUniquePtr() override {
		return std::make_unique<DerivedA>(std::move(*this));
	}
  
  public:
	std::unique_ptr<int> pi;
	
	DerivedA() = default;
	
	explicit DerivedA(int x) : Base(x), pi(std::make_unique<int>(x)) {
	}
	
	DerivedA(DerivedA &&) = default;
	
	[[nodiscard]] std::string toString() const override {
		std::string res;
		if (pi) {
			res = "DerivedA{pi->" + std::to_string(*pi) + ", ";
		} else {
			res = "Base{pi=nullptr, ";
		}
		res += "father=" + Base::toString() + " }";
		return res;
	}
};

class DerivedB : public Base {
  protected:
	[[nodiscard]] std::unique_ptr<Cutable> _cutToUniquePtr() override {
		return std::make_unique<DerivedB>(std::move(*this));
	}
  
  public:
	std::unique_ptr<int> pi;
	
	DerivedB() = default;
	
	explicit DerivedB(int x) : Base(x), pi(std::make_unique<int>(x)) {
	}
	
	DerivedB(DerivedB &&) = default;
	
	[[nodiscard]] std::string toString() const override {
		std::string res;
		if (pi) {
			res = "DerivedB{pi->" + std::to_string(*pi) + ", ";
		} else {
			res = "Base{pi=nullptr, ";
		}
		res += "father=" + Base::toString() + " }";
		return res;
	}
	
};

TEST(TestCutable, Test1) {
	auto pBa1 = std::make_unique<Base>(1);
	std::cout << *pBa1 << std::endl;
	auto pBa2 = com::dynamic_cast_uPtr<Base>(com::cutToUniquePtr(std::move(*pBa1)));
	std::cout << *pBa1 << std::endl;
	std::cout << *pBa2 << std::endl;
	auto pDerA1 = com::dynamic_cast_uPtr<Base>(std::make_unique<DerivedA>(-1));
	std::cout << *pDerA1 << std::endl;
	auto pDerA2 =
		com::dynamic_cast_uPtr<Base>(com::cutToUniquePtr(std::move(*pDerA1)));
	std::cout << *pDerA1 << std::endl;
	std::cout << *pDerA2 << std::endl;
	auto pDerB1 = com::dynamic_cast_uPtr<Base>(std::make_unique<DerivedB>(-2));
	std::cout << *pDerB1 << std::endl;
	auto pDerB2 =
		com::dynamic_cast_uPtr<Base>(com::cutToUniquePtr(std::move(*pDerB1)));
	std::cout << *pDerB1 << std::endl;
	std::cout << *pDerB2 << std::endl;
}


#endif

