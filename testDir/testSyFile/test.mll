target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "thumbv7m-unknown-unknown-unknown"

declare dso_local i32 @getint(...)

declare dso_local i32 @getch(...)

declare dso_local i32 @getarray(i32*)

declare dso_local float @getfloat(...)

declare dso_local float @getfarray(float*)

declare dso_local void @putint(i32)

declare dso_local void @putch(i32)

declare dso_local void @putarray(i32, i32*)

declare dso_local void @putfloat(float)

declare dso_local void @putfarray(i32, float*)

declare dso_local void @_sysy_starttime(i32)

declare dso_local void @_sysy_stoptime(i32)

define dso_local i32 @F.main(){
L50.Entry:
	%T26.retval = alloca i32, align 4
	%V30.x = alloca float, align 4
	call arm_aapcscc void @_sysy_starttime(i32 2)
	store float 0x4024000000000000, float* %V30.x, align 4
	%T32 = load float, float* %V30.x, align 4
	call arm_aapcscc void @putfloat(float %T32)
	call arm_aapcscc void @_sysy_stoptime(i32 5)
	store i32 0, i32* %T26.retval, align 4
	br label %L24.return
L24.return:
	%T37 = load i32, i32* %T26.retval, align 4
	ret i32 %T37
}


