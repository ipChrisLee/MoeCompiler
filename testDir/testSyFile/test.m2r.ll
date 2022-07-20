; ModuleID = 'testDir/testSyFile/test.ll'
source_filename = "testDir/testSyFile/test.sy"
target datalayout = "e-m:e-p:32:32-Fi8-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "armv7-unknown-linux-gnueabihf"

%struct.timeval = type { i32, i32 }

@_sysy_start = dso_local global %struct.timeval zeroinitializer, align 4
@_sysy_end = dso_local global %struct.timeval zeroinitializer, align 4
@_sysy_l1 = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_l2 = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_h = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_m = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_s = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_us = dso_local global [1024 x i32] zeroinitializer, align 4
@_sysy_idx = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind
define dso_local i32 @main() #0 {
entry:
  %"reg2mem alloca point" = bitcast i32 0 to i32
  call void @putfloat(float 1.000000e+01)
  ret i32 0
}

declare dso_local void @putfloat(float) #1

attributes #0 = { noinline nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+armv7-a,+dsp,+fp64,+vfp2,+vfp2sp,+vfp3d16,+vfp3d16sp,-thumb-mode" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+armv7-a,+dsp,+fp64,+vfp2,+vfp2sp,+vfp3d16,+vfp3d16sp,-thumb-mode" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"min_enum_size", i32 4}
!2 = !{!"Ubuntu clang version 12.0.0-1ubuntu1"}
