gcc-7 -march=armv7-a -O0 -g \
	mySyLib.s test.s -o main
./main < test.in > buffer.txt
