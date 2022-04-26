#!/bin/bash

if [ $1 == "-h" ];then
    echo Usage example : ./makeSysY.sh test -O2
    echo
    exit 0
fi

clang -x c -std=c11 -fno-discard-value-names -target armv7 -emit-llvm $1.sy -S $2 -o $1.ll
llc $1.ll -o $1.s
