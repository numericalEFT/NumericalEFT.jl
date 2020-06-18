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
 * @brief	EABI helper function which performs division of two unsigned 32-bit integers producing unsigned 32-bit quoties and unsigned 32-bit remainder.
 * @param	a	The 32-bit integer divisend. Passed in register r0.
 * @param	b	The 32-bit integer divisor. Passed in register r1. The divisor should not equal zero.
 * @return	The 32-bit unsigned integer quotient and remainder from division of @a a by @a b. The quotient is returned in r0 and the remainder in r1.
 * @note	To comply with ARM EABI speficiation this function should not change only registers other than r0, r1, r2, r3, and r12.
 * @note	Implementation is based on the paper "An overview of floating-point support and math library on the Intel XScale architecture" by C. Iordache and P.T.P.Tang
 */
BEGIN_ARM_FUNCTION __aeabi_uidivmod
	.arch armv5te
	n   .req r2
	Yr  .req r3
	Y   .req r1
	X   .req r0

	LDR r3, .div_rcp_table_address
	CLZ n, Y
	tmp .req r12
	LSL tmp, Y, n
	ADDS tmp, tmp, tmp
	LSR tmp, tmp, #22
	BEQ .power_of_two
	ADD r3, r3, tmp, LSL #1
	msk .req r12
	MOV msk, #2

	LDRH Yr, [r3]
	PUSH {r4, r5}
	LSL msk, msk, n
	RSB n, n, #15
	minusY .req r1
	NEG minusY, Y
	SUB msk, msk, #1
	AND Yr, msk, Yr, ROR n

	R .req r4
	MUL R, Yr, minusY
	Rj_LO .req r2
	Rj_HI .req r3
	UMULL Rj_LO, Rj_HI, X, Yr

	Rk_LO .req r2
	Rk_HI .req r12
	MOV Rk_HI, #0
	Q .req r5
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

	MLA Y, Q, minusY, X
	MOV X, Q
	POP {r4, r5}
	BX lr

.power_of_two:
	RSB n, n, #31
	BCC .zero
	msk .req r12
	MOV msk, #1
	RSB msk, msk, msk, LSL n
	AND Y, X, msk
	LSR X, X, n
	BX lr

.zero:
	BKPT
	BX lr
.align 2
.div_rcp_table_address: .long uidiv_rcp_table
END_ARM_FUNCTION __aeabi_uidivmod
