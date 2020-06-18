/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_i2d
	.arch armv5t
	MOV r12, 0xFFFFFFFF
	ANDS r1, r0, 0x80000000
	NEGNE r0, r0
	CLZ r2, r0
	RSB r3, r2, r12, LSR #22
	RSB r2, r2, 11
	ADD r3, r3, 31
	RORS r0, r0, r2
	ORRNE r1, r1, r3, LSL #20
	AND r2, r0, r12, LSR #12
	AND r0, r0, r12, LSL #21
	ORR r1, r1, r2
	BX lr
END_ARM_FUNCTION __aeabi_i2d
