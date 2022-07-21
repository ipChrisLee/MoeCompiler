#!/bin/bash
source ./scripts/shScripts/settings.sh

clang -Xclang -disable-O0-optnone -O0 -S -x c -emit-llvm \
	-include support/syLibFiles/sylib.h \
	-target armv7a-unknown-linux-gnueabihf -march=armv7-a \
	-fno-discard-value-names -mfloat-abi=hard \
	"${syBaseFilePath}".sy -o "${syBaseFilePath}".ll || touch "${syBaseFilePath}".ll

opt -S "${syBaseFilePath}".ll -o "${syBaseFilePath}".m2r.ll -debug-pass=Executions \
	-reg2mem -mem2reg

#opt -S "${syBaseFilePath}"".ll" -o "${syBaseFilePath}"".O2.ll" -debug-pass=Arguments -O2

llc "${syBaseFilePath}".ll -o "${syBaseFilePath}".s

sed -i '1 i\\t.arch armv7a' "${syBaseFilePath}".s
