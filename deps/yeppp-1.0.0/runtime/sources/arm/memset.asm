/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION memset
	.arch armv5t
	TST r2, r2
	BEQ .return

	MOV r3, r0
.store_byte:
	STRB r1, [r3], #1
	SUBS r2, #1
	BNE .store_byte

.return:
	BX lr
END_ARM_FUNCTION memset

