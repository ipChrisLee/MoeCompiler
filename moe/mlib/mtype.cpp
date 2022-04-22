#include "mtype.hpp"
#include "submain.hpp"
#include "cprt.hpp"

namespace mtype{
quat_t qFalse(quat_t::QValue::False);
quat_t qTrue(quat_t::QValue::True);
quat_t qUndefined(quat_t::QValue::Undefined);
quat_t qChaos(quat_t::QValue::Chaos);
}


AddSubMain(testquat_t,
    [](const std::vector<std::string>&)->int{
        using namespace mtype;
        quat_t qT=qTrue;
        quat_t qF=qFalse;
        quat_t qU=qUndefined;
        quat_t qC=qChaos;
        com::ccout.cprintLn(std::tuple(qF," ",qT," ",qU," ",qC));
        com::ccout.cprintLn(std::tuple(qF&&qT," ",qF&&qF," ",qT&&qT," ",qU&&qT," ",qC&&qT));
        return 0;
    }
);
