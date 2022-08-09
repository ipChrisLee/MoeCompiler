.arch armv7ve
.arm


.section .data
.align
L1.n:
	.long	0x4
.align
L2.viG:
	.long	0x1
	.long	0x2
	.long	0x3
	.long	0x4
	.long	0x5
	.long	0x6
	.space	0xc
	.long	0x7
	.long	0x8
	.long	0x9

.align
L3.viCG:
	.long	0x1
	.long	0x2
	.long	0x3
	.long	0x4
	.space	0x8
	.long	0x5
	.long	0x6
	.space	0x4
	.long	0x0
	.long	0xfffffff6
	.space	0x4

.align
L4.vfG:
	.long	0xc1200000
	.long	0x41200000
	.long	0x3f800000
	.long	0x412028f6
	.long	0xbf800000
	.long	0xc0000000
	.space	0x18

.align
L5.vfCG:
	.long	0xbf800000
	.long	0xc015c28f
	.long	0xc1200000
	.long	0x41a00000
	.long	0x3f4a3d71
	.long	0x4048f5c3
	.long	0x408428f6
	.long	0x3c23d70a
	.long	0x3f99999a
	.space	0xc



.global main 
.section .text


@	define dso_local i32 @main() #0 
main:
push {lr}
sub sp,sp,#452
@ L266.Entry: 
L30.Entry:
@ 	%LV31.retval = alloca i32, align 4 
@ 	%LV32.viL = alloca [4 x [3 x i32]], align 4 
@ 	%LV110.viCL.C = alloca [4 x [1 x i32]], align 4 
@ 	%LV112.vfL = alloca [4 x [3 x float]], align 4 
@ 	%LV172.vfCL.C = alloca [4 x [1 x float]], align 4 
@ 	%LV174.i = alloca i32, align 4 
@ 	%T74 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* %LV32.viL, i32 0, i32 0, i32 0 
movw r12,#0
add lr,sp,#400
add r12,r12,lr
str r12,[sp, #212]
@ 	%T35 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G25.viG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L2.viG
movt lr,:upper16:L2.viG
add r12,r12,lr
str r12,[sp, #272]
@ 	%T39 = getelementptr [3 x i32], [3 x i32]* %T35, i32 0, i32 0 
movw r12,#0
ldr lr,[sp, #272]
add r12,r12,lr
str r12,[sp, #156]
@ 	%T38 = load i32, i32* %T39, align 4 
ldr r12,[sp, #156]
ldr r12,[r12, #0]
str r12,[sp, #52]
@ 	%T78 = getelementptr i32, i32* %T74, i32 0 
movw r12,#0
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #144]
@ 	store i32 %T38, i32* %T78, align 4 
ldr lr,[sp, #144]
ldr r12,[sp, #52]
str r12,[lr, #0]
@ 	%T42 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G26.viCG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L3.viCG
movt lr,:upper16:L3.viCG
add r12,r12,lr
str r12,[sp, #252]
@ 	%T46 = getelementptr [3 x i32], [3 x i32]* %T42, i32 1, i32 1 
movw r12,#16
ldr lr,[sp, #252]
add r12,r12,lr
str r12,[sp, #168]
@ 	%T45 = load i32, i32* %T46, align 4 
ldr r12,[sp, #168]
ldr r12,[r12, #0]
str r12,[sp, #76]
@ 	%T80 = getelementptr i32, i32* %T74, i32 1 
movw r12,#4
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #232]
@ 	store i32 %T45, i32* %T80, align 4 
ldr lr,[sp, #232]
ldr r12,[sp, #76]
str r12,[lr, #0]
@ 	%T49 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G25.viG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L2.viG
movt lr,:upper16:L2.viG
add r12,r12,lr
str r12,[sp, #176]
@ 	%T53 = getelementptr [3 x i32], [3 x i32]* %T49, i32 0, i32 1 
movw r12,#4
ldr lr,[sp, #176]
add r12,r12,lr
str r12,[sp, #128]
@ 	%T52 = load i32, i32* %T53, align 4 
ldr r12,[sp, #128]
ldr r12,[r12, #0]
str r12,[sp, #84]
@ 	%T56 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G26.viCG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L3.viCG
movt lr,:upper16:L3.viCG
add r12,r12,lr
str r12,[sp, #184]
@ 	%T60 = getelementptr [3 x i32], [3 x i32]* %T56, i32 2, i32 1 
movw r12,#28
ldr lr,[sp, #184]
add r12,r12,lr
str r12,[sp, #108]
@ 	%T59 = load i32, i32* %T60, align 4 
ldr r12,[sp, #108]
ldr r12,[r12, #0]
str r12,[sp, #96]
@ 	%T61 = add i32 %T52, %T59 
ldr r12,[sp, #84]
ldr lr,[sp, #96]
add r12,r12,lr
str r12,[sp, #80]
@ 	%T82 = getelementptr i32, i32* %T74, i32 2 
movw r12,#8
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #72]
@ 	store i32 %T61, i32* %T82, align 4 
ldr lr,[sp, #72]
ldr r12,[sp, #80]
str r12,[lr, #0]
@ 	%T85 = getelementptr i32, i32* %T74, i32 3 
movw r12,#12
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #260]
@ 	store i32 0, i32* %T85, align 4 
ldr lr,[sp, #260]
movw r12,#0
str r12,[lr, #0]
@ 	%T88 = getelementptr i32, i32* %T74, i32 4 
movw r12,#16
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #124]
@ 	store i32 1, i32* %T88, align 4 
ldr lr,[sp, #124]
movw r12,#1
str r12,[lr, #0]
@ 	%T65 = sub i32 0, 2 
movw r12,#0
sub r12,r12,#2
str r12,[sp, #180]
@ 	%T90 = getelementptr i32, i32* %T74, i32 5 
movw r12,#20
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #56]
@ 	store i32 %T65, i32* %T90, align 4 
ldr lr,[sp, #56]
ldr r12,[sp, #180]
str r12,[lr, #0]
@ 	%T93 = getelementptr i32, i32* %T74, i32 6 
movw r12,#24
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #104]
@ 	store i32 1, i32* %T93, align 4 
ldr lr,[sp, #104]
movw r12,#1
str r12,[lr, #0]
@ 	%T96 = getelementptr i32, i32* %T74, i32 7 
movw r12,#28
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #140]
@ 	store i32 2, i32* %T96, align 4 
ldr lr,[sp, #140]
movw r12,#2
str r12,[lr, #0]
@ 	%T99 = getelementptr i32, i32* %T74, i32 8 
movw r12,#32
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #148]
@ 	store i32 3, i32* %T99, align 4 
ldr lr,[sp, #148]
movw r12,#3
str r12,[lr, #0]
@ 	%T102 = getelementptr i32, i32* %T74, i32 9 
movw r12,#36
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #172]
@ 	store i32 4, i32* %T102, align 4 
ldr lr,[sp, #172]
movw r12,#4
str r12,[lr, #0]
@ 	%T105 = getelementptr i32, i32* %T74, i32 10 
movw r12,#40
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #48]
@ 	store i32 5, i32* %T105, align 4 
ldr lr,[sp, #48]
movw r12,#5
str r12,[lr, #0]
@ 	%T108 = getelementptr i32, i32* %T74, i32 11 
movw r12,#44
ldr lr,[sp, #212]
add r12,r12,lr
str r12,[sp, #160]
@ 	store i32 6, i32* %T108, align 4 
ldr lr,[sp, #160]
movw r12,#6
str r12,[lr, #0]
@ 	store [4 x [1 x i32]] [[1 x i32][i32 0], [1 x i32][i32 1], [1 x i32][i32 2], [1 x i32][i32 3]], [4 x [1 x i32]]* %LV110.viCL.C, align 4 
add r12,sp,#384
movw lr,#0
str lr,[r12, #0]
add r12,r12,#4
movw lr,#1
str lr,[r12, #0]
add r12,r12,#4
movw lr,#2
str lr,[r12, #0]
add r12,r12,#4
movw lr,#3
str lr,[r12, #0]
add r12,r12,#4
@ 	%T133 = getelementptr [4 x [3 x float]], [4 x [3 x float]]* %LV112.vfL, i32 0, i32 0, i32 0 
movw r12,#0
add lr,sp,#336
add r12,r12,lr
str r12,[sp, #280]
@ 	%T115 = getelementptr [4 x [3 x float]], [4 x [3 x float]]* @G27.vfG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L4.vfG
movt lr,:upper16:L4.vfG
add r12,r12,lr
str r12,[sp, #100]
@ 	%T119 = getelementptr [3 x float], [3 x float]* %T115, i32 0, i32 0 
movw r12,#0
ldr lr,[sp, #100]
add r12,r12,lr
str r12,[sp, #88]
@ 	%T118 = load float, float* %T119, align 4 
ldr r12,[sp, #88]
vldr s14,[r12, #0]
vstr s14,[sp, #312]
@ 	%T122 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G26.viCG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L3.viCG
movt lr,:upper16:L3.viCG
add r12,r12,lr
str r12,[sp, #188]
@ 	%T126 = getelementptr [3 x i32], [3 x i32]* %T122, i32 0, i32 1 
movw r12,#4
ldr lr,[sp, #188]
add r12,r12,lr
str r12,[sp, #200]
@ 	%T125 = load i32, i32* %T126, align 4 
ldr r12,[sp, #200]
ldr r12,[r12, #0]
str r12,[sp, #68]
@ 	%T127 = sitofp i32 %T125 to float 
ldr r12,[sp, #68]
vmov s14,r12
vcvt.f32.s32 s14,s14
vstr s14,[sp, #308]
@ 	%T128 = fadd float %T118, %T127 
vldr s14,[sp, #312]
vldr s15,[sp, #308]
vadd.f32 s14,s14,s15
vstr s14,[sp, #304]
@ 	%T137 = getelementptr float, float* %T133, i32 0 
movw r12,#0
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #120]
@ 	store float %T128, float* %T137, align 4 
ldr lr,[sp, #120]
vldr s14,[sp, #304]
vstr s14,[lr, #0]
@ 	%T130 = sub i32 0, 10 
movw r12,#0
sub r12,r12,#10
str r12,[sp, #204]
@ 	%T139 = sitofp i32 %T130 to float 
ldr r12,[sp, #204]
vmov s14,r12
vcvt.f32.s32 s14,s14
vstr s14,[sp, #300]
@ 	%T140 = getelementptr float, float* %T133, i32 1 
movw r12,#4
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #136]
@ 	store float %T139, float* %T140, align 4 
ldr lr,[sp, #136]
vldr s14,[sp, #300]
vstr s14,[lr, #0]
@ 	%T143 = getelementptr float, float* %T133, i32 2 
movw r12,#8
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #192]
@ 	store float 0x0000000000000000, float* %T143, align 4 
ldr lr,[sp, #192]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T146 = getelementptr float, float* %T133, i32 3 
movw r12,#12
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #116]
@ 	store float 0x0000000000000000, float* %T146, align 4 
ldr lr,[sp, #116]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T149 = getelementptr float, float* %T133, i32 4 
movw r12,#16
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #208]
@ 	store float 0x0000000000000000, float* %T149, align 4 
ldr lr,[sp, #208]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T152 = getelementptr float, float* %T133, i32 5 
movw r12,#20
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #216]
@ 	store float 0x0000000000000000, float* %T152, align 4 
ldr lr,[sp, #216]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T155 = getelementptr float, float* %T133, i32 6 
movw r12,#24
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #224]
@ 	store float 0x0000000000000000, float* %T155, align 4 
ldr lr,[sp, #224]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T158 = getelementptr float, float* %T133, i32 7 
movw r12,#28
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #228]
@ 	store float 0x0000000000000000, float* %T158, align 4 
ldr lr,[sp, #228]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T161 = getelementptr float, float* %T133, i32 8 
movw r12,#32
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #236]
@ 	store float 0x0000000000000000, float* %T161, align 4 
ldr lr,[sp, #236]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T164 = getelementptr float, float* %T133, i32 9 
movw r12,#36
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #244]
@ 	store float 0x0000000000000000, float* %T164, align 4 
ldr lr,[sp, #244]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T167 = getelementptr float, float* %T133, i32 10 
movw r12,#40
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #256]
@ 	store float 0x0000000000000000, float* %T167, align 4 
ldr lr,[sp, #256]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	%T170 = getelementptr float, float* %T133, i32 11 
movw r12,#44
ldr lr,[sp, #280]
add r12,r12,lr
str r12,[sp, #164]
@ 	store float 0x0000000000000000, float* %T170, align 4 
ldr lr,[sp, #164]
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[lr, #0]
@ 	store [4 x [1 x float]] [[1 x float][float 0x40091EB860000000], [1 x float][float 0xC022000000000000], [1 x float][float 0x4034000000000000], [1 x float][float 0x403E000000000000]], [4 x [1 x float]]* %LV172.vfCL.C, align 4 
add r12,sp,#320
@	3.140000
movw lr,#62915
movt lr,#16456
str lr,[r12, #0]
add r12,r12,#4
@	-9.000000
movw lr,#0
movt lr,#49424
str lr,[r12, #0]
add r12,r12,#4
@	20.000000
movw lr,#0
movt lr,#16800
str lr,[r12, #0]
add r12,r12,#4
@	30.000000
movw lr,#0
movt lr,#16880
str lr,[r12, #0]
add r12,r12,#4
@ 	store i32 0, i32* %LV174.i, align 4 
movw r12,#0
str r12,[sp, #316]
@ 	br label %L179.while_cond 
b L87.while_cond
@ L179.while_cond: 
L87.while_cond:
@ 	%T180 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #264]
@ 	%T181 = load i32, i32* @G24.n, align 4 
movw r12,:lower16:L1.n
movt r12,:upper16:L1.n
ldr r12,[r12, #0]
str r12,[sp, #276]
@ 	%T182 = icmp slt i32 %T180, %T181 
ldr r12,[sp, #264]
ldr lr,[sp, #276]
cmp r12,lr
@ 	br i1 %T182, label %L177.while_body, label %L178.while_end 
blt L90.while_body
b L91.while_end
@ L177.while_body: 
L90.while_body:
@ 	%T185 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #152]
@ 	%T186 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G25.viG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L2.viG
movt lr,:upper16:L2.viG
add r12,r12,lr
str r12,[sp, #284]
@ 	%T189 = getelementptr [3 x i32], [3 x i32]* %T186, i32 %T185, i32 0 
ldr r12,[sp, #152]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #284]
add r12,r12,lr
str r12,[sp, #92]
@ 	call void @putarray(i32 3, i32* %T189) 
movw r0,#3
ldr r1,[sp, #92]
bl putarray
@ 	%T193 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #240]
@ 	%T194 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* @G26.viCG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L3.viCG
movt lr,:upper16:L3.viCG
add r12,r12,lr
str r12,[sp, #288]
@ 	%T197 = getelementptr [3 x i32], [3 x i32]* %T194, i32 %T193, i32 0 
ldr r12,[sp, #240]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #288]
add r12,r12,lr
str r12,[sp, #248]
@ 	call void @putarray(i32 3, i32* %T197) 
movw r0,#3
ldr r1,[sp, #248]
bl putarray
@ 	%T201 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #60]
@ 	%T202 = getelementptr [4 x [3 x i32]], [4 x [3 x i32]]* %LV32.viL, i32 0, i32 0 
movw r12,#0
add lr,sp,#400
add r12,r12,lr
str r12,[sp, #292]
@ 	%T205 = getelementptr [3 x i32], [3 x i32]* %T202, i32 %T201, i32 0 
ldr r12,[sp, #60]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #292]
add r12,r12,lr
str r12,[sp, #268]
@ 	call void @putarray(i32 3, i32* %T205) 
movw r0,#3
ldr r1,[sp, #268]
bl putarray
@ 	%T209 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #296]
@ 	%T210 = getelementptr [4 x [1 x i32]], [4 x [1 x i32]]* %LV110.viCL.C, i32 0, i32 0 
movw r12,#0
add lr,sp,#384
add r12,r12,lr
str r12,[sp, #220]
@ 	%T213 = getelementptr [1 x i32], [1 x i32]* %T210, i32 %T209, i32 0 
ldr r12,[sp, #296]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #220]
add r12,r12,lr
str r12,[sp, #44]
@ 	call void @putarray(i32 1, i32* %T213) 
movw r0,#1
ldr r1,[sp, #44]
bl putarray
@ 	%T217 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #196]
@ 	%T218 = getelementptr [4 x [3 x float]], [4 x [3 x float]]* @G27.vfG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L4.vfG
movt lr,:upper16:L4.vfG
add r12,r12,lr
str r12,[sp, #64]
@ 	%T221 = getelementptr [3 x float], [3 x float]* %T218, i32 %T217, i32 0 
ldr r12,[sp, #196]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #64]
add r12,r12,lr
str r12,[sp, #40]
@ 	call void @putfarray(i32 3, float* %T221) 
movw r0,#3
ldr r1,[sp, #40]
bl putfarray
@ 	%T225 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #36]
@ 	%T226 = getelementptr [4 x [3 x float]], [4 x [3 x float]]* @G28.vfCG, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L5.vfCG
movt lr,:upper16:L5.vfCG
add r12,r12,lr
str r12,[sp, #132]
@ 	%T229 = getelementptr [3 x float], [3 x float]* %T226, i32 %T225, i32 0 
ldr r12,[sp, #36]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #132]
add r12,r12,lr
str r12,[sp, #32]
@ 	call void @putfarray(i32 3, float* %T229) 
movw r0,#3
ldr r1,[sp, #32]
bl putfarray
@ 	%T233 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #28]
@ 	%T234 = getelementptr [4 x [3 x float]], [4 x [3 x float]]* %LV112.vfL, i32 0, i32 0 
movw r12,#0
add lr,sp,#336
add r12,r12,lr
str r12,[sp, #112]
@ 	%T237 = getelementptr [3 x float], [3 x float]* %T234, i32 %T233, i32 0 
ldr r12,[sp, #28]
movw lr,#12
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #112]
add r12,r12,lr
str r12,[sp, #24]
@ 	call void @putfarray(i32 3, float* %T237) 
movw r0,#3
ldr r1,[sp, #24]
bl putfarray
@ 	%T241 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #20]
@ 	%T242 = getelementptr [4 x [1 x float]], [4 x [1 x float]]* %LV172.vfCL.C, i32 0, i32 0 
movw r12,#0
add lr,sp,#320
add r12,r12,lr
str r12,[sp, #16]
@ 	%T245 = getelementptr [1 x float], [1 x float]* %T242, i32 %T241, i32 0 
ldr r12,[sp, #20]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #16]
add r12,r12,lr
str r12,[sp, #12]
@ 	call void @putfarray(i32 1, float* %T245) 
movw r0,#1
ldr r1,[sp, #12]
bl putfarray
@ 	%T247 = load i32, i32* %LV174.i, align 4 
ldr r12,[sp, #316]
str r12,[sp, #8]
@ 	%T250 = add i32 %T247, 1 
ldr r12,[sp, #8]
add r12,r12,#1
str r12,[sp, #4]
@ 	store i32 %T250, i32* %LV174.i, align 4 
ldr r12,[sp, #4]
str r12,[sp, #316]
@ 	br label %L179.while_cond 
b L87.while_cond
@ L178.while_end: 
L91.while_end:
@ 	store i32 0, i32* %LV31.retval, align 4 
movw r12,#0
str r12,[sp, #448]
@ 	br label %L29.return 
b L118.return
@ L29.return: 
L118.return:
@ 	%T253 = load i32, i32* %LV31.retval, align 4 
ldr r12,[sp, #448]
str r12,[sp, #0]
@ 	ret i32 %T253 
ldr r0,[sp, #0]
add sp,sp,#452
pop {pc}


