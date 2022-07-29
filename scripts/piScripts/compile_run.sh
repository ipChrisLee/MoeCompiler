gcc-7 -march=armv7-a -O0 \
	-Wl,--whole-archive libsysy.a -Wl,--no-whole-archive \
	test.s -o main
./main < test.in > buffer.txt
