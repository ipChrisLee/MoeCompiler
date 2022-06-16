#pragma once

#include <string>
#include <fstream>

#include "common.hpp"
#include "mdb.hpp"

namespace SysY {

/**
 * @brief Read from command line arguments, saved in variables in @c SysY .
 */
void parseArgs(int argc, char ** argv);


extern std::ifstream source;
extern std::ofstream dest;

template<typename OptionValT, OptionValT DefaultVal>
class Option {
  protected:
	OptionValT val;
  public:
	Option() : val(DefaultVal) { }
	
	[[nodiscard]] virtual OptionValT get() const { return val; }
	
	virtual void set(OptionValT newVal) { val = newVal; }
	
	virtual ~Option() = default;
};

class Options {
  public:
	Options() = default;
	
	struct Debug : public Option<bool, false> {
		void set(bool newVal) override {
			val = newVal;
			com::mdb::setSysEnable() = val;
		}
	} debug;
	
	struct OptimizationLevel : public Option<int, 0> {
	} optimizationLevel;
	
	struct Verbose : public Option<bool, false> {
	} verbose;
	
	struct EmitLLVM : public Option<bool, false> {
	} emitLLVM;
	
	struct FloatDecFormat : public Option<bool,false>{
	} floatDecFormat;
};

extern Options options;

}