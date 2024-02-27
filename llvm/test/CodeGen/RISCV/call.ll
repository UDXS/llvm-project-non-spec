; ModuleID = 'call.c'
source_filename = "call.c"
target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128"
target triple = "riscv64-unknown-unknown-elf"

@.str = private unnamed_addr constant [34 x i8] c"Values are not equal. Result: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [30 x i8] c"Values are equal. Result: %d\0A\00", align 1

; Function Attrs: nounwind
define dso_local void @compareValues(i32 noundef signext %0, i32 noundef signext %1) local_unnamed_addr #0 {
  %3 = icmp eq i32 %0, %1
  %4 = tail call i64 @time(ptr noundef null) #3
  %5 = trunc i64 %4 to i32
  tail call void @srand(i32 noundef signext %5) #3
  br i1 %3, label %17, label %8

6:                                                ; preds = %8
  %7 = tail call signext i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef signext %12)
  br label %24

8:                                                ; preds = %2, %8
  %9 = phi i32 [ %13, %8 ], [ 0, %2 ]
  %10 = phi i32 [ %12, %8 ], [ 0, %2 ]
  %11 = tail call signext i32 @rand() #3
  %12 = add nsw i32 %11, %10
  %13 = add nuw nsw i32 %9, 1
  %14 = icmp eq i32 %13, 10000
  br i1 %14, label %6, label %8, !llvm.loop !4

15:                                               ; preds = %17
  %16 = tail call signext i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str.1, i32 noundef signext %21)
  br label %24

17:                                               ; preds = %2, %17
  %18 = phi i32 [ %22, %17 ], [ 0, %2 ]
  %19 = phi i32 [ %21, %17 ], [ 0, %2 ]
  %20 = tail call signext i32 @rand() #3
  %21 = sub nsw i32 %19, %20
  %22 = add nuw nsw i32 %18, 1
  %23 = icmp eq i32 %22, 5000
  br i1 %23, label %15, label %17, !llvm.loop !6

24:                                               ; preds = %15, %6
  ret void
}

declare dso_local void @srand(i32 noundef signext) local_unnamed_addr #1

declare dso_local i64 @time(ptr noundef) local_unnamed_addr #1

declare dso_local signext i32 @rand() local_unnamed_addr #1

; Function Attrs: nofree nounwind
declare dso_local noundef signext i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2

; Function Attrs: nounwind
define dso_local signext i32 @main() local_unnamed_addr #0 {
  %1 = tail call i64 @time(ptr noundef null) #3
  %2 = trunc i64 %1 to i32
  tail call void @srand(i32 noundef signext %2) #3
  %3 = tail call signext i32 @rand() #3
  %4 = tail call signext i32 @rand() #3
  %5 = icmp eq i32 %3, %4
  %6 = tail call i64 @time(ptr noundef null) #3
  %7 = trunc i64 %6 to i32
  tail call void @srand(i32 noundef signext %7) #3
  br i1 %5, label %19, label %10

8:                                                ; preds = %10
  %9 = tail call signext i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef signext %14)
  br label %26

10:                                               ; preds = %0, %10
  %11 = phi i32 [ %15, %10 ], [ 0, %0 ]
  %12 = phi i32 [ %14, %10 ], [ 0, %0 ]
  %13 = tail call signext i32 @rand() #3
  %14 = add nsw i32 %13, %12
  %15 = add nuw nsw i32 %11, 1
  %16 = icmp eq i32 %15, 10000
  br i1 %16, label %8, label %10, !llvm.loop !4

17:                                               ; preds = %19
  %18 = tail call signext i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str.1, i32 noundef signext %23)
  br label %26

19:                                               ; preds = %0, %19
  %20 = phi i32 [ %24, %19 ], [ 0, %0 ]
  %21 = phi i32 [ %23, %19 ], [ 0, %0 ]
  %22 = tail call signext i32 @rand() #3
  %23 = sub nsw i32 %21, %22
  %24 = add nuw nsw i32 %20, 1
  %25 = icmp eq i32 %24, 5000
  br i1 %25, label %17, label %19, !llvm.loop !6

26:                                               ; preds = %8, %17
  ret i32 0
}

attributes #0 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv64" "target-features"="+64bit,+a,+c,+m,+relax,-d,-e,-experimental-smaia,-experimental-ssaia,-experimental-zacas,-experimental-zfa,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicond,-experimental-ztso,-experimental-zvbb,-experimental-zvbc,-experimental-zvfbfmin,-experimental-zvfbfwma,-experimental-zvkb,-experimental-zvkg,-experimental-zvkn,-experimental-zvknc,-experimental-zvkned,-experimental-zvkng,-experimental-zvknha,-experimental-zvknhb,-experimental-zvks,-experimental-zvksc,-experimental-zvksed,-experimental-zvksg,-experimental-zvksh,-experimental-zvkt,-f,-h,-save-restore,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvmac,-xcvsimd,-xsfcie,-xsfvcp,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zicbom,-zicbop,-zicboz,-zicntr,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #1 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv64" "target-features"="+64bit,+a,+c,+m,+relax,-d,-e,-experimental-smaia,-experimental-ssaia,-experimental-zacas,-experimental-zfa,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicond,-experimental-ztso,-experimental-zvbb,-experimental-zvbc,-experimental-zvfbfmin,-experimental-zvfbfwma,-experimental-zvkb,-experimental-zvkg,-experimental-zvkn,-experimental-zvknc,-experimental-zvkned,-experimental-zvkng,-experimental-zvknha,-experimental-zvknhb,-experimental-zvks,-experimental-zvksc,-experimental-zvksed,-experimental-zvksg,-experimental-zvksh,-experimental-zvkt,-f,-h,-save-restore,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvmac,-xcvsimd,-xsfcie,-xsfvcp,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zicbom,-zicbop,-zicboz,-zicntr,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #2 = { nofree nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv64" "target-features"="+64bit,+a,+c,+m,+relax,-d,-e,-experimental-smaia,-experimental-ssaia,-experimental-zacas,-experimental-zfa,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicond,-experimental-ztso,-experimental-zvbb,-experimental-zvbc,-experimental-zvfbfmin,-experimental-zvfbfwma,-experimental-zvkb,-experimental-zvkg,-experimental-zvkn,-experimental-zvknc,-experimental-zvkned,-experimental-zvkng,-experimental-zvknha,-experimental-zvknhb,-experimental-zvks,-experimental-zvksc,-experimental-zvksed,-experimental-zvksg,-experimental-zvksh,-experimental-zvkt,-f,-h,-save-restore,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvmac,-xcvsimd,-xsfcie,-xsfvcp,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zicbom,-zicbop,-zicboz,-zicntr,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"target-abi", !"lp64"}
!2 = !{i32 8, !"SmallDataLimit", i32 8}
!3 = !{!"clang version 18.0.0 (https://github.com/Nish1809/llvm-project-non-spec.git a906bd2708ef3a022fcea8b4aafafabf770c4b87)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
