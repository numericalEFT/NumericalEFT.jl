/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION memcpy
	.arch armv5t
	TST r2, r2
	BEQ .finish
	MOV r3, r0
.copy_byte:
	LDRB r12, [r1], #1
	STRB r12, [r3], #1
	SUBS r2, #1
	BNE .copy_byte
.finish:
	BX lr
END_ARM_FUNCTION memcpy
