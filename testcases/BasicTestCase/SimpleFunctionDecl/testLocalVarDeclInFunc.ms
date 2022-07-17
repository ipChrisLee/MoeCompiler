	.text
	.file	"buffer.txt"
	.globl	F.ret0i                         # -- Begin function F.ret0i
	.p2align	4, 0x90
	.type	F.ret0i,@function
F.ret0i:                                # @F.ret0i
	.cfi_startproc
# %bb.0:                                # %L154.Entry
	movl	$0, -4(%rsp)
	movl	-4(%rsp), %eax
	retq
.Lfunc_end0:
	.size	F.ret0i, .Lfunc_end0-F.ret0i
	.cfi_endproc
                                        # -- End function
	.globl	F.funi                          # -- Begin function F.funi
	.p2align	4, 0x90
	.type	F.funi,@function
F.funi:                                 # @F.funi
	.cfi_startproc
# %bb.0:                                # %L155.Entry
	movl	%edi, -4(%rsp)
	movl	%esi, -8(%rsp)
	addl	%esi, %edi
	movl	%edi, -12(%rsp)
	movl	-12(%rsp), %eax
	retq
.Lfunc_end1:
	.size	F.funi, .Lfunc_end1-F.funi
	.cfi_endproc
                                        # -- End function
	.globl	F.testi                         # -- Begin function F.testi
	.p2align	4, 0x90
	.type	F.testi,@function
F.testi:                                # @F.testi
	.cfi_startproc
# %bb.0:                                # %L156.Entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$48, %rsp
	.cfi_def_cfa_offset 64
	.cfi_offset %rbx, -16
	movl	$-10, 12(%rsp)
	movl	$-6, 32(%rsp)
	callq	F.ret0i
	movl	%eax, 16(%rsp)
	movl	12(%rsp), %edi
	movl	32(%rsp), %esi
	callq	F.funi
	movl	%eax, 20(%rsp)
	callq	F.ret0i
	movl	16(%rsp), %esi
	imull	12(%rsp), %esi
	addl	20(%rsp), %esi
	movl	%eax, %edi
	callq	F.funi
	movl	%eax, 44(%rsp)
	movl	12(%rsp), %edi
	movl	16(%rsp), %esi
	callq	F.funi
	movl	12(%rsp), %esi
	movl	%eax, %edi
	callq	F.funi
	movl	12(%rsp), %esi
	movl	%eax, %edi
	callq	F.funi
	movl	%eax, 28(%rsp)
	movl	12(%rsp), %edi
	movl	16(%rsp), %esi
	callq	F.funi
	cltd
	idivl	20(%rsp)
	movl	%edx, 24(%rsp)
	callq	F.ret0i
	movl	%eax, %ebx
	movl	24(%rsp), %edi
	movl	28(%rsp), %esi
	callq	F.funi
	movl	%eax, %ecx
	movl	%ebx, %eax
	cltd
	idivl	%ecx
	movl	%edx, 40(%rsp)
	movl	$0, 36(%rsp)
	movl	36(%rsp), %eax
	addq	$48, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	F.testi, .Lfunc_end2-F.testi
	.cfi_endproc
                                        # -- End function
	.globl	F.ret0f                         # -- Begin function F.ret0f
	.p2align	4, 0x90
	.type	F.ret0f,@function
F.ret0f:                                # @F.ret0f
	.cfi_startproc
# %bb.0:                                # %L157.Entry
	movl	$0, -4(%rsp)
	movss	-4(%rsp), %xmm0                 # xmm0 = mem[0],zero,zero,zero
	retq
.Lfunc_end3:
	.size	F.ret0f, .Lfunc_end3-F.ret0f
	.cfi_endproc
                                        # -- End function
	.globl	F.funf                          # -- Begin function F.funf
	.p2align	4, 0x90
	.type	F.funf,@function
F.funf:                                 # @F.funf
	.cfi_startproc
# %bb.0:                                # %L158.Entry
	movss	%xmm0, -4(%rsp)
	movss	%xmm1, -8(%rsp)
	addss	%xmm1, %xmm0
	movss	%xmm0, -12(%rsp)
	movss	-12(%rsp), %xmm0                # xmm0 = mem[0],zero,zero,zero
	retq
.Lfunc_end4:
	.size	F.funf, .Lfunc_end4-F.funf
	.cfi_endproc
                                        # -- End function
	.globl	F.testf                         # -- Begin function F.testf
	.p2align	4, 0x90
	.type	F.testf,@function
F.testf:                                # @F.testf
	.cfi_startproc
# %bb.0:                                # %L159.Entry
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	$-1054867456, 12(%rsp)          # imm = 0xC1200000
	movl	$-1061158912, 24(%rsp)          # imm = 0xC0C00000
	callq	F.ret0f
	movss	%xmm0, 16(%rsp)
	movss	12(%rsp), %xmm0                 # xmm0 = mem[0],zero,zero,zero
	movss	24(%rsp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	callq	F.funf
	movss	%xmm0, 20(%rsp)
	callq	F.ret0f
	movss	16(%rsp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	mulss	12(%rsp), %xmm1
	addss	20(%rsp), %xmm1
	callq	F.funf
	movss	%xmm0, 36(%rsp)
	movss	12(%rsp), %xmm0                 # xmm0 = mem[0],zero,zero,zero
	movss	16(%rsp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	callq	F.funf
	movss	12(%rsp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	callq	F.funf
	movss	12(%rsp), %xmm1                 # xmm1 = mem[0],zero,zero,zero
	callq	F.funf
	movss	%xmm0, 32(%rsp)
	movl	$0, 28(%rsp)
	movss	28(%rsp), %xmm0                 # xmm0 = mem[0],zero,zero,zero
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end5:
	.size	F.testf, .Lfunc_end5-F.testf
	.cfi_endproc
                                        # -- End function
	.globl	F.main                          # -- Begin function F.main
	.p2align	4, 0x90
	.type	F.main,@function
F.main:                                 # @F.main
	.cfi_startproc
# %bb.0:                                # %L160.Entry
	movl	$10, -4(%rsp)
	movl	$1056964608, -8(%rsp)           # imm = 0x3F000000
	movl	$5, -12(%rsp)
	movl	$10, -16(%rsp)
	movl	$1092091904, -20(%rsp)          # imm = 0x41180000
	movl	$1101004800, -24(%rsp)          # imm = 0x41A00000
	movl	$0, -28(%rsp)
	movl	-28(%rsp), %eax
	retq
.Lfunc_end6:
	.size	F.main, .Lfunc_end6-F.main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
