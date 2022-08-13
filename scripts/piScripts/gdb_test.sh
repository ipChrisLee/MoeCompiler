gcc-7 -march=armv7-a -O0 -g \
	-Wl,--whole-archive libsysy.a -Wl,--no-whole-archive \
	test.s -o main
gdb -q -ex 'set args < test.in > buffer.txt' \
	-ex 'layout asm' -ex 'layout reg' \
	-ex 'break main' ./main
