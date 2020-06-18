/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

.text

.abicalls

.macro BEGIN_MIPS_FUNCTION name
	.globl \name
	.align 2
	.set nomips16
	.func \name
	.type \name, @function
\name:
	.set noreorder
.endm

.macro END_MIPS_FUNCTION name
	.set reorder
	.endfunc
	.size \name, .-\name
.endm

BEGIN_MIPS_FUNCTION _yepLibrary_ProbeR2
	.internal _yepLibrary_ProbeR2
	.set mips32r2
	MOVE $v0, $zero
	# If the next line raises SIGILL,  the signal handler will change $v0 to 1 and skip the instruction (4 bytes)
	WSBH $t0, $zero
	JR $ra
	NOP
END_MIPS_FUNCTION _yepLibrary_ProbeR2

BEGIN_MIPS_FUNCTION _yepLibrary_ProbePairedSingle
	.internal _yepLibrary_ProbePairedSingle
	.set mips32r2
	MOVE $v0, $zero
	# If the next line raises SIGILL,  the signal handler will change $v0 to 1 and skip the instruction (4 bytes)
	ADD.PS $f4, $f4, $f4
	JR $ra
	NOP
END_MIPS_FUNCTION _yepLibrary_ProbePairedSingle
