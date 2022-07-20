	.arch armv7a
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
	.eabi_attribute	28, 1	@ Tag_ABI_VFP_args
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
	.save	{r11, lr}
	push	{r11, lr}
	.setfp	r11, sp
	mov	r11, sp
	.pad	#8
	sub	sp, sp, #8
	vmov.f32	s0, #1.000000e+01
	movw	r0, #0
	movt	r0, #16672
	str	r0, [sp]
	mov	r0, #0
	str	r0, [sp, #4]
	bl	putfloat
	mov	r0, #0
	mov	sp, r11
	pop	{r11, pc}
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cantunwind
	.fnend
                                        @ -- End function
	.type	_sysy_start,%object             @ @_sysy_start
	.bss
	.globl	_sysy_start
	.p2align	2
_sysy_start:
	.zero	8
	.size	_sysy_start, 8

	.type	_sysy_end,%object               @ @_sysy_end
	.globl	_sysy_end
	.p2align	2
_sysy_end:
	.zero	8
	.size	_sysy_end, 8

	.type	_sysy_l1,%object                @ @_sysy_l1
	.globl	_sysy_l1
	.p2align	2
_sysy_l1:
	.zero	4096
	.size	_sysy_l1, 4096

	.type	_sysy_l2,%object                @ @_sysy_l2
	.globl	_sysy_l2
	.p2align	2
_sysy_l2:
	.zero	4096
	.size	_sysy_l2, 4096

	.type	_sysy_h,%object                 @ @_sysy_h
	.globl	_sysy_h
	.p2align	2
_sysy_h:
	.zero	4096
	.size	_sysy_h, 4096

	.type	_sysy_m,%object                 @ @_sysy_m
	.globl	_sysy_m
	.p2align	2
_sysy_m:
	.zero	4096
	.size	_sysy_m, 4096

	.type	_sysy_s,%object                 @ @_sysy_s
	.globl	_sysy_s
	.p2align	2
_sysy_s:
	.zero	4096
	.size	_sysy_s, 4096

	.type	_sysy_us,%object                @ @_sysy_us
	.globl	_sysy_us
	.p2align	2
_sysy_us:
	.zero	4096
	.size	_sysy_us, 4096

	.type	_sysy_idx,%object               @ @_sysy_idx
	.globl	_sysy_idx
	.p2align	2
_sysy_idx:
	.long	0                               @ 0x0
	.size	_sysy_idx, 4

	.ident	"Ubuntu clang version 12.0.0-1ubuntu1"
	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1	@ Tag_ABI_optimization_goals
