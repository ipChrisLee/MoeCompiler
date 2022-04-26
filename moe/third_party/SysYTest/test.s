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
	.globl	main                            @ -- Begin function main
	.p2align	2
	.type	main,%function
	.code	32                              @ @main
main:
	.fnstart
@ %bb.0:                                @ %entry
	sub	sp, sp, #12
	mov	r0, #0
	str	r0, [sp, #8]
	mov	r0, #10
	str	r0, [sp, #4]
	b	.LBB0_1
.LBB0_1:                                @ %while.cond
                                        @ =>This Inner Loop Header: Depth=1
	movw	r0, :lower16:ar
	movt	r0, :upper16:ar
	ldr	r0, [r0, #12]
	cmp	r0, #0
	beq	.LBB0_3
	b	.LBB0_2
.LBB0_2:                                @ %while.body
                                        @   in Loop: Header=BB0_1 Depth=1
	mov	r0, #20
	str	r0, [sp]
	b	.LBB0_1
.LBB0_3:                                @ %while.end
	ldr	r0, [sp, #8]
	add	sp, sp, #12
	bx	lr
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	ar,%object                      @ @ar
	.section	.rodata,"a",%progbits
	.globl	ar
	.p2align	2
ar:
	.long	1                               @ 0x1
	.long	2                               @ 0x2
	.long	3                               @ 0x3
	.long	4                               @ 0x4
	.size	ar, 16

	.type	oar,%object                     @ @oar
	.globl	oar
	.p2align	2
oar:
	.long	1                               @ 0x1
	.long	2                               @ 0x2
	.long	3                               @ 0x3
	.long	4                               @ 0x4
	.size	oar, 16

	.type	x,%object                       @ @x
	.globl	x
	.p2align	2
x:
	.long	0                               @ 0x0
	.size	x, 4

	.ident	"Ubuntu clang version 12.0.0-1ubuntu1"
	.section	".note.GNU-stack","",%progbits
