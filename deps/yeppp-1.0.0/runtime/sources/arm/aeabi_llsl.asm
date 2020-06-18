/*
 *               Yeppp! library runtime infrastructure
 *
 * This file is part of Yeppp! library and licensed under MIT license.
 * See runtime/LICENSE.txt for details.
 *
 */

.include "common.inc"

.syntax unified

BEGIN_ARM_FUNCTION __aeabi_llsl
	.arch armv5t
	LSL r0, r0, r2 /* r0 = hi(x) << n */
	RSB r3, r2, #32 /* r3 = 32 - n */
	ORR r0, r0, r1, LSR r3 /* r0 = (hi(x) << n) | (lo(x) >> (32 - n)) */
	SUBS r3, r2, #32 /* r3 = n - 32 */
	LSLHS r0, r1, r3 /* If n >= 32 then r0 = r1 << (n - 32) */
	LSL r1, r1, r2 /* r1 = lo(x) << n. If n >= 32 then r1 = 0 */
	BX lr
END_ARM_FUNCTION __aeabi_llsl
