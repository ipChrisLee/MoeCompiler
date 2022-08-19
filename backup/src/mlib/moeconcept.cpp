#include "moeconcept.hpp"

namespace com {

using moeconcept::Cutable;

[[nodiscard]] std::unique_ptr<Cutable> cutToUniquePtr(Cutable && o) {
	std::unique_ptr<Cutable> cutOPtr = o._cutToUniquePtr();
	Cutable * cutPtr =
		cutOPtr.get(); /* NOLINT */ // For `-Wpotentially-evaluated-expression`.
	if (typeid(*cutPtr) != typeid(o)) {
		com::Throw(
			com::concatToString(
				{
					"Calling `_cutToUniquePtr`method from type [",
					typeid(*cutPtr).name(),
					"], which is different to the type of `this` [",
					typeid(o).name(),
					"]. Check if you have implemented `_cutToUniquePtr` method of type",
					typeid(o).name(),
					" first."}
			)
		);
	}
	return cutOPtr;
}
}


