	.text
	.file	"buffer.txt"
	.globl	F.gun                           # -- Begin function F.gun
	.p2align	4, 0x90
	.type	F.gun,@function
F.gun:                                  # @F.gun
	.cfi_startproc
# %bb.0:                                # %L44.Entry
	movq	%rdi, -8(%rsp)
	movl	4(%rdi), %eax
	imull	8(%rdi), %eax
	movl	%eax, -12(%rsp)
	movl	-12(%rsp), %eax
	retq
.Lfunc_end0:
	.size	F.gun, .Lfunc_end0-F.gun
	.cfi_endproc
                                        # -- End function
	.globl	F.fun                           # -- Begin function F.fun
	.p2align	4, 0x90
	.type	F.fun,@function
F.fun:                                  # @F.fun
	.cfi_startproc
# %bb.0:                                # %L45.Entry
	movl	%edi, -12(%rsp)
	movl	%esi, -16(%rsp)
	movq	%rdx, -8(%rsp)
	subl	%edi, %esi
	addl	4(%rdx), %esi
	movl	%esi, -20(%rsp)
	movl	-20(%rsp), %eax
	retq
.Lfunc_end1:
	.size	F.fun, .Lfunc_end1-F.fun
	.cfi_endproc
                                        # -- End function
	.globl	F.main                          # -- Begin function F.main
	.p2align	4, 0x90
	.type	F.main,@function
F.main:                                 # @F.main
	.cfi_startproc
# %bb.0:                                # %L46.Entry
	movl	$0, -4(%rsp)
	movl	-4(%rsp), %eax
	retq
.Lfunc_end2:
	.size	F.main, .Lfunc_end2-F.main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
