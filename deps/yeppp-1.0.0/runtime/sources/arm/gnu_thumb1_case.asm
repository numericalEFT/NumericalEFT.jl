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
 * @brief	GCC helper function for switch statements optimized using a jump table with 8-bit signed values.
 * @details	GCC may generate a call to this function from Thumb-1 code when optimization is on.
 *         	The function makes a table lookup to get an 8-bit signed element, which specifies the offset of the target label from the caller site in 16-bit Thumb-1 instructions.
 * @param	index	An index for the lookup table with 8-bit signed offsets. Passed in register r0.
 * @param	lut	An address of the instruction following the BX instruction which transfers execution to this function. Passed in register lr.
 *       	   	The lookup table starts immediately after the BX instruction which transfers execution to this function.
 *       	   	In EABI this function is called with BL instruction, so the least significant bit of lr is always 0, and the address of the table is the same as return address.
 * @note	The function does not change any registers other than lr.
 * @note	This function never returns to its return address specified in lr. Instead, in returns to lr + lut[index] * 2.
 */
BEGIN_THUMB_FUNCTION __gnu_thumb1_case_sqi
	.arch armv4t
	PUSH  {r1}
	MOV   r1, lr
	LSRS  r1, r1, 1
	ADDS  r1, r1, r1
	LDRSB r1, [r1, r0]
	ADDS  r1, r1
	ADD   lr, r1
	POP   {r1}
	MOV   pc, lr
END_THUMB_FUNCTION __gnu_thumb1_case_sqi

/**
 * @brief	GCC helper function for switch statements optimized using a jump table with 8-bit unsigned values.
 * @details	GCC may generate a call to this function from Thumb-1 code when optimization is on.
 *         	The function makes a table lookup to get an 8-bit unsigned element, which specifies the offset of the target label from the caller site in 16-bit Thumb-1 instructions.
 * @param	index	An index for the lookup table with 8-bit unsigned offsets. Passed in register r0.
 * @param	lut	An address of the instruction following the BX instruction which transfers execution to this function. Passed in register lr.
 *       	   	The lookup table starts immediately after the BX instruction which transfers execution to this function.
 *       	   	In EABI this function is called with BL instruction, so the least significant bit of lr is always 0, and the address of the table is the same as return address.
 * @note	The function does not change any registers other than lr.
 * @note	This function never returns to its return address specified in lr. Instead, in returns to lr + lut[index] * 2.
 */
BEGIN_THUMB_FUNCTION __gnu_thumb1_case_uqi
	.arch armv4t
	PUSH {r1}
	MOV  r1, lr
	LSRS r1, r1, 1
	ADDS r1, r1, r1
	LDRB r1, [r1, r0]
	ADDS r1, r1
	ADD  lr, r1
	POP  {r1}
	MOV  pc, lr
END_THUMB_FUNCTION __gnu_thumb1_case_uqi

/**
 * @brief	GCC helper function for switch statements optimized using a jump table with 16-bit signed values.
 * @details	GCC may generate a call to this function from Thumb-1 code when optimization is on.
 *         	The function makes a table lookup to get an 16-bit signed element, which specifies the offset of the target label from the caller site in 16-bit Thumb-1 instructions.
 * @param	index	An index for the lookup table with 16-bit signed offsets. Passed in register r0.
 * @param	lut	An address of the instruction following the BX instruction which transfers execution to this function. Passed in register lr.
 *       	   	The lookup table starts immediately after the BX instruction which transfers execution to this function.
 *       	   	In EABI this function is called with BL instruction, so the least significant bit of lr is always 0, and the address of the table is the same as return address.
 * @note	The function does not change any registers other than lr.
 * @note	This function never returns to its return address specified in lr. Instead, in returns to lr + lut[index] * 2.
 */
BEGIN_THUMB_FUNCTION __gnu_thumb1_case_shi
	.arch armv4t
	PUSH  {r1}
	MOV   r1, lr
	LSRS  r1, r1, 1
	ADDS  r1, r1, r0
	LDRSH r1, [r1, r1]
	ADDS  r1, r1
	ADD   lr, r1
	POP   {r1}
	MOV   pc, lr
END_THUMB_FUNCTION __gnu_thumb1_case_shi

/**
 * @brief	GCC helper function for switch statements optimized using a jump table with 16-bit unsigned values.
 * @details	GCC may generate a call to this function from Thumb-1 code when optimization is on.
 *         	The function makes a table lookup to get an 16-bit unsigned element, which specifies the offset of the target label from the caller site in 16-bit Thumb-1 instructions.
 * @param	index	An index for the lookup table with 16-bit unsigned offsets. Passed in register r0.
 * @param	lut	An address of the instruction following the BX instruction which transfers execution to this function. Passed in register lr.
 *       	   	The lookup table starts immediately after the BX instruction which transfers execution to this function.
 *       	   	In EABI this function is called with BL instruction, so the least significant bit of lr is always 0, and the address of the table is the same as return address.
 * @note	The function does not change any registers other than lr.
 * @note	This function never returns to its return address specified in lr. Instead, in returns to lr + lut[index] * 2.
 */
BEGIN_THUMB_FUNCTION __gnu_thumb1_case_uhi
	.arch armv4t
	PUSH {r1}
	MOV  r1, lr
	LSRS r1, r1, 1
	ADDS r1, r1, r0
	LDRH r1, [r1, r1]
	ADDS r1, r1
	ADD  lr, r1
	POP  {r1}
	MOV  pc, lr
END_THUMB_FUNCTION __gnu_thumb1_case_uhi

/**
 * @brief	GCC helper function for switch statements optimized using a jump table with 32-bit signed values.
 * @details	GCC may generate a call to this function from Thumb-1 code when optimization is on.
 *         	The function makes a table lookup to get an 32-bit signed element, which specifies the offset of the target label from the caller site in 16-bit Thumb-1 instructions.
 * @param	index	An index for the lookup table with 32-bit signed offsets. Passed in register r0.
 * @param	lut	An address of the instruction following the BX instruction which transfers execution to this function. Passed in register lr.
 *       	   	The lookup table starts immediately after the BX instruction which transfers execution to this function.
 *       	   	In EABI this function is called with BL instruction, so the least significant bit of lr is always 0, and the address of the table is the same as return address.
 * @note	The function does not change any registers other than lr.
 * @note	This function never returns to its return address specified in lr. Instead, in returns to lr + lut[index] * 2.
 */
BEGIN_THUMB_FUNCTION __gnu_thumb1_case_si
	.arch armv4t
	PUSH {r1}
	MOV  r1, lr
	LSRS r1, 2 /* r1 = lr / 4; If lr mod 4 == 2 then C = 1 */
	ADCS r1, r1, r0 /* If lr mod 4 < 2 then r1 = lr / 4 + index. Otherwise r1 = (lr + 4) / 4 + index. */
	ADDS r1, r1, r1 /* r1 = ceil(lr / 4) * 2 + index * 2 */
	LDR  r1, [r1, r1]
	ADDS r1, r1
	ADD  lr, r1
	POP  {r1}
	MOV  pc, lr
END_THUMB_FUNCTION __gnu_thumb1_case_si
