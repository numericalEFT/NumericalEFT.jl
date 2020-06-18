/*
 *                          Yeppp! library header
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 *
 * Copyright (C) 2010-2012 Marat Dukhan
 * Copyright (C) 2012-2013 Georgia Institute of Technology
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Georgia Institute of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <yepPredefines.h>
#include <yepTypes.h>

/** @defgroup yepAtomic yepAtomic.h: atomic operations. */

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup yepAtomic
	 * @defgroup yepAtomic_Swap	Swap/exchange (atomic read-write)
	 */

	/**
	 * @ingroup yepAtomic_Swap
	 * @brief	Atomically reads the old value of the variable and replaces it with a new value. The memory state is not synchronized, and changes made by other cores to other variables are not guranteed to be visible to the local thread.
	 * @param	value	Pointer to a synchronization variable to be atomically swapped.
	 * @param	newValue	The new value to be written to the synchronization variable.
	 * @param	oldValue	Pointer to the variable to receive the old value of the synchronization variable.
	 * @retval	#YepStatusOk	The atomic swap operation successfully finished.
	 * @retval	#YepStatusNullPointer	Either @a value or @a oldValue pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a value or @a oldValue pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_Swap_Relaxed_S32uS32u_S32u(volatile Yep32u *value, Yep32u newValue, Yep32u *oldValue);

	/**
	 * @ingroup yepAtomic_Swap
	 * @brief	Atomically reads the old value of the variable and replaces it with a new value. The memory state is synchronized after the exchange operation.
	 * @param	value	Pointer to a synchronization variable to be atomically swapped.
	 * @param	newValue	The new value to be written to the synchronization variable.
	 * @param	oldValue	Pointer to the variable to receive the old value of the synchronization variable.
	 * @retval	#YepStatusOk	The atomic swap operation successfully finished.
	 * @retval	#YepStatusNullPointer	Either @a value or @a oldValue pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a value or @a oldValue pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_Swap_Acquire_S32uS32u_S32u(volatile Yep32u *value, Yep32u newValue, Yep32u *oldValue);

	/**
	 * @ingroup yepAtomic_Swap
	 * @brief	Atomically reads the old value of the variable and replaces it with a new value. The memory state is synchronized before the exchange operation.
	 * @param	value	Pointer to a synchronization variable to be atomically swapped.
	 * @param	newValue	The new value to be written to the synchronization variable.
	 * @param	oldValue	Pointer to the variable to receive the old value of the synchronization variable.
	 * @retval	#YepStatusOk	The atomic swap operation successfully finished.
	 * @retval	#YepStatusNullPointer	Either @a value or @a oldValue pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a value or @a oldValue pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_Swap_Release_S32uS32u_S32u(volatile Yep32u *value, Yep32u newValue, Yep32u *oldValue);

	/**
	 * @ingroup yepAtomic_Swap
	 * @brief	Atomically reads the old value of the variable and replaces it with a new value. The memory state is synchronized both before and after the exchange operation.
	 * @param	value	Pointer to a synchronization variable to be atomically swapped.
	 * @param	newValue	The new value to be written to the synchronization variable.
	 * @param	oldValue	Pointer to the variable to receive the old value of the synchronization variable.
	 * @retval	#YepStatusOk	The atomic swap operation successfully finished.
	 * @retval	#YepStatusNullPointer	Either @a value or @a oldValue pointer is null.
	 * @retval	#YepStatusMisalignedPointer	Either @a value or @a oldValue pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_Swap_Ordered_S32uS32u_S32u(volatile Yep32u *value, Yep32u newValue, Yep32u *oldValue);

	/**
	 * @ingroup yepAtomic
	 * @defgroup yepAtomic_CompareAndSwap	Compare-and-swap
	 */

	/**
	 * @ingroup yepAtomic_CompareAndSwap
	 * @brief	Atomically reads the value of the variable and, if it equals the expected value, replaces it with the old value. The memory state is not synchronized, and changes made by other cores to other variables are not guranteed to be visible to the local thread.
	 * @param	value	Pointer to a synchronization variable to be atomically compared and swapped.
	 * @param	newValue	The new value to be written to the @a value variable if comparison is successful.
	 * @param	oldValue	The expected value of the @a value variable.
	 * @retval	#YepStatusOk	The @a newValue was successfully written to the @a value variable.
	 * @retval	#YepStatusInvalidState	The operation failed because the @a value is different than the specified @a oldValue.
	 * @retval	#YepStatusNullPointer	The @a value pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a value pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_CompareAndSwap_Relaxed_S32uS32uS32u(volatile Yep32u *value, Yep32u newValue, Yep32u oldValue);

	/**
	 * @ingroup yepAtomic_CompareAndSwap
	 * @brief	Atomically reads the value of the variable and, if it equals the expected value, replaces it with the old value. The memory state is synchronized after the compare-and-swap operation.
	 * @param	value	Pointer to a synchronization variable to be atomically compared and swapped.
	 * @param	newValue	The new value to be written to the @a value variable if comparison is successful.
	 * @param	oldValue	The expected value of the @a value variable.
	 * @retval	#YepStatusOk	The @a newValue was successfully written to the @a value variable.
	 * @retval	#YepStatusInvalidState	The operation failed because the @a value is different than the specified @a oldValue.
	 * @retval	#YepStatusNullPointer	The @a value pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a value pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_CompareAndSwap_Acquire_S32uS32uS32u(volatile Yep32u *value, Yep32u newValue, Yep32u oldValue);

	/**
	 * @ingroup yepAtomic_CompareAndSwap
	 * @brief	Atomically reads the value of the variable and, if it equals the expected value, replaces it with the old value. The memory state is synchronized before the compare-and-swap operation.
	 * @param	value	Pointer to a synchronization variable to be atomically compared and swapped.
	 * @param	newValue	The new value to be written to the @a value variable if comparison is successful.
	 * @param	oldValue	The expected value of the @a value variable.
	 * @retval	#YepStatusOk	The @a newValue was successfully written to the @a value variable.
	 * @retval	#YepStatusInvalidState	The operation failed because the @a value is different than the specified @a oldValue.
	 * @retval	#YepStatusNullPointer	The @a value pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a value pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_CompareAndSwap_Release_S32uS32uS32u(volatile Yep32u *value, Yep32u newValue, Yep32u oldValue);

	/**
	 * @ingroup yepAtomic_CompareAndSwap
	 * @brief	Atomically reads the value of the variable and, if it equals the expected value, replaces it with the old value. The memory state is synchronized both before and after the compare-and-swap operation.
	 * @param	value	Pointer to a synchronization variable to be atomically compared and swapped.
	 * @param	newValue	The new value to be written to the @a value variable if comparison is successful.
	 * @param	oldValue	The expected value of the @a value variable.
	 * @retval	#YepStatusOk	The @a newValue was successfully written to the @a value variable.
	 * @retval	#YepStatusInvalidState	The operation failed because the @a value is different than the specified @a oldValue.
	 * @retval	#YepStatusNullPointer	The @a value pointer is null.
	 * @retval	#YepStatusMisalignedPointer	The @a value pointer is not naturally aligned.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepAtomic_CompareAndSwap_Ordered_S32uS32uS32u(volatile Yep32u *value, Yep32u newValue, Yep32u oldValue);

#ifdef __cplusplus
}
#endif
