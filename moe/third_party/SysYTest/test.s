	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 10	@ Tag_CPU_arch
	.eabi_attribute	7, 65	@ Tag_CPU_arch_profile
	.eabi_attribute	8, 1	@ Tag_ARM_ISA_use
	.eabi_attribute	9, 2	@ Tag_THUMB_ISA_use
	.fpu	vfpv3
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 0	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	18, 4	@ Tag_ABI_PCS_wchar_t
	.eabi_attribute	26, 2	@ Tag_ABI_enum_size
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"test.sy"
	.globl	floyd                           @ -- Begin function floyd
	.p2align	2
	.type	floyd,%function
	.code	32                              @ @floyd
floyd:
	.fnstart
@ %bb.0:                                @ %entry
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #20
	ldr	r12, [r11, #20]
	ldr	r12, [r11, #16]
	ldr	r12, [r11, #12]
	ldr	r12, [r11, #8]
	str	r0, [r11, #-4]
	str	r1, [r11, #-8]
	str	r2, [sp, #8]
	str	r3, [sp, #4]
	mov	r0, #1
	str	r0, [r11, #16]
	b	.LBB0_1
.LBB0_1:                                @ %while.cond
                                        @ =>This Loop Header: Depth=1
                                        @     Child Loop BB0_3 Depth 2
                                        @       Child Loop BB0_5 Depth 3
	ldr	r0, [r11, #16]
	ldr	r1, [r11, #-4]
	cmp	r0, r1
	bgt	.LBB0_11
	b	.LBB0_2
.LBB0_2:                                @ %while.body
                                        @   in Loop: Header=BB0_1 Depth=1
	mov	r0, #1
	str	r0, [r11, #8]
	b	.LBB0_3
.LBB0_3:                                @ %while.cond1
                                        @   Parent Loop BB0_1 Depth=1
                                        @ =>  This Loop Header: Depth=2
                                        @       Child Loop BB0_5 Depth 3
	ldr	r0, [r11, #8]
	ldr	r1, [r11, #-4]
	cmp	r0, r1
	bgt	.LBB0_10
	b	.LBB0_4
.LBB0_4:                                @ %while.body3
                                        @   in Loop: Header=BB0_3 Depth=2
	mov	r0, #1
	str	r0, [r11, #12]
	b	.LBB0_5
.LBB0_5:                                @ %while.cond4
                                        @   Parent Loop BB0_1 Depth=1
                                        @     Parent Loop BB0_3 Depth=2
                                        @ =>    This Inner Loop Header: Depth=3
	ldr	r0, [r11, #12]
	ldr	r1, [r11, #-4]
	cmp	r0, r1
	bgt	.LBB0_9
	b	.LBB0_6
.LBB0_6:                                @ %while.body6
                                        @   in Loop: Header=BB0_5 Depth=3
	ldr	r0, [r11, #-8]
	ldr	r1, [r11, #8]
	mov	r12, #440
	mla	r1, r1, r12, r0
	ldr	r3, [r11, #12]
	ldr	lr, [r1, r3, lsl #2]
	ldr	r2, [r11, #16]
	ldr	r1, [r1, r2, lsl #2]
	mla	r0, r2, r12, r0
	ldr	r0, [r0, r3, lsl #2]
	add	r0, r1, r0
	cmp	lr, r0
	ble	.LBB0_8
	b	.LBB0_7
.LBB0_7:                                @ %if.then
                                        @   in Loop: Header=BB0_5 Depth=3
	ldr	r0, [r11, #-8]
	ldr	r1, [r11, #8]
	mov	r12, #440
	mla	r1, r1, r12, r0
	ldr	r3, [r11, #16]
	ldr	r2, [r1, r3, lsl #2]
	mla	r0, r3, r12, r0
	ldr	r3, [r11, #12]
	ldr	r0, [r0, r3, lsl #2]
	add	r0, r2, r0
	str	r0, [r1, r3, lsl #2]
	b	.LBB0_8
.LBB0_8:                                @ %if.end
                                        @   in Loop: Header=BB0_5 Depth=3
	ldr	r0, [r11, #12]
	add	r0, r0, #1
	str	r0, [r11, #12]
	b	.LBB0_5
.LBB0_9:                                @ %while.end
                                        @   in Loop: Header=BB0_3 Depth=2
	ldr	r0, [r11, #8]
	add	r0, r0, #1
	str	r0, [r11, #8]
	b	.LBB0_3
.LBB0_10:                               @ %while.end22
                                        @   in Loop: Header=BB0_1 Depth=1
	ldr	r0, [r11, #16]
	add	r0, r0, #1
	str	r0, [r11, #16]
	b	.LBB0_1
.LBB0_11:                               @ %while.end24
	movw	r0, #16191
	movt	r0, #16191
	str	r0, [sp]
	mov	r0, #1
	str	r0, [r11, #8]
	b	.LBB0_12
.LBB0_12:                               @ %while.cond25
                                        @ =>This Loop Header: Depth=1
                                        @     Child Loop BB0_14 Depth 2
	ldr	r0, [r11, #8]
	ldr	r1, [r11, #-4]
	cmp	r0, r1
	bgt	.LBB0_19
	b	.LBB0_13
.LBB0_13:                               @ %while.body27
                                        @   in Loop: Header=BB0_12 Depth=1
	b	.LBB0_14
.LBB0_14:                               @ %while.cond28
                                        @   Parent Loop BB0_12 Depth=1
                                        @ =>  This Inner Loop Header: Depth=2
	ldr	r0, [r11, #12]
	ldr	r1, [r11, #-4]
	cmp	r0, r1
	bgt	.LBB0_18
	b	.LBB0_15
.LBB0_15:                               @ %while.body30
                                        @   in Loop: Header=BB0_14 Depth=2
	ldr	r0, [sp]
	ldr	r1, [r11, #-8]
	ldr	r2, [r11, #8]
	mov	r3, #440
	mla	r1, r2, r3, r1
	ldr	r2, [r11, #12]
	ldr	r1, [r1, r2, lsl #2]
	cmp	r0, r1
	ble	.LBB0_17
	b	.LBB0_16
.LBB0_16:                               @ %if.then34
                                        @   in Loop: Header=BB0_14 Depth=2
	ldr	r0, [r11, #-8]
	ldr	r1, [r11, #8]
	mov	r2, #440
	mla	r0, r1, r2, r0
	ldr	r1, [r11, #12]
	ldr	r0, [r0, r1, lsl #2]
	str	r0, [sp]
	b	.LBB0_17
.LBB0_17:                               @ %if.end37
                                        @   in Loop: Header=BB0_14 Depth=2
	ldr	r0, [r11, #12]
	add	r0, r0, #1
	str	r0, [r11, #12]
	b	.LBB0_14
.LBB0_18:                               @ %while.end39
                                        @   in Loop: Header=BB0_12 Depth=1
	ldr	r0, [r11, #8]
	add	r0, r0, #1
	str	r0, [r11, #8]
	b	.LBB0_12
.LBB0_19:                               @ %while.end41
	ldr	r0, [sp]
	mov	sp, r11
	pop	{r11, pc}
.Lfunc_end0:
	.size	floyd, .Lfunc_end0-floyd
	.cantunwind
	.fnend
                                        @ -- End function
	.globl	main                            @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                              @ @main
main:
	.fnstart
@ %bb.0:                                @ %entry
	push	{r4, r10, r11, lr}
	add	r11, sp, #8
	sub	sp, sp, #32
	mov	r0, #0
	str	r0, [r11, #-12]
	bl	getint
	movw	r1, :lower16:n
	movt	r1, :upper16:n
	str	r0, [r1]
	bl	getint
	movw	r1, :lower16:m
	movt	r1, :upper16:m
	str	r0, [r1]
	mov	r0, #1
	str	r0, [r11, #-16]
	b	.LBB1_1
.LBB1_1:                                @ %while.cond
                                        @ =>This Inner Loop Header: Depth=1
	ldr	r0, [r11, #-16]
	movw	r1, :lower16:m
	movt	r1, :upper16:m
	ldr	r1, [r1]
	cmp	r0, r1
	bgt	.LBB1_3
	b	.LBB1_2
.LBB1_2:                                @ %while.body
                                        @   in Loop: Header=BB1_1 Depth=1
	bl	getint
	ldr	r1, [r11, #-16]
	movw	r4, :lower16:es
	movt	r4, :upper16:es
	str	r0, [r4, r1, lsl #3]
	bl	getint
	ldr	r1, [r11, #-16]
	add	r1, r4, r1, lsl #3
	str	r0, [r1, #4]
	ldr	r0, [r11, #-16]
	add	r0, r0, #1
	str	r0, [r11, #-16]
	b	.LBB1_1
.LBB1_3:                                @ %while.end
	movw	r0, :lower16:n
	movt	r0, :upper16:n
	ldr	r0, [r0]
	movw	r1, :lower16:m
	movt	r1, :upper16:m
	ldr	r2, [r1]
	mov	r1, #1
	str	r1, [sp, #12]
	str	r1, [sp, #8]
	str	r1, [sp, #4]
	str	r1, [sp]
	movw	r1, :lower16:dis
	movt	r1, :upper16:dis
	movw	r3, :lower16:es
	movt	r3, :upper16:es
	bl	floyd
	mov	r0, #0
	sub	sp, r11, #8
	pop	{r4, r10, r11, pc}
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	n,%object                       @ @n
	.bss
	.globl	n
	.p2align	2
n:
	.long	0                               @ 0x0
	.size	n, 4

	.type	m,%object                       @ @m
	.globl	m
	.p2align	2
m:
	.long	0                               @ 0x0
	.size	m, 4

	.type	es,%object                      @ @es
	.globl	es
	.p2align	2
es:
	.zero	880
	.size	es, 880

	.type	dis,%object                     @ @dis
	.globl	dis
	.p2align	2
dis:
	.zero	48400
	.size	dis, 48400

	.ident	"Ubuntu clang version 12.0.0-1ubuntu1"
	.section	".note.GNU-stack","",%progbits
