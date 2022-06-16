syBaseFilePath="testDir/test"

clang -Xclang -disable-O0-optnone -O0 -S -x c -emit-llvm \
  -target armv7m-unknown-unknown-unknown \
  -fno-discard-value-names \
  $syBaseFilePath".sy" -o $syBaseFilePath".ll"

opt -S -mem2reg $syBaseFilePath".ll" -o $syBaseFilePath".m2r.ll"

llc $syBaseFilePath".ll" -o $syBaseFilePath".s"
