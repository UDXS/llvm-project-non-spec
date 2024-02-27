	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1_zicsr2p0_zifencei2p0"
	.file	"call.c"
	.option	push
	.option	arch, +a, +c, +m, -zicsr, -zifencei
	.globl	compareValues                   # -- Begin function compareValues
	.p2align	1
	.type	compareValues,@function
compareValues:                          # @compareValues
# %bb.0:
	addi	sp, sp, -32
	sd	ra, 24(sp)                      # 8-byte Folded Spill
	sd	s0, 16(sp)                      # 8-byte Folded Spill
	sd	s1, 8(sp)                       # 8-byte Folded Spill
	mv	s1, a0
	li	a0, 0
	mv	s0, a1
	call	time
	sext.w	a0, a0
	call	srand
	sub	s1, s1, s0
	seqz	a0, s1
	bmovt	bt0, .LBB0_2
	bmovs	bs0, .LBB0_2
	bmovc	bc0, a0, 1
	pb	s:bs0, t:bt0, c:bc0
# %bb.2:                                # %.preheader
	lui	a0, 1
	li	s0, 0
	addiw	s1, a0, 904
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	call	rand
	addiw	s1, s1, -1
	sub	a1, s1, zero
	seqz	a1, a1
	bmovc	bc0, a1, 1
	bmovt	bt0, .LBB0_5
	bmovs	bs0, .LBB0_5
	subw	s0, s0, a0
	pb	s:bs0, t:bt0, c:bc0
# %bb.5:
	mv	a1, s0
	ld	ra, 24(sp)                      # 8-byte Folded Reload
	ld	s0, 16(sp)                      # 8-byte Folded Reload
	ld	s1, 8(sp)                       # 8-byte Folded Reload
	lui	a0, %hi(.L.str.1)
	addi	a0, a0, %lo(.L.str.1)
	addi	sp, sp, 32
	tail	printf
.LBB0_1:                                # %.preheader1
	lui	a0, 2
	li	s0, 0
	addiw	s1, a0, 1808
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	call	rand
	addiw	s1, s1, -1
	sub	a1, s1, zero
	seqz	a1, a1
	bmovc	bc0, a1, 1
	bmovt	bt0, .LBB0_3
	bmovs	bs0, .LBB0_3
	addw	s0, s0, a0
	pb	s:bs0, t:bt0, c:bc0
# %bb.3:
	mv	a1, s0
	ld	ra, 24(sp)                      # 8-byte Folded Reload
	ld	s0, 16(sp)                      # 8-byte Folded Reload
	ld	s1, 8(sp)                       # 8-byte Folded Reload
	lui	a0, %hi(.L.str)
	addi	a0, a0, %lo(.L.str)
	addi	sp, sp, 32
	tail	printf
.Lfunc_end0:
	.size	compareValues, .Lfunc_end0-compareValues
                                        # -- End function
	.option	pop
	.option	push
	.option	arch, +a, +c, +m, -zicsr, -zifencei
	.globl	main                            # -- Begin function main
	.p2align	1
	.type	main,@function
main:                                   # @main
# %bb.0:
	addi	sp, sp, -32
	sd	ra, 24(sp)                      # 8-byte Folded Spill
	sd	s0, 16(sp)                      # 8-byte Folded Spill
	sd	s1, 8(sp)                       # 8-byte Folded Spill
	li	a0, 0
	call	time
	sext.w	a0, a0
	call	srand
	call	rand
	mv	s0, a0
	call	rand
	mv	s1, a0
	li	a0, 0
	call	time
	sext.w	a0, a0
	call	srand
	sub	s0, s0, s1
	seqz	a0, s0
	bmovt	bt0, .LBB1_1
	bmovs	bs0, .LBB1_1
	bmovc	bc0, a0, 1
	pb	s:bs0, t:bt0, c:bc0
# %bb.1:                                # %.preheader
	lui	a0, 1
	li	s0, 0
	addiw	s1, a0, 904
.LBB1_2:                                # =>This Inner Loop Header: Depth=1
	call	rand
	addiw	s1, s1, -1
	sub	a1, s1, zero
	seqz	a1, a1
	bmovc	bc0, a1, 1
	bmovt	bt0, .LBB1_3
	bmovs	bs0, .LBB1_3
	subw	s0, s0, a0
	pb	s:bs0, t:bt0, c:bc0
# %bb.3:
	lui	a0, %hi(.L.str.1)
	addi	a0, a0, %lo(.L.str.1)
	j	.LBB1_7
.LBB1_4:                                # %.preheader1
	lui	a0, 2
	li	s0, 0
	addiw	s1, a0, 1808
.LBB1_5:                                # =>This Inner Loop Header: Depth=1
	call	rand
	addiw	s1, s1, -1
	sub	a1, s1, zero
	seqz	a1, a1
	bmovc	bc0, a1, 1
	bmovt	bt0, .LBB1_6
	bmovs	bs0, .LBB1_6
	addw	s0, s0, a0
	pb	s:bs0, t:bt0, c:bc0
# %bb.6:
	lui	a0, %hi(.L.str)
	addi	a0, a0, %lo(.L.str)
.LBB1_7:
	mv	a1, s0
	call	printf
# %bb.8:
	ld	ra, 24(sp)                      # 8-byte Folded Reload
	ld	s0, 16(sp)                      # 8-byte Folded Reload
	ld	s1, 8(sp)                       # 8-byte Folded Reload
	li	a0, 0
	addi	sp, sp, 32
	ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        # -- End function
	.option	pop
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Values are not equal. Result: %d\n"
	.size	.L.str, 34

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"Values are equal. Result: %d\n"
	.size	.L.str.1, 30

	.ident	"clang version 18.0.0 (https://github.com/Nish1809/llvm-project-non-spec.git a906bd2708ef3a022fcea8b4aafafabf770c4b87)"
	.section	".note.GNU-stack","",@progbits
