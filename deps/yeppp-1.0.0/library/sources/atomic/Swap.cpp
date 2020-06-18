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

YepStatus YEPABI yepAtomic_Swap_Relaxed_S32uS32u_S32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u *oldValuePointer) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(oldValuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(oldValuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		*oldValuePointer = _InterlockedExchange_nf((volatile long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_X86_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_IA64_CPU)
		*oldValuePointer = _InterlockedExchange_acq((long*)(valuePointer), static_cast<long>(newValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return YepStatusOk;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value = newValue;
		asm volatile (
			"XCHGL %[value], %[valuePointer];"
			: [value] "+r" (value)
			: [valuePointer] "m" (*valuePointer)
			:
		);
		*oldValuePointer = value;
		return YepStatusOk;
	#elif defined(YEP_MIPS_CPU)
		Yep32u status, oldValue;
		asm volatile(
			".set mips32\n"
		"1:\n"
			"LL %[oldValue], %[value];"
			"MOVE %[status], %[newValue];"
			"SC %[status], %[value];"
			"BEQZ %[status], 1b;"
			"NOP;"
			: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
			: [value] "m" (*valuePointer), [newValue] "r" (newValue)
			: "memory"
		);
		*oldValuePointer = oldValue;
		return YepStatusOk;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS)
			Yep32u status, oldValue;
			asm volatile(
			"1:\n"
				"LDREX %[oldValue], %[value];"
				"STREX %[status], %[newValue], %[value];"
				"TST %[status], %[status];"
				"BNE 1b;"
				: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [newValue] "r" (newValue)
				: "memory"
			);
			*oldValuePointer = oldValue;
			return YepStatusOk;
		#else
			if YEP_LIKELY(_isaFeatures & YepARMIsaFeatureV6) {
				Yep32u status, oldValue;
				asm volatile(
					".arch armv6\n"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else {
				Yep32u oldValue;
				asm volatile(
					".arch armv5te\n"
					"SWP %[oldValue], %[newValue], [%[valuePointer]];"
					: [oldValue] "=&r" (oldValue)
					: [valuePointer] "r" (valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_Swap_Acquire_S32uS32u_S32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u *oldValuePointer) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(oldValuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(oldValuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		*oldValuePointer = _InterlockedExchange_acq((volatile long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_X86_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_IA64_CPU)
		*oldValuePointer = _InterlockedExchange_acq((long*)(valuePointer), static_cast<long>(newValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return YepStatusOk;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value = newValue;
		asm volatile (
			"XCHGL %[value], %[valuePointer];"
			: [value] "+r" (value)
			: [valuePointer] "m" (*valuePointer)
			:
		);
		*oldValuePointer = value;
		return YepStatusOk;
	#elif defined(YEP_MIPS_CPU)
		Yep32u status, oldValue;
		asm volatile(
			".set mips32\n"
		"1:\n"
			"LL %[oldValue], %[value];"
			"MOVE %[status], %[newValue];"
			"SC %[status], %[value];"
			"BEQZ %[status], 1b;"
			"NOP;"
			"SYNC;"
			: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
			: [value] "m" (*valuePointer), [newValue] "r" (newValue)
			: "memory"
		);
		*oldValuePointer = oldValue;
		return YepStatusOk;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u status, oldValue;
			asm volatile(
			"1:\n"
				"LDREX %[oldValue], %[value];"
				"STREX %[status], %[newValue], %[value];"
				"TST %[status], %[status];"
				"BNE 1b;"
				"DMB ish;"
				: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [newValue] "r" (newValue)
				: "memory"
			);
			*oldValuePointer = oldValue;
			return YepStatusOk;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u status = 0, oldValue;
				asm volatile(
					".arch armv6\n"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					"MCR p15, 0, %[status], c7, c10, 5;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u status, oldValue;
				asm volatile(
					".arch armv7-a\n"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					"DMB ish;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else {
				Yep32u oldValue;
				asm volatile(
					".arch armv5te\n"
					"SWP %[oldValue], %[newValue], [%[valuePointer]];"
					: [oldValue] "=&r" (oldValue)
					: [valuePointer] "r" (valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_Swap_Release_S32uS32u_S32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u *oldValuePointer) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(oldValuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(oldValuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		__dmb(_ARM_BARRIER_ISH);
		_ReadWriteBarrier();
		*oldValuePointer = _InterlockedExchange_nf((volatile long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_X86_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_IA64_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return YepStatusOk;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value = newValue;
		asm volatile (
			"XCHGL %[value], %[valuePointer];"
			: [value] "+r" (value)
			: [valuePointer] "m" (*valuePointer)
			:
		);
		*oldValuePointer = value;
		return YepStatusOk;
	#elif defined(YEP_MIPS_CPU)
		Yep32u status, oldValue;
		asm volatile(
			".set mips32\n"
			"SYNC;"
		"1:\n"
			"LL %[oldValue], %[value];"
			"MOVE %[status], %[newValue];"
			"SC %[status], %[value];"
			"BEQZ %[status], 1b;"
			"NOP;"
			: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
			: [value] "m" (*valuePointer), [newValue] "r" (newValue)
			: "memory"
		);
		*oldValuePointer = oldValue;
		return YepStatusOk;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u status, oldValue;
			asm volatile(
				"DMB ish;"
			"1:\n"
				"LDREX %[oldValue], %[value];"
				"STREX %[status], %[newValue], %[value];"
				"TST %[status], %[status];"
				"BNE 1b;"
				: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [newValue] "r" (newValue)
				: "memory"
			);
			*oldValuePointer = oldValue;
			return YepStatusOk;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u status = 0, oldValue;
				asm volatile(
					".arch armv6\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u status, oldValue;
				asm volatile(
					".arch armv7-a\n"
					"DMB ish;"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else {
				Yep32u oldValue;
				asm volatile(
					".arch armv5te\n"
					"SWP %[oldValue], %[newValue], [%[valuePointer]];"
					: [oldValue] "=&r" (oldValue)
					: [valuePointer] "r" (valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}

YepStatus YEPABI yepAtomic_Swap_Ordered_S32uS32u_S32u(volatile Yep32u *valuePointer, Yep32u newValue, Yep32u *oldValuePointer) {
	if YEP_UNLIKELY(valuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment((void*)valuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
	if YEP_UNLIKELY(oldValuePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(oldValuePointer, sizeof(Yep32u)) != 0) {
		return YepStatusMisalignedPointer;
	}
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#if defined(YEP_ARM_CPU)
		*oldValuePointer = _InterlockedExchange((volatile long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_X86_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#elif defined(YEP_IA64_CPU)
		*oldValuePointer = _InterlockedExchange((long*)(valuePointer), static_cast<long>(newValue));
	#else
		#error "Architecture-specific implementation needed"
	#endif
	return YepStatusOk;
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(YEP_X86_CPU)
		Yep32u value = newValue;
		asm volatile (
			"XCHGL %[value], %[valuePointer];"
			: [value] "+r" (value)
			: [valuePointer] "m" (*valuePointer)
			:
		);
		*oldValuePointer = value;
		return YepStatusOk;
	#elif defined(YEP_MIPS_CPU)
		Yep32u status, oldValue;
		asm volatile(
			".set mips32\n"
			"SYNC;"
		"1:\n"
			"LL %[oldValue], %[value];"
			"MOVE %[status], %[newValue];"
			"SC %[status], %[value];"
			"BEQZ %[status], 1b;"
			"NOP;"
			"SYNC;"
			: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
			: [value] "m" (*valuePointer), [newValue] "r" (newValue)
			: "memory"
		);
		*oldValuePointer = oldValue;
		return YepStatusOk;
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			Yep32u status, oldValue;
			asm volatile(
				"DMB ish;"
			"1:\n"
				"LDREX %[oldValue], %[value];"
				"STREX %[status], %[newValue], %[value];"
				"TST %[status], %[status];"
				"BNE 1b;"
				"DMB ish;"
				: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
				: [value] "m" (*valuePointer), [newValue] "r" (newValue)
				: "memory"
			);
			*oldValuePointer = oldValue;
			return YepStatusOk;
		#else
			const Yep64u isaMask = YepARMIsaFeatureV6 | YepARMIsaFeatureV7;
			if YEP_LIKELY((_isaFeatures & isaMask) == YepARMIsaFeatureV6) {
				Yep32u status = 0, oldValue;
				asm volatile(
					".arch armv6\n"
					"MCR p15, 0, %[status], c7, c10, 5;"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					"MCR p15, 0, %[status], c7, c10, 5;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else if YEP_LIKELY((_isaFeatures & isaMask) == (YepARMIsaFeatureV6 | YepARMIsaFeatureV7)) {
				Yep32u status, oldValue;
				asm volatile(
					".arch armv7-a\n"
					"DMB ish;"
				"1:\n"
					"LDREX %[oldValue], %[value];"
					"STREX %[status], %[newValue], %[value];"
					"TST %[status], %[status];"
					"BNE 1b;"
					"DMB ish;"
					: [oldValue] "=&r" (oldValue), [status] "=&r" (status)
					: [value] "m" (*valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			} else {
				Yep32u oldValue;
				asm volatile(
					".arch armv5te\n"
					"SWP %[oldValue], %[newValue], [%[valuePointer]];"
					: [oldValue] "=&r" (oldValue)
					: [valuePointer] "r" (valuePointer), [newValue] "r" (newValue)
					: "memory"
				);
				*oldValuePointer = oldValue;
				return YepStatusOk;
			}
		#endif
	#else
		#error "Architecture-specific implementation needed"
	#endif
#else
	#error "Compiler-specific implementation needed"
#endif
}
