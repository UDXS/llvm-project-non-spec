	# RUN: llvm-mc %s -triple=riscv32f -riscv-no-aliases -show-encoding \
	# RUN:     | FileCheck -check-prefixes=RV32IF-LP64F %s
	# RUN: llvm-mc -filetype=obj -triple=riscv32f < %s \
	# RUN:     | llvm-objdump -riscv-no-aliases -d -r - \
	# RUN:     | FileCheck -check-prefixes=RV32IF-LP64F %s

	# CHECK-ASM-AND-OBJ: bitrev a0, a1
	fadd.s f0, f1, f2