@G.len = dso_local constant i32 2, align 4
@G.x = dso_local constant i32 5, align 4
@G.arr1 = dso_local constant [2 x [2 x i32]] [[2 x i32] [i32 5, i32 10], [2 x i32] [i32 0, i32 0]], align 4
@G.i1 = dso_local global i32 7, align 4
@G.i2 = dso_local global i32 5, align 4
@G.i3 = dso_local global [2 x [2 x i32]] [[2 x i32] [i32 5, i32 5], [2 x i32] [i32 0, i32 0]], align 4
@G.i4 = dso_local global [3 x [4 x i32]] [[4 x i32] [i32 2, i32 0, i32 0, i32 0], [4 x i32] [i32 0, i32 0, i32 0, i32 0], [4 x i32] [i32 0, i32 0, i32 0, i32 0]], align 4
