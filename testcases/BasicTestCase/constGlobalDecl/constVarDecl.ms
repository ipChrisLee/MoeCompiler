	.text
	.file	"buffer.txt"
	.type	G1.len,@object                  # @G1.len
	.section	.rodata,"a",@progbits
	.globl	G1.len
	.p2align	2
G1.len:
	.long	2                               # 0x2
	.size	G1.len, 4

	.type	G2.len2,@object                 # @G2.len2
	.globl	G2.len2
	.p2align	2
G2.len2:
	.long	4                               # 0x4
	.size	G2.len2, 4

	.type	G3.pi,@object                   # @G3.pi
	.globl	G3.pi
	.p2align	2
G3.pi:
	.long	0x4048f5c3                      # float 3.1400001
	.size	G3.pi, 4

	.type	G4.pi2,@object                  # @G4.pi2
	.globl	G4.pi2
	.p2align	2
G4.pi2:
	.long	0xbf5c2900                      # float -0.86000061
	.size	G4.pi2, 4

	.type	G5.pim2,@object                 # @G5.pim2
	.globl	G5.pim2
	.p2align	2
G5.pim2:
	.long	0xc048f5c3                      # float -3.1400001
	.size	G5.pim2, 4

	.section	".note.GNU-stack","",@progbits
