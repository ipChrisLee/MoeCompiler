#!/bin/bash

if [ $1 == "-h" ];then
    echo Usage example : ./makeSysY.sh test -O2
    echo
    exit 0
fi

clang -fno-discard-value-names -target armv7 -Xclang -disable-O0-optnone -O0 -emit-llvm $1.c -S $2 -o $1_default.ll
opt -S -mem2reg $1_default.ll -o $1_mem2reg.ll
opt -S -reg2mem $1_default.ll -o $1_reg2mem.ll
llc $1_default.ll -o $1.s
