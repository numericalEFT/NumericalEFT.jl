/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION memcmp
	.arch armv5t
	TST r2, r2
	BEQ .return_zero
.compare_byte:
	LDRB r3, [r0], #1
	LDRB r12, [r1], #1
	SUBS r3, r12
	BNE .return_sign
	SUBS r2, #1
	BNE .compare_byte
.return_zero:
	MOV r0, #0
	BX lr
.return_sign:
	MOV r0, r3
	BX lr
END_ARM_FUNCTION memcmp
