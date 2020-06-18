/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_dcmpeq
	.arch armv5t
	/* Check for +-0.0 == +=0.0 case */
	ORR r12, r0, r2
	ORR r12, r12, r1, LSL #1
	ORRS r12, r12, r3, LSL #1
	MOVEQ r1, r3

	/* Check for NaN in the first operand */
	ORR r12, r1, 0x80000000
	CMP r12, 0xFFF00000
	TEQEQ r0, 0
	ADCHI r0, r2, 0
	
	/* Check for NaN in the second operand */
	ORR r12, r3, 0x80000000
	CMP r12, 0xFFF00000
	TEQEQ r2, 0
	ADCHI r0, r2, 0

	/* Normal case */
	CMP r1, r3
	CMPEQ r0, r2
	MOVEQ r0, #1
	MOVNE r0, #0
	BX lr
END_ARM_FUNCTION __aeabi_dcmpeq
