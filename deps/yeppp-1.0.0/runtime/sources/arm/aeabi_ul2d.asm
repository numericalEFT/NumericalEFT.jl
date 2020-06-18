/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_ul2d
	/* Check if high part is zero */
	hi .req r1
	lo .req r0
	TEQ hi, #0
	MOVEQ hi, lo
	MOVEQ lo, #0
	/* Normalize */
	n .req r3
	CLZ n, hi
	LSL r2, lo, n
	nexp .req r12
	RSB nexp, n, #32
	LSL hi, hi, n
	ORR hi, hi, lo, LSR nexp
	.unreq lo
	lo .req r2
	SUBEQ nexp, nexp, #32
	/* Round */
	msk .req r3
	MOV msk, #1
	TST lo, 0x00001000 /* Check is lo is even */
	RSB msk, msk, msk, LSL #10 /* msk is 0x7FF */
	ADD nexp, nexp, msk
	ADDNE msk, msk, #1
	ADDS lo, lo, msk
	ADCS hi, hi, #0
	/* Shift mantissa to its place */
	LSL r0, hi, #21
	LSR hi, hi, #11
	ORR r0, r0, lo, LSR #11
	.unreq lo
	lo .req r0
	ORRCS hi, hi, #0x00200000
	/* Add exponent */
	ADD nexp, nexp, 0x1E
	ORRS r3, hi, lo
	ADDNE hi, hi, nexp, LSL #20
	BX lr
END_ARM_FUNCTION __aeabi_ul2d
