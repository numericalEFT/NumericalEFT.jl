/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepBuiltin.h>
#include <yepLibrary.h>
#include <library/functions.h>
#if defined(YEP_WINDOWS_OS)
	#include <windows.h>
#elif defined(YEP_LINUX_OS)
	#include <yepSyscalls.hpp>
	#include <time.h>
#elif defined(YEP_MACOSX_OS)
	#include <stdint.h>
	#include <mach/mach.h>
	#include <mach/mach_time.h>
	#include <unistd.h>
#endif

YepStatus YEPABI yepLibrary_GetTimerTicks(Yep64u* ticksPointer) {
	if YEP_UNLIKELY(ticksPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_WINDOWS_OS)
	LARGE_INTEGER ticks;
	BOOL qpcResult = ::QueryPerformanceCounter(&ticks);
	if YEP_UNLIKELY(qpcResult == 0) {
		return YepStatusSystemError;
	} else {
		*ticksPointer = ticks.QuadPart;
		return YepStatusOk;
	}
#elif defined(YEP_LINUX_OS)
	struct timespec ticks;
	int syscall_result = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &ticks);
	if YEP_UNLIKELY(syscall_result != 0) {
		return YepStatusSystemError;
	} else {
		*ticksPointer = 1000000000ull * Yep64u(ticks.tv_sec) + Yep64u(ticks.tv_nsec);
		return YepStatusOk;
	}
#elif defined(YEP_MACOSX_OS)
	const uint64_t ticks = mach_absolute_time();
	mach_timebase_info_data_t timebaseInfo;
	if YEP_UNLIKELY(mach_timebase_info(&timebaseInfo) != KERN_SUCCESS) {
		return YepStatusSystemError;
	} else {
		*ticksPointer = ticks * timebaseInfo.numer;
		return YepStatusOk;
	}
#else
	return YepStatusUnsupportedSoftware;
#endif
}

YepStatus YEPABI yepLibrary_GetTimerFrequency(Yep64u* frequencyPointer) {
	if YEP_UNLIKELY(frequencyPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_WINDOWS_OS)
	LARGE_INTEGER frequency;
	BOOL qpfResult = ::QueryPerformanceFrequency(&frequency);
	if YEP_UNLIKELY(qpfResult == 0) {
		return YepStatusSystemError;
	} else {
		*frequencyPointer = frequency.QuadPart;
		return YepStatusOk;
	}
#elif defined(YEP_LINUX_OS)
	*frequencyPointer = 1000000000ull;
	return YepStatusOk;
#elif defined(YEP_MACOSX_OS)
	mach_timebase_info_data_t timebaseInfo;
	if YEP_UNLIKELY(mach_timebase_info(&timebaseInfo) != KERN_SUCCESS) {
		return YepStatusSystemError;
	} else {
		*frequencyPointer = timebaseInfo.denom * 1000000000ull;
		return YepStatusOk;
	}
#else
	return YepStatusUnsupportedSoftware;
#endif
}

YepStatus YEPABI yepLibrary_GetTimerAccuracy(Yep64u* accuracyPointer) {
	const YepSize defaultIterations = 128;
	const YepSize maxIterations = 1024;
	
	if YEP_UNLIKELY(accuracyPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
#if defined(YEP_WINDOWS_OS)
	LARGE_INTEGER frequency;
	BOOL qpfResult = ::QueryPerformanceFrequency(&frequency);
	if YEP_UNLIKELY(qpfResult == 0) {
		return YepStatusSystemError;
	} else {
		*accuracyPointer = (1000000000ull + (frequency.QuadPart + 1) / 2) / frequency.QuadPart;
		return YepStatusOk;
	}
#elif defined(YEP_LINUX_OS)
	struct timespec startTime;
	int syscallResult = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &startTime);
	if YEP_UNLIKELY(syscallResult != 0) {
		return YepStatusSystemError;
	}
	Yep64u start = 1000000000ull * Yep64u(startTime.tv_sec) + Yep64u(startTime.tv_nsec);
	Yep64u bestAccuracy = Yep64u(-1);
	for (YepSize iteration = 0; iteration < defaultIterations; iteration++) {
		struct timespec endTime;
		syscallResult = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &endTime);
		if YEP_UNLIKELY(syscallResult != 0){ 
			return YepStatusSystemError;
		}
		const Yep64u end = 1000000000ull * Yep64u(endTime.tv_sec) + Yep64u(endTime.tv_nsec);
		if YEP_LIKELY(end != start) {
			const Yep64u accuracy = end - start;
			bestAccuracy = yepBuiltin_Min_64u64u_64u(bestAccuracy, accuracy);
			start = end;
		}
	}
	if YEP_UNLIKELY(bestAccuracy == Yep32u(-1)) {
		struct timespec endTime;
		Yep64u end;
		for (YepSize iteration = defaultIterations; iteration < maxIterations; iteration++) {
			syscallResult = yepSyscall_clock_gettime(CLOCK_MONOTONIC, &endTime);
			if YEP_UNLIKELY(syscallResult != 0) {
				return YepStatusSystemError;
			}
			end = 1000000000ull * Yep64u(endTime.tv_sec) + Yep64u(endTime.tv_nsec);
			if (end != start) {
				bestAccuracy = end - start;
				*accuracyPointer = bestAccuracy;
				return YepStatusOk;
			}
		}
		// Some problem with OS timer.
		return YepStatusUnsupportedHardware;
	} else {
		*accuracyPointer = bestAccuracy;
		return YepStatusOk;
	}
#elif defined(YEP_MACOSX_OS)
	mach_timebase_info_data_t timebaseInfo;
	if YEP_UNLIKELY(mach_timebase_info(&timebaseInfo) != KERN_SUCCESS) {
		return YepStatusSystemError;
	} else {
		*accuracyPointer = (timebaseInfo.numer + (timebaseInfo.denom + 1) / 2) / timebaseInfo.denom;
		return YepStatusOk;
	}
#else
	return YepStatusUnsupportedSoftware;
#endif
}
