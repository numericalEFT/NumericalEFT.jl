/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.abicalls
.include "common.inc"
 
BEGIN_MIPS_FUNCTION memcmp
	.set mips32
	BEQZ $a2, .return_zero
	NOP

.compare_byte:
	LBU $t0, ($a0)
	ADDIU $a2, $a2, -1
	LBU $t1, ($a1)
	BNE $t0, $t1, .return_sign
	ADDIU $a0, $a0, 1

	BNEZ $a2, .compare_byte
	ADDIU $a1, $a1, 1

.return_zero:
	JR $ra
	MOVE $v0, $zero

.return_sign:
	SLTU $v0, $t1, $t0 
	SLTU $t0, $t0, $t1
	JR $ra
	SUBU $v0, $v0, $t0
END_MIPS_FUNCTION memcmp
