syFilePath="testDir/test.sy"
export CLASSPATH=".:/usr/local/lib/antlr-4.10.1-complete.jar:$CLASSPATH"
export PATH=$PATH:/opt/riscv/bin
cd src/third_party/JavaGrunSupport && java org.antlr.v4.gui.TestRig SysY compUnit -gui < ../../../"$syFilePath"