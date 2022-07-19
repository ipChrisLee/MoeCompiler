	.text
	.file	"buffer.txt"
	.globl	F.fun                           # -- Begin function F.fun
	.p2align	4, 0x90
	.type	F.fun,@function
F.fun:                                  # @F.fun
	.cfi_startproc
# %bb.0:                                # %L166.Entry
	movq	%rdi, -8(%rsp)
	movl	(%rdi), %eax
	addl	4(%rdi), %eax
	addl	8(%rdi), %eax
	movl	%eax, -12(%rsp)
	retq
.Lfunc_end0:
	.size	F.fun, .Lfunc_end0-F.fun
	.cfi_endproc
                                        # -- End function
	.globl	F.fun2                          # -- Begin function F.fun2
	.p2align	4, 0x90
	.type	F.fun2,@function
F.fun2:                                 # @F.fun2
	.cfi_startproc
# %bb.0:                                # %L167.Entry
	movq	%rdi, -8(%rsp)
	movl	(%rdi), %eax
	addl	16(%rdi), %eax
	addl	32(%rdi), %eax
	movl	%eax, -12(%rsp)
	retq
.Lfunc_end1:
	.size	F.fun2, .Lfunc_end1-F.fun2
	.cfi_endproc
                                        # -- End function
	.globl	F.main                          # -- Begin function F.main
	.p2align	4, 0x90
	.type	F.main,@function
F.main:                                 # @F.main
	.cfi_startproc
# %bb.0:                                # %L168.Entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$96, %rsp
	.cfi_def_cfa_offset 112
	.cfi_offset %rbx, -16
	movabsq	$8589934593, %rbx               # imm = 0x200000001
	movq	%rbx, 8(%rsp)
	movl	$3, 16(%rsp)
	leaq	8(%rsp), %rdi
	callq	F.fun
	movl	%eax, 4(%rsp)
	movq	%rbx, 24(%rsp)
	movabsq	$17179869187, %rax              # imm = 0x400000003
	movq	%rax, 32(%rsp)
	movabsq	$25769803781, %rax              # imm = 0x600000005
	movq	%rax, 40(%rsp)
	movabsq	$34359738375, %rax              # imm = 0x800000007
	movq	%rax, 48(%rsp)
	movabsq	$4294967305, %rax               # imm = 0x100000009
	movq	%rax, 56(%rsp)
	leaq	60(%rsp), %rdi
	movabsq	$12884901890, %rax              # imm = 0x300000002
	movq	%rax, 64(%rsp)
	movabsq	$21474836484, %rax              # imm = 0x500000004
	movq	%rax, 72(%rsp)
	movabsq	$30064771078, %rax              # imm = 0x700000006
	movq	%rax, 80(%rsp)
	movabsq	$38654705672, %rax              # imm = 0x900000008
	movq	%rax, 88(%rsp)
	callq	F.fun2
	movl	%eax, (%rsp)
	leaq	24(%rsp), %rdi
	callq	F.fun2
	movl	$0, 20(%rsp)
	xorl	%eax, %eax
	addq	$96, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	F.main, .Lfunc_end2-F.main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
