; ModuleID = 'test.sy'
source_filename = "test.sy"
target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "armv7-unknown-unknown"

@n = dso_local global i32 0, align 4
@m = dso_local global i32 0, align 4
@es = dso_local global [110 x [2 x i32]] zeroinitializer, align 4
@dis = dso_local global [110 x [110 x i32]] zeroinitializer, align 4

; Function Attrs: noinline nounwind optnone
define dso_local arm_aapcscc i32 @floyd(i32 %n, [110 x i32]* %dis, i32 %m, [2 x i32]* %es, i32 %i, i32 %j, i32 %k, i32 %z) #0 {
entry:
  %n.addr = alloca i32, align 4
  %dis.addr = alloca [110 x i32]*, align 4
  %m.addr = alloca i32, align 4
  %es.addr = alloca [2 x i32]*, align 4
  %i.addr = alloca i32, align 4
  %j.addr = alloca i32, align 4
  %k.addr = alloca i32, align 4
  %z.addr = alloca i32, align 4
  %res = alloca i32, align 4
  store i32 %n, i32* %n.addr, align 4
  store [110 x i32]* %dis, [110 x i32]** %dis.addr, align 4
  store i32 %m, i32* %m.addr, align 4
  store [2 x i32]* %es, [2 x i32]** %es.addr, align 4
  store i32 %i, i32* %i.addr, align 4
  store i32 %j, i32* %j.addr, align 4
  store i32 %k, i32* %k.addr, align 4
  store i32 %z, i32* %z.addr, align 4
  store i32 1, i32* %k.addr, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.end22, %entry
  %0 = load i32, i32* %k.addr, align 4
  %1 = load i32, i32* %n.addr, align 4
  %cmp = icmp sle i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end24

while.body:                                       ; preds = %while.cond
  store i32 1, i32* %i.addr, align 4
  br label %while.cond1

while.cond1:                                      ; preds = %while.end, %while.body
  %2 = load i32, i32* %i.addr, align 4
  %3 = load i32, i32* %n.addr, align 4
  %cmp2 = icmp sle i32 %2, %3
  br i1 %cmp2, label %while.body3, label %while.end22

while.body3:                                      ; preds = %while.cond1
  store i32 1, i32* %j.addr, align 4
  br label %while.cond4

while.cond4:                                      ; preds = %if.end, %while.body3
  %4 = load i32, i32* %j.addr, align 4
  %5 = load i32, i32* %n.addr, align 4
  %cmp5 = icmp sle i32 %4, %5
  br i1 %cmp5, label %while.body6, label %while.end

while.body6:                                      ; preds = %while.cond4
  %6 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %7 = load i32, i32* %i.addr, align 4
  %arrayidx = getelementptr inbounds [110 x i32], [110 x i32]* %6, i32 %7
  %8 = load i32, i32* %j.addr, align 4
  %arrayidx7 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx, i32 0, i32 %8
  %9 = load i32, i32* %arrayidx7, align 4
  %10 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %11 = load i32, i32* %i.addr, align 4
  %arrayidx8 = getelementptr inbounds [110 x i32], [110 x i32]* %10, i32 %11
  %12 = load i32, i32* %k.addr, align 4
  %arrayidx9 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx8, i32 0, i32 %12
  %13 = load i32, i32* %arrayidx9, align 4
  %14 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %15 = load i32, i32* %k.addr, align 4
  %arrayidx10 = getelementptr inbounds [110 x i32], [110 x i32]* %14, i32 %15
  %16 = load i32, i32* %j.addr, align 4
  %arrayidx11 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx10, i32 0, i32 %16
  %17 = load i32, i32* %arrayidx11, align 4
  %add = add nsw i32 %13, %17
  %cmp12 = icmp sgt i32 %9, %add
  br i1 %cmp12, label %if.then, label %if.end

if.then:                                          ; preds = %while.body6
  %18 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %19 = load i32, i32* %i.addr, align 4
  %arrayidx13 = getelementptr inbounds [110 x i32], [110 x i32]* %18, i32 %19
  %20 = load i32, i32* %k.addr, align 4
  %arrayidx14 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx13, i32 0, i32 %20
  %21 = load i32, i32* %arrayidx14, align 4
  %22 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %23 = load i32, i32* %k.addr, align 4
  %arrayidx15 = getelementptr inbounds [110 x i32], [110 x i32]* %22, i32 %23
  %24 = load i32, i32* %j.addr, align 4
  %arrayidx16 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx15, i32 0, i32 %24
  %25 = load i32, i32* %arrayidx16, align 4
  %add17 = add nsw i32 %21, %25
  %26 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %27 = load i32, i32* %i.addr, align 4
  %arrayidx18 = getelementptr inbounds [110 x i32], [110 x i32]* %26, i32 %27
  %28 = load i32, i32* %j.addr, align 4
  %arrayidx19 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx18, i32 0, i32 %28
  store i32 %add17, i32* %arrayidx19, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body6
  %29 = load i32, i32* %j.addr, align 4
  %add20 = add nsw i32 %29, 1
  store i32 %add20, i32* %j.addr, align 4
  br label %while.cond4, !llvm.loop !3

while.end:                                        ; preds = %while.cond4
  %30 = load i32, i32* %i.addr, align 4
  %add21 = add nsw i32 %30, 1
  store i32 %add21, i32* %i.addr, align 4
  br label %while.cond1, !llvm.loop !5

while.end22:                                      ; preds = %while.cond1
  %31 = load i32, i32* %k.addr, align 4
  %add23 = add nsw i32 %31, 1
  store i32 %add23, i32* %k.addr, align 4
  br label %while.cond, !llvm.loop !6

while.end24:                                      ; preds = %while.cond
  store i32 1061109567, i32* %res, align 4
  store i32 1, i32* %i.addr, align 4
  br label %while.cond25

while.cond25:                                     ; preds = %while.end39, %while.end24
  %32 = load i32, i32* %i.addr, align 4
  %33 = load i32, i32* %n.addr, align 4
  %cmp26 = icmp sle i32 %32, %33
  br i1 %cmp26, label %while.body27, label %while.end41

while.body27:                                     ; preds = %while.cond25
  br label %while.cond28

while.cond28:                                     ; preds = %if.end37, %while.body27
  %34 = load i32, i32* %j.addr, align 4
  %35 = load i32, i32* %n.addr, align 4
  %cmp29 = icmp sle i32 %34, %35
  br i1 %cmp29, label %while.body30, label %while.end39

while.body30:                                     ; preds = %while.cond28
  %36 = load i32, i32* %res, align 4
  %37 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %38 = load i32, i32* %i.addr, align 4
  %arrayidx31 = getelementptr inbounds [110 x i32], [110 x i32]* %37, i32 %38
  %39 = load i32, i32* %j.addr, align 4
  %arrayidx32 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx31, i32 0, i32 %39
  %40 = load i32, i32* %arrayidx32, align 4
  %cmp33 = icmp sgt i32 %36, %40
  br i1 %cmp33, label %if.then34, label %if.end37

if.then34:                                        ; preds = %while.body30
  %41 = load [110 x i32]*, [110 x i32]** %dis.addr, align 4
  %42 = load i32, i32* %i.addr, align 4
  %arrayidx35 = getelementptr inbounds [110 x i32], [110 x i32]* %41, i32 %42
  %43 = load i32, i32* %j.addr, align 4
  %arrayidx36 = getelementptr inbounds [110 x i32], [110 x i32]* %arrayidx35, i32 0, i32 %43
  %44 = load i32, i32* %arrayidx36, align 4
  store i32 %44, i32* %res, align 4
  br label %if.end37

if.end37:                                         ; preds = %if.then34, %while.body30
  %45 = load i32, i32* %j.addr, align 4
  %add38 = add nsw i32 %45, 1
  store i32 %add38, i32* %j.addr, align 4
  br label %while.cond28, !llvm.loop !7

while.end39:                                      ; preds = %while.cond28
  %46 = load i32, i32* %i.addr, align 4
  %add40 = add nsw i32 %46, 1
  store i32 %add40, i32* %i.addr, align 4
  br label %while.cond25, !llvm.loop !8

while.end41:                                      ; preds = %while.cond25
  %47 = load i32, i32* %res, align 4
  ret i32 %47
}

; Function Attrs: noinline nounwind optnone
define dso_local arm_aapcscc i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call arm_aapcscc i32 bitcast (i32 (...)* @getint to i32 ()*)()
  store i32 %call, i32* @n, align 4
  %call1 = call arm_aapcscc i32 bitcast (i32 (...)* @getint to i32 ()*)()
  store i32 %call1, i32* @m, align 4
  store i32 1, i32* %i, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* @m, align 4
  %cmp = icmp sle i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %call2 = call arm_aapcscc i32 bitcast (i32 (...)* @getint to i32 ()*)()
  %2 = load i32, i32* %i, align 4
  %arrayidx = getelementptr inbounds [110 x [2 x i32]], [110 x [2 x i32]]* @es, i32 0, i32 %2
  %arrayidx3 = getelementptr inbounds [2 x i32], [2 x i32]* %arrayidx, i32 0, i32 0
  store i32 %call2, i32* %arrayidx3, align 4
  %call4 = call arm_aapcscc i32 bitcast (i32 (...)* @getint to i32 ()*)()
  %3 = load i32, i32* %i, align 4
  %arrayidx5 = getelementptr inbounds [110 x [2 x i32]], [110 x [2 x i32]]* @es, i32 0, i32 %3
  %arrayidx6 = getelementptr inbounds [2 x i32], [2 x i32]* %arrayidx5, i32 0, i32 1
  store i32 %call4, i32* %arrayidx6, align 4
  %4 = load i32, i32* %i, align 4
  %add = add nsw i32 %4, 1
  store i32 %add, i32* %i, align 4
  br label %while.cond, !llvm.loop !9

while.end:                                        ; preds = %while.cond
  %5 = load i32, i32* @n, align 4
  %6 = load i32, i32* @m, align 4
  %call7 = call arm_aapcscc i32 @floyd(i32 %5, [110 x i32]* getelementptr inbounds ([110 x [110 x i32]], [110 x [110 x i32]]* @dis, i32 0, i32 0), i32 %6, [2 x i32]* getelementptr inbounds ([110 x [2 x i32]], [110 x [2 x i32]]* @es, i32 0, i32 0), i32 1, i32 1, i32 1, i32 1)
  ret i32 0
}

declare dso_local arm_aapcscc i32 @getint(...) #1

attributes #0 = { noinline nounwind optnone "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+armv7-a,+dsp,+soft-float,+strict-align,-bf16,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-mve,-mve.fp,-neon,-thumb-mode,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "unsafe-fp-math"="false" "use-soft-float"="true" }
attributes #1 = { "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+armv7-a,+dsp,+soft-float,+strict-align,-bf16,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-mve,-mve.fp,-neon,-thumb-mode,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "unsafe-fp-math"="false" "use-soft-float"="true" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"min_enum_size", i32 4}
!2 = !{!"Ubuntu clang version 12.0.0-1ubuntu1"}
!3 = distinct !{!3, !4}
!4 = !{!"llvm.loop.mustprogress"}
!5 = distinct !{!5, !4}
!6 = distinct !{!6, !4}
!7 = distinct !{!7, !4}
!8 = distinct !{!8, !4}
!9 = distinct !{!9, !4}
