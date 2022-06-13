#pragma once

#include <string>

#include <common.hpp>

namespace ircode {
struct LLVMable {
	[[nodiscard]] virtual std::string toLLVMIR() const = 0;
	
	virtual ~LLVMable() = default;
};

std::string floatToString(float f);

std::string intToString(int i);
}
