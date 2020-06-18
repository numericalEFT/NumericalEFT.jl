/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.abicalls
.include "common.inc"
 
BEGIN_MIPS_FUNCTION __ashldi3
	.set mips32
	ANDI $t0, $a2, 32
	NEGU $t1, $a2
	
	MOVN $a1, $a0, $t0
	MOVN $a0, $zero, $t0
	
	SLLV $v1, $a1, $a2
	SLLV $v0, $a0, $a2
	
	SRLV $t0, $a0, $t1
	
	JR $ra
	OR $v1, $v1, $t0
END_MIPS_FUNCTION __ashldi3

