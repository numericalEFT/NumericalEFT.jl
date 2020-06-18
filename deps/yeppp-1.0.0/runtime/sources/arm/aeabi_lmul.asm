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
 * @brief	EABI helper function which performs multiplication of two 64-bit integers producing 64-bit result.
 * @param	a	The first 64-bit integer to be multiplied. Passed in registers r0 (low part) and r1 (high part).
 * @param	b	The second 64-bit integer to be multiplied. Passed in registers r2 (low part) and r2 (high part).
 * @return	The low 64 bits of the product of @a a and @b b. The low 32 bits of the result are returned in r0, and the high 32 bits of the results are returned in r1.
 * @note	To comply with ARM EABI speficiation this function should not change only registers other than r0, r1, r2, r3, and r12.
 */
BEGIN_ARM_FUNCTION __aeabi_lmul
	.arch armv5t
	MUL r1, r2, r1
	MLA r1, r0, r3, r1
	UMLAL r0, r1, r2, r0
	BX lr
END_ARM_FUNCTION __aeabi_lmul
