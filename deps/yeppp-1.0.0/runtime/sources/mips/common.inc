/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.macro BEGIN_MIPS_FUNCTION name
	.section .text.\name,"ax",@progbits
	.globl \name
	.align 2
	.set nomips16
	.func \name
	.type \name, @function
	.internal \name
\name:
	.set noreorder
.endm

.macro END_MIPS_FUNCTION name
	.set reorder
	.endfunc
	.size \name, .-\name
.endm
