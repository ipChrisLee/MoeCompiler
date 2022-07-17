	.text
	.file	"buffer.txt"
	.type	G1.i_2_3_Arr_1,@object          # @G1.i_2_3_Arr_1
	.section	.rodata,"a",@progbits
	.globl	G1.i_2_3_Arr_1
	.p2align	2
G1.i_2_3_Arr_1:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	3                               # 0x3
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	6                               # 0x6
	.size	G1.i_2_3_Arr_1, 24

	.type	G2.i_2_3_Arr_2,@object          # @G2.i_2_3_Arr_2
	.globl	G2.i_2_3_Arr_2
	.p2align	2
G2.i_2_3_Arr_2:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	0                               # 0x0
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	0                               # 0x0
	.size	G2.i_2_3_Arr_2, 24

	.type	G3.x,@object                    # @G3.x
	.globl	G3.x
	.p2align	2
G3.x:
	.long	42                              # 0x2a
	.size	G3.x, 4

	.type	G4.i_2_3_Arr_3,@object          # @G4.i_2_3_Arr_3
	.globl	G4.i_2_3_Arr_3
	.p2align	2
G4.i_2_3_Arr_3:
	.long	42                              # 0x2a
	.long	84                              # 0x54
	.long	21                              # 0x15
	.long	43                              # 0x2b
	.long	0                               # 0x0
	.long	0                               # 0x0
	.size	G4.i_2_3_Arr_3, 24

	.type	G5.i_2_3_Arr_4,@object          # @G5.i_2_3_Arr_4
	.globl	G5.i_2_3_Arr_4
	.p2align	2
G5.i_2_3_Arr_4:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	0                               # 0x0
	.zero	12
	.size	G5.i_2_3_Arr_4, 24

	.type	G6.i_2_3_Arr_5,@object          # @G6.i_2_3_Arr_5
	.globl	G6.i_2_3_Arr_5
	.p2align	2
G6.i_2_3_Arr_5:
	.zero	12
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	0                               # 0x0
	.size	G6.i_2_3_Arr_5, 24

	.type	G7.i_2_3_Arr_6,@object          # @G7.i_2_3_Arr_6
	.globl	G7.i_2_3_Arr_6
	.p2align	2
G7.i_2_3_Arr_6:
	.zero	24
	.size	G7.i_2_3_Arr_6, 24

	.type	G8.i_2_3_Arr_7,@object          # @G8.i_2_3_Arr_7
	.globl	G8.i_2_3_Arr_7
	.p2align	2
G8.i_2_3_Arr_7:
	.zero	24
	.size	G8.i_2_3_Arr_7, 24

	.type	G9.i_3_3_Arr_1,@object          # @G9.i_3_3_Arr_1
	.globl	G9.i_3_3_Arr_1
	.p2align	2
G9.i_3_3_Arr_1:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	0                               # 0x0
	.long	3                               # 0x3
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	6                               # 0x6
	.long	0                               # 0x0
	.long	0                               # 0x0
	.size	G9.i_3_3_Arr_1, 36

	.type	G10.i_3_3_3_Arr_1,@object       # @G10.i_3_3_3_Arr_1
	.globl	G10.i_3_3_3_Arr_1
	.p2align	2
G10.i_3_3_3_Arr_1:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	3                               # 0x3
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	6                               # 0x6
	.zero	12
	.long	7                               # 0x7
	.long	8                               # 0x8
	.long	9                               # 0x9
	.long	10                              # 0xa
	.long	0                               # 0x0
	.long	0                               # 0x0
	.zero	12
	.zero	36
	.size	G10.i_3_3_3_Arr_1, 108

	.type	G11.i_3_3_3_Arr_2,@object       # @G11.i_3_3_3_Arr_2
	.globl	G11.i_3_3_3_Arr_2
	.p2align	2
G11.i_3_3_3_Arr_2:
	.long	1                               # 0x1
	.long	2                               # 0x2
	.long	3                               # 0x3
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	0                               # 0x0
	.zero	12
	.zero	36
	.zero	36
	.size	G11.i_3_3_3_Arr_2, 108

	.type	G12.i_3_3_2_Arr_1,@object       # @G12.i_3_3_2_Arr_1
	.globl	G12.i_3_3_2_Arr_1
	.p2align	2
G12.i_3_3_2_Arr_1:
	.long	1                               # 0x1
	.long	0                               # 0x0
	.long	2                               # 0x2
	.long	0                               # 0x0
	.long	3                               # 0x3
	.long	0                               # 0x0
	.long	9                               # 0x9
	.long	10                              # 0xa
	.long	11                              # 0xb
	.long	0                               # 0x0
	.zero	8
	.long	4                               # 0x4
	.long	5                               # 0x5
	.long	6                               # 0x6
	.long	7                               # 0x7
	.long	8                               # 0x8
	.long	9                               # 0x9
	.size	G12.i_3_3_2_Arr_1, 72

	.section	".note.GNU-stack","",@progbits
