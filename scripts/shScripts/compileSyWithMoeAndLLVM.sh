syBaseFilePath="testDir/testSyFile/test"
moe="cmake-build-debug/compiler"

${moe} ${syBaseFilePath}".sy" -S -o ${syBaseFilePath}".mll" --emit-llvm

llc $syBaseFilePath".mll" -o $syBaseFilePath".ms"
