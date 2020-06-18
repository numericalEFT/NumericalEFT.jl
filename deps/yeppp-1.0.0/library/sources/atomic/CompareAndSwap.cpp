/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <atomic/functions.h>
#include <yepAtomic.h>
#include <yepBuiltin.h>

YepStatus YEPABI yepAtomic_CompareAndSwap_Relaxed_S32uS32uS32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u oldValue) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		const long value = _InterlockedCompareExchange_nf((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_X86_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_IA64_CPU)
		const long value = _InterlockedCompareExchange_acq((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return static_cast<Yep32u>(value) == oldValue ? YepStatusOk : YepStatusInvalidState;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value;
		asm volatile (
			"LOCK CMPXCHGL %[newValue], %[valuePointer];"
			: "=a" (value)
			: "a" (oldValue), [newValue] "r" (newValue), [valuePointer] "m" (*valuePointer)
			: "cc", "memory"
		);
		return (value == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_MIPS_CPU)
		Yep32u tempValue, currentValue;
		asm volatile(
			".set mips32\n"
		"1:\n"
			/* Load current value */
			"LL %[currentValue], %[valuePointer];"
			/* Current value matches expected value? */
			"XOR %[tempValue], %[currentValue], %[oldValue];"
			/* If no, exit. */
			"BNEZ %[tempValue], 2f;"
			/* Set the value to be written */
			"MOVE %[tempValue], %[newValue];"
			/* Try to store the value. */
			"SC %[tempValue], %[valuePointer];"
			/* If the write failed, repeat the iteration. */
			"BEQZ %[tempValue], 1b;"
			/* Branch slot */
			"NOP;"
		"2:\n"
			: [currentValue] "=&r" (currentValue), [tempValue] "=&r" (tempValue)
			: [valuePointer] "m" (*valuePointer), [newValue] "r" (newValue), [oldValue] "r" (oldValue)
			: "memory"
		);
		return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS)
			Yep32u currentValue, status;
			asm volatile(
			"1:\n"
				/* Load current value */
				"LDREX %[currentValue], %[value];"
				/* Current value matches expected value? */
				"CMP %[currentValue], %[oldValue];"
				/* If no, exit */
				"BNE 2f;"
				/* Try to write the new value. */
				"STREX %[status], %[newValue], %[value];"
				/* Check if the write was successful. */
				"TST %[status], %[status];"
				/* If the write failed, repeat the iteration. */
				"BNE 1b;"
			"2:\n"
				: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
				: "cc", "memory"
			);
			return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
		#else
			if YEP_LIKELY(_isaFeatures & YepARMIsaFeatureV6) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv6\n"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else {
				const Yep32s __kuser_helper_version = *(reinterpret_cast<const Yep32s*>(0xFFFF0FFCu));
				if (__kuser_helper_version >= 2) {
					typedef Yep32u (__kuser_cmpxchg_t)(Yep32u oldValue, Yep32u newValue, volatile Yep32u *valuePointer);
					const Yep32u status = (reinterpret_cast<__kuser_cmpxchg_t*>(0xFFFF0FC0u))(oldValue, newValue, valuePointer);
					return (status == 0) ? YepStatusOk : YepStatusInvalidState;					
				} else {
					return YepStatusUnsupportedSoftware;
				}
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_CompareAndSwap_Acquire_S32uS32uS32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u oldValue) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		const long value = _InterlockedCompareExchange_acq((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_X86_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_IA64_CPU)
		const long value = _InterlockedCompareExchange_acq((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return static_cast<Yep32u>(value) == oldValue ? YepStatusOk : YepStatusInvalidState;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value;
		asm volatile (
			"LOCK CMPXCHGL %[newValue], %[valuePointer];"
			: "=a" (value)
			: "a" (oldValue), [newValue] "r" (newValue), [valuePointer] "m" (*valuePointer)
			: "cc", "memory"
		);
		return (value == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_MIPS_CPU)
		Yep32u tempValue, currentValue;
		asm volatile(
			".set mips32\n"
		"1:\n"
			/* Load current value */
			"LL %[currentValue], %[valuePointer];"
			/* Current value matches expected value? */
			"XOR %[tempValue], %[currentValue], %[oldValue];"
			/* If no, exit. */
			"BNEZ %[tempValue], 2f;"
			/* Set the value to be written */
			"MOVE %[tempValue], %[newValue];"
			/* Try to store the value. */
			"SC %[tempValue], %[valuePointer];"
			/* If the write failed, repeat the iteration. */
			"BEQZ %[tempValue], 1b;"
			/* Branch slot */
			"NOP;"
		"2:\n"
			"SYNC;"
			: [currentValue] "=&r" (currentValue), [tempValue] "=&r" (tempValue)
			: [valuePointer] "m" (*valuePointer), [newValue] "r" (newValue), [oldValue] "r" (oldValue)
			: "memory"
		);
		return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u currentValue, status;
			asm volatile(
			"1:\n"
				/* Load current value */
				"LDREX %[currentValue], %[value];"
				/* Current value matches expected value? */
				"CMP %[currentValue], %[oldValue];"
				/* If no, exit */
				"BNE 2f;"
				/* Try to write the new value. */
				"STREX %[status], %[newValue], %[value];"
				/* Check if the write was successful. */
				"TST %[status], %[status];"
				/* If the write failed, repeat the iteration. */
				"BNE 1b;"
			"2:\n"
				"DMB ish;"
				: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
				: "cc", "memory"
			);
			return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv6\n"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv7-a\n"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					"DMB ish;"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else {
				const Yep32s __kuser_helper_version = *(reinterpret_cast<const Yep32s*>(0xFFFF0FFCu));
				if (__kuser_helper_version >= 2) {
					typedef Yep32u (__kuser_cmpxchg_t)(Yep32u oldValue, Yep32u newValue, volatile Yep32u *valuePointer);
					const Yep32u status = (reinterpret_cast<__kuser_cmpxchg_t*>(0xFFFF0FC0u))(oldValue, newValue, valuePointer);
					return (status == 0) ? YepStatusOk : YepStatusInvalidState;					
				} else {
					return YepStatusUnsupportedSoftware;
				}
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_CompareAndSwap_Release_S32uS32uS32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u oldValue) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		const long value = _InterlockedCompareExchange_rel((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_X86_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_IA64_CPU)
		const long value = _InterlockedCompareExchange_rel((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return static_cast<Yep32u>(value) == oldValue ? YepStatusOk : YepStatusInvalidState;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value;
		asm volatile (
			"LOCK CMPXCHGL %[newValue], %[valuePointer];"
			: "=a" (value)
			: "a" (oldValue), [newValue] "r" (newValue), [valuePointer] "m" (*valuePointer)
			: "cc", "memory"
		);
		return (value == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_MIPS_CPU)
		Yep32u tempValue, currentValue;
		asm volatile(
			".set mips32\n"
			"SYNC;"
		"1:\n"
			/* Load current value */
			"LL %[currentValue], %[valuePointer];"
			/* Current value matches expected value? */
			"XOR %[tempValue], %[currentValue], %[oldValue];"
			/* If no, exit. */
			"BNEZ %[tempValue], 2f;"
			/* Set the value to be written */
			"MOVE %[tempValue], %[newValue];"
			/* Try to store the value. */
			"SC %[tempValue], %[valuePointer];"
			/* If the write failed, repeat the iteration. */
			"BEQZ %[tempValue], 1b;"
			/* Branch slot */
			"NOP;"
		"2:\n"
			: [currentValue] "=&r" (currentValue), [tempValue] "=&r" (tempValue)
			: [valuePointer] "m" (*valuePointer), [newValue] "r" (newValue), [oldValue] "r" (oldValue)
			: "memory"
		);
		return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u currentValue, status;
			asm volatile(
				"DMB ish;"
			"1:\n"
				/* Load current value */
				"LDREX %[currentValue], %[value];"
				/* Current value matches expected value? */
				"CMP %[currentValue], %[oldValue];"
				/* If no, exit */
				"BNE 2f;"
				/* Try to write the new value. */
				"STREX %[status], %[newValue], %[value];"
				/* Check if the write was successful. */
				"TST %[status], %[status];"
				/* If the write failed, repeat the iteration. */
				"BNE 1b;"
			"2:\n"
				: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
				: "cc", "memory"
			);
			return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv6\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv7-a\n"
					"DMB ish;"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else {
				const Yep32s __kuser_helper_version = *(reinterpret_cast<const Yep32s*>(0xFFFF0FFCu));
				if (__kuser_helper_version >= 2) {
					typedef Yep32u (__kuser_cmpxchg_t)(Yep32u oldValue, Yep32u newValue, volatile Yep32u *valuePointer);
					const Yep32u status = (reinterpret_cast<__kuser_cmpxchg_t*>(0xFFFF0FC0u))(oldValue, newValue, valuePointer);
					return (status == 0) ? YepStatusOk : YepStatusInvalidState;					
				} else {
					return YepStatusUnsupportedSoftware;
				}
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_CompareAndSwap_Ordered_S32uS32uS32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u oldValue) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_X86_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#elif defined(YEP_IA64_CPU)
		const long value = _InterlockedCompareExchange((long*)(valuePointer), static_cast<long>(newValue), static_cast<long>(oldValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return static_cast<Yep32u>(value) == oldValue ? YepStatusOk : YepStatusInvalidState;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value;
		asm volatile (
			"LOCK CMPXCHGL %[newValue], %[valuePointer];"
			: "=a" (value)
			: "a" (oldValue), [newValue] "r" (newValue), [valuePointer] "m" (*valuePointer)
			: "cc", "memory"
		);
		return (value == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_MIPS_CPU)
		Yep32u tempValue, currentValue;
		asm volatile(
			".set mips32\n"
			"SYNC;"
		"1:\n"
			/* Load current value */
			"LL %[currentValue], %[valuePointer];"
			/* Current value matches expected value? */
			"XOR %[tempValue], %[currentValue], %[oldValue];"
			/* If no, exit. */
			"BNEZ %[tempValue], 2f;"
			/* Set the value to be written */
			"MOVE %[tempValue], %[newValue];"
			/* Try to store the value. */
			"SC %[tempValue], %[valuePointer];"
			/* If the write failed, repeat the iteration. */
			"BEQZ %[tempValue], 1b;"
			/* Branch slot */
			"NOP;"
		"2:\n"
			"SYNC;"
			: [currentValue] "=&r" (currentValue), [tempValue] "=&r" (tempValue)
			: [valuePointer] "m" (*valuePointer), [newValue] "r" (newValue), [oldValue] "r" (oldValue)
			: "memory"
		);
		return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u currentValue, status;
			asm volatile(
				"DMB ish;"
			"1:\n"
				/* Load current value */
				"LDREX %[currentValue], %[value];"
				/* Current value matches expected value? */
				"CMP %[currentValue], %[oldValue];"
				/* If no, exit */
				"BNE 2f;"
				/* Try to write the new value. */
				"STREX %[status], %[newValue], %[value];"
				/* Check if the write was successful. */
				"TST %[status], %[status];"
				/* If the write failed, repeat the iteration. */
				"BNE 1b;"
			"2:\n"
				"DMB ish;"
				: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
				: "cc", "memory"
			);
			return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv6\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u currentValue, status;
				asm volatile(
					".arch armv7-a\n"
					"DMB ish;"
				"1:\n"
					/* Load current value */
					"LDREX %[currentValue], %[value];"
					/* Current value matches expected value? */
					"CMP %[currentValue], %[oldValue];"
					/* If no, exit */
					"BNE 2f;"
					/* Try to write the new value. */
					"STREX %[status], %[newValue], %[value];"
					/* Check if the write was successful. */
					"TST %[status], %[status];"
					/* If the write failed, repeat the iteration. */
					"BNE 1b;"
				"2:\n"
					"DMB ish;"
					: [currentValue] "=&r" (currentValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [oldValue] "r" (oldValue), [newValue] "r" (newValue)
					: "cc", "memory"
				);
				return (currentValue == oldValue) ? YepStatusOk : YepStatusInvalidState;
			} else {
				const Yep32s __kuser_helper_version = *(reinterpret_cast<const Yep32s*>(0xFFFF0FFCu));
				if (__kuser_helper_version >= 2) {
					typedef Yep32u (__kuser_cmpxchg_t)(Yep32u oldValue, Yep32u newValue, volatile Yep32u *valuePointer);
					const Yep32u status = (reinterpret_cast<__kuser_cmpxchg_t*>(0xFFFF0FC0u))(oldValue, newValue, valuePointer);
					return (status == 0) ? YepStatusOk : YepStatusInvalidState;					
				} else {
					return YepStatusUnsupportedSoftware;
				}
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

