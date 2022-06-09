syBaseFilePath="testDir/test"
clang -target armv7m-unknown-unknown-unknown -x c -emit-llvm -fno-discard-value-names -S $syBaseFilePath".sy" -o $syBaseFilePath".ll"
llc $syBaseFilePath".ll" -o $syBaseFilePath".s"