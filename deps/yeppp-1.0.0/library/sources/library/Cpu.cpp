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

Yep64u _isaFeatures = YepIsaFeaturesDefault;
Yep64u _simdFeatures = YepSimdFeaturesDefault;
Yep64u _systemFeatures = YepSystemFeaturesDefault;
const YepCpuMicroarchitecture* _dispatchList = YEP_NULL_POINTER;
Yep32u _logicalCoresCount = 0;
YepCpuVendor _vendor = YepCpuVendorUnknown;
YepCpuMicroarchitecture _microarchitecture = YepCpuMicroarchitectureUnknown;
CacheHierarchyInfo _cache;

ConstantString _briefCpuName;
ConstantString _fullCpuName;


YepStatus YEPABI yepLibrary_GetCpuVendor(YepCpuVendor *vendorPointer) {
	if YEP_UNLIKELY(vendorPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*vendorPointer = _vendor;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuArchitecture(YepCpuArchitecture *architecturePointer) {
	if YEP_UNLIKELY(architecturePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	#if defined(YEP_X86_CPU)
		*architecturePointer = YepCpuArchitectureX86;
	#elif defined(YEP_ARM_CPU)
		*architecturePointer = YepCpuArchitectureARM;
	#elif defined(YEP_MIPS_CPU)
		*architecturePointer = YepCpuArchitectureMIPS;
	#elif defined(YEP_POWERPC_CPU)
		*architecturePointer = YepCpuArchitecturePowerPC;
	#elif defined(YEP_IA64_CPU)
		*architecturePointer = YepCpuArchitectureIA64;
	#elif defined(YEP_SPARC_CPU)
		*architecturePointer = YepCpuArchitectureSPARC;
	#else
		#error "Unsupported processor architecture"
	#endif
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuMicroarchitecture(YepCpuMicroarchitecture *microarchitecturePointer) {
	if YEP_UNLIKELY(microarchitecturePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*microarchitecturePointer = _microarchitecture;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetLogicalCoresCount(Yep32u *logicalCoresCount) {
	if YEP_UNLIKELY(logicalCoresCount == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*logicalCoresCount = _logicalCoresCount;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuIsaFeatures(Yep64u *isaFeaturesPointer) {
	if YEP_UNLIKELY(isaFeaturesPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*isaFeaturesPointer = _isaFeatures;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuSimdFeatures(Yep64u *simdFeaturesPointer) {
	if YEP_UNLIKELY(simdFeaturesPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*simdFeaturesPointer = _simdFeatures;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuSystemFeatures(Yep64u *systemFeaturesPointer) {
	if YEP_UNLIKELY(systemFeaturesPointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	*systemFeaturesPointer = _systemFeatures;
	return YepStatusOk;
}

YepStatus YEPABI yepLibrary_GetCpuDataCacheSize(Yep32u level, Yep32u *cacheSizePointer) {
	if YEP_UNLIKELY(cacheSizePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(cacheSizePointer, sizeof(Yep32u))) {
		return YepStatusMisalignedPointer;
	}
	switch (level) {
		case 0:
			*cacheSizePointer = _cache.L0DCacheInfo.cacheSize;
			return YepStatusOk;
		case 1:
			*cacheSizePointer = _cache.L1DCacheInfo.cacheSize;
			return YepStatusOk;
		case 2:
			*cacheSizePointer = _cache.L2CacheInfo.cacheSize;
			return YepStatusOk;
		case 3:
			*cacheSizePointer = _cache.L3CacheInfo.cacheSize;
			return YepStatusOk;
		default:
			*cacheSizePointer = 0;
			return YepStatusOk;
	}
}

YepStatus YEPABI yepLibrary_GetCpuInstructionCacheSize(Yep32u level, Yep32u *cacheSizePointer) {
	if YEP_UNLIKELY(cacheSizePointer == YEP_NULL_POINTER) {
		return YepStatusNullPointer;
	}
	if YEP_UNLIKELY(yepBuiltin_GetPointerMisalignment(cacheSizePointer, sizeof(Yep32u))) {
		return YepStatusMisalignedPointer;
	}
	switch (level) {
		case 0:
			*cacheSizePointer = _cache.L0ICacheInfo.cacheSize;
			return YepStatusOk;
		case 1:
			*cacheSizePointer = _cache.L1ICacheInfo.cacheSize;
			return YepStatusOk;
		case 2:
			*cacheSizePointer = _cache.L2CacheInfo.cacheSize;
			return YepStatusOk;
		case 3:
			*cacheSizePointer = _cache.L3CacheInfo.cacheSize;
			return YepStatusOk;
		default:
			*cacheSizePointer = 0;
			return YepStatusOk;
	}
}
