.arch armv7ve
.arm


.global main 
.section .text


@	define dso_local i32 @F.sum(i32* %P24.ar, i32 %P25.n) #0 
sum:
push {lr}
sub sp,sp,#68
@ L107.Entry: 
L27.Entry:
@ 	%LV29.ar = alloca i32*, align 4 
@ 	%LV30.n = alloca i32, align 4 
@ 	%LV28.retval = alloca i32, align 4 
@ 	%LV31.i = alloca i32, align 4 
@ 	%LV34.s = alloca i32, align 4 
@ 	store i32* %P24.ar, i32** %LV29.ar, align 4 
str r0,[sp, #64]
@ 	store i32 %P25.n, i32* %LV30.n, align 4 
str r1,[sp, #60]
@ 	store i32 0, i32* %LV31.i, align 4 
movw r12,#0
str r12,[sp, #56]
@ 	store i32 0, i32* %LV34.s, align 4 
movw r12,#0
str r12,[sp, #48]
@ 	br label %L39.while_cond 
b L33.while_cond
@ L39.while_cond: 
L33.while_cond:
@ 	%T40 = load i32, i32* %LV31.i, align 4 
ldr r12,[sp, #56]
str r12,[sp, #44]
@ 	%T41 = load i32, i32* %LV30.n, align 4 
ldr r12,[sp, #60]
str r12,[sp, #28]
@ 	%T42 = icmp slt i32 %T40, %T41 
ldr r12,[sp, #44]
ldr lr,[sp, #28]
cmp r12,lr
@ 	br i1 %T42, label %L37.while_body, label %L38.while_end 
blt L36.while_body
b L37.while_end
@ L37.while_body: 
L36.while_body:
@ 	%T43 = load i32, i32* %LV34.s, align 4 
ldr r12,[sp, #48]
str r12,[sp, #40]
@ 	%T44 = load i32, i32* %LV31.i, align 4 
ldr r12,[sp, #56]
str r12,[sp, #20]
@ 	%T45 = load i32*, i32** %LV29.ar, align 4 
ldr r12,[sp, #64]
str r12,[sp, #36]
@ 	%T47 = getelementptr i32, i32* %T45, i32 %T44 
ldr r12,[sp, #20]
movw lr,#4
mul r12,r12,lr
str r12,[sp, #-4]
movw r12,#0
ldr lr,[sp, #-4]
add r12,r12,lr
ldr lr,[sp, #36]
add r12,r12,lr
str r12,[sp, #24]
@ 	%T46 = load i32, i32* %T47, align 4 
ldr r12,[sp, #24]
ldr r12,[r12, #0]
str r12,[sp, #12]
@ 	%T48 = add i32 %T43, %T46 
ldr r12,[sp, #40]
ldr lr,[sp, #12]
add r12,r12,lr
str r12,[sp, #8]
@ 	store i32 %T48, i32* %LV34.s, align 4 
ldr r12,[sp, #8]
str r12,[sp, #48]
@ 	%T49 = load i32, i32* %LV31.i, align 4 
ldr r12,[sp, #56]
str r12,[sp, #4]
@ 	%T52 = add i32 %T49, 1 
ldr r12,[sp, #4]
add r12,r12,#1
str r12,[sp, #0]
@ 	store i32 %T52, i32* %LV31.i, align 4 
ldr r12,[sp, #0]
str r12,[sp, #56]
@ 	br label %L39.while_cond 
b L33.while_cond
@ L38.while_end: 
L37.while_end:
@ 	%T53 = load i32, i32* %LV34.s, align 4 
ldr r12,[sp, #48]
str r12,[sp, #16]
@ 	store i32 %T53, i32* %LV28.retval, align 4 
ldr r12,[sp, #16]
str r12,[sp, #52]
@ 	br label %L26.return 
b L47.return
@ L26.return: 
L47.return:
@ 	%T54 = load i32, i32* %LV28.retval, align 4 
ldr r12,[sp, #52]
str r12,[sp, #32]
@ 	ret i32 %T54 
ldr r0,[sp, #32]
add sp,sp,#68
pop {pc}

@	define dso_local i32 @main() #0 
main:
push {lr}
sub sp,sp,#76
@ L108.Entry: 
L50.Entry:
@ 	%LV57.retval = alloca i32, align 4 
@ 	%LV58.ar = alloca [6 x i32], align 4 
@ 	%T65 = getelementptr [6 x i32], [6 x i32]* %LV58.ar, i32 0, i32 0 
movw r12,#0
add lr,sp,#44
add r12,r12,lr
str r12,[sp, #40]
@ 	%T69 = getelementptr i32, i32* %T65, i32 0 
movw r12,#0
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #32]
@ 	store i32 0, i32* %T69, align 4 
ldr lr,[sp, #32]
movw r12,#0
str r12,[lr, #0]
@ 	%T72 = getelementptr i32, i32* %T65, i32 1 
movw r12,#4
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #24]
@ 	store i32 1, i32* %T72, align 4 
ldr lr,[sp, #24]
movw r12,#1
str r12,[lr, #0]
@ 	%T75 = getelementptr i32, i32* %T65, i32 2 
movw r12,#8
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #20]
@ 	store i32 2, i32* %T75, align 4 
ldr lr,[sp, #20]
movw r12,#2
str r12,[lr, #0]
@ 	%T78 = getelementptr i32, i32* %T65, i32 3 
movw r12,#12
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #12]
@ 	store i32 3, i32* %T78, align 4 
ldr lr,[sp, #12]
movw r12,#3
str r12,[lr, #0]
@ 	%T81 = getelementptr i32, i32* %T65, i32 4 
movw r12,#16
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #4]
@ 	store i32 4, i32* %T81, align 4 
ldr lr,[sp, #4]
movw r12,#4
str r12,[lr, #0]
@ 	%T84 = getelementptr i32, i32* %T65, i32 5 
movw r12,#20
ldr lr,[sp, #40]
add r12,r12,lr
str r12,[sp, #0]
@ 	store i32 5, i32* %T84, align 4 
ldr lr,[sp, #0]
movw r12,#5
str r12,[lr, #0]
@ 	%T86 = getelementptr [6 x i32], [6 x i32]* %LV58.ar, i32 0, i32 0 
movw r12,#0
add lr,sp,#44
add r12,r12,lr
str r12,[sp, #36]
@ 	%T89 = getelementptr i32, i32* %T86, i32 0 
movw r12,#0
ldr lr,[sp, #36]
add r12,r12,lr
str r12,[sp, #28]
@ 	%T93 = call i32 @F.sum(i32* %T89, i32 6) 
ldr r0,[sp, #28]
movw r1,#6
bl sum
str r0,[sp, #8]
@ 	store i32 %T93, i32* %LV57.retval, align 4 
ldr r12,[sp, #8]
str r12,[sp, #68]
@ 	br label %L55.return 
b L63.return
@ L55.return: 
L63.return:
@ 	%T94 = load i32, i32* %LV57.retval, align 4 
ldr r12,[sp, #68]
str r12,[sp, #16]
@ 	ret i32 %T94 
ldr r0,[sp, #16]
add sp,sp,#76
pop {pc}


