	# RUN: llvm-mc %s -triple=riscv32 -riscv-no-aliases -show-encoding \
	# RUN:     | FileCheck -check-prefixes=CHECK-ASM-AND-OBJ %s
	# RUN: llvm-mc -filetype=obj -triple=riscv32 < %s \
	# RUN:     | llvm-objdump -riscv-no-aliases -d -r - \
	# RUN:     | FileCheck -check-prefixes=CHECK-ASM-AND-OBJ %s

	# CHECK-ASM-AND-OBJ: bmovt b1, 0x0, b1
    
	bmovc b1, b1, a0