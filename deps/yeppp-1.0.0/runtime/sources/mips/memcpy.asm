/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.abicalls
.include "common.inc"
 
BEGIN_MIPS_FUNCTION memcpy
	.set mips32
	BEQZ $a2, .finish
	MOVE $v0, $a0

.copy_byte:
	LBU $t0, ($a1)
	ADDIU $a2, $a2, -1
	SB $t0, ($a0)
	ADDIU $a1, $a1, 1
	BNEZ $a2, .copy_byte
	ADDIU $a0, $a0, 1
	
.finish:
	JR $ra
	NOP
END_MIPS_FUNCTION memcpy
