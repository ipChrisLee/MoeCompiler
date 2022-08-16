#pragma once
#include <string>

#include <common.hpp>
#include <SysY.hpp>

namespace sup {
struct LLVMable {
	[[nodiscard]] virtual std::string toLLVMIR() const = 0;
	
	// [[nodiscard]] virtual std::string toLLVMIRWithoutType() const {
	// 	com::addRuntimeWarning(
	// 		com::concatToString(
	// 			{
	// 				"Type [", typeid(*this).name(),
	// 				"] does NOT implement `toLLVMIRWithoutType`. Using `toLLVMIR` instead."
	// 			}
	// 		), CODEPOS
	// 	);
	// 	return toLLVMIR();
	// }
	
	virtual ~LLVMable() = default;
};
}

namespace sup {

std::string floatToString(float f);

std::string intToString(int i);

int literalToInt(const std::string &);

float literalToFloat(const std::string &);

}
