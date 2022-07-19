	.text
	.file	"buffer.txt"
	.globl	F.main                          # -- Begin function F.main
	.p2align	4, 0x90
	.type	F.main,@function
F.main:                                 # @F.main
	.cfi_startproc
# %bb.0:                                # %L7.Entry
	movl	$0, -4(%rsp)
	movl	-4(%rsp), %eax
	retq
.Lfunc_end0:
	.size	F.main, .Lfunc_end0-F.main
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
