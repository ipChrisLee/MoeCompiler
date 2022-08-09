.arch armv7ve
.arm


.global main 
.section .text


@	define dso_local i32 @main() #0 
main:
push {lr}
sub sp,sp,#76
@ L72.Entry: 
L25.Entry:
@ 	%LV26.retval = alloca i32, align 4 
@ 	%LV27.x = alloca i32, align 4 
@ 	%LV54.f = alloca float, align 4 
@ 	store i32 0, i32* %LV27.x, align 4 
movw r12,#0
str r12,[sp, #64]
@ 	%T30 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #8]
@ 	%T33 = add i32 %T30, 1032 
ldr r12,[sp, #8]
movw lr,#1032
add r12,r12,lr
str r12,[sp, #16]
@ 	store i32 %T33, i32* %LV27.x, align 4 
ldr r12,[sp, #16]
str r12,[sp, #64]
@ 	%T34 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #12]
@ 	call void @putint(i32 %T34) 
ldr r0,[sp, #12]
bl putint
@ 	%T35 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #20]
@ 	%T38 = add i32 %T35, 516 
ldr r12,[sp, #20]
add r12,r12,#516
str r12,[sp, #24]
@ 	store i32 %T38, i32* %LV27.x, align 4 
ldr r12,[sp, #24]
str r12,[sp, #64]
@ 	%T39 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #28]
@ 	call void @putint(i32 %T39) 
ldr r0,[sp, #28]
bl putint
@ 	%T40 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #32]
@ 	%T43 = sub i32 %T40, 1032 
ldr r12,[sp, #32]
movw lr,#1032
sub r12,r12,lr
str r12,[sp, #36]
@ 	store i32 %T43, i32* %LV27.x, align 4 
ldr r12,[sp, #36]
str r12,[sp, #64]
@ 	%T44 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #40]
@ 	call void @putint(i32 %T44) 
ldr r0,[sp, #40]
bl putint
@ 	%T45 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #44]
@ 	%T48 = sub i32 %T45, 516 
ldr r12,[sp, #44]
sub r12,r12,#516
str r12,[sp, #48]
@ 	store i32 %T48, i32* %LV27.x, align 4 
ldr r12,[sp, #48]
str r12,[sp, #64]
@ 	%T49 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #52]
@ 	call void @putint(i32 %T49) 
ldr r0,[sp, #52]
bl putint
@ 	%T50 = load i32, i32* %LV27.x, align 4 
ldr r12,[sp, #64]
str r12,[sp, #56]
@ 	%T53 = sub i32 %T50, 65536 
ldr r12,[sp, #56]
sub r12,r12,#65536
str r12,[sp, #4]
@ 	store i32 %T53, i32* %LV27.x, align 4 
ldr r12,[sp, #4]
str r12,[sp, #64]
@ 	store float 0x0000000000000000, float* %LV54.f, align 4 
@	0.000000
movw r12,#0
vmov s14,r12
vstr s14,[sp, #60]
@ 	store i32 0, i32* %LV26.retval, align 4 
movw r12,#0
str r12,[sp, #68]
@ 	br label %L24.return 
b L43.return
@ L24.return: 
L43.return:
@ 	%T59 = load i32, i32* %LV26.retval, align 4 
ldr r12,[sp, #68]
str r12,[sp, #0]
@ 	ret i32 %T59 
ldr r0,[sp, #0]
add sp,sp,#76
pop {pc}


