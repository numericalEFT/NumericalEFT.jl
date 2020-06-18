/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */
 
.include "common.inc"

.syntax unified

/**
 * @brief	EABI helper function which performs three-way comparison of two unsigned 64-bit integers.
 * @param	a	The first unsigned 64-bit integer to be compared. Passed in registers r0 (low part) and r1 (high part).
 * @param	b	The second unsigned 64-bit integer to be compared. Passed in registers r2 (low part) and r2 (high part).
 * @retval	1	If a is greater than b (unsigned comparison).
 * @retval	0	If a equals b.
 * @retval	-1	If a is less than b (unsigned comparison).
 * @note	To comply with ARM EABI speficiation this function should not change only registers other than r0, r1, r2, r3, and r12.
 */
BEGIN_ARM_FUNCTION __aeabi_ulcmp
	.arch armv5t
	CMP r1, r3
	SUBSEQ r0, r2 @ If a == b then r0 is set to 0
	MOVHI r0, #1
	MOVLO r0, #-1
END_ARM_FUNCTION __aeabi_ulcmp
