; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "armv7-unknown-unknown"

@ar = dso_local global [3 x [3 x i32]] zeroinitializer, align 4

; Function Attrs: noinline nounwind
define dso_local arm_aapcscc i32 @fun([3 x i32]* %a) #0 {
entry:
  %a.addr = alloca [3 x i32]*, align 4
  store [3 x i32]* %a, [3 x i32]** %a.addr, align 4
  %0 = load [3 x i32]*, [3 x i32]** %a.addr, align 4
  %arrayidx = getelementptr inbounds [3 x i32], [3 x i32]* %0, i32 1
  %arrayidx1 = getelementptr inbounds [3 x i32], [3 x i32]* %arrayidx, i32 0, i32 2
  %1 = load i32, i32* %arrayidx1, align 4
  %2 = load [3 x i32]*, [3 x i32]** %a.addr, align 4
  %arrayidx2 = getelementptr inbounds [3 x i32], [3 x i32]* %2, i32 0
  %arrayidx3 = getelementptr inbounds [3 x i32], [3 x i32]* %arrayidx2, i32 0, i32 1
  %3 = load i32, i32* %arrayidx3, align 4
  %add = add nsw i32 %1, %3
  %4 = load [3 x i32]*, [3 x i32]** %a.addr, align 4
  %arrayidx4 = getelementptr inbounds [3 x i32], [3 x i32]* %4, i32 2
  %arrayidx5 = getelementptr inbounds [3 x i32], [3 x i32]* %arrayidx4, i32 0, i32 0
  %5 = load i32, i32* %arrayidx5, align 4
  %add6 = add nsw i32 %add, %5
  ret i32 %add6
}

; Function Attrs: noinline nounwind
define dso_local arm_aapcscc i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call arm_aapcscc i32 @fun([3 x i32]* getelementptr inbounds ([3 x [3 x i32]], [3 x [3 x i32]]* @ar, i32 0, i32 0))
  ret i32 %call
}

attributes #0 = { noinline nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+armv7-a,+dsp,+soft-float,+strict-align,-bf16,-crypto,-d32,-dotprod,-fp-armv8,-fp-armv8d16,-fp-armv8d16sp,-fp-armv8sp,-fp16,-fp16fml,-fp64,-fpregs,-fullfp16,-mve,-mve.fp,-neon,-thumb-mode,-vfp2,-vfp2sp,-vfp3,-vfp3d16,-vfp3d16sp,-vfp3sp,-vfp4,-vfp4d16,-vfp4d16sp,-vfp4sp" "unsafe-fp-math"="false" "use-soft-float"="true" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"min_enum_size", i32 4}
!2 = !{!"Ubuntu clang version 12.0.0-1ubuntu1"}
