/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See LICENSE for details.
 *
 */


.macro BEGIN_X86_FUNCTION name
	.section .text.\name,"ax",@progbits
	.globl \name
	.align 4
	.func \name
	.internal \name
	.intel_syntax noprefix
\name:
.endm

.macro END_X86_FUNCTION name
	.att_syntax
	.endfunc
	.type \name, @function
	.size \name, .-\name
.endm

.macro BEGIN_X86_SUBFUNCTION name
	.globl \name
	.internal \name
\name:
.endm

.macro END_X86_SUBFUNCTION name
	.type \name, @function
	.size \name, .-\name
.endm
