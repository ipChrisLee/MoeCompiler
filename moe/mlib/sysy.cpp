#include <string>
#include <set>
#include <sysy.hpp>

const std::set<std::string>sysy::reservedWords={
    "void","int","float",
    "while","if","else","break","continue","return",
    "const",
};

const std::set<std::string>sysy::operators={
    "+","-","*","/","%",
    ">","<","=","!",
    "<=",">=","!=","==",
    "&&","||",
};

const std::set<std::string>sysy::delimiter={
    "{","}","[","]","(",")",
    ";",","
};

const std::set<char>sysy::blanks={
    '\n','\t','\b'
};

std::unique_ptr<sysy> sysy::single=nullptr;
