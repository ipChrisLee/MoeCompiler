	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"lots_of_parameters_01.sy"
	.text
	.align	1
	.p2align 2,,3
	.global	sum
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	sum, %function
sum:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r1, #0
	ble	.L4
	add	r1, r0, r1, lsl #2
	mov	r3, r0
	movs	r0, #0
.L3:
	ldr	r2, [r3], #4
	cmp	r3, r1
	add	r0, r0, r2
	bne	.L3
	bx	lr
.L4:
	movs	r0, #0
	bx	lr
	.size	sum, .-sum
	.section	.text.startup,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r0, #15
	bx	lr
	.size	main, .-main
	.comm	_sysy_idx,4,4
	.comm	_sysy_us,4096,4
	.comm	_sysy_s,4096,4
	.comm	_sysy_m,4096,4
	.comm	_sysy_h,4096,4
	.comm	_sysy_l2,4096,4
	.comm	_sysy_l1,4096,4
	.comm	_sysy_end,8,4
	.comm	_sysy_start,8,4
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",%progbits
