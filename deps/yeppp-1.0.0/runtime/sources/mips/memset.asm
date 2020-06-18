/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.abicalls
.include "common.inc"
 
BEGIN_MIPS_FUNCTION memset
	.set mips32
	BEQZ $a2, .finish
	MOVE $v0, $a0

.write_byte:
	ADDIU $a2, -1
	SB $a1, ($a0)
	BNEZ $a2, .write_byte
	ADDIU $a0, $a0, 1
	
.finish:
	JR $ra
	NOP
END_MIPS_FUNCTION memset
