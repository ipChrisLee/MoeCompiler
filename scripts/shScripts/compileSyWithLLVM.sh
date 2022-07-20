#!/bin/bash
syBaseFilePath="testDir/testSyFile/test"

set -x
clang -Xclang -disable-O0-optnone -O0 -S -x c -emit-llvm \
  -target armv7a-unknown-unknown-unknown \
  -fno-discard-value-names \
  $syBaseFilePath".sy" -o $syBaseFilePath".ll" || touch $syBaseFilePath".ll"

opt -S $syBaseFilePath".ll" -o $syBaseFilePath".m2r.ll" -debug-pass=Executions \
  -reg2mem -mem2reg

#opt -S $syBaseFilePath".ll" -o $syBaseFilePath".O2.ll" -debug-pass=Arguments -O2

llc --float-abi=hard \
  $syBaseFilePath".ll" -o $syBaseFilePath".s"
