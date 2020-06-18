/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#if defined(_FORTIFY_SOURCE)
	#undef _FORTIFY_SOURCE
#endif

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <library/functions.h>
#include <yepBuiltin.h>
#include <string.h>

#ifndef YEP_ARM_CPU
	#error "The functions in this file should only be used in and compiled for ARM"
#endif

#if defined(YEP_LINUX_OS)
	struct ProcCpuInfo {
		
		struct Features {
			Features() :
				swp(false),
				half(false),
				thumb(false),
				twentySixBit(false),
				fastmult(false),
				fpa(false),
				vfp(false),
				edsp(false),
				java(false),
				iwmmxt(false),
				crunch(false),
				thumbee(false),
				neon(false),
				vfpv3(false),
				vfpv3d16(false),
				tls(false),
				vfpv4(false),
				idiva(false),
				idivt(false),
				isValid(false)
			{
			}
			
			YepBoolean swp;
			YepBoolean half;
			YepBoolean thumb;
			YepBoolean twentySixBit;
			YepBoolean fastmult;
			YepBoolean fpa;
			YepBoolean vfp;
			YepBoolean edsp;
			YepBoolean java;
			YepBoolean iwmmxt;
			YepBoolean crunch;
			YepBoolean thumbee;
			YepBoolean neon;
			YepBoolean vfpv3;
			YepBoolean vfpv3d16;
			YepBoolean tls;
			YepBoolean vfpv4;
			YepBoolean idiva;
			YepBoolean idivt;

			YepBoolean isValid;
		};
		
		struct CpuArchitecture {
			CpuArchitecture():
				version(0),
				T(false),
				E(false),
				J(false),
				isValid(false)
			{
			}
			
			Yep32u version;
			YepBoolean T;
			YepBoolean E;
			YepBoolean J;
			YepBoolean isValid;
		};
		
		struct CacheInfo {
			CacheInfo() :
				iSize(0),
				iAssoc(0),
				iLineLength(0),
				iSets(0),
				dSize(0),
				dAssoc(0),
				dLineLength(0),
				dSets(0),
				isValid(false)
			{
			}
			
			Yep32u iSize;
			Yep32u iAssoc;
			Yep32u iLineLength;
			Yep32u iSets;
			Yep32u dSize;
			Yep32u dAssoc;
			Yep32u dLineLength;
			Yep32u dSets;

			YepBoolean isValid;
		};
		
		ProcCpuInfo() :
			processors(0),
			cpuImplementer(0),
			cpuVariant(0),
			cpuPart(0),
			cpuRevision(0)
		{
		}

		Features features;

		Yep32u processors;

		Yep32u cpuImplementer;
		Yep32u cpuVariant;
		Yep32u cpuPart;
		Yep32u cpuRevision;
		CpuArchitecture cpuArchitecture;
		CacheInfo cacheInfo;
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
	 *	Full list of ARM features reported in /proc/cpuinfo:
	 *	
	 *	* swp - support for SWP instruction (deprecated in ARMv7, can be removed in future)
	 *	* half - support for half-word loads and stores. These instruction are part of ARMv4,
	 *	         so no need to check it on supported CPUs.
	 *	* thumb - support for 16-bit Thumb instruction set. Note that BX instruction is detected
	 *	          by ARMv4T architecture, not by this flag.
	 *	* 26bit - old CPUs merged 26-bit PC and program status register (flags) into 32-bit PC
	 *	          and had special instructions for working with packed PC. Now it is all deprecated.
	 *	* fastmult - most old ARM CPUs could only compute 2 bits of multiplication result per clock
	 *	             cycle, but CPUs with M suffix (e.g. ARM7TDMI) could compute 4 bits per cycle.
	 *	             Of course, now it makes no sense.
	 *	* fpa - floating point accelerator available. On original ARM ABI all floating-point operations
	 *	        generated FPA instructions. If FPA was not available, these instructions generated
	 *	        "illegal operation" interrupts, and the OS processed them by emulating the FPA instructions.
	 *	        Debian used this ABI before it switched to EABI. Now FPA is deprecated.
	 *	* vfp - vector floating point instructions. Available on most modern CPUs (as part of VFPv3).
	 *	        Required by Android ARMv7A ABI and by Ubuntu on ARM.
	 *              Note: there is no flag for VFPv2.
	 *	* edsp - V5E instructions: saturating add/sub and 16-bit x 16-bit -> 32/64-bit multiplications.
	 *	         Required on Android, supported by all CPUs in production.
	 *	* java - Jazelle extension. Supported on most CPUs.
	 *	* iwmmxt - Intel/Marvell Wireless MMX instructions. 64-bit integer SIMD.
	 *	           Supported on XScale (Since PXA270) and Sheeva (PJ1, PJ4) architectures.
	 *	           Note that there is no flag for WMMX2 instructions.
	 *	* crunch - Maverick Crunch instructions. Junk.
	 *	* thumbee - ThumbEE instructions. Almost no documentation is available.
	 *	* neon - NEON instructions (aka Advanced SIMD). MVFR1 register gives more
	 *	         fine-grained information on particular supported features, but
	 *	         the Linux kernel exports only a single flag for all of them.
	 *	         According to ARMv7A docs it also implies the availability of VFPv3
	 *	         (with 32 double-precision registers d0-d31).
	 *	* vfpv3 - VFPv3 instructions. Available on most modern CPUs. Augment VFPv2 by
	 *	          conversion to/from integers and load constant instructions.
	 *	          Required by Android ARMv7A ABI and by Ubuntu on ARM.
	 *	* vfpv3d16 - VFPv3 instructions with only 16 double-precision registers (d0-d15).
	 *	* tls - software thread ID registers.
	 *	        Used by kernel (and likely libc) for efficient implementation of TLS.
	 *	* vfpv4 - fused multiply-add instructions.
	 *	* idiva - DIV instructions available in ARM mode.
	 *	* idivt - DIV instructions available in Thumb mode.
	 *
	 *	/proc/cpuinfo on ARM is populated in file arch/arm/kernel/setup.c in Linux kernel
	 *	Note that some devices may use patched Linux kernels with different feature names.
	 *	However, the names above were checked on a large number of /proc/cpuinfo listings.
	 */
	static void parseFeatures(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const char* featureStart = valueStart;
		const char* featureEnd;
		
		/* Mark the features as valid */
		cpuInfo.features.isValid = true;
		
		do {
			featureEnd = featureStart + 1;
			for (; featureEnd != valueEnd; featureEnd++) {
				if (*featureEnd == ' ')
					break;
			}
			const YepSize featureLength = featureEnd - featureStart;
			
			switch (featureLength) {
				case 3:
					if (memcmp(featureStart, "swp", featureLength) == 0) {
						cpuInfo.features.swp = true;
					} else if (memcmp(featureStart, "fpa", featureLength) == 0) {
						cpuInfo.features.fpa = true;
					} else if (memcmp(featureStart, "vfp", featureLength) == 0) {
						cpuInfo.features.vfp = true;
					} else if (memcmp(featureStart, "tls", featureLength) == 0) {
						cpuInfo.features.tls = true;
					}
					break;
				case 4:
					if (memcmp(featureStart, "half", featureLength) == 0) {
						cpuInfo.features.half = true;
					} else if (memcmp(featureStart, "edsp", featureLength) == 0) {
						cpuInfo.features.edsp = true;
					} else if (memcmp(featureStart, "java", featureLength) == 0) {
						cpuInfo.features.java = true;
					} else if (memcmp(featureStart, "neon", featureLength) == 0) {
						cpuInfo.features.neon = true;
					}
					break;
				case 5:
					if (memcmp(featureStart, "thumb", featureLength) == 0) {
						cpuInfo.features.thumb = true;
					} else if (memcmp(featureStart, "26bit", featureLength) == 0) {
						cpuInfo.features.twentySixBit = true;
					} else if (memcmp(featureStart, "vfpv3", featureLength) == 0) {
						cpuInfo.features.vfpv3 = true;
					} else if (memcmp(featureStart, "vfpv4", featureLength) == 0) {
						cpuInfo.features.vfpv4 = true;
					} else if (memcmp(featureStart, "idiva", featureLength) == 0) {
						cpuInfo.features.idiva = true;
					} else if (memcmp(featureStart, "idivt", featureLength) == 0) {
						cpuInfo.features.idivt = true;
					}
					break;
				case 6:
					if (memcmp(featureStart, "iwmmxt", featureLength) == 0) {
						cpuInfo.features.iwmmxt = true;
					} else if (memcmp(featureStart, "crunch", featureLength) == 0) {
						cpuInfo.features.crunch = true;
					}
					break;
				case 7:
					if (memcmp(featureStart, "thumbee", featureLength) == 0) {
						cpuInfo.features.thumbee = true;
					}
					break;
				case 8:
					if (memcmp(featureStart, "fastmult", featureLength) == 0) {
						cpuInfo.features.fastmult = true;
					} else if (memcmp(featureStart, "vfpv3d16", featureLength) == 0) {
						cpuInfo.features.vfpv3d16 = true;
					}
					break;
			}
			featureStart = featureEnd;
			for (; featureStart != valueEnd; featureStart++) {
				if (*featureStart != ' ')
					break;
			}
		} while (featureStart != featureEnd);
	}

	static void parseCpuPart(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		// Value should contain hex prefix (0x) and one to three hex digits.
		// I have never seen less than three digits as a value of this field,
		// but I don't think it is impossible to see such values in future.
		// Value can not contain more than three hex digits since
		// Main ID Register (MIDR) assigns only a 12-bit value for CPU part.
		if ((valueLength < 3) || (valueLength > 5))
			return;

		// Skip if there is no hex prefix (0x)
		if ((valueStart[0] != '0') || (valueStart[1] != 'x'))
			return;

		// Check if the values after hex prefix are really hex digits and decode them.
		Yep32u cpuPart = 0;
		for (const char* digitPosition = valueStart + 2; digitPosition != valueEnd; digitPosition++) {
			const char digitCharacter = *digitPosition;
			Yep32u digit;
			if ((digitCharacter >= '0') && (digitCharacter <= '9')) {
				digit = digitCharacter - '0';
			} else if ((digitCharacter >= 'A') && (digitCharacter <= 'F')) {
				digit = 10 + (digitCharacter - 'A');
			} else if ((digitCharacter >= 'a') && (digitCharacter <= 'f')) {
				digit = 10 + (digitCharacter - 'a');
			} else {
				// Not a hex digit
				return;
			}
			cpuPart = cpuPart * 16 + digit;
		}

		cpuInfo.cpuPart = cpuPart;
	}

	static void parseCpuImplementer(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		// Value should contain hex prefix (0x) and one or two hex digits.
		// I have never seen single hex digit as a value of this field,
		// but I don't think it is impossible in future.
		// Value can not contain more than two hex digits since
		// Main ID Register (MIDR) assigns only an 8-bit value for CPU implementer.
		if ((valueLength != 3) && (valueLength != 4))
			return;

		// Skip if there is no hex prefix (0x)
		if ((valueStart[0] != '0') || (valueStart[1] != 'x'))
			return;

		// Check if the values after hex prefix are really hex digits and decode them.
		const char hexDigits[2] = { ((valueLength == 3) ? '0' : valueEnd[-2]), valueEnd[-1] };
		Yep32u digitValues[2];
		for (YepSize digitPosition = 0; digitPosition < 2; digitPosition++) {
			const char digit = hexDigits[digitPosition];
			if ((digit >= '0') && (digit <= '9')) {
				digitValues[digitPosition] = digit - '0';
			} else if ((digit >= 'A') && (digit <= 'F')) {
				digitValues[digitPosition] = 10 + (digit - 'A');
			} else if ((digit >= 'a') && (digit <= 'f')) {
				digitValues[digitPosition] = 10 + (digit - 'a');
			} else {
				// Not a hex digit
				return;
			}
		}

		cpuInfo.cpuImplementer = digitValues[0] * 16 + digitValues[1];
	}

	static void parseCpuVariant(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		const YepSize valueLength = valueEnd - valueStart;

		// Value should contain hex prefix (0x) and one hex digit.
		// Value can not contain more than one hex digits since
		// Main ID Register (MIDR) assigns only a 4-bit value for CPU variant.
		if (valueLength != 3)
			return;

		// Skip if there is no hex prefix (0x)
		if ((valueStart[0] != '0') || (valueStart[1] != 'x'))
			return;

		// Check if the value after hex prefix is indeed a hex digit and decode it.
		const char digit = valueStart[2];
		if ((digit >= '0') && (digit <= '9')) {
			cpuInfo.cpuVariant = digit - '0';
		} else if ((digit >= 'A') && (digit <= 'F')) {
			cpuInfo.cpuVariant = 10 + (digit - 'A');
		} else if ((digit >= 'a') && (digit <= 'f')) {
			cpuInfo.cpuVariant = 10 + (digit - 'a');
		} else {
			// Not a hex digit
			return;
		}
	}

	static void parseCpuArchitecture(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		Yep32u architecture = 0;
		const char* valuePointer = valueStart;
		for (; valuePointer != valueEnd; valuePointer++) {
			const char character = *valuePointer;
			// Verify that CPU architecture is a decimal number
			if ((character < '0') || (character > '9'))
				break;

			architecture = architecture * 10 + (character - '0');
		}
		if (architecture != 0) {
			cpuInfo.cpuArchitecture.isValid = true;
			cpuInfo.cpuArchitecture.version = architecture;
		}

		for (; valuePointer != valueEnd; valuePointer++) {
			const char character = *valuePointer;
			switch (character) {
				case 'E':
					cpuInfo.cpuArchitecture.E = true;
					break;
				case 'J':
					cpuInfo.cpuArchitecture.J = true;
					break;
				case 'T':
					cpuInfo.cpuArchitecture.T = true;
					break;
			}
		}
	}

	static void parseCpuRevision(const char* valueStart, const char* valueEnd, ProcCpuInfo& cpuInfo) {
		Yep32u revision = 0;
		for (const char* valuePointer = valueStart; valuePointer != valueEnd; valuePointer++) {
			const char character = *valuePointer;
			// Verify that CPU revision is a decimal number
			if ((character < '0') || (character > '9'))
				return;

			revision = revision * 10 + (character - '0');
		}

		cpuInfo.cpuRevision = revision;
	}

	static YepStatus parseCacheNumber(const char* valueStart, const char* valueEnd, Yep32u& cacheNumber) {
		Yep32u number = 0;
		for (const char* valuePointer = valueStart; valuePointer != valueEnd; valuePointer++) {
			const char character = *valuePointer;
			// Verify that the value is a decimal number
			if ((character < '0') || (character > '9'))
				return YepStatusInvalidData;

			number = number * 10 + (character - '0');
		}

		cacheNumber = number;
		return YepStatusOk;
	}

	/*	Decode a single line of /proc/cpuinfo information.
	 *	Lines have format <words-with-spaces>[ ]*:[ ]<space-separated words>
	 *	An example of /proc/cpuinfo (from Pandaboard-ES):
	 *
	 *		Processor       : ARMv7 Processor rev 10 (v7l)
	 *		processor       : 0
	 *		BogoMIPS        : 1392.74
	 *
	 *		processor       : 1
	 *		BogoMIPS        : 1363.33
	 *
	 *		Features        : swp half thumb fastmult vfp edsp thumbee neon vfpv3
	 *		CPU implementer : 0x41
	 *		CPU architecture: 7
	 *		CPU variant     : 0x2
	 *		CPU part        : 0xc09
	 *		CPU revision    : 10
	 *
	 *		Hardware        : OMAP4 Panda board
	 *		Revision        : 0020
	 *		Serial          : 0000000000000000
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
			case 6:
				if YEP_LIKELY(memcmp(lineStart, "I size", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.iSize) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				} else if YEP_LIKELY(memcmp(lineStart, "I sets", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.iSets) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				} else if YEP_LIKELY(memcmp(lineStart, "D size", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.dSize) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				} else if YEP_LIKELY(memcmp(lineStart, "D sets", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.dSets) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				}
				break;
			case 7:
				if YEP_LIKELY(memcmp(lineStart, "I assoc", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.iAssoc) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				} else if YEP_LIKELY(memcmp(lineStart, "D assoc", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.dAssoc) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				}
				break;
			case 8:
				if YEP_LIKELY(memcmp(lineStart, "CPU part", infoKeyLength) == 0) {
					parseCpuPart(infoValueStart, infoValueEnd, *cpuInfo);
				} else if YEP_LIKELY(memcmp(lineStart, "Features", infoKeyLength) == 0) {
					parseFeatures(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 9:
				if YEP_LIKELY(memcmp(lineStart, "processor", infoKeyLength) == 0) {
					parseProcessorNumber(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 11:
				if YEP_LIKELY(memcmp(lineStart, "CPU variant", infoKeyLength) == 0) {
					parseCpuVariant(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 12:
				if YEP_LIKELY(memcmp(lineStart, "CPU revision", infoKeyLength) == 0) {
					parseCpuRevision(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 13:
				if YEP_LIKELY(memcmp(lineStart, "I line length", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.iLineLength) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				} else if YEP_LIKELY(memcmp(lineStart, "D line length", infoKeyLength) == 0) {
					if YEP_LIKELY(parseCacheNumber(infoValueStart, infoValueEnd, cpuInfo->cacheInfo.dLineLength) == YepStatusOk) {
						cpuInfo->cacheInfo.isValid = true;
					}
				}
				break;
			case 15:
				if YEP_LIKELY(memcmp(lineStart, "CPU implementer", infoKeyLength) == 0) {
					parseCpuImplementer(infoValueStart, infoValueEnd, *cpuInfo);
				} else if YEP_UNLIKELY(memcmp(lineStart, "CPU implementor", infoKeyLength) == 0) {
					parseCpuImplementer(infoValueStart, infoValueEnd, *cpuInfo);
				}
				break;
			case 16:
				if YEP_LIKELY(memcmp(lineStart, "CPU architecture", infoKeyLength) == 0) {
					parseCpuArchitecture(infoValueStart, infoValueEnd, *cpuInfo);
				}
		}
	}

	static void decodeMicroarchitecture(const ProcCpuInfo& cpuInfo, YepCpuVendor& vendor, YepCpuMicroarchitecture& microarchitecture) {
		switch (cpuInfo.cpuImplementer) {
			case 'A':
				vendor = YepCpuVendorARM;
				switch (cpuInfo.cpuPart) {
					case 0xB56: // ARM1156
					case 0xB02: // ARM11 MPCore
					case 0xB36: // ARM1136
					case 0xB76: // ARM1176
						microarchitecture = YepCpuMicroarchitectureARM11;
						break;
					case 0xC05:
						microarchitecture = YepCpuMicroarchitectureCortexA5;
						break;
					case 0xC07:
						microarchitecture = YepCpuMicroarchitectureCortexA7;
						break;
					case 0xC08:
						microarchitecture = YepCpuMicroarchitectureCortexA8;
						break;
					case 0xC09:
						microarchitecture = YepCpuMicroarchitectureCortexA9;
						break;
					case 0xC0F:
						microarchitecture = YepCpuMicroarchitectureCortexA15;
						break;
					default:
						if ((cpuInfo.cpuPart & 0xF00) == 0x700) {
							microarchitecture = YepCpuMicroarchitectureARM7;
						} else if ((cpuInfo.cpuPart & 0xF00) == 0x900) {
							microarchitecture = YepCpuMicroarchitectureARM9;
						}
						break;
				}
				break;
			case 'D':
				vendor = YepCpuVendorDEC;
				microarchitecture = YepCpuMicroarchitectureStrongARM;
				break;
			case 'M':
				vendor = YepCpuVendorMotorola;
				break;
			case 'T':
				vendor = YepCpuVendorTI;
				if (cpuInfo.cpuPart == 0x925) {
					vendor = YepCpuVendorARM;
					microarchitecture = YepCpuMicroarchitectureARM9;
				}
				break;
			case 'Q':
				vendor = YepCpuVendorQualcomm;
				switch (cpuInfo.cpuPart) {
					case 0x00F: // Mostly Scorpions, but some Cortex A5 may report this value as well
						if (cpuInfo.features.vfpv4) {
							// Unlike Scorpion, Cortex-A5 comes with VFPv4
							vendor = YepCpuVendorARM;
							microarchitecture = YepCpuMicroarchitectureCortexA5;
						} else {
							microarchitecture = YepCpuMicroarchitectureScorpion;
						}
						break;
					case 0x02D: // Dual-core Scorpions
						microarchitecture = YepCpuMicroarchitectureScorpion;
						break;
					case 0x04D: // Dual-core Krait
					case 0x06F: // Quad-core Krait
						microarchitecture = YepCpuMicroarchitectureKrait;
						break;
				}
				break;
			case 'V':
				// I don't understand the logic behind Marvell's CPU parts,
				// so only verified CPU parts are listed here
				switch (cpuInfo.cpuPart) {
					case 0x693: // PXA 935
						microarchitecture = YepCpuMicroarchitecturePJ1;
						break;
					case 0x131: // Feroceon 88FR131
					case 0x301: // Feroceon 88FR301 ?
					case 0x331: // Feroceon 88FR331 ?
					case 0x531: // Feroceon 88FR531
					case 0x571: // Feroceon 88FR571 ?
						microarchitecture = YepCpuMicroarchitecturePJ1;
						break;
					case 0x581: // Armada 510
						microarchitecture = YepCpuMicroarchitecturePJ4;
						break;
				}
				vendor = YepCpuVendorMarvell;
				break;
			case 'i':
				if (cpuInfo.cpuPart == 0xB11) {
					microarchitecture = YepCpuMicroarchitectureStrongARM;
				} else if ((cpuInfo.cpuPart & 0xF00) == 0x200) {
					// PXA 210/25X/26X
					microarchitecture = YepCpuMicroarchitectureXScale;
				} else if ((cpuInfo.cpuPart & 0xF00) == 0x400) {
					// PXA 27X
					microarchitecture = YepCpuMicroarchitectureXScale;
				} else if ((cpuInfo.cpuPart & 0xF00) == 0x600) {
					// PXA 3XX
					microarchitecture = YepCpuMicroarchitectureXScale;
				}
				vendor = YepCpuVendorIntel;
				break;
		}
	}

	static void decodeIsaFeatures(const ProcCpuInfo& cpuInfo, YepCpuVendor vendor, YepCpuMicroarchitecture microarchitecture, Yep64u& isaFeatures, Yep64u& simdFeatures, Yep64u& systemFeatures) {
		if YEP_UNLIKELY(cpuInfo.cpuArchitecture.T) {
			isaFeatures |= YepARMIsaFeatureThumb;
		}
		if YEP_UNLIKELY(cpuInfo.cpuArchitecture.J) {
			isaFeatures |= YepARMIsaFeatureJazelle;
		}

		if YEP_LIKELY(cpuInfo.cpuArchitecture.version >= 4) {
			isaFeatures |= YepARMIsaFeatureV4;
		}
		if YEP_LIKELY(cpuInfo.cpuArchitecture.version >= 5) {
			isaFeatures |= YepARMIsaFeatureV5;
			if YEP_LIKELY(cpuInfo.cpuArchitecture.E) {
				isaFeatures |= YepARMIsaFeatureV5E;
			}

			const Yep32u probeXScaleResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeXScale);
			if YEP_UNLIKELY(probeXScaleResult == 0) {
				simdFeatures |= YepARMSimdFeatureXScale;
			}
		}
		if YEP_LIKELY(cpuInfo.cpuArchitecture.version >= 6) {
			isaFeatures |= YepARMIsaFeatureV6;
			
			if YEP_UNLIKELY(cpuInfo.cpuArchitecture.version == 6) {
				const Yep32u probeV6KResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeV6K);
				if (probeV6KResult == 0) {
					isaFeatures |= YepARMIsaFeatureV6K;
				}
			}
		}
		if YEP_LIKELY(cpuInfo.cpuArchitecture.version >= 7) {
			// WTF? Raspberry Pi shows Architecture: 7 in /proc/cpuinfo
			if YEP_LIKELY(microarchitecture != YepCpuMicroarchitectureARM11) {
				isaFeatures |= YepARMIsaFeatureV5E;
				isaFeatures |= YepARMIsaFeatureV6K;
				isaFeatures |= YepARMIsaFeatureV7;
				isaFeatures |= YepARMIsaFeatureThumb;
				isaFeatures |= YepARMIsaFeatureThumb2;

				const Yep32u probeV7MPResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeV7MP);
				if YEP_LIKELY(probeV7MPResult == 0) {
					isaFeatures |= YepARMIsaFeatureV7MP;
				}
			}
		}

		if YEP_LIKELY(cpuInfo.features.thumb) {
			isaFeatures |= YepARMIsaFeatureThumb;
		}
		if YEP_UNLIKELY(cpuInfo.features.fpa) {
			isaFeatures |= YepARMIsaFeatureFPA;
			systemFeatures |= YepARMSystemFeatureFPA;
		}
		if YEP_LIKELY(cpuInfo.features.vfp) {
			isaFeatures |= YepARMIsaFeatureVFP;
			systemFeatures |= YepARMSystemFeatureS32;
		}
		if YEP_LIKELY(cpuInfo.features.edsp) {
			isaFeatures |= YepARMIsaFeatureV5E;
		}
		if YEP_UNLIKELY(cpuInfo.features.java) {
			isaFeatures |= YepARMIsaFeatureJazelle;
		}
		if YEP_UNLIKELY(cpuInfo.features.iwmmxt) {
			systemFeatures |= YepARMSystemFeatureWMMX;
			const Yep64u wcidReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadWCID);
			const Yep32u wcidReadSuccess = yepBuiltin_GetLowPart_64u_32u(wcidReadResult);
			if YEP_LIKELY(wcidReadSuccess) {
				const Yep32u wcid = yepBuiltin_GetHighPart_64u_32u(wcidReadResult);
				const Yep32u coprocessorType = (wcid >> 8) & 0xFF;
				if YEP_LIKELY(coprocessorType >= 0x10) {
					simdFeatures |= YepARMSimdFeatureWMMX;
				}
				if YEP_LIKELY(coprocessorType >= 0x20) {
					simdFeatures |= YepARMSimdFeatureWMMX2;
				}
			}
		}
		if YEP_LIKELY(cpuInfo.features.thumbee) {
			isaFeatures |= YepARMIsaFeatureThumbEE;
		}
		if YEP_LIKELY(cpuInfo.features.neon) {
			simdFeatures |= YepARMSimdFeatureNEON;
			/* NEON mandates support for VFPv3-D32 */
			isaFeatures |= YepARMIsaFeatureVFP;
			isaFeatures |= YepARMIsaFeatureVFP2;
			isaFeatures |= YepARMIsaFeatureVFP3;
			isaFeatures |= YepARMIsaFeatureVFPd32;
			/* NEON mandates 32 D registers */
			systemFeatures |= YepARMSystemFeatureS32;
			systemFeatures |= YepARMSystemFeatureD32;
		}
		if YEP_LIKELY(cpuInfo.features.vfpv3) {
			systemFeatures |= YepARMSystemFeatureS32;
			isaFeatures |= YepARMIsaFeatureVFP;
			isaFeatures |= YepARMIsaFeatureVFP2;
			isaFeatures |= YepARMIsaFeatureVFP3;
			if YEP_UNLIKELY(!cpuInfo.features.neon) {
				/* VFPv3 could mean either VFPv3-D16 or VFPv3-D32 */
				const Yep32u probeVFPd32Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFPd32);
				if YEP_LIKELY(probeVFPd32Result == 0) {
					/* Processor supports VFP-D32, but there is no guarantee that the upper D registers are preserved during context switch */
					isaFeatures |= YepARMIsaFeatureVFPd32;
				}
			}
		}
		if YEP_UNLIKELY(cpuInfo.features.vfpv3d16) {
			systemFeatures |= YepARMSystemFeatureS32;
			systemFeatures &= ~YepARMSystemFeatureD32;
			isaFeatures |= YepARMIsaFeatureVFP;
			isaFeatures |= YepARMIsaFeatureVFP2;
			isaFeatures |= YepARMIsaFeatureVFP3;
			isaFeatures &= ~YepARMIsaFeatureVFPd32;
		}
		if YEP_LIKELY(cpuInfo.features.vfpv4) {
			systemFeatures |= YepARMSystemFeatureS32;
			isaFeatures |= YepARMIsaFeatureVFP;
			isaFeatures |= YepARMIsaFeatureVFP2;
			isaFeatures |= YepARMIsaFeatureVFP3;
			isaFeatures |= YepARMIsaFeatureVFP3HP;
			isaFeatures |= YepARMIsaFeatureVFP4;
			if YEP_UNLIKELY(!cpuInfo.features.neon) {
				/* VFPv4 could mean either VFPv4-D16 or VFPv4-D32 */
				const Yep32u probeVFPd32Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFPd32);
				if YEP_LIKELY(probeVFPd32Result == 0) {
					/* Processor supports VFP-D32, but there is no guarantee that the upper D registers are preserved during context switch */
					isaFeatures |= YepARMIsaFeatureVFPd32;
				}
			}
		}
		if YEP_LIKELY(cpuInfo.features.idiva) {
			isaFeatures |= YepARMIsaFeatureDiv;
		}

		// Old kernels might not report DIV instructions,
		// so probe them to know for sure
		if (!(isaFeatures & YepARMIsaFeatureDiv)) {
			const Yep32u probeDivResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeDiv);
			if (probeDivResult == 0) {
				isaFeatures |= YepARMIsaFeatureDiv;
			}
		}
		// If CPU supports VFP, but doesn't support VFPv3, it seems strange...
		if ((isaFeatures & YepARMIsaFeatureVFP) && !(isaFeatures & YepARMIsaFeatureVFP3)) {
			const Yep32u probeVFP3Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFP3);
			if YEP_LIKELY(probeVFP3Result == 0) {
				isaFeatures |= YepARMIsaFeatureVFP2;
				isaFeatures |= YepARMIsaFeatureVFP3;
				const Yep32u probeVFPd32Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFPd32);
				if (probeVFPd32Result == 0) {
					isaFeatures |= YepARMIsaFeatureVFPd32;
				}
			}
		}
		// If CPU supports VFPv3, perhaps it supports half-precision extension
		if YEP_LIKELY(isaFeatures & YepARMIsaFeatureVFP3) {
			if YEP_UNLIKELY(!(isaFeatures & YepARMIsaFeatureVFP3HP)) {
				const Yep32u probeVFP3HPResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFP3HP);
				if YEP_LIKELY(probeVFP3HPResult == 0) {
					isaFeatures |= YepARMIsaFeatureVFP3HP;
				}
			}
		}
		// If CPU supports VFPv3HP, but doesn't support VFPv4, it seems strange...
		if ((isaFeatures & YepARMIsaFeatureVFP3HP) && !(isaFeatures & YepARMIsaFeatureVFP4)) {
			const Yep32u probeVFP4Result = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeVFP4);
			if YEP_LIKELY(probeVFP4Result == 0) {
				isaFeatures |= YepARMIsaFeatureVFP4;
			}
		}
		// If CPU supports NEON, perhaps it supports half-precision extension
		if (simdFeatures & YepARMSimdFeatureNEON) {
			const Yep32u probeNeonHpResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeNeonHp);
			if YEP_LIKELY(probeNeonHpResult == 0) {
				simdFeatures |= YepARMSimdFeatureNEONHP;
			}
		}
		// If CPU supports NEON and VFPv4, perhaps it supports NEONv2
		if ((isaFeatures & YepARMIsaFeatureVFP4) && (simdFeatures & YepARMSimdFeatureNEON)) {
			const Yep32u probeNeonResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeNeon2);
			if YEP_LIKELY(probeNeonResult == 0) {
				simdFeatures |= YepARMSimdFeatureNEON2;
			}
		}
		// On Marvell CPUs check Marvell Armada extensions.
		// I doubt that any third-party processors will ever support them.
		if YEP_UNLIKELY(vendor == YepCpuVendorMarvell) {
			const Yep32u probeArmadaResult = _yepLibrary_ProbeInstruction(&_yepLibrary_ProbeCnt32);
			if YEP_LIKELY(probeArmadaResult == 0) {
				isaFeatures |= YepARMIsaFeatureArmada;
			}
		}

		if YEP_LIKELY(isaFeatures & YepARMIsaFeatureV6) {
			systemFeatures |= YepSystemFeatureMisalignedAccess;
		}

		// VFPv2 is not permitted on ARMv7.
		if YEP_UNLIKELY(!(isaFeatures & YepARMIsaFeatureV7)) {
			if YEP_UNLIKELY(!(isaFeatures & YepARMIsaFeatureVFP2)) {
				const Yep64u fpsidReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadFPSID);
				const Yep32u fpsidReadSuccess = yepBuiltin_GetLowPart_64u_32u(fpsidReadResult);
				if YEP_LIKELY(fpsidReadSuccess) {
					const Yep32u fpsid = yepBuiltin_GetHighPart_64u_32u(fpsidReadResult);
					const Yep32u subarchitecture = (fpsid >> 16) & 0x7F;
					if YEP_LIKELY(subarchitecture >= 0x01) {
						isaFeatures |= YepARMIsaFeatureVFP2;
					}
				}
			}
			const Yep64u mvfr0ReadResult = _yepLibrary_ReadCoprocessor(&_yepLibrary_ReadMVFR0);
			const Yep32u mvfr0ReadSuccess = yepBuiltin_GetLowPart_64u_32u(mvfr0ReadResult);
			if YEP_LIKELY(mvfr0ReadSuccess) {
				const Yep32u mvfr0 = yepBuiltin_GetHighPart_64u_32u(mvfr0ReadResult);
				const Yep32u shortVectorsSupport = (mvfr0 >> 24) & 0xF;
				if YEP_UNLIKELY(shortVectorsSupport == 0x1) {
					systemFeatures |= YepARMSystemFeatureVFPVectorMode;
				}
			}
		}
	}

	static void decodeCacheInfo(const ProcCpuInfo& cpuInfo, YepCpuMicroarchitecture microarchitecture, Yep32u logicalCores, CacheHierarchyInfo& cache) {
		// For old architectures (pre-v7) the kernel exported the cache information in /proc/cpuinfo
		// If there is information about the cache, use it
		if YEP_UNLIKELY(cpuInfo.cacheInfo.isValid) {

			cache.L1ICacheInfo.cacheSize = cpuInfo.cacheInfo.iSize;
			cache.L1ICacheInfo.lineSize = cpuInfo.cacheInfo.iLineLength;
			cache.L1ICacheInfo.associativity = cpuInfo.cacheInfo.iAssoc;
			cache.L1ICacheInfo.isUnified = false;
			
			cache.L1DCacheInfo.cacheSize = cpuInfo.cacheInfo.dSize;
			cache.L1DCacheInfo.lineSize = cpuInfo.cacheInfo.dLineLength;
			cache.L1DCacheInfo.associativity = cpuInfo.cacheInfo.dAssoc;
			cache.L1DCacheInfo.isUnified = false;
		} else {
			// There is no way to determine the cache size in user mode in Linux
			// So I make a best guess from the architecture.
			// Guess about L1 cache size is almost always accurate,
			// but L2 might be different on an actual platform.
			switch (microarchitecture) {
				case YepCpuMicroarchitectureCortexA5:
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.lineSize = 32;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 256*1024;
					cache.L2CacheInfo.lineSize = 32;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureCortexA7:
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.lineSize = 64;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.lineSize = 64;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 512*1024;
					cache.L2CacheInfo.lineSize = 64;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureCortexA8:
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.lineSize = 64;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.lineSize = 64;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 256*1024;
					cache.L2CacheInfo.lineSize = 64;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureCortexA9:
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.lineSize = 32;
					cache.L1DCacheInfo.isUnified = false;
					if (logicalCores >= 2) {
						cache.L2CacheInfo.cacheSize = 1024*1024;
						cache.L2CacheInfo.associativity = 16;
					} else {
						cache.L2CacheInfo.cacheSize = 512*1024;
						cache.L2CacheInfo.associativity = 8; /* Speculation */
					}
					cache.L2CacheInfo.lineSize = 32;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureCortexA15:
					cache.L1ICacheInfo.cacheSize = 64*1024;
					cache.L1ICacheInfo.lineSize = 64;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 64*1024;
					cache.L1DCacheInfo.lineSize = 64;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 1024*1024;
					cache.L2CacheInfo.lineSize = 64;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureARM11:
					cache.L1ICacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 16*1024;
					cache.L1DCacheInfo.lineSize = 32;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 128*1024;
					cache.L2CacheInfo.lineSize = 32;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureARM9:
					cache.L1ICacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1DCacheInfo.isUnified = false;
					break;
				case YepCpuMicroarchitectureScorpion:
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.lineSize = 32;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.lineSize = 32;
					cache.L1DCacheInfo.isUnified = false;
					if (logicalCores >= 2) {
						cache.L2CacheInfo.cacheSize = 512*1024;
					} else {
						cache.L2CacheInfo.cacheSize = 256*1024;
					}
					cache.L2CacheInfo.lineSize = 32;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureKrait:
					cache.L0ICacheInfo.cacheSize = 4*1024;
					cache.L0ICacheInfo.isUnified = false;
					cache.L0ICacheInfo.associativity = 1;
					cache.L0DCacheInfo.cacheSize = 4*1024;
					cache.L0DCacheInfo.isUnified = false;
					cache.L0DCacheInfo.associativity = 1;
				
					cache.L1ICacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1ICacheInfo.associativity = 4;
					cache.L1DCacheInfo.cacheSize = 16*1024;
					cache.L1DCacheInfo.isUnified = false;
					cache.L1DCacheInfo.associativity = 4;
					if (logicalCores >= 4) {
						cache.L2CacheInfo.cacheSize = 2*1024*1024;
						cache.L2CacheInfo.associativity = 16; /* Speculation */
					} else {
						cache.L2CacheInfo.cacheSize = 1024*1024;
						cache.L2CacheInfo.associativity = 8;
					}
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitecturePJ1:
					// Marvell Kirkwood 88F6281 characteristics
					cache.L1ICacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 16*1024;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 256*1024;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitecturePJ4:
					// Marvell Armada 510 characteristics
					cache.L1ICacheInfo.cacheSize = 32*1024;
					cache.L1ICacheInfo.isUnified = false;
					cache.L1DCacheInfo.cacheSize = 32*1024;
					cache.L1DCacheInfo.isUnified = false;
					cache.L2CacheInfo.cacheSize = 512*1024;
					cache.L2CacheInfo.isUnified = true;
					break;
				case YepCpuMicroarchitectureXScale:
					if ((cpuInfo.cpuPart & 0xF00) == 0x200) {
						cache.L1ICacheInfo.cacheSize = 16*1024;
						cache.L1ICacheInfo.isUnified = false;
						cache.L1DCacheInfo.cacheSize = 16*1024;
						cache.L1DCacheInfo.isUnified = false;
					} else {
						cache.L1ICacheInfo.cacheSize = 32*1024;
						cache.L1ICacheInfo.isUnified = false;
						cache.L1DCacheInfo.cacheSize = 32*1024;
						cache.L1DCacheInfo.isUnified = false;
					}
					if ((cpuInfo.cpuPart & 0xF00) == 0x600) {
						// Third-generation XScale CPUs also have L2 cache
						cache.L2CacheInfo.cacheSize = 256*1024;
						cache.L2CacheInfo.isUnified = true;
					}
					break;
			}
		}
	}

	struct KernelLogInfo {
		enum Vendor {
			VendorUnknown,
			VendorSamsung,
			VendorNVidia,
			VendorTexasInstruments,
			VendorFreeScale,
			VendorRockchip,
			VendorBoxchip,
			VendorAllwinner,
			VendorTelechips,
			VendorSTEricson,
			VendorMediaTek
		};
		
		enum Machine {
			MachineUnknown,
			MachineRK28Board,
			MachineRK29Board,
			MachineRK30Board,
			MachineRK31Board,
			MachineSun3i,
			MachineSun4i,
			MachineSun5i,
			MachineSun6i,
			MachineSun7i
		};
		
		KernelLogInfo() :
			vendor(VendorUnknown),
			machine(MachineUnknown),
			nameLength(0),
			id(0)
		{
		}
		
		ConstantString getVendorString() const {
			switch (this->vendor) {
				case VendorSamsung:
					return YEP_MAKE_CONSTANT_STRING("Samsung");
				case VendorNVidia:
					return YEP_MAKE_CONSTANT_STRING("nVidia");
				case VendorTexasInstruments:
					return YEP_MAKE_CONSTANT_STRING("TI");
				case VendorFreeScale:
					return YEP_MAKE_CONSTANT_STRING("FreeScale");
				case VendorRockchip:
					return YEP_MAKE_CONSTANT_STRING("Rockchip");
				case VendorBoxchip:
					return YEP_MAKE_CONSTANT_STRING("Boxchip");
				case VendorAllwinner:
					return YEP_MAKE_CONSTANT_STRING("Allwinner");
				case VendorTelechips:
					return YEP_MAKE_CONSTANT_STRING("Telechips");
				case VendorSTEricson:
					return YEP_MAKE_CONSTANT_STRING("ST-Eriscon");
				case VendorMediaTek:
					return YEP_MAKE_CONSTANT_STRING("MediaTek");
				default:
					return ConstantString();
			}
		}

		ConstantString decodeMachineName(YepCpuMicroarchitecture microarchitecture, Yep32u cores) const {
			/* The name is not available, but probably we could reconstruct it from other information */
			switch (this->machine) {
				case MachineRK28Board:
					/* Could be one of the following (all ARM9)
					 *   Rockchip RK2806 (600 MHz)
					 *   Rockchip RK2808A (560 MHz)
					 *   Rockchip Rockchip 2818 (640 MHz)
					 */
					if (microarchitecture == YepCpuMicroarchitectureARM9) {
						return YEP_MAKE_CONSTANT_STRING("RK28xx");
					}
					break;
				case MachineRK29Board:
					/* Could be one of the following:
					 *   Rockchip RK2918 (1.0-1.2 GHz, Cortex-A8)
					 *   Rockchip RK2928 (up to 1.2 GHz, Cortex-A9, single-core)
					 */
					if (microarchitecture == YepCpuMicroarchitectureCortexA8) {
						return YEP_MAKE_CONSTANT_STRING("RK2918");
					} else if ((microarchitecture == YepCpuMicroarchitectureCortexA9) && (cores == 1)) {
						return YEP_MAKE_CONSTANT_STRING("RK2928");
					}
					break;
				case MachineRK30Board:
					/* Currently only one model: Rockchip RK3066 (up to 1.6 GHz, Cortex-A9, dual-core) */
					if ((microarchitecture == YepCpuMicroarchitectureCortexA9) && (cores == 2)) {
						return YEP_MAKE_CONSTANT_STRING("RK3066");
					}
					break;
				case KernelLogInfo::MachineRK31Board:
					/* Currently only one model: Rockchip RK3188 (up to 1.8 GHz, Cortex-A9, quad-core) */
					if ((microarchitecture == YepCpuMicroarchitectureCortexA9) && (cores == 2)) {
						return YEP_MAKE_CONSTANT_STRING("RK3188");
					}
					break;
				case KernelLogInfo::MachineSun3i:
					/* Only one model: Boxchip F20 (ARM926-EJS) */
					if (microarchitecture == YepCpuMicroarchitectureARM9) {
						return YEP_MAKE_CONSTANT_STRING("F20");
					}
					break;
				case KernelLogInfo::MachineSun4i:
					/* Only one model: Allwinner A10 (up to 1.0 GHz, Cortex-A8) */
					if (microarchitecture == YepCpuMicroarchitectureCortexA8) {
						return YEP_MAKE_CONSTANT_STRING("A10");
					}
					break;
				case KernelLogInfo::MachineSun5i:
					/* Currently two models: Allwinner A10s/A13 (up to 1.0 GHz, Cortex-A8, no difference between the two) */
					if (microarchitecture == YepCpuMicroarchitectureCortexA8) {
						return YEP_MAKE_CONSTANT_STRING("A13");
					}
					break;
				case MachineSun6i:
					/* Currently one model: Allwinner A31 (Cortex-A7, quad-core) */
					if ((microarchitecture == YepCpuMicroarchitectureCortexA7) && (cores == 4)) {
						return YEP_MAKE_CONSTANT_STRING("A31");
					}
					break;
				case MachineSun7i:
					/* Currently one model: Allwinner A20 (Cortex-A7, dual-core */
					if ((microarchitecture == YepCpuMicroarchitectureCortexA7) && (cores == 2)) {
						return YEP_MAKE_CONSTANT_STRING("A20");
					}
					break;
			}
			return ConstantString();
		}
		
		Vendor vendor;
		Machine machine;
		
		char nameBuffer[128];
		YepSize nameLength;
		Yep32u id;
	};
	
	struct ParserState {
		enum FirstWord {
			FirstWordUnknown,
			FirstWordCPU,
			FirstWordMachine,
			FirstWordTegra
		};
		
		ParserState() :
			firstWord(FirstWordUnknown),
			previousWords_CPU_is(false)
		{
		}
		
		FirstWord firstWord;
		YepBoolean previousWords_CPU_is;
	};
	
	static YEP_INLINE YepBoolean isSpace(char c) {
		switch (c) {
			case ' ':
			case '\t':
				return true;
			default:
				return false;
		}
	}
	
	static YEP_INLINE YepBoolean isDigit(char c) {
		return Yep32u(c - '0') < 10u;
	}
	
	static YEP_INLINE YepBoolean isWordCPU(const char* wordStart, YepSize wordLength) {
		if (wordLength == 3) {
			return memcmp(wordStart, "CPU", 3) == 0;
		} else {
			return false;
		}
	}
	
	static YEP_INLINE YepBoolean isWordOMAPModel(const char* wordStart, YepSize wordLength) {
		if (wordLength >= 7u) {
			return memcmp(wordStart,  "OMAP", 4) == 0;
		} else {
			return false;
		}
	}
	
	static YEP_INLINE YepBoolean isWordTegra(const char* wordStart, YepSize wordLength) {
		if (wordLength == 5u) {
			return memcmp(wordStart, "Tegra", 5) == 0;
		} else {
			return false;
		}
	}
	
	static YEP_INLINE YepBoolean isWordIs(const char* wordStart, YepSize wordLength) {
		if (wordLength == 2u) {
			const YepBoolean p0 = (wordStart[0] == 'i');
			const YepBoolean p1 = (wordStart[1] == 's');
			return p0 && p1;
		} else {
			return false;
		}
	}
	
	static YEP_INLINE YepBoolean isWordExynosModel(const char* wordStart, YepSize wordLength) {
		if (wordLength > 6u) {
			return memcmp(wordStart, "EXYNOS", 6) == 0;
		} else {
			return false;
		}
	}
	
	static YEP_INLINE YepBoolean isWordSxModel(const char* wordStart, YepSize wordLength) {
		if (wordLength > 6u) {
			const YepBoolean p0 = (wordStart[0] == 'S');
			const YepBoolean p1a = (wordStart[1] == '3');
			const YepBoolean p1b = (wordStart[1] == '5');
			return p0 && (p1a || p1b);
		} else {
			return false;
		}
	}
	
	static YepBoolean parseKernelLogWord(const char* wordStart, const char* wordEnd, KernelLogInfo& logInfo, ParserState& parserState) {
		const YepSize wordLength = wordEnd - wordStart;
		switch (parserState.firstWord) {
			/* Parsing the first word */
			case ParserState::FirstWordUnknown:
				if (isWordCPU(wordStart, wordLength)) {
					parserState.firstWord = ParserState::FirstWordCPU;
					return true;
				} else if (isWordTegra(wordStart, wordLength)) {
					parserState.firstWord = ParserState::FirstWordTegra;
					return true;
				} else if (isWordOMAPModel(wordStart, wordLength)) {
					logInfo.vendor = KernelLogInfo::VendorTexasInstruments;
					const char* const modelStart = wordStart + 4;
					const char* modelEnd = modelStart;
					while (modelEnd != wordEnd) {
						if (isDigit(*modelEnd)) {
							modelEnd++;
						} else {
							break;
						}
					}
					const YepSize modelDigits = modelEnd - modelStart;
					if ((modelDigits == 3) || (modelDigits == 4)) {
						/* Copy the "OMAP" string from the beginning of the word. */
						memcpy(logInfo.nameBuffer, wordStart, 4);
						/* Insert space before the model number. */
						logInfo.nameBuffer[4] = ' ';
						/* Copy 3- or 4-digit model number. */
						memcpy(logInfo.nameBuffer + 5, modelStart, modelDigits);
						logInfo.nameLength = 5 + modelDigits;
					}
					return false;
				} else if ((wordLength == 8) && YEP_LIKELY(memcmp(wordStart, "Machine:", wordLength) == 0)) {
					parserState.firstWord = ParserState::FirstWordMachine;
					return true;
				} else {
					return false;
				}
			case ParserState::FirstWordMachine:
				if (wordLength == 5) {
					/* Boxchip F20 and Allwinner have "sunXi" machine names, e.g.
					 *    "sun3i" -> Boxchip F20
					 *    "sun4i" -> Allwinner A10
					 */
					if ((memcmp(wordStart, "sun", 3) == 0) && (wordStart[4] == 'i')) {
						switch (wordStart[3]) {
							case '3':
								logInfo.vendor = KernelLogInfo::VendorBoxchip;
								logInfo.machine = KernelLogInfo::MachineSun3i;
								break;
							case '4':
								logInfo.vendor = KernelLogInfo::VendorAllwinner;
								logInfo.machine = KernelLogInfo::MachineSun4i;
								break;
							case '5':
								logInfo.vendor = KernelLogInfo::VendorAllwinner;
								logInfo.machine = KernelLogInfo::MachineSun5i;
								break;
							case '6':
								logInfo.vendor = KernelLogInfo::VendorAllwinner;
								logInfo.machine = KernelLogInfo::MachineSun6i;
								break;
							case '7':
								logInfo.vendor = KernelLogInfo::VendorAllwinner;
								logInfo.machine = KernelLogInfo::MachineSun7i;
								break;
							case '8':
							case '9':
								logInfo.vendor = KernelLogInfo::VendorAllwinner;
								break;
						}
					}
				} else if (wordLength == 7) {
					/* Rockchip SoCs have RK##board machine names:
					 *    "RK28board"
					 *    "RK29board"
					 *    "RK30board"
					 *    "RK31board"
					 */
					const YepBoolean p0 = (wordStart[0] == 'R');
					const YepBoolean p1 = (wordStart[1] == 'K');
					const YepBoolean p2 = isDigit(wordStart[2]);
					const YepBoolean p3 = isDigit(wordStart[3]);
					const YepBoolean p4 = (memcmp(&wordStart[4], "board", 5) == 0);
					if (p0 && p1 && p2 && p3 && p4) {
						logInfo.vendor = KernelLogInfo::VendorRockchip;
						const Yep32u boardModel = (Yep32u(Yep8u(wordStart[2])) << 8) | Yep32u(Yep8u(wordStart[3]));
						switch (boardModel) {
							case (('2' << 8) + '8'):
								logInfo.machine = KernelLogInfo::MachineRK28Board; break;
							case (('2' << 8) + '9'):
								logInfo.machine = KernelLogInfo::MachineRK29Board; break;
							case (('3' << 8) + '0'):
								logInfo.machine = KernelLogInfo::MachineRK30Board; break;
							case (('3' << 8) + '1'):
								logInfo.machine = KernelLogInfo::MachineRK31Board; break;
						}
					}
				}
				return false;
			case ParserState::FirstWordCPU:
				if (parserState.previousWords_CPU_is) {
					/* FreeScale SoCs report "CPU is <Model>", e.g.
					 *    "CPU is i.MX50 Revision 1.1"
					 *    "CPU is i.MX53 Revision 2.1"
					 */
					if (wordLength > 4) {
						if (memcmp(wordStart, "i.MX", 4) == 0) {
							logInfo.vendor = KernelLogInfo::VendorFreeScale;
							/* Copy "i.MX" string from the beginning of the word. */
							memcpy(logInfo.nameBuffer, wordStart, 4);
							/* Insert space before the model number. */
							logInfo.nameBuffer[4] = ' ';
							/* Copy the model number. */
							memcpy(logInfo.nameBuffer + 5, wordStart + 4, wordLength - 4);
							logInfo.nameLength = wordLength + 1;
						}
					}
				} else {
					if (isWordIs(wordStart, wordLength)) {
						parserState.previousWords_CPU_is = true;
						return true;
					} else if (isWordExynosModel(wordStart, wordLength)) {
						logInfo.vendor = KernelLogInfo::VendorSamsung;
						/* Copy "Exynos" string into the name buffer. Note that in log file this string is uppercased, i.e. "EXYNOS". */
						memcpy(logInfo.nameBuffer, "Exynos", 6);
						/* Insert space before the model number. */
						logInfo.nameBuffer[6] = ' ';
						/* Copy the model number. */
						memcpy(logInfo.nameBuffer + 7, wordStart + 6, wordLength - 6);
						logInfo.nameLength = wordLength + 1;
					} else if (isWordSxModel(wordStart, wordLength)) {
						logInfo.vendor = KernelLogInfo::VendorSamsung;
						/* For Samsung S3xxxxx or S5xxxxx, copy the model name as is. */
						memcpy(logInfo.nameBuffer, wordStart, wordLength);
						logInfo.nameLength = wordLength;
					}
				}
				return false;
			default:
				return false;
		}
	}

	static void parseKernelLogLine(const char* lineStart, const char* lineEnd, void* state) {
		KernelLogInfo* logInfo = static_cast<KernelLogInfo*>(state);
		ParserState parserState;

		if (lineStart != lineEnd) {
			/* Skip the log level */
			if (*lineStart == '<') {
				lineStart++;
				while (*lineStart++ != '>') {
					if YEP_UNLIKELY(lineStart == lineEnd) {
						return;
					}
				}
			}
			/* Skip the timestamp */
			if (*lineStart == '[') {
				lineStart++;
				while (*lineStart++ != ']') {
					if YEP_UNLIKELY(lineStart == lineEnd) {
						return;
					}
				}
			}
			/* Skip the whitespace before the message */
			while (isSpace(*lineStart)) {
				lineStart++;
				if YEP_UNLIKELY(lineStart == lineEnd) {
					return;
				}
			}

			/* Iterate through all words untill the end of the line or until the word parser will tell to stop. */
			{
				bool isWord = false;
				const char* wordStart;
				for (const char* currentCharacter = lineStart; currentCharacter != lineEnd; currentCharacter++) {
					const char character = *currentCharacter;
					if (isSpace(character)) {
						if (isWord) {
							isWord = false;
							if (!parseKernelLogWord(wordStart, currentCharacter, *logInfo, parserState)) {
								break;
							}
						}
					} else {
						if (!isWord) {
							isWord = true;
							wordStart = currentCharacter;
						}
					}
				}
				if (isWord) {
					parseKernelLogWord(wordStart, lineEnd, *logInfo, parserState);
				}
			}
		}
	}

	static ConstantString getNameFromIsa(Yep32u isaFeatures) {
		if (isaFeatures & YepARMIsaFeatureV7) {
			return YEP_MAKE_CONSTANT_STRING("ARMv7-A compatible");
		} else if (isaFeatures & YepARMIsaFeatureV6K) {
			return YEP_MAKE_CONSTANT_STRING("ARMv6K compatible");
		} else if (isaFeatures & YepARMIsaFeatureV6) {
			return YEP_MAKE_CONSTANT_STRING("ARMv6 compatible");
		} else if (isaFeatures & YepARMIsaFeatureV5) {
			if (isaFeatures & YepARMIsaFeatureThumb) {
				if (isaFeatures & YepARMIsaFeatureV5E) {
					if (isaFeatures & YepARMIsaFeatureJazelle) {
						return YEP_MAKE_CONSTANT_STRING("ARMv5TEJ compatible");
					} else {
						return YEP_MAKE_CONSTANT_STRING("ARMv5TE compatible");
					}
				} else {
					return YEP_MAKE_CONSTANT_STRING("ARMv5T compatible");
				}
			} else {
				return YEP_MAKE_CONSTANT_STRING("ARMv5 compatible");
			}
		} else {
			if (isaFeatures & YepARMIsaFeatureThumb2) {
				return YEP_MAKE_CONSTANT_STRING("ARMv4T compatible");
			} else {
				return YEP_MAKE_CONSTANT_STRING("ARMv4 compatible");
			}
		}
	}

	static void initCpuName(ConstantString& briefCpuName, ConstantString& fullCpuName, YepCpuVendor vendor, YepCpuMicroarchitecture microarchitecture, Yep32u cores, Yep64u isaFeatures) {
		static char cpuNameBuffer[128];
		YepSize cpuNameBufferSpace = 128;
		YepSize briefNameOffset;

		KernelLogInfo logInfo;
		YepStatus status = _yepLibrary_ParseKernelLog(parseKernelLogLine, &logInfo);
		briefNameOffset = 0;

		if (status == YepStatusOk) {
			const ConstantString vendorString = logInfo.getVendorString();
			/* SoC vendor is always known if its model it known */
			if (!vendorString.isEmpty()) {
				/* Check that it fits into the buffer */
				if (vendorString.length <= cpuNameBufferSpace) {
					memcpy(cpuNameBuffer, vendorString.pointer, vendorString.length);
					cpuNameBuffer[vendorString.length] = ' ';
					briefNameOffset = vendorString.length + 1;
					cpuNameBufferSpace -= briefNameOffset;

					/* The name was retrieved from the kernel log */
					if (logInfo.nameLength != 0) {
						/* Make sure no buffer overflow is possible */
						if (logInfo.nameLength <= cpuNameBufferSpace) {
							memcpy(cpuNameBuffer + briefNameOffset, logInfo.nameBuffer, logInfo.nameLength);
							briefCpuName = ConstantString(cpuNameBuffer + briefNameOffset, logInfo.nameLength);
							fullCpuName = ConstantString(cpuNameBuffer, briefNameOffset + logInfo.nameLength);
							return;
						}
					} else {
						/* Try to recover the CPU name from machine name */
						const ConstantString processorString = logInfo.decodeMachineName(microarchitecture, cores);
						if (!processorString.isEmpty()) {
							/* Ok, we got something useful. Make sure it fits. */
							if (processorString.length < cpuNameBufferSpace) {
								memcpy(cpuNameBuffer + briefNameOffset, processorString.pointer, processorString.length);
								briefCpuName = ConstantString(cpuNameBuffer + briefNameOffset, processorString.length);
								fullCpuName = ConstantString(cpuNameBuffer, briefNameOffset + processorString.length);
								return;
							}
						}
					}
				}
			}
		}
		/* If the execution reached this line, we could not retrieve full SoC name, but probably have SoC vendor name. */

		YepBoolean isVendorArm = false;
		/* Check if the SoC vendor name was retrieved. */
		if (briefNameOffset == 0) {
			/* Could not determine the SoC vendor. Use the microarchitecture vendor instead. */
			if (vendor != YepCpuVendorUnknown) {
				const ConstantString vendorString = _yepLibrary_GetCpuVendorDescription(vendor);
				if (!vendorString.isEmpty()) {
					/* Check for buffer overflow */
					if (vendorString.length <= cpuNameBufferSpace) {
						memcpy(cpuNameBuffer, vendorString.pointer, vendorString.length);
						cpuNameBuffer[vendorString.length] = ' ';
						briefNameOffset = vendorString.length + 1;
						cpuNameBufferSpace -= briefNameOffset;
						isVendorArm = (vendor == YepCpuVendorARM);
					}
				}
			}
		}

		/* At this stage we did everything possible to get some vendor name, and have no SoC name. */
		const ConstantString microarchitectureString = _yepLibrary_GetCpuMicroarchitectureDescription(microarchitecture);
		if (microarchitectureString.length <= cpuNameBufferSpace) {
			memcpy(cpuNameBuffer + briefNameOffset, microarchitectureString.pointer, microarchitectureString.length);
			cpuNameBufferSpace -= microarchitectureString.length;
			/* Add " based" in the end to get something like "Samsung Cortex-A15 based" */
			const ConstantString basedString = YEP_MAKE_CONSTANT_STRING(" based");
			/* Check if it fits */
			if (basedString.length <= cpuNameBufferSpace) {
				memcpy(cpuNameBuffer + briefNameOffset + microarchitectureString.length, basedString.pointer, basedString.length);
				briefCpuName = ConstantString(cpuNameBuffer + briefNameOffset, microarchitectureString.length + basedString.length);
				fullCpuName = ConstantString(cpuNameBuffer, briefNameOffset + microarchitectureString.length + basedString.length);
				return;
			} else {
				/* If does not fit, then leave the name as is, i.e. "Samsung Cortex-A15". Still better than nothing. */
				briefCpuName = ConstantString(cpuNameBuffer + briefNameOffset, microarchitectureString.length);
				fullCpuName = ConstantString(cpuNameBuffer, briefNameOffset + microarchitectureString.length);
				return;
			}
		}

		/* At this stage nothing has worked. Probably we have vendor string */
		if (isVendorArm) {
			/* Avoid output like "ARM ARMv7-A compatible". Instead output just "ARMv7-A compatible". */
			cpuNameBufferSpace += briefNameOffset;
			briefNameOffset = 0;
		}
		const ConstantString processorString = getNameFromIsa(isaFeatures);
		/* Check if the string fits after the vendor name */
		if (processorString.length <= cpuNameBufferSpace) {
			memcpy(cpuNameBuffer + briefNameOffset, processorString.pointer, processorString.length);
			briefCpuName = ConstantString(cpuNameBuffer + briefNameOffset, processorString.length);
			fullCpuName = ConstantString(cpuNameBuffer, briefNameOffset + processorString.length);
			return;
		} else {
			/* Never give up: return name without vendor */
			briefCpuName = processorString;
			fullCpuName = processorString;
			return;
		}
	}
#endif

#if defined(YEP_WINDOWS_OS)
	YepStatus _yepLibrary_InitCpuInfo() {
		_yepLibrary_InitWindowsLogicalCoresCount();
		_yepLibrary_InitWindowsARMCpuIsaInfo();
		_yepLibrary_InitWindowsARMMicroarchitectureInfo();
		YepStatus status = _yepLibrary_InitWindowsARMCacheInfo();
		_dispatchList = _yepLibrary_GetMicroarchitectureDispatchList(_microarchitecture);
		return status;
	}
#elif defined(YEP_LINUX_OS)
	YepStatus _yepLibrary_InitCpuInfo() {
		_yepLibrary_InitLinuxLogicalCoresCount(_logicalCoresCount, _systemFeatures);
		ProcCpuInfo cpuInfo;
		const YepStatus status = _yepLibrary_ParseProcCpuInfo(parseCpuInfoLine, &cpuInfo);
		if YEP_LIKELY(status == YepStatusOk) {
			decodeMicroarchitecture(cpuInfo, _vendor, _microarchitecture);
			decodeIsaFeatures(cpuInfo, _vendor, _microarchitecture, _isaFeatures, _simdFeatures, _systemFeatures);
			decodeCacheInfo(cpuInfo, _microarchitecture, _logicalCoresCount, _cache);
		}
		const YepStatus perfEventSupportStatus = _yepLibrary_DetectLinuxPerfEventSupport(_systemFeatures);
		if YEP_LIKELY(perfEventSupportStatus != YepStatusOk) {
			_yepLibrary_DetectLinuxARMCycleCounterAccess(_systemFeatures);
		}

		initCpuName(_briefCpuName, _fullCpuName, _vendor, _microarchitecture, _logicalCoresCount, _isaFeatures);

		_dispatchList = _yepLibrary_GetMicroarchitectureDispatchList(_microarchitecture);
		return status;
	}
#else
	#error "The target Operating System is not supported yet"
#endif
