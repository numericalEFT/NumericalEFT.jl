/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <yepBuiltin.h>

#if defined(YEP_MACOSX_OS)
	#include <sys/types.h>
	#include <sys/sysctl.h>
#else
	#error "The functions in this file should only be used in and compiled for Mac OS X"
#endif

YepStatus _yepLibrary_InitMacOSXLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures) {
	int names[2] = { CTL_HW, HW_AVAILCPU };
	uint32_t cpuCount;
	size_t cpuCountSize = sizeof(cpuCount);
	YepStatus status = YepStatusOk;

	if YEP_LIKELY(sysctl(names, YEP_COUNT_OF(names), &cpuCount, &cpuCountSize, NULL, 0) == 0) {
		if YEP_UNLIKELY(cpuCount == 1) {
			// Set this flag only if we know for sure.
			systemFeatures |= YepSystemFeatureSingleThreaded;
		} else if YEP_UNLIKELY(cpuCount == 0) {
			status = YepStatusSystemError;
			cpuCount = 1;
		}
		logicalCoresCount = cpuCount;
	} else {
		logicalCoresCount = 1;
		status = YepStatusSystemError;
	}
	return status;
}
