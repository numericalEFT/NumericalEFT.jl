/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_fcmpne
	MOV r12, #0xFF000000

	/* Check if a is NaN */
	CMP r12, r0, LSL #1
	/* If a is NaN, make it 1 greater than b to guarantee that it never equals b */
	ADDLO r0, r1, #1

	/* If b is NaN, but a is not, then they are already not equal, and no adjustment is needed */

	/* Check if a is negative zero */
	TEQ r0, #0x80000000
	/* Replace with positive zero */
	MOVEQ r0, #0

	/* Check if b is negative zero */
	TEQ r1, #0x80000000
	/* Replace with positive zero */
	MOVEQ r1, #0

	/* Normal case */
	SUBS r0, r1
	MOVNE r0, #1
	BX lr
END_ARM_FUNCTION __aeabi_fcmpne
