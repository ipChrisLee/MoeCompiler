	.text
	.file	"buffer.txt"
	.type	G1.len,@object                  # @G1.len
	.section	.rodata,"a",@progbits
	.globl	G1.len
	.p2align	2
G1.len:
	.long	2                               # 0x2
	.size	G1.len, 4

	.type	G2.x,@object                    # @G2.x
	.globl	G2.x
	.p2align	2
G2.x:
	.long	5                               # 0x5
	.size	G2.x, 4

	.type	G3.arr1,@object                 # @G3.arr1
	.globl	G3.arr1
	.p2align	2
G3.arr1:
	.long	5                               # 0x5
	.long	10                              # 0xa
	.zero	8
	.size	G3.arr1, 16

	.type	G4.i1,@object                   # @G4.i1
	.data
	.globl	G4.i1
	.p2align	2
G4.i1:
	.long	7                               # 0x7
	.size	G4.i1, 4

	.type	G5.i2,@object                   # @G5.i2
	.globl	G5.i2
	.p2align	2
G5.i2:
	.long	5                               # 0x5
	.size	G5.i2, 4

	.type	G6.i3,@object                   # @G6.i3
	.globl	G6.i3
	.p2align	2
G6.i3:
	.long	5                               # 0x5
	.long	5                               # 0x5
	.zero	8
	.size	G6.i3, 16

	.type	G7.i4,@object                   # @G7.i4
	.globl	G7.i4
	.p2align	2
G7.i4:
	.long	2                               # 0x2
	.long	0                               # 0x0
	.long	0                               # 0x0
	.long	0                               # 0x0
	.zero	16
	.zero	16
	.size	G7.i4, 48

	.section	".note.GNU-stack","",@progbits
