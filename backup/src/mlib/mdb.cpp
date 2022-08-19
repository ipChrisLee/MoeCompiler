#include <memory>

#include "mdb.hpp"
#include "cprt.hpp"


bool com::mdb::sysEnable = false;
com::mdb com::ccdbg(
		std::make_unique<com::cprt>(
				std::cout, com::color::blue), true, "[ccdbg] "
);

