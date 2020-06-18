/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See LICENSE for details.
 *
 */

.macro BEGIN_ARM_FUNCTION name
	.section .text.\name,"ax",%progbits
	.arm
	.globl \name
	.align 2
	.func \name
	.internal \name
\name:
.endm

.macro END_ARM_FUNCTION name
	.endfunc
	.type \name, %function
	.size \name, .-\name
.endm

.macro BEGIN_THUMB_FUNCTION name
	.section .text.\name,"ax",%progbits
	.thumb
	.globl \name
	.align 2
	.func \name
	.internal \name
	.thumb_func
\name:
.endm

.macro END_THUMB_FUNCTION name
	.endfunc
	.type \name, %function
	.size \name, .-\name
.endm
