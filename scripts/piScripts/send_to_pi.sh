#!/bin/bash
files="test.py"
pi="pi@192.168.0.101"
projInPi="~/Proj/MoeCompiler"

scp -r ${files} ${pi}:${projInPi}
