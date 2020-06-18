/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_dcmpge
	.arch armv5t
	/* Check for NaN in the first operand */
	ORR r12, r1, 0x80000000
	CMP r12, 0xFFF00000
	TEQEQ r0, 0
	ORRHI r3, r3, r12, ASR 31 /* Make the second operand NaN */
	
	/* Check for NaN in the second operand */
	ORR r12, r3, 0x80000000
	CMP r12, 0xFFF00000
	TEQEQ r2, 0
	MOVHI r1, 1 /* Make the second operand larger than the first */
	MOVHI r3, 2

	MOV r12, 0x7FFFFFFF

	SUBS r1, 0x80000000
	TEQEQ r0, 0
	MOVEQ r1, 0x80000000
	RSBSHS r0, r0, 0
	TST r1, 0x80000000
	RSCEQ r1, r1, r12
	
	SUBS r3, 0x80000000
	TEQEQ r2, 0
	MOVEQ r3, 0x80000000
	RSBSHS r2, r2, 0
	TST r3, 0x80000000
	RSCEQ r3, r3, r12

	/* Normal case */
	CMP r1, r3
	CMPEQ r0, r2
	MOVHS r0, #1
	MOVLO r0, #0
	BX lr
END_ARM_FUNCTION __aeabi_dcmpge
