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
#include <string.h>

#ifndef YEP_MIPS_CPU
	#error "The functions in this file should only be used in and compiled for MIPS"
#endif

#if defined(YEP_LINUX_OS)
	struct ProcCpuInfo {
		
		//	* JZ4720
		//		- 240 MHz
		//		- SIMD extension
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4725B
		//		- 360 MHz
		//		- SIMD extension
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4730
		//		- 336 MHz
		//		- MIPS32 R1
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4740/JZ4732
		//		- 360 MHz
		//		- MIPS32 R1
		//		- SIMD extension
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4750
		//		- 360 MHz
		//		- MIPS32 R1
		//		- SIMD extension (SIMD2)
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4755
		//		- 360 MHz
		//		- MIPS32 R1
		//		- SIMD extension (SIMD2)
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4760
		//		- 600 MHz
		//		- MIPS32 R1
		//		- FPU
		//		- SIMD extension (SIMD2)
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4770
		//		- 1000 MHz
		//		- MIPS32 R2
		//		- FPU
		//		- SIMD extension (SIMD2)
		//		- 16 KB L1I
		//		- 16 KB L1D
		//		- 256 KB L2
		//		- 32-entry dual-pages joint-TLB
		//		- 4-entry ITLB
		//		- 4-entry DTLB
		//	* JZ4780
		//		- 1500 MHz
		//		- MIPS32 R2
		//		- FPU
		//		- SIMD extension (SIMD2)
		//		- 32 KB L1I
		//		- 32 KB L1D
		//		- 512 KB L2
		//		- Dual-core
		enum SystemType {
			SystemTypeUnknown,
			SystemTypeJZ4720,
			SystemTypeJZ4725B,
			SystemTypeJZ4730,
			SystemTypeJZ4740,
			SystemTypeJZ4750,
			SystemTypeJZ4755,
			SystemTypeJZ4760,
			SystemTypeJZ4770,
			SystemTypeJZ4780
		};

		ConstantString decodeSystemType() const {
			switch (this->systemType) {
				case SystemTypeJZ4720:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4720");
				case SystemTypeJZ4725B:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4725B");
				case SystemTypeJZ4730:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4730");
				case SystemTypeJZ4740:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4740");
				case SystemTypeJZ4750:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4750");
				case SystemTypeJZ4755:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4755");
				case SystemTypeJZ4760:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4760");
				case SystemTypeJZ4770:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4770");
				case SystemTypeJZ4780:
					return YEP_MAKE_CONSTANT_STRING("Ingenic JZ4780");
				default:
					return ConstantString();
			}
		}

		enum CpuModel {
			CpuModelUnknown,
			CpuModelIngenicJZRISC,
			CpuModelMIPS24K,
			CpuModelMIPS24Kc,
			CpuModelMIPS34K,
			CpuModelMIPS74K
		};
		
		struct ASE {
			ASE() :
				mips16(false),
				mdmx(false),
				mips3d(false),
				smartmips(false),
				dsp(false),
				dsp2(false),
				mt(false),
				mxu(false),
				vz(false),
				isValid(false)
			{
			}
			
			YepBoolean mips16;
			YepBoolean mdmx;
			YepBoolean mips3d;
			YepBoolean smartmips;
			YepBoolean dsp;
			YepBoolean dsp2;
			YepBoolean mt;
			YepBoolean mxu;
			YepBoolean vz;

			YepBoolean isValid;
		};
		
		ProcCpuInfo() :
			hasWaitInstruction(false),
			systemType(SystemTypeUnknown),
			cpuModel(CpuModelUnknown),
			tlbEntries(0),
			processors(0)
		{
		}

		ASE ase;
		
		YepBoolean hasWaitInstruction;

		SystemType systemType;
		CpuModel cpuModel;
		Yep32u tlbEntries;
		Yep32u processors;
	};

	static void parseProcessorNumber(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		for (const char* valuePointer = valueStart; valuePointer != valueEnd; valuePointer++) {
			const char digit = *valuePointer;
			if ((digit < '0') || (digit > '9'))
				return;
		}
		cpuInfo.processors += 1;
	}

	/*
	 *	Full list of MIPS application-specific extensions reported in /proc/cpuinfo:
	 *	
	 *	* mips16 - support for MIPS16 mode (16-bit encoding for MIPS instructions).
	 *	* mdmx - integer SIMD extension (seems to be deprecated).
	 *	* mips3d - floating-point SIMD extension (seems to be deprecated).
	 *	* smartmips - Cryptography extension (targets mostly smart cards).
	 *	* dsp - integer and fixed-point SIMD extension.
	 *	* dsp2 - integer and fixed-point SIMD extension (release 2).
	 *	* mt - instructions for better exploitation of multithreaded architectures.
	 *	* mxu - Ingenic Media Extension 32-bit SIMD instructions (not in official kernel sources).
	 *
	 *	/proc/cpuinfo on MIPS is populated in file arch/mips/kernel/proc.c in Linux kernel
	 *	Note that some devices may use patched Linux kernels with different ASE names.
	 *	However, the names above were checked on a large number of /proc/cpuinfo listings.
	 *	Also note that Loongson and Ingenic extensions are not reported in ASE list,
	 *	so we use "system type" and "cpu model" info to detect them.
	 */
	static void parseAsesImplemented(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const char* aseStart = valueStart;
		const char* aseEnd;

		/* Mark the ASEs as valid */
		cpuInfo.ase.isValid = true;
		do {
			aseEnd = aseStart + 1;
			for (; aseEnd != valueEnd; aseEnd++) {
				if (*aseEnd == ' ')
					break;
			}
			const YepSize aseLength = aseEnd - aseStart;

			switch (aseLength) {
				case 2:
					if (memcmp(aseStart, "mt", aseLength) == 0) {
						cpuInfo.ase.mt = true;
					} else if (memcmp(aseStart, "vz", aseLength) == 0) {
						cpuInfo.ase.vz = true;
					}
					break;
				case 3:
					if (memcmp(aseStart, "dsp", aseLength) == 0) {
						cpuInfo.ase.dsp = true;
					} else if (memcmp(aseStart, "mxu", aseLength) == 0) {
						cpuInfo.ase.mxu = true;
					}
					break;
				case 4:
					if (memcmp(aseStart, "mdmx", aseLength) == 0) {
						cpuInfo.ase.mdmx = true;
					} else if (memcmp(aseStart, "dsp2", aseLength) == 0) {
						cpuInfo.ase.dsp2 = true;
					}
					break;
				case 6:
					if (memcmp(aseStart, "mips3d", aseLength) == 0) {
						cpuInfo.ase.mips3d = true;
					} else if (memcmp(aseStart, "mips16", aseLength) == 0) {
						cpuInfo.ase.mips16 = true;
					}
					break;
				case 9:
					if (memcmp(aseStart, "smartmips", aseLength) == 0) {
						cpuInfo.ase.smartmips = true;
					}
					break;
			}
			aseStart = aseEnd;
			for (; aseStart != valueEnd; aseStart++) {
				if (*aseStart != ' ')
					break;
			}
		} while (aseStart != aseEnd);
	}

	static void parseWaitInstruction(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		// Value should contain either "yes" or "no".
		// Since MIPSCpuInfo.hasWaitInstruction is initialized to false
		// we only check for value "yes".
		if (valueLength == 3) {
			if (memcmp(valueStart, "yes", valueLength) == 0) {
				cpuInfo.hasWaitInstruction = true;
			}
		}
	}

	static void parseSystemType(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		if (valueLength == 6) {
			if (memcmp(valueStart, "JZ4720", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4720;
			} else if (memcmp(valueStart, "JZ4730", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4730;
			} else if (memcmp(valueStart, "JZ4732", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4740;
			} else if (memcmp(valueStart, "JZ4740", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4740;
			} else if (memcmp(valueStart, "JZ4750", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4750;
			} else if (memcmp(valueStart, "JZ4755", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4755;
			} else if (memcmp(valueStart, "JZ4760", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4760;
			} else if (memcmp(valueStart, "JZ4770", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4770;
			} else if (memcmp(valueStart, "JZ4780", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4780;
			}
		} else if (valueLength == 7) {
			if (memcmp(valueStart, "JZ4725B", valueLength) == 0) {
				cpuInfo.systemType = ProcCpuInfo::SystemTypeJZ4725B;
			}
		}
	}

	static YEP_INLINE YepBoolean isSpace(char c) {
		switch (c) {
			case ' ':
			case '\t':
				return true;
			default:
				return false;
		}
	}
	
	static void parseCpuModel(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		if (valueLength >= 14) {
			if (memcmp(valueStart, "Ingenic JZRISC", 14) == 0) {
				if ((valueLength == 14) || (isSpace(valueStart[14]))) {
					cpuInfo.cpuModel = ProcCpuInfo::CpuModelIngenicJZRISC;
					return;
				}
			}
		}
		if (valueLength >= 8) {
			if (memcmp(valueStart, "MIPS 24K", 8) == 0) {
				if ((valueLength == 8) || (isSpace(valueStart[8]))) {
					cpuInfo.cpuModel = ProcCpuInfo::CpuModelMIPS24K;
					return;
				}
			}
			if (memcmp(valueStart, "MIPS 34K", 8) == 0) {
				if ((valueLength == 8) || (isSpace(valueStart[8]))) {
					cpuInfo.cpuModel = ProcCpuInfo::CpuModelMIPS34K;
					return;
				}
			}
			if (memcmp(valueStart, "MIPS 74K", 8) == 0) {
				if ((valueLength == 8) || (isSpace(valueStart[8]))) {
					cpuInfo.cpuModel = ProcCpuInfo::CpuModelMIPS74K;
					return;
				}
			}
		}
		if (valueLength >= 9) {
			if (memcmp(valueStart, "MIPS 24Kc", 9) == 0) {
				if ((valueLength == 9) || (isSpace(valueStart[9]))) {
					cpuInfo.cpuModel = ProcCpuInfo::CpuModelMIPS24Kc;
					return;
				}
			}
		}
	}

	static void parseTlbEntries(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		Yep32u tlbEntries = 0;
		for (const char* valuePointer = valueStart; valuePointer != valueEnd; valuePointer++) {
			const char character = *valuePointer;
			// Verify that tlb_entries is a decimal number
			if ((character < '0') || (character > '9'))
				return;

			tlbEntries = tlbEntries * 10 + (character - '0');
		}

		cpuInfo.tlbEntries = tlbEntries;
	}

	/*	Decode a single line of /proc/cpuinfo information.
	 *	Lines have format <words-with-spaces>[ ]*:[ ]<space-separated words>
	 *	An example of /proc/cpuinfo:
	 *
	 *		system type             : JZ4770
	 *		processor               : 0
	 *		cpu model               : Ingenic JZRISC V4.15  FPU V0.0
	 *		BogoMIPS                : 814.28
	 *		wait instruction        : yes
	 *		microsecond timers      : no
	 *		tlb_entries             : 32
	 *		extra interrupt vector  : yes
	 *		hardware watchpoint     : yes, count: 1, address/irw mask: [0x0fff]
	 *		ASEs implemented        :
	 *		shadow register sets    : 1
	 *		core                    : 0
	 *		VCED exceptions         : not available
	 *		VCEI exceptions         : not available
	 */
	static void parseCpuInfoLine(const char* lineStart, const char* lineEnd, void* state) {
		// Empty line. Skip.
		if (lineStart == lineEnd)
			return;

		// Search for ':' on the line.
		const char* separator = lineStart;
		for (; separator != lineEnd; separator++) {
			if (*separator == ':')
				break;
		}
		// Skip line if no ':' separator was found.
		if (separator == lineEnd)
			return;

		// Skip trailing spaces in key part.
		const char* infoKeyEnd = separator;
		for (; infoKeyEnd != lineStart; infoKeyEnd--) {
			if ((infoKeyEnd[-1] != ' ') && (infoKeyEnd[-1] != '\t'))
				break;
		}
		// Skip line if key contains nothing but spaces.
		if (infoKeyEnd == lineStart)
			return;

		// Skip leading spaces in value part.
		const char* infoValueStart = separator + 1;
		for (; infoValueStart != lineEnd; infoValueStart++) {
			if (*infoValueStart != ' ')
				break;
		}
		// Value part contains nothing but spaces. Skip line.
		if (infoValueStart == lineEnd)
			return;

		// Skip trailing spaces in value part (if any)
		const char* infoValueEnd = lineEnd;
		for (; infoValueEnd != separator; infoValueEnd--) {
			if (infoValueEnd[-1] != ' ')
				break;
		}

		ProcCpuInfo* cpuInfo = static_cast<ProcCpuInfo*>(state);

		YepSize infoKeyLength = infoKeyEnd - lineStart;
		switch (infoKeyLength) {
			case 9:
				if (memcmp(lineStart, "processor", infoKeyLength) == 0) {
					parseProcessorNumber(infoValueStart, infoValueEnd, *cpuInfo);
				} else if (memcmp(lineStart, "cpu model", infoKeyLength) == 0) {
					parseCpuModel(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 11:
				if (memcmp(lineStart, "system type", infoKeyLength) == 0) {
					parseSystemType(infoValueStart, infoValueEnd, *cpuInfo);
				} else if (memcmp(lineStart, "tlb_entries", infoKeyLength) == 0) {
					parseTlbEntries(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 16:
				if (memcmp(lineStart, "wait instruction", infoKeyLength) == 0) {
					parseWaitInstruction(infoValueStart, infoValueEnd, *cpuInfo);
				} else if (memcmp(lineStart, "ASEs implemented", infoKeyLength) == 0) {
					parseAsesImplemented(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
		}
	}

	static void decodeMicroarchitecture(const ProcCpuInfo& cpuInfo, YepCpuVendor& vendor, YepCpuMicroarchitecture& microarchitecture) {
		switch (cpuInfo.systemType) {
			case ProcCpuInfo::SystemTypeJZ4720:
			case ProcCpuInfo::SystemTypeJZ4725B:
			case ProcCpuInfo::SystemTypeJZ4730:
			case ProcCpuInfo::SystemTypeJZ4740:
			case ProcCpuInfo::SystemTypeJZ4750:
			case ProcCpuInfo::SystemTypeJZ4755:
			case ProcCpuInfo::SystemTypeJZ4760:
			case ProcCpuInfo::SystemTypeJZ4770:
			case ProcCpuInfo::SystemTypeJZ4780:
				vendor = YepCpuVendorIngenic;
				microarchitecture = YepCpuMicroarchitectureXBurst;
				return;
		}
		switch (cpuInfo.cpuModel) {
			case ProcCpuInfo::CpuModelIngenicJZRISC:
				vendor = YepCpuVendorIngenic;
				microarchitecture = YepCpuMicroarchitectureXBurst;
				return;
			case ProcCpuInfo::CpuModelMIPS24K:
			case ProcCpuInfo::CpuModelMIPS24Kc:
				vendor = YepCpuVendorMIPS;
				microarchitecture = YepCpuMicroarchitectureMIPS24K;
				return;
			case ProcCpuInfo::CpuModelMIPS34K:
				vendor = YepCpuVendorMIPS;
				microarchitecture = YepCpuMicroarchitectureMIPS34K;
				return;
			case ProcCpuInfo::CpuModelMIPS74K:
				vendor = YepCpuVendorMIPS;
				microarchitecture = YepCpuMicroarchitectureMIPS74K;
				return;
		}
	}

	static void decodeIsaFeatures(const ProcCpuInfo& cpuInfo, Yep64u& isaFeatures, Yep64u& simdFeatures, Yep64u& systemFeatures) {
		#if defined(YEP_ANDROID_LINUX_OS)
			isaFeatures |= YepMIPSIsaFeatureFPU;
			isaFeatures |= YepMIPSIsaFeatureMIPS_I;
			isaFeatures |= YepMIPSIsaFeatureMIPS_II;
			isaFeatures |= YepMIPSIsaFeatureR1;
		#endif
		if (cpuInfo.ase.mt) {
			isaFeatures |= YepMIPSIsaFeatureMT;
		}
		if (cpuInfo.ase.mips16) {
			isaFeatures |= YepMIPSIsaFeatureMIPS16;
		}
		if (cpuInfo.ase.smartmips) {
			isaFeatures |= YepMIPSIsaFeatureSmartMIPS;
		}
		if (cpuInfo.ase.mdmx) {
			simdFeatures |= YepMIPSSimdFeatureMDMX;
		}
		if (cpuInfo.ase.mips3d) {
			simdFeatures |= YepMIPSSimdFeatureMIPS3D;
			simdFeatures |= YepMIPSSimdFeaturePairedSingle;
		}
		if (cpuInfo.ase.dsp) {
			simdFeatures |= YepMIPSSimdFeatureDSP;
		}
		if (cpuInfo.ase.dsp2) {
			simdFeatures |= YepMIPSSimdFeatureDSP;
			simdFeatures |= YepMIPSSimdFeatureDSP2;
		}
		if (cpuInfo.ase.mxu) {
			simdFeatures |= YepMIPSSimdFeatureMXU;
		}
		switch (cpuInfo.systemType) {
			/* All known XBurst CPUs except JZ4730 support SIMD */
			case ProcCpuInfo::SystemTypeJZ4720:
			case ProcCpuInfo::SystemTypeJZ4725B:
			case ProcCpuInfo::SystemTypeJZ4740:
				simdFeatures |= YepMIPSSimdFeatureMXU;
				break;
			case ProcCpuInfo::SystemTypeJZ4750:
			case ProcCpuInfo::SystemTypeJZ4755:
			case ProcCpuInfo::SystemTypeJZ4760:
			case ProcCpuInfo::SystemTypeJZ4770:
			case ProcCpuInfo::SystemTypeJZ4780:
				simdFeatures |= YepMIPSSimdFeatureMXU;
				simdFeatures |= YepMIPSSimdFeatureMXU2;
				break;
		}
		if (cpuInfo.ase.vz) {
			isaFeatures |= YepMIPSIsaFeatureVZ;
		}
		if YEP_LIKELY(!(isaFeatures & YepMIPSIsaFeatureR2)) {
			const Yep32u probeR2Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeR2);
			if (probeR2Result == 0) {
				isaFeatures |= YepMIPSIsaFeatureR2;
			}
		}
		if YEP_LIKELY(!(simdFeatures & YepMIPSSimdFeaturePairedSingle)) {
			const Yep32u probePairedSingleResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbePairedSingle);
			if (probePairedSingleResult == 0) {
				simdFeatures |= YepMIPSSimdFeaturePairedSingle;
			}
		}
	}

	static void decodeCacheInfo(const ProcCpuInfo& cpuInfo, YepCpuMicroarchitecture microarchitecture, CacheHierarchyInfo& cache) {
		switch (microarchitecture) {
			case YepCpuMicroarchitectureMIPS24K:
			case YepCpuMicroarchitectureMIPS34K:
			case YepCpuMicroarchitectureMIPS74K:
				cache.L1DCacheInfo.cacheSize = 64*1024;
				cache.L1DCacheInfo.isUnified = false;
				cache.L1DCacheInfo.lineSize = 32;
				cache.L1DCacheInfo.associativity = 4;

				cache.L1ICacheInfo.cacheSize = 64*1024;
				cache.L1ICacheInfo.isUnified = false;
				cache.L1ICacheInfo.lineSize = 32;
				cache.L1ICacheInfo.associativity = 4;
				return;
		}
		switch (cpuInfo.systemType) {
			case ProcCpuInfo::SystemTypeJZ4720:
			case ProcCpuInfo::SystemTypeJZ4725B:
			case ProcCpuInfo::SystemTypeJZ4730:
			case ProcCpuInfo::SystemTypeJZ4740:
			case ProcCpuInfo::SystemTypeJZ4750:
			case ProcCpuInfo::SystemTypeJZ4755:
			case ProcCpuInfo::SystemTypeJZ4760:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.isUnified = false;
				cache.L1ICacheInfo.cacheSize = 16*1024;
				cache.L1ICacheInfo.isUnified = false;
				return;
			case ProcCpuInfo::SystemTypeJZ4770:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.isUnified = false;
				cache.L1ICacheInfo.cacheSize = 16*1024;
				cache.L1ICacheInfo.isUnified = false;
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.isUnified = true;
				return;
			case ProcCpuInfo::SystemTypeJZ4780:
				cache.L1DCacheInfo.cacheSize = 32*1024;
				cache.L1DCacheInfo.isUnified = false;
				cache.L1ICacheInfo.cacheSize = 32*1024;
				cache.L1ICacheInfo.isUnified = false;
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.isUnified = true;
				return;
		}
	}

	static ConstantString getNameFromIsa(Yep32u isaFeatures) {
		if (isaFeatures & YepMIPSIsaFeatureR2) {
			return YEP_MAKE_CONSTANT_STRING("MIPS32 R2 compatible");
		} else {
			return YEP_MAKE_CONSTANT_STRING("MIPS32 R1 compatible");
		}
	}

	static void initCpuName(ConstantString& briefCpuName, ConstantString& fullCpuName, const ProcCpuInfo& cpuInfo, Yep64u isaFeatures) {
		const ConstantString processorString = cpuInfo.decodeSystemType();
		if (processorString.isEmpty()) {
			briefCpuName = fullCpuName = getNameFromIsa(isaFeatures);
		} else {
			briefCpuName = fullCpuName = cpuInfo.decodeSystemType();
		}
	}

	YepStatus _yepLibrary_InitCpuInfo() {
		_yepLibrary_InitLinuxLogicalCoresCount(_logicalCoresCount, _systemFeatures);
		ProcCpuInfo cpuInfo;
		YepStatus status = _yepLibrary_ParseProcCpuInfo(parseCpuInfoLine, &cpuInfo);
		if YEP_LIKELY(status == YepStatusOk) {
			decodeMicroarchitecture(cpuInfo, _vendor, _microarchitecture);
			decodeIsaFeatures(cpuInfo, _isaFeatures, _simdFeatures, _systemFeatures);
			decodeCacheInfo(cpuInfo, _microarchitecture, _cache);
		}
		_yepLibrary_DetectLinuxPerfEventSupport(_systemFeatures);

		initCpuName(_briefCpuName, _fullCpuName, cpuInfo, _isaFeatures);

		_dispatchList = _yepLibrary_GetMicroarchitectureDispatchList(_microarchitecture);
		return status;
	}
#else
	#error "The target Operating System is not supported yet"
#endif
