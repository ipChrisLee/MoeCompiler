	.text
	.syntax unified
	.eabi_attribute	67, "2.09"	@ Tag_conformance
	.eabi_attribute	6, 10	@ Tag_CPU_arch
	.eabi_attribute	7, 77	@ Tag_CPU_arch_profile
	.eabi_attribute	8, 0	@ Tag_ARM_ISA_use
	.eabi_attribute	9, 2	@ Tag_THUMB_ISA_use
	.eabi_attribute	34, 1	@ Tag_CPU_unaligned_access
	.eabi_attribute	17, 1	@ Tag_ABI_PCS_GOT_use
	.eabi_attribute	20, 1	@ Tag_ABI_FP_denormal
	.eabi_attribute	21, 1	@ Tag_ABI_FP_exceptions
	.eabi_attribute	23, 3	@ Tag_ABI_FP_number_model
	.eabi_attribute	24, 1	@ Tag_ABI_align_needed
	.eabi_attribute	25, 1	@ Tag_ABI_align_preserved
	.eabi_attribute	38, 1	@ Tag_ABI_FP_16bit_format
	.eabi_attribute	14, 0	@ Tag_ABI_PCS_R9_use
	.file	"test.mll"
	.globl	F.main                          @ -- Begin function F.main
	.p2align	1
	.type	F.main,%function
	.code	16                              @ @F.main
	.thumb_func
F.main:
	.fnstart
@ %bb.0:                                @ %L40.Entry
	push	{r7, lr}
	sub	sp, #8
	movs	r0, #0
	movt	r0, #16672
	str	r0, [sp]
	bl	putfloat
	movs	r0, #0
	str	r0, [sp, #4]
	movs	r0, #0
	add	sp, #8
	pop	{r7, pc}
.Lfunc_end0:
	.size	F.main, .Lfunc_end0-F.main
	.fnend
                                        @ -- End function
	.section	".note.GNU-stack","",%progbits
