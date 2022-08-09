.arch armv7ve
.arm


.section .data
.align
L1.bit:
	.space	0x80



.global main 
.section .text


@	define dso_local void @F.valToBit(i32 %P25.x) #0 
valToBit:
push {lr}
sub sp,sp,#100
@ L154.Entry: 
L27.Entry:
@ 	%LV28.x = alloca i32, align 4 
@ 	%LV36.i = alloca i32, align 4 
@ 	%LV66.i = alloca i32, align 4 
@ 	store i32 %P25.x, i32* %LV28.x, align 4 
str r0,[sp, #96]
@ 	%T32 = load i32, i32* %LV28.x, align 4 
ldr r12,[sp, #96]
str r12,[sp, #48]
@ 	%T35 = icmp slt i32 %T32, 0 
ldr r12,[sp, #48]
cmp r12,#0
@ 	br i1 %T35, label %L29.if_then, label %L30.if_else 
blt L32.if_then
b L33.if_else
@ L29.if_then: 
L32.if_then:
@ 	store i32 0, i32* %LV36.i, align 4 
movw r12,#0
str r12,[sp, #92]
@ 	br label %L41.while_cond 
b L34.while_cond
@ L41.while_cond: 
L34.while_cond:
@ 	%T42 = load i32, i32* %LV36.i, align 4 
ldr r12,[sp, #92]
str r12,[sp, #32]
@ 	%T45 = icmp slt i32 %T42, 32 
ldr r12,[sp, #32]
cmp r12,#32
@ 	br i1 %T45, label %L39.while_body, label %L40.while_end 
blt L36.while_body
b L37.while_end
@ L39.while_body: 
L36.while_body:
@ 	%T46 = load i32, i32* %LV28.x, align 4 
ldr r12,[sp, #96]
str r12,[sp, #36]
@ 	%T49 = srem i32 %T46, 2 
ldr r12,[sp, #36]
movw lr,#2
push {r0}
sdiv r0,r12,lr
mls r12,lr,r0,r12
pop {r0}
str r12,[sp, #56]
@ 	%T50 = load i32, i32* %LV36.i, align 4 
ldr r12,[sp, #92]
str r12,[sp, #44]
@ 	%T51 = getelementptr [32 x i32], [32 x i32]* @G24.bit, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L1.bit
movt lr,:upper16:L1.bit
add r12,r12,lr
str r12,[sp, #64]
@ 	%T54 = getelementptr i32, i32* %T51, i32 %T50 
ldr r12,[sp, #44]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #64]
add r12,r12,lr
str r12,[sp, #28]
@ 	store i32 %T49, i32* %T54, align 4 
ldr lr,[sp, #28]
ldr r12,[sp, #56]
str r12,[lr, #0]
@ 	%T55 = load i32, i32* %LV28.x, align 4 
ldr r12,[sp, #96]
str r12,[sp, #72]
@ 	%T58 = sub i32 %T55, 1 
ldr r12,[sp, #72]
sub r12,r12,#1
str r12,[sp, #76]
@ 	%T61 = sdiv i32 %T58, 2 
ldr r12,[sp, #76]
movw lr,#2
sdiv r12,r12,lr
str r12,[sp, #80]
@ 	store i32 %T61, i32* %LV28.x, align 4 
ldr r12,[sp, #80]
str r12,[sp, #96]
@ 	%T62 = load i32, i32* %LV36.i, align 4 
ldr r12,[sp, #92]
str r12,[sp, #84]
@ 	%T65 = add i32 %T62, 1 
ldr r12,[sp, #84]
add r12,r12,#1
str r12,[sp, #52]
@ 	store i32 %T65, i32* %LV36.i, align 4 
ldr r12,[sp, #52]
str r12,[sp, #92]
@ 	br label %L41.while_cond 
b L34.while_cond
@ L40.while_end: 
L37.while_end:
@ 	br label %L31.if_end 
b L48.if_end
@ L30.if_else: 
L33.if_else:
@ 	store i32 0, i32* %LV66.i, align 4 
movw r12,#0
str r12,[sp, #88]
@ 	br label %L71.while_cond 
b L49.while_cond
@ L71.while_cond: 
L49.while_cond:
@ 	%T72 = load i32, i32* %LV66.i, align 4 
ldr r12,[sp, #88]
str r12,[sp, #68]
@ 	%T75 = icmp slt i32 %T72, 32 
ldr r12,[sp, #68]
cmp r12,#32
@ 	br i1 %T75, label %L69.while_body, label %L70.while_end 
blt L51.while_body
b L52.while_end
@ L69.while_body: 
L51.while_body:
@ 	%T76 = load i32, i32* %LV28.x, align 4 
ldr r12,[sp, #96]
str r12,[sp, #24]
@ 	%T79 = srem i32 %T76, 2 
ldr r12,[sp, #24]
movw lr,#2
push {r0}
sdiv r0,r12,lr
mls r12,lr,r0,r12
pop {r0}
str r12,[sp, #16]
@ 	%T80 = load i32, i32* %LV66.i, align 4 
ldr r12,[sp, #88]
str r12,[sp, #12]
@ 	%T81 = getelementptr [32 x i32], [32 x i32]* @G24.bit, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L1.bit
movt lr,:upper16:L1.bit
add r12,r12,lr
str r12,[sp, #8]
@ 	%T84 = getelementptr i32, i32* %T81, i32 %T80 
ldr r12,[sp, #12]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #8]
add r12,r12,lr
str r12,[sp, #4]
@ 	store i32 %T79, i32* %T84, align 4 
ldr lr,[sp, #4]
ldr r12,[sp, #16]
str r12,[lr, #0]
@ 	%T85 = load i32, i32* %LV28.x, align 4 
ldr r12,[sp, #96]
str r12,[sp, #40]
@ 	%T88 = sdiv i32 %T85, 2 
ldr r12,[sp, #40]
movw lr,#2
sdiv r12,r12,lr
str r12,[sp, #60]
@ 	store i32 %T88, i32* %LV28.x, align 4 
ldr r12,[sp, #60]
str r12,[sp, #96]
@ 	%T89 = load i32, i32* %LV66.i, align 4 
ldr r12,[sp, #88]
str r12,[sp, #0]
@ 	%T92 = add i32 %T89, 1 
ldr r12,[sp, #0]
add r12,r12,#1
str r12,[sp, #20]
@ 	store i32 %T92, i32* %LV66.i, align 4 
ldr r12,[sp, #20]
str r12,[sp, #88]
@ 	br label %L71.while_cond 
b L49.while_cond
@ L70.while_end: 
L52.while_end:
@ 	br label %L31.if_end 
b L48.if_end
@ L31.if_end: 
L48.if_end:
@ 	br label %L26.return 
b L62.return
@ L26.return: 
L62.return:
@ 	ret void 
add sp,sp,#100
pop {pc}

@	define dso_local i32 @F.bitToVal(i32 %P93.len) #0 
bitToVal:
push {lr}
sub sp,sp,#84
@ L155.Entry: 
L65.Entry:
@ 	%LV97.len = alloca i32, align 4 
@ 	%LV96.retval = alloca i32, align 4 
@ 	%LV98.i = alloca i32, align 4 
@ 	%LV101.v = alloca i32, align 4 
@ 	%LV104.res = alloca i32, align 4 
@ 	store i32 %P93.len, i32* %LV97.len, align 4 
str r0,[sp, #76]
@ 	store i32 0, i32* %LV98.i, align 4 
movw r12,#0
str r12,[sp, #64]
@ 	store i32 1, i32* %LV101.v, align 4 
movw r12,#1
str r12,[sp, #60]
@ 	store i32 0, i32* %LV104.res, align 4 
movw r12,#0
str r12,[sp, #72]
@ 	br label %L109.while_cond 
b L71.while_cond
@ L109.while_cond: 
L71.while_cond:
@ 	%T110 = load i32, i32* %LV98.i, align 4 
ldr r12,[sp, #64]
str r12,[sp, #4]
@ 	%T111 = load i32, i32* %LV97.len, align 4 
ldr r12,[sp, #76]
str r12,[sp, #12]
@ 	%T112 = icmp slt i32 %T110, %T111 
ldr r12,[sp, #4]
ldr lr,[sp, #12]
cmp r12,lr
@ 	br i1 %T112, label %L107.while_body, label %L108.while_end 
blt L74.while_body
b L75.while_end
@ L107.while_body: 
L74.while_body:
@ 	%T115 = load i32, i32* %LV98.i, align 4 
ldr r12,[sp, #64]
str r12,[sp, #8]
@ 	%T116 = getelementptr [32 x i32], [32 x i32]* @G24.bit, i32 0, i32 0 
movw r12,#0
movw lr,:lower16:L1.bit
movt lr,:upper16:L1.bit
add r12,r12,lr
str r12,[sp, #40]
@ 	%T120 = getelementptr i32, i32* %T116, i32 %T115 
ldr r12,[sp, #8]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #16]
@ 	%T119 = load i32, i32* %T120, align 4 
ldr r12,[sp, #16]
ldr r12,[r12, #0]
str r12,[sp, #52]
@ 	%T121 = icmp ne i32 %T119, 0 
ldr r12,[sp, #52]
cmp r12,#0
@ 	br i1 %T121, label %L113.if_then, label %L114.if_end 
bne L80.if_then
b L81.if_end
@ L113.if_then: 
L80.if_then:
@ 	%T123 = load i32, i32* %LV104.res, align 4 
ldr r12,[sp, #72]
str r12,[sp, #32]
@ 	%T124 = load i32, i32* %LV101.v, align 4 
ldr r12,[sp, #60]
str r12,[sp, #36]
@ 	%T125 = add i32 %T123, %T124 
ldr r12,[sp, #32]
ldr lr,[sp, #36]
add r12,r12,lr
str r12,[sp, #24]
@ 	store i32 %T125, i32* %LV104.res, align 4 
ldr r12,[sp, #24]
str r12,[sp, #72]
@ 	br label %L114.if_end 
b L81.if_end
@ L114.if_end: 
L81.if_end:
@ 	%T126 = load i32, i32* %LV98.i, align 4 
ldr r12,[sp, #64]
str r12,[sp, #44]
@ 	%T129 = add i32 %T126, 1 
ldr r12,[sp, #44]
add r12,r12,#1
str r12,[sp, #48]
@ 	store i32 %T129, i32* %LV98.i, align 4 
ldr r12,[sp, #48]
str r12,[sp, #64]
@ 	%T130 = load i32, i32* %LV101.v, align 4 
ldr r12,[sp, #60]
str r12,[sp, #56]
@ 	%T133 = mul i32 %T130, 2 
ldr r12,[sp, #56]
movw lr,#2
mul r12,r12,lr
str r12,[sp, #20]
@ 	store i32 %T133, i32* %LV101.v, align 4 
ldr r12,[sp, #20]
str r12,[sp, #60]
@ 	br label %L109.while_cond 
b L71.while_cond
@ L108.while_end: 
L75.while_end:
@ 	%T134 = load i32, i32* %LV104.res, align 4 
ldr r12,[sp, #72]
str r12,[sp, #0]
@ 	store i32 %T134, i32* %LV96.retval, align 4 
ldr r12,[sp, #0]
str r12,[sp, #68]
@ 	br label %L94.return 
b L90.return
@ L94.return: 
L90.return:
@ 	%T135 = load i32, i32* %LV96.retval, align 4 
ldr r12,[sp, #68]
str r12,[sp, #28]
@ 	ret i32 %T135 
ldr r0,[sp, #28]
add sp,sp,#84
pop {pc}

@	define dso_local i32 @main() #0 
main:
push {lr}
sub sp,sp,#12
@ L156.Entry: 
L93.Entry:
@ 	%LV138.retval = alloca i32, align 4 
@ 	store i32 0, i32* %LV138.retval, align 4 
movw r12,#0
str r12,[sp, #4]
@ 	br label %L136.return 
b L95.return
@ L136.return: 
L95.return:
@ 	%T141 = load i32, i32* %LV138.retval, align 4 
ldr r12,[sp, #4]
str r12,[sp, #0]
@ 	ret i32 %T141 
ldr r0,[sp, #0]
add sp,sp,#12
pop {pc}


