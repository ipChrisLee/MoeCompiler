#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <iostream>

#include "common.hpp"
#include "submain.hpp"
#include "cprt.hpp"

std::unique_ptr<com::submain> com::submain::single(nullptr);


static int listallWithPositions(std::vector<std::string>){
    com::ccout.cprintLn("Available submain : {");
    auto mp=com::submain::getSingle().getCases();
    for(auto & it:mp){
        com::ccout.cprint(std::tuple("    ",it.first," { "));
        for(auto & pos:it.second.positions){
            com::ccout.cprint(std::tuple("(",pos,") "));
        }
        com::ccout.cprintLn("} ");
    }
    com::ccout.cprintLn("} ");
    return 0;
}

AddSubMain(listallWithPositions,listallWithPositions);

AddSubMain(listall,
    [](std::vector<std::string>)->int{
        com::ccout.cprintLn("Available submain : {");
        com::ccout.cprint("    ");
        auto mp=com::submain::getSingle().getCases();
        for(auto & it:mp){
            com::ccout.cprint(std::tuple(it.first," "));
        }
        com::ccout.cprintLn("");
        com::ccout.cprintLn("} ");
        return 0;
    }
);

