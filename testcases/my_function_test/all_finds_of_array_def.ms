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
	.file	"all_finds_of_array_def.sy"
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
	@ args = 0, pretend = 0, frame = 144
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L8
	mov	ip, #0
	ldr	r3, .L8+4
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
.LPIC10:
	add	r2, pc
	ldr	r4, .L8+8
	sub	sp, sp, #148
	ldr	r3, [r2, r3]
	add	r5, sp, #12
.LPIC2:
	add	r4, pc
	ldr	r7, .L8+12
	ldr	r3, [r3]
	str	r3, [sp, #140]
	mov	r3,#0
	mov	lr, #1
	ldm	r4, {r0, r1, r2, r3}
.LPIC0:
	add	r7, pc
	mvn	r6, #1
	str	ip, [sp, #48]
	strd	ip, lr, [sp, #56]
	add	r10, r4, #32
	str	r6, [sp, #64]
	movs	r6, #3
	stm	r5, {r0, r1, r2, r3}
	mov	r1, ip
	ldr	r3, [r7, #4]
	movs	r2, #40
	add	r0, sp, #100
	str	r6, [sp, #76]
	str	lr, [sp, #68]
	movs	r6, #5
	str	r3, [sp, #4]
	add	r9, sp, #44
	ldr	r3, [r7]
	add	r8, sp, #92
	str	r6, [sp, #84]
	mov	r6, r7
	adds	r7, r7, #48
	str	r3, [sp, #44]
	ldr	r3, [sp, #4]
	adds	r3, r3, #6
	str	r3, [sp, #52]
	movs	r3, #2
	str	r3, [sp, #72]
	movs	r3, #4
	str	r3, [sp, #80]
	movs	r3, #6
	str	r3, [sp, #88]
	bl	memset(PLT)
	vmov.f32	s14, #2.0e+0
	vldr.32	s15, [r7]
	add	r3, r4, #16
	add	ip, sp, #28
	adds	r4, r4, #80
	ldm	r3, {r0, r1, r2, r3}
	mov	fp, ip
	str	r7, [sp, #4]
	stm	ip, {r0, r1, r2, r3}
	movs	r2, #0
	vadd.f32	s15, s15, s14
	movt	r2, 49440
	str	r2, [sp, #96]	@ float
	vstr.32	s15, [sp, #92]
.L2:
	mov	r1, r6
	movs	r0, #3
	bl	putarray(PLT)
	mov	r1, r10
	movs	r0, #3
	adds	r6, r6, #12
	bl	putarray(PLT)
	mov	r1, r9
	movs	r0, #3
	add	r10, r10, #12
	bl	putarray(PLT)
	mov	r1, r5
	movs	r0, #1
	add	r9, r9, #12
	bl	putarray(PLT)
	mov	r1, r7
	movs	r0, #3
	adds	r5, r5, #4
	bl	putfarray(PLT)
	mov	r1, r4
	movs	r0, #3
	adds	r7, r7, #12
	bl	putfarray(PLT)
	mov	r1, r8
	movs	r0, #3
	adds	r4, r4, #12
	bl	putfarray(PLT)
	mov	r1, fp
	movs	r0, #1
	add	r8, r8, #12
	bl	putfarray(PLT)
	ldr	r3, [sp, #4]
	add	fp, fp, #4
	cmp	r3, r6
	bne	.L2
	ldr	r2, .L8+16
	ldr	r3, .L8+4
.LPIC9:
	add	r2, pc
	ldr	r3, [r2, r3]
	ldr	r2, [r3]
	ldr	r3, [sp, #140]
	eors	r2, r3, r2
	mov	r3, #0
	bne	.L7
	movs	r0, #0
	add	sp, sp, #148
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L7:
	bl	__stack_chk_fail(PLT)
.L9:
	.align	2
.L8:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC10+4)
	.word	__stack_chk_guard(GOT)
	.word	.LANCHOR1-(.LPIC2+4)
	.word	.LANCHOR0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC9+4)
	.size	main, .-main
	.global	vfCG
	.global	vfG
	.global	viCG
	.global	viG
	.global	n
	.comm	_sysy_idx,4,4
	.comm	_sysy_us,4096,4
	.comm	_sysy_s,4096,4
	.comm	_sysy_m,4096,4
	.comm	_sysy_h,4096,4
	.comm	_sysy_l2,4096,4
	.comm	_sysy_l1,4096,4
	.comm	_sysy_end,8,4
	.comm	_sysy_start,8,4
	.section	.rodata
	.align	2
	.set	.LANCHOR1,. + 0
.LC0:
	.word	0
	.word	1
	.word	2
	.word	3
.LC1:
	.word	1078523331
	.word	3239051264
	.word	1101004800
	.word	1106247680
	.type	viCG, %object
	.size	viCG, 48
viCG:
	.word	1
	.word	2
	.word	3
	.word	4
	.space	8
	.word	5
	.word	6
	.space	4
	.word	0
	.word	-10
	.space	4
	.type	vfCG, %object
	.size	vfCG, 48
vfCG:
	.word	3212836864
	.word	3222651535
	.word	3240099840
	.word	1101004800
	.word	1061830001
	.word	1078523331
	.word	1082403062
	.word	1008981770
	.word	1067030938
	.space	12
	.type	n, %object
	.size	n, 4
n:
	.word	4
	.data
	.align	2
	.set	.LANCHOR0,. + 0
	.type	viG, %object
	.size	viG, 48
viG:
	.word	1
	.word	2
	.word	3
	.word	4
	.word	5
	.word	6
	.space	12
	.word	7
	.word	8
	.word	9
	.type	vfG, %object
	.size	vfG, 48
vfG:
	.word	3240099840
	.word	1092616192
	.word	1065353216
	.word	1092626678
	.word	3212836864
	.word	3221225472
	.space	12
	.space	12
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",%progbits
