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
 * @details "An Overview of Floating-Point Support and Math Library on the Intel XScale Architecture"
 * @details "Handbook of Floating-Point Arithmetic", Chapter 10, Section 10.3
 */
BEGIN_ARM_FUNCTION __aeabi_fmul
	.arch armv5t
	MOV r12, 0x000000FF
	ANDS r2, r12, r0, LSR #23 /* r2[8:31] = 0, r2[0:7] = a0.biased_exponent, APSR.Z = (a0.biased_exponent == 0 [zero or subnormal a0]) */
	CMPNE r2, 0xFF /* APSR.Z ||= (a0.biased_exponent == 0xFF [infinity or NaN a0]) */
	ANDSNE r3, r12, r1, LSR #23 /* If APSR.Z != 0 then r3[8:31] = 0, r3[0:7] = a1.biased_exponent, APSR.Z ||= (a1.biased_exponent == 0 [zero or subnormal a1]) */
	CMPNE r3, 0xFF /* APSR.Z ||= (a1.biased_exponent == 0xFF [infinity or NaN a1]) */
	BEQ .special_input /* Handle situations where any operand is zero, subnormal, infinite, or NaN */

	/* Both operands are finite normalized normals */
	EOR r12, r0, r1 /* r12[31] = a0.sign ^ a1.sign, r12[0:30] = garbage */
	ADD r2, r3 /* r2 = a0.biased_exponent + a1.biased_exponent */
	MOV r3, 0x80000000
	ORR r0, r3, r0, LSL #8 /* r0[8:31] = a0.mantissa_with_implied_bit (r0[31] == 1), r0[0:7] = 0 */
	ORR r1, r3, r1, LSL #8 /* r1[8:31] = a1.mantissa_with_implied_bit (r1[31] == 1), r1[0:7] = 0 */
	UMULL r0, r3, r1, r0 /* r3:r0 = (a0.mantissa_with_implied_bit * a1.mantissa_with_implied_bit) << 16 */
	AND r12, r12, 0x80000000 /* r12[31] = a0.sign ^ a1.sign, r12[0:30] = 0 */
	
	CMP r3, 0x80000000 /* APSR.C = (r3 >= 0x80000000) */
	ADDLO r3, r3, r3 /* If the high part of multiplication is only 31-bit wide, double it */
	ADC r2, r2, -128 /* and add 1 to exponent. Here it is combined with subtracting 0x7F from biased exponent */

	CMP r0, 0x0000001 /* APSR.C = (r0 != 0) */
	LSRSCC r1, r3, #9 /* APSR.C ||= (r3[9] == 1)) */
	ADC r3, r3, 0x0000007F /* If ((r0 != 0) || (r3[9] == 1)) add 1 to guard bit. If guard bit is also one, addition will propogate to the least significant bit. */

	CMP r2, 0xFE
	BHS .normal_path_special_output
	
	ORR r0, r12, r2, LSL #23
	ADD r0, r0, r3, LSR #8
	
	BX lr
	
.special_input:
	ADD r2, r0, r0
	ADD r3, r1, r1

	/* Check for infinity and NaN */
	CMP r2, 0xFF000000
	CMPLO r3, 0xFF000000
	BHS .inf_nan_input
	
	/* Check for zero input */
	CMP r2, 0
	CMPNE r3, 0
	EOREQ r0, r0, r1
	ANDEQ r0, r0, 0x80000000
	BXEQ lr
	
	/* Denormal input */
	EOR r12, r0, r1

	CLZ r0, r2
	SUBS r0, r0, #8 /* a0 is denormalized iff r0 >= 8 */
	ADDHS r0, r0, #1
	LSLHS r2, r2, r0
	MOVLO r0, #0
	RSB r0, r0, r2, LSR #24
	
	CLZ r1, r3
	SUBS r1, r1, #8 /* a1 is denormalized iff r1 >= 8 */
	ADDHS r1, r1, #1
	LSLHS r3, r3, r1
	MOVLO r1, #0
	RSB r1, r1, r3, LSR #24
	
	ADD r0, r1
	MOV r1, 0x80000000
	ORR r2, r1, r2, LSL #7 /* r2 = a0.mantissa_with_implied_bit */
	ORR r3, r1, r3, LSL #7 /* r3 = a1.mantissa_with_implied_bit */
	UMULL r2, r1, r3, r2
	AND r12, r12, 0x80000000
	
	CMP r1, 0x80000000 /* APSR.C = (r12 >= 0x80000000) */
	ADDLO r1, r1, r1
	ADC r0, r0, -128

	CMP r2, 0x0000001 /* APSR.C = (r2 != 0) */
	LSRSCC r3, r1, #9 /* APSR.C = ((r2 != 0) || (r1[9] == 1)) */
	ADC r1, r1, 0x0000007F

	CMP r0, 0xFE
	BHS .denormal_path_special_output
	
	ORR r0, r12, r0, LSL #23
	ADD r0, r0, r1, LSR #8

	BX lr

.normal_path_special_output:
	/* r12[31] = sign, r12[0:30] = 0 */
	/* r2 = exponent, APSR set according to CMP r2, 0xFE */
	/* r3[8:31] = mantissa_with_implied_bit */

	/* Handle overflow */
	MOV r0, 0xFF000000
	ORR r0, r12, r0, LSR #1
	BXPL lr

	/* Handle underflow */
	LSR r3, r3, #8
	MOV r1, #1
	RSB r2, r2, #0
	RSB r1, r1, r1, LSL r2 /* r1 has r2 ones on the right */
	AND r0, r3, r1, LSR #1
	CMP r0, #1
	LSR r0, r3, r2
	LSRSCC r0, r0, #1
	ADC r3, r3, r1, LSR #1

	ORR r0, r12, r3, LSR r2
	BX lr

.denormal_path_special_output:
	/* r12[31] = sign, r12[0:30] = 0 */
	/* r0 = exponent, APSR set according to CMP r0, 0xFE */
	/* r1[8:31] = mantissa_with_implied_bit */
	
	/* Never overflows */

	/* Handle underflow */
	LSR r1, r1, #8
	MOV r3, #1
	RSB r0, r0, #0
	RSB r3, r3, r3, LSL r0 /* r3 has r0 ones on the right */
	AND r2, r1, r3, LSR #1
	CMP r2, #1
	LSR r2, r1, r0
	LSRSCC r2, r2, #1
	ADC r1, r1, r3, LSR #1

	ORR r0, r12, r1, LSR r0
	BX lr

.inf_nan_input:
	CMP r2, 0xFF000000
	CMPLS r3, 0xFF000000
	BHI .nan_input
	
	EOR r0, r0, r1
	AND r0, r0, 0x80000000
	ORR r0, r0, r12, LSL #23
	
	CMP r2, 0
	CMPNE r3, 0
	ORREQ r0, r0, 0x00400000
	ANDEQ r0, r0, 0x7FFFFFFF
	
	BX lr

.nan_input:
	CMP r2, 0xFF000000
	MOVLS r0, r1
	ORR r0, 0x00400000
	BX lr
	
END_ARM_FUNCTION __aeabi_fmul
