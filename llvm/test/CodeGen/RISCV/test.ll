; RUN: llc -mtriple=riscv32 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s -check-prefix=RV64
declare i32 @llvm.bmov.i32(i32)

define i32 @bmov(i32 %a, i32 %b) {
%1 = add i32 %a, %b
%2 = and i32 %a, %b
%3 = li i32 %1, 10
  ret i32 %3
}