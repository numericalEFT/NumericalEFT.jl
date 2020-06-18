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
 * @brief	EABI helper function which performs division of two unsigned 32-bit integers producing unsigned 32-bit result.
 * @param	a	The 32-bit integer dividend. Passed in registers r0.
 * @param	b	The 32-bit integer divisor. Passed in registers r1. The divisor should not equal zero.
 * @return	The 32-bit unsigned integer quotient of @a a and @a b. The result is returned in r0.
 * @note	To comply with ARM EABI speficiation this function should not change only registers other than r0, r1, r2, r3, and r12.
 * @note	Implementation is based on description from http://me.henri.net/fp-div.html
 */
/*
BEGIN_ARM_FUNCTION __aeabi_uidiv
	NEG r12, r1
	BICS r1, r1, r12
	BEQ .power_of_two

	MOV r1, #0
	ADDS r0, r0, r0
	.rept 31
		ADCS r1, r12, r1, LSL #1
		SUBCC r1, r1, r12
		ADCS r0, r0, r0
	.endr
	ADCS r1, r12, r1, LSL #1
	ADCS r0, r0, r0
	BX lr

.power_of_two:
	NEGS r1, r12
	BEQ .zero
	AND r1, r12
	CLZ r1, r1
	RSB r1, r1, #31
	LSR r0, r0, r1
	BX lr

.zero:
	BKPT
	BX lr	
END_ARM_FUNCTION __aeabi_uidiv
*/

/**
 * @brief	EABI helper function which performs division of two unsigned 32-bit integers producing unsigned 32-bit result.
 * @param	a	The 32-bit integer dividend. Passed in registers r0.
 * @param	b	The 32-bit integer divisor. Passed in registers r1. The divisor should not equal zero.
 * @return	The 32-bit unsigned integer quotient of @a a and @a b. The result is returned in r0.
 * @note	To comply with ARM EABI speficiation this function should not change only registers other than r0, r1, r2, r3, and r12.
 * @note	Implementation is based on the paper "An overview of floating-point support and math library on the Intel XScale architecture" by C. Iordache and P.T.P.Tang
 */
BEGIN_ARM_FUNCTION __aeabi_uidiv
	.arch armv5te
	n   .req r2
	Yr  .req r3
	Y   .req r1
	X   .req r0

	LDR r3, .div_rcp_table_address
	CLZ n, Y
	tmp .req r12
	LSL tmp, Y, n
	NEG Y, Y
	ADDS tmp, tmp, tmp
	LSR tmp, tmp, #22
	BEQ .power_of_two
	ADD r3, r3, tmp, LSL #1
	msk .req r12
	MOV msk, #2

	LDRH Yr, [r3]
	LSL msk, msk, n
	RSB n, n, #15
	SUB msk, msk, #1
	AND Yr, msk, Yr, ROR n

	R .req r1
	MUL R, Yr, Y
	Rj_LO .req r2
	Rj_HI .req r3
	UMULL Rj_LO, Rj_HI, X, Yr

	Rk_LO .req r2
	Rk_HI .req r12
	MOV Rk_HI, #0
	Q .req r0
	MOV Q, Rj_HI

	UMLAL Rk_LO, Rk_HI, Rj_HI, R
	
	MOV Rj_HI, #0
	ADD Q, Q, Rk_HI

	UMLAL Rj_LO, Rj_HI, Rk_HI, R

	MOV Rk_HI, #0
	ADD Q, Q, Rj_HI
	
	UMLAL Rk_LO, Rk_HI, Rj_HI, R
	CMN Rk_LO, R
	ADC Q, Q, Rk_HI

	BX lr

.power_of_two:
	RSB n, n, #31
	BCC .zero
	LSR X, X, n
	BX lr

.zero:
	BKPT
	BX lr
.align 2
.div_rcp_table_address: .long uidiv_rcp_table
END_ARM_FUNCTION __aeabi_uidiv
