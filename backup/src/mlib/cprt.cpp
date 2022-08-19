#include "cprt.hpp"

com::cprt::TOS com::cprt::tos = cprt::TOS();

namespace com {
namespace color {
const std::string black = "\033[0;30m";
const std::string red = "\033[0;31m";
const std::string err = red;
const std::string green = "\033[0;32m";
const std::string fin = green;
const std::string yellow = "\033[0;33m";
const std::string iterct = yellow;
const std::string blue = "\033[0;34m";
const std::string purple = "\033[0;35m";
const std::string cyan = "\033[0;36m";
const std::string working = cyan;
const std::string bpurple = "\033[1;35m";
const std::string warning = bpurple;
const std::string noc = "\033[0m";
}
cprt ccout(std::cout, color::iterct);
cprt ccerr(std::cerr, color::err);
const std::string nullFilePath("/nul/dev");
cprt cnull(nullFilePath);
}
