#!/bin/bash

export CLASSPATH=".:/usr/local/lib/antlr-4.10.1-complete.jar:$CLASSPATH"xport PATH=$PATH:/opt/riscv/bin
cp CommonLex.g4 SysY.g4 JavaGrunSupport/
cd JavaGrunSupport;java -Xmx500M -cp "/usr/local/lib/antlr-4.10.1-complete.jar:$CLASSPATH" org.antlr.v4.Tool SysY.g4;javac SysY*.java
