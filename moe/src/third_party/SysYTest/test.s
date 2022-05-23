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
	.file	"test.c"
	.globl	fun                             @ -- Begin function fun
	.p2align	2
	.type	fun,%function
	.code	32                              @ @fun
fun:
	.fnstart
@ %bb.0:                                @ %entry
	sub	sp, sp, #4
	ldr	r1, [r0, #4]
	ldr	r2, [r0, #20]
	str	r0, [sp]
	ldr	r0, [r0, #24]
	add	r1, r2, r1
	add	r0, r1, r0
	add	sp, sp, #4
	bx	lr
.Lfunc_end0:
	.size	fun, .Lfunc_end0-fun
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
	push	{r11, lr}
	mov	r11, sp
	sub	sp, sp, #8
	mov	r0, #0
	str	r0, [sp, #4]
	movw	r0, :lower16:ar
	movt	r0, :upper16:ar
	bl	fun
	mov	sp, r11
	pop	{r11, pc}
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	ar,%object                      @ @ar
	.bss
	.globl	ar
	.p2align	2
ar:
	.zero	36
	.size	ar, 36

	.ident	"Ubuntu clang version 12.0.0-1ubuntu1"
	.section	".note.GNU-stack","",%progbits
