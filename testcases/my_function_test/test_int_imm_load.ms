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
	.file	"test_int_imm_load.sy"
	.text
	.section	.text.startup,"ax",%progbits
	.align	1
	.p2align 2,,3
	.global	main
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, lr}
	mov	r0, #1032
	bl	putint(PLT)
	movw	r0, #1548
	bl	putint(PLT)
	mov	r0, #516
	bl	putint(PLT)
	movs	r0, #0
	bl	putint(PLT)
	movs	r0, #0
	pop	{r3, pc}
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
