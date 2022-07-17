	.text
	.file	"buffer.txt"
	.type	G1.len,@object                  # @G1.len
	.section	.rodata,"a",@progbits
	.globl	G1.len
	.p2align	2
G1.len:
	.long	2                               # 0x2
	.size	G1.len, 4

	.type	G2.minV,@object                 # @G2.minV
	.globl	G2.minV
	.p2align	2
G2.minV:
	.long	2147483648                      # 0x80000000
	.size	G2.minV, 4

	.type	G3.maxV,@object                 # @G3.maxV
	.globl	G3.maxV
	.p2align	2
G3.maxV:
	.long	2147483647                      # 0x7fffffff
	.size	G3.maxV, 4

	.type	G4.x,@object                    # @G4.x
	.globl	G4.x
	.p2align	2
G4.x:
	.long	5                               # 0x5
	.size	G4.x, 4

	.type	G5.fa,@object                   # @G5.fa
	.globl	G5.fa
	.p2align	2
G5.fa:
	.long	0x423c0000                      # float 47
	.size	G5.fa, 4

	.type	G6.fb,@object                   # @G6.fb
	.globl	G6.fb
	.p2align	2
G6.fb:
	.long	0x41300000                      # float 11
	.size	G6.fb, 4

	.type	G7.fc,@object                   # @G7.fc
	.globl	G7.fc
	.p2align	2
G7.fc:
	.long	0x40000000                      # float 2
	.size	G7.fc, 4

	.type	G8.fd,@object                   # @G8.fd
	.globl	G8.fd
	.p2align	2
G8.fd:
	.long	0xbf800000                      # float -1
	.size	G8.fd, 4

	.section	".note.GNU-stack","",@progbits
