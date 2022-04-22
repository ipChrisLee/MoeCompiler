#include "cprt.hpp"

com::cprt::TOS com::cprt::tos=cprt::TOS();

namespace com{
    const std::string cprt::black=   "\033[0;30m";
    const std::string cprt::red=     "\033[0;31m";
    const std::string cprt::err=     "\033[0;31m";
    const std::string cprt::green=   "\033[0;32m";
    const std::string cprt::fin=     "\033[0;32m";
    const std::string cprt::yellow=  "\033[0;33m";
    const std::string cprt::iterct=  "\033[0;33m";
    const std::string cprt::blue=    "\033[0;34m";
    const std::string cprt::purple=  "\033[0;35m";
    const std::string cprt::cyan=    "\033[0;36m";
    const std::string cprt::working= "\033[0;36m";
    const std::string cprt::bpurple= "\033[1;35m";
    const std::string cprt::warning= "\033[1;35m";
    const std::string cprt::noc=     "\033[0m";
    cprt ccout(std::cout,cprt::iterct);
    cprt ccerr(std::cerr,cprt::err);
    const std::string nullFilePath("/nul/dev");
    cprt cnull(nullFilePath);
}
