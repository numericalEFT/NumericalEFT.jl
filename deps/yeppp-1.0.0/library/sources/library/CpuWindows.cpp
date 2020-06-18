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

#if defined(YEP_WINDOWS_OS)
	#include <windows.h>
#else
	#error "The functions in this file should only be used in and compiled for Windows"
#endif

YepStatus _yepLibrary_InitWindowsLogicalCoresCount(Yep32u& logicalCoresCount, Yep64u& systemFeatures) {
	SYSTEM_INFO systemInfo;
	::GetSystemInfo(&systemInfo);
	if (systemInfo.dwNumberOfProcessors != 0) {
		logicalCoresCount = systemInfo.dwNumberOfProcessors;
		if (systemInfo.dwNumberOfProcessors == 1u) {
			systemFeatures |= YepSystemFeatureSingleThreaded;
		}
	} else {
		logicalCoresCount = 1;
	}
	return YepStatusOk;
}

#if defined(YEP_ARM_CPU) && defined(YEP_WINDOWS_OS)
	void _yepLibrary_InitWindowsARMCpuIsaInfo() {
		Yep32u isaFeatures = YepARMIsaFeatureDefault;
		Yep32u simdFeatures = YepARMSimdFeatureDefault;

		isaFeatures |= YepARMFeatureV4;
		isaFeatures |= YepARMFeatureV5;
		isaFeatures |= YepARMFeatureV6;
		isaFeatures |= YepARMFeatureV7;
		isaFeatures |= YepARMFeatureThumb;
		isaFeatures |= YepARMFeatureThumb2;
		isaFeatures |= YepARMFeatureEDSP;
		isaFeatures |= YepARMFeatureVFP3;

		if YEP_LIKELY(::IsProcessorFeaturePresent(PF_ARM_VFP_32_REGISTERS_AVAILABLE) != 0) {
			isaFeatures |= YepARMIsaFeatureVFPd32;
		}
		if YEP_LIKELY(::IsProcessorFeaturePresent(PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE) != 0) {
			isaFeatures |= YepARMFeatureVFP4;
		}
		if YEP_LIKELY(::IsProcessorFeaturePresent(PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE) != 0) {
			isaFeatures |= YepARMFeatureDiv;
		}
		if YEP_LIKELY(::IsProcessorFeaturePresent(PF_ARM_64BIT_LOADSTORE_ATOMIC) != 0) {
			isaFeatures |= YepARMIsaFeatureV6K;
		}
		if YEP_LIKELY(::IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE) != 0) {
			simdFeatures |= YepARMSimdFeatureNEON;
			isaFeatures |= YepARMIsaFeatureVFPd32;
		}

		_isaFeatures = isaFeatures;
		_simdFeatures = simdFeatures;
	}

	void _yepLibrary_InitWindowsARMMicroarchitectureInfo() {
		SYSTEM_INFO systemInfo;
		::GetNativeSystemInfo(&systemInfo);
		// On WinCE SYSTEM_INFO.dwProcessorType matches the "Part number" field of Main ID register,
		// at least for those CPUs listed in WinNT.h
		// Hope this will also hold for future CPUs on Windows 8
		// Since Windows 8 will only run on ARMv7 we check only new CPUs
		switch (systemInfo.dwProcessorType) {
			case 0xC05: // ARM Cortex-A5
				_microarchitecture = YepCpuMicroarchitectureCortexA5;
				_vendor = YepCpuVendorARM;
				break;
			case 0xC07: // ARM Cortex-A7
				_microarchitecture = YepCpuMicroarchitectureCortexA7;
				_vendor = YepCpuVendorARM;
				break;
			case 0xC08: // ARM Cortex-A8
				_microarchitecture = YepCpuMicroarchitectureCortexA8;
				_vendor = YepCpuVendorARM;
				break;
			case 0xC09: // ARM Cortex-A9
				_microarchitecture = YepCpuMicroarchitectureCortexA9;
				_vendor = YepCpuVendorARM;
				break;
			case 0xC0F: // ARM Cortex-A15
				_microarchitecture = YepCpuMicroarchitectureCortexA15;
				_vendor = YepCpuVendorARM;
				break;
			case 0x00F: // Single-core Qualcomm Snapdragon
				// These are mainly Scorpions, but might be some Cortex-A5 as well
				// I know no simple way to distinguish between them, so lets assume everything is Scorpion
				// In future, cache size could be used to separate the two microarchitectures.
			case 0x02D: // Dual-core Qualcomm Scorpion
				_microarchitecture = YepCpuMicroarchitectureScorpion;
				_vendor = YepCpuVendorQualcomm;
				break;
			case 0x04D: // Dual-core Qualcomm Krait
				_microarchitecture = YepCpuMicroarchitectureKrait;
				_vendor = YepCpuVendorQualcomm;
				break;
			case 0x06F: // Quad-core Qualcomm Krait
				_microarchitecture = YepCpuMicroarchitectureKrait;
				_vendor = YepCpuVendorQualcomm;
				break;
			case 0x581: // Marvell Armada 510 (PJ4 core, ARMv7)
				_microarchitecture = YepCpuMicroarchitecturePJ4;
				_vendor = YepCpuVendorMarvell;
				break;
		}
	}

	YepStatus _yepLibrary_InitWindowsARMMicroarchitectureInfo() {
		DWORD infoLength = 0;
		BOOL syscallResult = ::GetLogicalProcessorInformation(NULL, &infoLength);
		if YEP_LIKELY(syscallResult == TRUE) {
			// Windows does not report CPU information
			return YepStatusUnsupportedSoftware;
		} else if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
			return YepStatusSystemError;
		}
		HANDLE heap = ::GetProcessHeap();
		if (heap == NULL) {
			return YepStatusSystemError;
		}
		const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION infoBuffer =
			static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(::HeapAlloc(heap, HEAP_ZERO_MEMORY, infoLength));
		if (infoBuffer == NULL) {
			return YepStatusSystemError;
		}
		
		syscallResult = ::GetLogicalProcessorInformation(buffer, &infoLength);
		
		DWORD infoChunks = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		for (DWORD infoChunk = 0; infoChunk < infoChunks; infoChunk++) {

			// We look only at caches connected to the first CPU
			if (infoBuffer[infoChunk].ProcessorMask & 0x00000001) {

				// Ignore other information (NUMA, Hyperthreading, CPU Package)
				if (infoBuffer[infoChunk].Relationship == RelationCache) {

					const BYTE level = infoBuffer[infoChunk].Cache.Level;
					const BYTE associativity = infoBuffer[infoChunk].Cache.Associativity;
					const WORD lineSize = infoBuffer[infoChunk].Cache.LineSize;
					const DWORD size = infoBuffer[infoChunk].Cache.Size;
					if (associativity == 0xFFu) {
						associativity = size / lineSize;
					}

					switch (infoBuffer[infoChunk].Cache.Type == 1) {
						case CacheUnified:
							switch (level) {
								case 1:
									_cache.L1ICacheInfo.cacheSize = size;
									_cache.L1ICacheInfo.lineSize = lineSize;
									_cache.L1ICacheInfo.associativity = associativity;
									_cache.L1ICacheInfo.isUnified = true;

									_cache.L1DCacheInfo.cacheSize = size;
									_cache.L1DCacheInfo.lineSize = lineSize;
									_cache.L1DCacheInfo.associativity = associativity;
									_cache.L1DCacheInfo.isUnified = true;
									break;
								case 2:
									_cache.L2CacheInfo.cacheSize = size;
									_cache.L2CacheInfo.lineSize = lineSize;
									_cache.L2CacheInfo.associativity = associativity;
									_cache.L2CacheInfo.isUnified = true;
									break;
								case 3:
									_cache.L3CacheInfo.cacheSize = size;
									_cache.L3CacheInfo.lineSize = lineSize;
									_cache.L3CacheInfo.associativity = associativity;
									_cache.L3CacheInfo.isUnified = true;
									break;
							}
							break;
						case CacheInstruction:
							if (level == 1) {
								_cache.L1ICacheInfo.cacheSize = size;
								_cache.L1ICacheInfo.lineSize = lineSize;
								_cache.L1ICacheInfo.associativity = associativity;
								_cache.L1ICacheInfo.isUnified = false;
							}
							break;
						case CacheData:
							if (level == 1) {
								_cache.L1DCacheInfo.cacheSize = size;
								_cache.L1DCacheInfo.lineSize = lineSize;
								_cache.L1DCacheInfo.associativity = associativity;
								_cache.L1DCacheInfo.isUnified = false;
							}
							break;
						case CacheTrace:
							if (level == 1) {
								_cache.traceCacheInfo.microops = size;
								if (infoBuffer[infoChunk].Cache.Associativity == 0xFFu) {
									_cache.traceCacheInfo.associativity = size;
								} else {
									_cache.traceCacheInfo.associativity = associativity;
								}
							}
							break;
					}
				}
			}
		}
		syscallResult = ::HeapFree(heap, 0, infoBuffer);
		if (syscallResult == 0) {
			return YepStatusSystemError;
		}
		return YepStatusOk;
	}
#endif
