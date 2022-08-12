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
	.file	"rand_gen.sy"
	.text
	.align	1
	.p2align 2,,3
	.global	valToBit
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	valToBit, %function
valToBit:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L9
	cmp	r0, #0
	ldr	r2, .L9+4
.LPIC0:
	add	r3, pc
	ldr	r1, [r3, r2]
	sub	r2, r1, #4
	add	r1, r1, #124
	blt	.L3
.L5:
	and	r3, r0, #1
	str	r3, [r2, #4]!
	cmp	r2, r1
	asr	r0, r0, #1
	bne	.L5
	bx	lr
.L3:
	subs	r3, r0, #1
	cmp	r0, #0
	and	r0, r0, #1
	it	lt
	rsblt	r0, r0, #0
	add	r3, r3, r3, lsr #31
	str	r0, [r2, #4]!
	cmp	r1, r2
	asr	r0, r3, #1
	bne	.L3
	bx	lr
.L10:
	.align	2
.L9:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC0+4)
	.word	bit(GOT)
	.size	valToBit, .-valToBit
	.align	1
	.p2align 2,,3
	.global	bitToVal
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	bitToVal, %function
bitToVal:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r1, .L21
	push	{r4}
	subs	r4, r0, #0
.LPIC1:
	add	r1, pc
	ble	.L15
	ldr	r3, .L21+4
	movs	r0, #0
	movs	r2, #1
	ldr	r3, [r1, r3]
	subs	r3, r3, #4
	add	r4, r3, r4, lsl #2
.L14:
	ldr	r1, [r3, #4]!
	cbz	r1, .L13
	add	r0, r0, r2
.L13:
	cmp	r3, r4
	lsl	r2, r2, #1
	bne	.L14
	ldr	r4, [sp], #4
	bx	lr
.L15:
	movs	r0, #0
	ldr	r4, [sp], #4
	bx	lr
.L22:
	.align	2
.L21:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	bit(GOT)
	.size	bitToVal, .-bitToVal
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
	movs	r0, #0
	bx	lr
	.size	main, .-main
	.comm	bit,128,4
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
