/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_fcmple
	MOV r12, #0xFF000000

	/* Check if a is NaN */
	CMP r12, r0, LSL #1
	/* If b is NaN, make it positive to guarantee that its value (0x7F8nnnnn) is greater than any b */
	ANDLO r0, r0, #0x7FFFFFFF
	
	/* Check if b is NaN */
	CMP r12, r1, LSL #1
	/* If a is NaN, make it negative to guarantee that its value (0xFF8nnnnn) is less than any a */
	ORRLO r1, r1, #0x80000000
	
	/* Check if a is negative */
	CMP r0, #0
	/* If negative than a = 0x80000000 - a */
	RSBMI r0, r0, #0x80000000
	
	/* Check if b is negative */
	CMP r1, #0
	/* If negative than b = 0x80000000 - b */
	RSBMI r1, r1, #0x80000000

	/* Normal case */
	CMP r0, r1
	MOV r0, #1
	MOVGT r0, #0
	BX lr
END_ARM_FUNCTION __aeabi_fcmple
