#include <string>
#include <set>
#include <sysy.hpp>

const std::set<std::string>sysy::reservedWords={
    "void","int","float",
    "while","if","break","continue","return",
    "const",
};

std::unique_ptr<sysy> sysy::single=nullptr;
