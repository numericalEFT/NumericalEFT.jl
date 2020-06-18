/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_ui2f
	.arch armv5t

	x .req r0
	n .req r1
	tmp .req r2
	/* Normalize x */
	CLZ n, x
	LSLS x, x, n
	RSB n, n, #157
	MOVEQ n, #0
	/* Round x */
	LSRS tmp, x, #9
	ADCS x, x, #0x7F
	/* Add 1 for MSB position increase, and 1 because the leading bit is not 1 anymore (normally added to exponent) */
	ADDCS n, n, #2
	/* Add exponent */
	LSL n, n, #23
	ADD x, n, x, LSR #8
	BX lr
END_ARM_FUNCTION __aeabi_ui2f
