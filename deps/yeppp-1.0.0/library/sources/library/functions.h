/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#pragma once

struct CacheLevelInfo {
	YEP_INLINE CacheLevelInfo() :
		cacheSize(0),
		lineSize(0),
		associativity(0),
		isUnified(false)
	{
	}

	Yep32u cacheSize;
	Yep16u lineSize;
	Yep8u associativity;
	Yep8u isUnified;
};

struct TraceCacheInfo {
	YEP_INLINE TraceCacheInfo() :
		microops(0),
		associativity(0)
	{
	}

	Yep32u microops;
	Yep32u associativity;
};

struct CacheHierarchyInfo {
	YEP_INLINE CacheHierarchyInfo() :
		prefetchLineSize(0),
		clflushLineSize(0)
	{
	}

	CacheLevelInfo L0ICacheInfo;
	CacheLevelInfo L0DCacheInfo;
	CacheLevelInfo L1ICacheInfo;
	CacheLevelInfo L1DCacheInfo;
	CacheLevelInfo L2CacheInfo;
	CacheLevelInfo L3CacheInfo;
	TraceCacheInfo traceCacheInfo;
	Yep16u prefetchLineSize;
	Yep16u clflushLineSize;
};

struct ConstantString {
	YEP_INLINE ConstantString(const char* pointer, YepSize length) :
		pointer(pointer),
		length(length)
	{
	}

	YEP_INLINE ConstantString() :
		pointer(YEP_NULL_POINTER),
		length(0)
	{
	}

	YEP_INLINE YepBoolean isEmpty() const {
		const YepBoolean isNullPointer = (pointer == YEP_NULL_POINTER);
		const YepBoolean isZeroLength = (length == 0);
		return isNullPointer && isZeroLength;
	}

	const char* pointer;
	YepSize length;
};

#define YEP_MAKE_CONSTANT_STRING(text) \
	(ConstantString(text, YEP_COUNT_OF(text) - 1))

YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_InitCpuInfo();
YEP_PRIVATE_SYMBOL FunctionPointer _yepLibrary_InitFunction(const FunctionDescriptor<YepStatus (*)()>* dispatchTable);
YEP_PRIVATE_SYMBOL const YepCpuMicroarchitecture *_yepLibrary_GetMicroarchitectureDispatchList(YepCpuMicroarchitecture microarchitecture);
extern YEP_PRIVATE_SYMBOL Yep64u _isaFeatures;
extern YEP_PRIVATE_SYMBOL Yep64u _simdFeatures;
extern YEP_PRIVATE_SYMBOL Yep64u _systemFeatures;
extern YEP_PRIVATE_SYMBOL YepCpuVendor _vendor;
extern YEP_PRIVATE_SYMBOL YepCpuMicroarchitecture _microarchitecture;
extern YEP_PRIVATE_SYMBOL Yep32u _logicalCoresCount;
extern YEP_PRIVATE_SYMBOL CacheHierarchyInfo _cache;
#if defined(YEP_X86_CPU)
	struct ModelInfo {
		ModelInfo() :
			baseModel(0xFFu),
			baseFamily(0xFFu),
			stepping(0xFFu),
			extModel(0xFFu),
			extFamily(0xFFu),
			processorType(0xFFu)
		{
		}

		Yep16u model;
		Yep16u family;

		Yep8u baseModel;
		Yep8u baseFamily;
		Yep8u stepping;
		Yep8u extModel;
		Yep8u extFamily;
		Yep8u processorType;
	};

	extern YEP_PRIVATE_SYMBOL ModelInfo _modelInfo;
#endif
extern YEP_PRIVATE_SYMBOL const YepCpuMicroarchitecture* _dispatchList;

extern YEP_PRIVATE_SYMBOL ConstantString _briefCpuName;
extern YEP_PRIVATE_SYMBOL ConstantString _fullCpuName;

YEP_PRIVATE_SYMBOL ConstantString _yepLibrary_GetCpuVendorDescription(YepCpuVendor vendor);
YEP_PRIVATE_SYMBOL ConstantString _yepLibrary_GetCpuMicroarchitectureDescription(YepCpuMicroarchitecture microarchitecture);

typedef void (*LineParser)(const char* lineStart, const char* lineEnd, void* state);

#if defined(YEP_LINUX_OS)
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_ParseProcCpuInfo(LineParser lineParser, void* state);
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_ParseKernelLog(LineParser lineParser, void* state);

	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_InitLinuxLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures);
#elif defined(YEP_MACOSX_OS)
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_InitMacOSXLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures);
#elif defined(YEP_WINDOWS_OS)
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_InitWindowsLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures);
#endif

#if defined(YEP_ARM_CPU) && defined(YEP_LINUX_OS)
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeV6K();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeV7MP();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeDiv();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeXScale();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeCnt32();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeVFP3();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeVFPd32();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeVFP3HP();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeVFP4();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeNeonHp();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeNeon2();
	extern "C" YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadFPSID();
	extern "C" YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadMVFR0();
	extern "C" YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadWCID();
	extern "C" YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadPMCCNTR();
	extern "C" YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadPMCR();

	YEP_PRIVATE_SYMBOL Yep64u _yepLibrary_ReadCoprocessor(Yep64u (*ReadFunction)());
	YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeInstruction(Yep32u (*ProbeFunction)());
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_DetectLinuxPerfEventSupport(Yep64u &systemFeatures);
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_DetectLinuxARMCycleCounterAccess(Yep64u &systemFeatures);
#endif
#if defined(YEP_MIPS_CPU) && defined(YEP_LINUX_OS)
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeR2();
	extern "C" YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbePairedSingle();
	YEP_PRIVATE_SYMBOL Yep32u _yepLibrary_ProbeInstruction(Yep32u (*ProbeFunction)());
	YEP_PRIVATE_SYMBOL YepStatus _yepLibrary_DetectLinuxPerfEventSupport(Yep64u &systemFeatures);
#endif
