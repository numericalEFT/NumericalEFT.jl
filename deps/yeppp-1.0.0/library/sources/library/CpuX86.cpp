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

#if defined(YEP_X86_CPU)
	static void getCpuidMaxIndex(Yep32u& maxBaseCpuidIndex, Yep32u& maxExtendedCpuidIndex) {
		int registers[4];
		__cpuid(registers, 0u);
		maxBaseCpuidIndex = registers[0];
		
		__cpuid(registers, 0x80000000u);
		if (registers[0] >= 0x80000000u) {
			maxExtendedCpuidIndex = registers[0];
		} else {
			maxExtendedCpuidIndex = 0u;
		}
	}

	static void initVendor(YepCpuVendor& vendor) {
		/// Intel vendor string: "GenuineIntel"
		const Yep32u Genu = 0x756E6547U;
		const Yep32u ineI = 0x49656E69U;
		const Yep32u ntel = 0x6C65746EU;
		/// AMD vendor strings: "AuthenticAMD", "AMDisbetter!", "AMD ISBETTER"
		const Yep32u Auth = 0x68747541U;
		const Yep32u enti = 0x69746E65U;
		const Yep32u cAMD = 0x444D4163U;
		const Yep32u AMDi = 0x69444D41U;
		const Yep32u sbet = 0x74656273U;
		const Yep32u ter = 0x21726574U;
		const Yep32u AMD = 0x20444D41U;
		const Yep32u ISBE = 0x45425349U;
		const Yep32u TTER = 0x52455454U;
		/// VIA (Centaur) vendor strings: "CentaurHauls", "VIA VIA VIA "
		const Yep32u Cent = 0x746E6543U;
		const Yep32u aurH = 0x48727561U;
		const Yep32u auls = 0x736C7561U;
		const Yep32u VIA = 0x20414956U;
		/// Transmeta vendor strings: "GenuineTMx86", "TransmetaCPU"
		const Yep32u ineT = 0x54656E69U;
		const Yep32u Mx86 = 0x3638784DU;
		const Yep32u Tran = 0x6E617254U;
		const Yep32u smet = 0x74656D73U;
		const Yep32u aCPU = 0x55504361U;
		/// Cyrix vendor string: "CyrixInstead"
		const Yep32u Cyri = 0x69727943U;
		const Yep32u xIns = 0x736E4978U;
		const Yep32u tead = 0x64616574U;
		/// Rise vendor string: "RiseRiseRise"
		const Yep32u Rise = 0x65736952U;
		/// NSC vendor string: "Geode by NSC"
		const Yep32u Geod = 0x646F6547U;
		const Yep32u e_by = 0x79622065U;
		const Yep32u NSC = 0x43534E20U;
		/// SiS vendor string: "SiS SiS SiS "
		const Yep32u SiS = 0x20536953U;
		/// NexGen vendor string: "NexGenDriven"
		const Yep32u NexG = 0x4778654EU;
		const Yep32u enDr = 0x72446E65U;
		const Yep32u iven = 0x6E657669U;
		/// UMC vendor string: "UMC UMC UMC "
		const Yep32u UMC = 0x20434D55U;
		/// RDC vendor string: "Genuine  RDC"
		const Yep32u ine = 0x20656E69U;
		const Yep32u RDC = 0x43445220U;
		/// D&MP vendor string: "Vortex86 SoC"
		const Yep32u Vort = 0x74726F56U;
		const Yep32u ex86 = 0x36387865U;
		const Yep32u SoC = 0x436F5320U;
		Yep32u vendorString0, vendorString1, vendorString2;
		{
			int registers[4];
			__cpuid(registers, 0);
			vendorString0 = registers[1];
			vendorString1 = registers[3];
			vendorString2 = registers[2];
		}
		if( (vendorString0 == Genu) && (vendorString1 == ineI) && (vendorString2 == ntel) ) {
			vendor = YepCpuVendorIntel;
		} else if( (vendorString0 == Auth) && (vendorString1 == enti) && (vendorString2 == cAMD) ) {
			vendor = YepCpuVendorAMD;
		} else if( (vendorString0 == AMDi) && (vendorString1 == sbet) && (vendorString2 == ter) ) {
			vendor = YepCpuVendorAMD;
		} else if( (vendorString0 == AMD) && (vendorString1 == ISBE) && (vendorString2 == TTER) ) {
			vendor = YepCpuVendorAMD;
		} else if( (vendorString0 == Cent) && (vendorString1 == aurH) && (vendorString2 == auls) ) {
			vendor = YepCpuVendorVIA;
		} else if( (vendorString0 == VIA) && (vendorString1 == VIA) && (vendorString2 == VIA) ) {
			vendor = YepCpuVendorVIA;
		} else if( (vendorString0 == Genu) && (vendorString1 == ineT) && (vendorString2 == Mx86) ) {
			vendor = YepCpuVendorTransmeta;
		} else if( (vendorString0 == Tran) && (vendorString1 == smet) && (vendorString2 == aCPU) ) {
			vendor = YepCpuVendorTransmeta;
		} else if( (vendorString0 == Cyri) && (vendorString1 == xIns) && (vendorString2 == tead) ) {
			vendor = YepCpuVendorCyrix;
		} else if( (vendorString0 == Rise) && (vendorString1 == Rise) && (vendorString2 == Rise) ) {
			vendor = YepCpuVendorRise;
		} else if( (vendorString0 == Geod) && (vendorString1 == e_by) && (vendorString2 == NSC) ) {
			vendor = YepCpuVendorNSC;
		} else if( (vendorString0 == SiS) && (vendorString1 == SiS) && (vendorString2 == SiS) ) {
			vendor = YepCpuVendorSiS;
		} else if( (vendorString0 == NexG) && (vendorString1 == enDr) && (vendorString2 == iven) ) {
			vendor = YepCpuVendorNexGen;
		} else if( (vendorString0 == UMC) && (vendorString1 == UMC) && (vendorString2 == UMC) ) {
			vendor = YepCpuVendorUMC;
		} else if( (vendorString0 == Genu) && (vendorString1 == ine) && (vendorString2 == RDC) ) {
			vendor = YepCpuVendorRDC;
		} else if( (vendorString0 == Vort) && (vendorString1 == ex86) && (vendorString2 == SoC) ) {
			vendor = YepCpuVendorDMP;
		} else {
			vendor = YepCpuVendorUnknown;
		}
	}

	static void initModelInfo(ModelInfo& modelInfo, Yep32u maxBaseCpuidIndex) {
		if (maxBaseCpuidIndex >= 1u) {
			int registers[4] = { 0, 0, 0, 0 };
			__cpuid(registers, 1);
			modelInfo.stepping = registers[0] & 0xF;
			modelInfo.baseModel = (registers[0] >> 4) & 0xF;
			modelInfo.baseFamily = (registers[0] >> 8) & 0xF;
			modelInfo.processorType = (registers[0] >> 12) & 0x3;
			modelInfo.extModel = (registers[0] >> 16) & 0xF;
			modelInfo.extFamily = (registers[0] >> 20) & 0xFF;

			modelInfo.family = modelInfo.baseFamily + modelInfo.extFamily;
			modelInfo.model = modelInfo.baseModel + modelInfo.extModel * 16;
		}
	}

	static void initMicroarchitecture(YepCpuMicroarchitecture& microarchitecture, const ModelInfo& modelInfo, YepCpuVendor vendor) {
		switch (vendor) {
			case YepCpuVendorIntel:
				switch (modelInfo.family) {
	#ifndef YEP_X64_ABI
					case 0x05:
						microarchitecture = YepCpuMicroarchitectureP5;
						break;
	#endif
					case 0x06:
						switch (modelInfo.model) {
	#ifndef YEP_X64_ABI
							case 0x01: // Pentium Pro
							case 0x03: // Pentium II (Klamath) and Pentium II Overdrive
							case 0x05: // Pentium II (Deschutes, Tonga), Pentium II Celeron (Covington), Pentium II Xeon (Drake)
							case 0x06: // Pentium II (Dixon), Pentium II Celeron (Mendocino)
							case 0x07: // Pentium III (Katmai), Pentium III Xeon (Tanner)
							case 0x08: // Pentium III (Coppermine), Pentium II Celeron (Coppermine-128), Pentium III Xeon (Cascades)
							case 0x0A: // Pentium III Xeon (Cascades-2MB)
							case 0x0B: // Pentium III (Tualatin), Pentium III Celeron (Tualatin-256)
								microarchitecture = YepCpuMicroarchitectureP6;
								break;
							case 0x09: // Pentium M (Banias), Pentium M Celeron (Banias-0, Banias-512)
							case 0x0D: // Pentium M (Dothan), Pentium M Celeron (Dothan-512, Dothan-1024)
							case 0x15: // Intel 80579 (Tolapai)
								microarchitecture = YepCpuMicroarchitectureDothan;
								break;
							case 0x0E: // Core Solo/Duo (Yonah), Pentium Dual-Core T2xxx (Yonah), Celeron M (Yonah-512, Yonah-1024), Dual-Core Xeon (Sossaman)
								microarchitecture = YepCpuMicroarchitectureYonah;
								break;
	#endif
							case 0x0F: // Core 2 Duo (Conroe, Conroe-2M, Merom), Core 2 Quad (Tigerton), Xeon (Woodcrest, Clovertown, Kentsfield)
							case 0x16: // Celeron (Conroe-L, Merom-L), Core 2 Duo (Merom)
								microarchitecture = YepCpuMicroarchitectureConroe;
								break;
							case 0x17: // Core 2 Duo (Penryn-3M), Core 2 Quad (Yorkfield), Core 2 Extreme (Yorkfield), Xeon (Harpertown), Pentium Dual-Core (Penryn)
							case 0x1D: // Xeon (Dunnington)
								microarchitecture = YepCpuMicroarchitecturePenryn;
								break;
							case 0x1C: // Diamondville, Silverthorne, Pineview
							case 0x26: // Tunnel Creek
								microarchitecture = YepCpuMicroarchitectureBonnell;
								break;
							case 0x27: // Medfield
							case 0x35: // Cloverview
							case 0x36: // Cedarview, Centerton 
								microarchitecture = YepCpuMicroarchitectureSaltwell;
								break;
							case 0x37:
							case 0x4A:
							case 0x4D:
								microarchitecture = YepCpuMicroarchitectureSilvermont;
								break;
							case 0x1A: // Core iX (Bloomfield), Xeon (Gainestown)
							case 0x1E: // Core iX (Lynnfield, Clarksfield)
							case 0x1F: // Core iX (Havendale)
							case 0x2E: // Xeon (Beckton)
							case 0x25: // Core iX (Clarkdale)
							case 0x2C: // Core iX (Gulftown), Xeon (Gulftown)
							case 0x2F: // Xeon (Eagleton)
								microarchitecture = YepCpuMicroarchitectureNehalem;
								break;
							case 0x2A: // Core iX (Sandy Bridge)
							case 0x2D: // Core iX (Sandy Bridge-E), Xeon (Sandy Bridge EP/EX)
								microarchitecture = YepCpuMicroarchitectureSandyBridge;
								break;
							case 0x3A: // Core iX (Ivy Bridge)
							case 0x3E: // Ivy Bridge-E
								microarchitecture = YepCpuMicroarchitectureIvyBridge;
								break;
							case 0x3C:
							case 0x3F: // Haswell-E
							case 0x45: // Haswell ULT
							case 0x46: // Haswell with eDRAM
								microarchitecture = YepCpuMicroarchitectureHaswell;
								break;
						}
						break;
					case 0x0B:
						switch (modelInfo.model) {
							case 0x01:
								microarchitecture = YepCpuMicroarchitectureKnightsCorner;
								break;
						}
						break;
					case 0x0F:
						switch (modelInfo.model) {
							case 0x00: // Pentium 4 Xeon (Foster)
							case 0x01: // Pentium 4 Celeron (Willamette-128), Pentium 4 Xeon (Foster, Foster MP)
							case 0x02: // Pentium 4 (Northwood), Pentium 4 EE (Gallatin), Pentium 4 Celeron (Northwood-128, Northwood-256), Pentium 4 Xeon (Gallatin DP, Prestonia)
								microarchitecture = YepCpuMicroarchitectureWillamette;
								break;
							case 0x03: // Pentium 4 (Prescott), Pentium 4 Xeon (Nocona)
							case 0x04: // Pentium 4 (Prescott-2M), Pentium 4 EE (Prescott-2M), Pentium D (Smithfield), Celeron D (Prescott-256), Pentium 4 Xeon (Cranford, Irwindale, Paxville)
							case 0x06: // Pentium 4 (Cedar Mill), Pentium D EE (Presler), Celeron D (Cedar Mill), Pentium 4 Xeon (Dempsey, Tulsa)
								microarchitecture = YepCpuMicroarchitecturePrescott;
								break;
						}
						break;
				}
				break;
			case YepCpuVendorAMD:
				switch (modelInfo.family) {
	#ifndef YEP_X64_ABI
					case 0x5:
						switch (modelInfo.model) {
							case 0x00:
							case 0x01:
							case 0x02:
								microarchitecture = YepCpuMicroarchitectureK5;
								break;
							case 0x06:
							case 0x07:
							case 0x08:
							case 0x0D:
								microarchitecture = YepCpuMicroarchitectureK6;
								break;
							case 0x0A:
								microarchitecture = YepCpuMicroarchitectureGeode;
								break;
						}
						break;
					case 0x6:
						microarchitecture = YepCpuMicroarchitectureK7;
						break;
	#endif
					case 0xF: // Opteron, Athlon 64, Sempron
					case (0xF + 0x02): // Turion
						microarchitecture = YepCpuMicroarchitectureK8;
						break;
					case (0xF + 0x01): // Opteron, Phenom, Athlon, Sempron
					case (0xF + 0x03): // Llano APU
						microarchitecture = YepCpuMicroarchitectureK10;
						break;
					case (0xF + 0x05):
						microarchitecture = YepCpuMicroarchitectureBobcat;
						break;
					case (0xF + 0x06):
						switch (modelInfo.model) {
							case 0x00: // Engineering samples
							case 0x01: // Zambezi, Interlagos
								microarchitecture = YepCpuMicroarchitectureBulldozer;
								break;
							case 0x02: // Vishera
							case 0x10: // Trinity
							case 0x13: // Richland
								microarchitecture = YepCpuMicroarchitecturePiledriver;
								break;
							default:
								switch (modelInfo.extModel) {
									case 0x0:
										microarchitecture = YepCpuMicroarchitectureBulldozer;
										break;
									case 0x1: // No L3 cache
									case 0x2: // With L3 cache
										microarchitecture = YepCpuMicroarchitecturePiledriver;
										break;
									case 0x3: // With L3 cache
									case 0x4: // No L3 cache
										microarchitecture = YepCpuMicroarchitectureSteamroller;
										break;
								}
								break;
						}
						break;
					case (0xF + 0x07):
						microarchitecture = YepCpuMicroarchitectureJaguar;
						break;
				}
				break;
			default:
				break;
		}
	}

	static void initIsaInfo(Yep64u& isaFeatures, Yep64u& simdFeatures, Yep64u& systemFeatures, YepCpuVendor vendor, YepCpuMicroarchitecture microarchitecture, Yep32u maxBaseCpuidIndex, Yep32u maxExtendedCpuidIndex) {
		// Processors without CPUID instruction are not supported
		isaFeatures |= YepX86IsaFeatureCpuid;
		// All x86 and x86-64 processors support misaligned memory access
		systemFeatures |= YepSystemFeatureMisalignedAccess;
	#if defined(YEP_X64_ABI)
		systemFeatures |= YepSystemFeatureAddressSpace64Bit;
		systemFeatures |= YepSystemFeatureGPRegisters64Bit;
	#endif
		
		#if defined(YEP_K1OM_X64_ABI)
			simdFeatures |= YepX86SimdFeatureKNC;
			systemFeatures |= YepX86SystemFeatureZMM;
		#endif

		int basicInfo[4] = { 0, 0, 0, 0 };
		if YEP_LIKELY(maxBaseCpuidIndex >= 1u) {
			__cpuid(basicInfo, 1);
		}
		int structuredFeatureInfo[4] = { 0, 0, 0, 0 };
		if YEP_LIKELY(maxBaseCpuidIndex >= 7u) {
			__cpuidex(structuredFeatureInfo, 7, 0);
		}
		int extendedInfo[4] = { 0, 0, 0, 0 };
		if YEP_LIKELY(maxExtendedCpuidIndex >= 0x80000001u) {
			__cpuid(extendedInfo, 0x80000001);
		}

		// Check for RDTSC instruction.
	#if defined(YEP_X86_ABI)
		// Intel, AMD: edx[bit 4] in basic info.
		// AMD: edx [bit 4] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00000010u) {
			isaFeatures |= YepX86IsaFeatureRdtsc;
			systemFeatures |= YepSystemFeatureCycleCounter;
			systemFeatures |= YepSystemFeatureCycleCounter64Bit;
		}
	#else
		isaFeatures |= YepX86IsaFeatureRdtsc;
		systemFeatures |= YepSystemFeatureCycleCounter;
		systemFeatures |= YepSystemFeatureCycleCounter64Bit;
	#endif

		// Intel, AMD: edx[bit 11] in basic info.
		if YEP_LIKELY(basicInfo[3] & 0x00000800u) {
			isaFeatures |= YepX86IsaFeatureSYSENTER;
		}
		// Intel, AMD: edx[bit 11] in extended info.
		if YEP_LIKELY(extendedInfo[3] & 0x00000800u) {
			isaFeatures |= YepX86IsaFeatureSYSCALL;
		}
		// Intel, AMD: edx[bit 5] in basic info.
		// AMD: edx[bit 5] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00000020u) {
			isaFeatures |= YepX86IsaFeatureMSR;
		}
		// Intel, AMD: edx[bit 19] in basic info.
		if YEP_LIKELY(basicInfo[3] & 0x00080000u) {
			isaFeatures |= YepX86IsaFeatureClflush;
		}
		// Intel, AMD: ecx[bit 3] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00000008u) {
			isaFeatures |= YepX86IsaFeatureMONITOR;
		}
		// Intel, AMD: edx[bit 24] in basic info.
		// AMD: edx[bit 24] in extended info (zero bit on Intel CPUs, EMMX bit on Cyrix CPUs).
		if YEP_LIKELY(basicInfo[3] & 0x01000000u) {
			isaFeatures |= YepX86IsaFeatureFXSAVE;
		}
		if YEP_LIKELY(extendedInfo[3] & 0x01000000u) {
	#if defined(YEP_X86_ABI)
			if YEP_UNLIKELY(vendor == YepCpuVendorCyrix) {
				simdFeatures |= YepX86SimdFeatureEMMX;
			} else {
				isaFeatures |= YepX86IsaFeatureFXSAVE;
			}
	#else
			isaFeatures |= YepX86IsaFeatureFXSAVE;
	#endif
		}
		// Intel, AMD: ecx[bit 26] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x04000000u) {
			isaFeatures |= YepX86IsaFeatureXSAVE;
		}
		// Intel, AMD: edx[bit 0] in basic info.
		// AMD: edx[bit 0] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00000001u) {
			isaFeatures |= YepX86IsaFeatureFPU;
		}
		
		// Intel, AMD: edx[bit 23] in basic info.
		// AMD: edx[bit 23] in extended info (zero bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00800000u) {
			simdFeatures |= YepX86SimdFeatureMMX;
		}
		// Intel, AMD: edx[bit 25] in basic info (SSE feature flag).
		// Pre-SSE AMD: edx[bit 22] in extended info (zero bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] & 0x02000000u) | (extendedInfo[3] & 0x00400000u)) {
			simdFeatures |= YepX86SimdFeatureMMXPlus;
		}

		// AMD: edx[bit 31] of extended info (zero bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[3] & 0x80000000u) {
			simdFeatures |= YepX86SimdFeature3dnow;
		}
		// AMD: edx[bit 30] of extended info (zero bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[3] & 0x40000000u) {
			simdFeatures |= YepX86SimdFeature3dnowPlus;
			if YEP_UNLIKELY(microarchitecture == YepCpuMicroarchitectureGeode) {
				simdFeatures |= YepX86SimdFeature3dnowGeode;
			}
		}
		// AMD, Intel: ecx[bit 8] of extended info (called PREFETCHW in Intel manuals).
		// AMD: edx[bit 31] of extended info (same as for 3dnow!)
		// AMD: edx[bit 30] of extended info (same as for 3dnow!+)
		if YEP_LIKELY((extendedInfo[2] & 0x00000100u) | (extendedInfo[3] & 0xC0000000u)) {
			simdFeatures |= YepX86SimdFeature3dnowPrefetch;
		}
	#if defined(YEP_MICROSOFT_X64_ABI) || defined(YEP_SYSTEMV_X64_ABI)
		isaFeatures |= YepX86IsaFeatureX64;
		simdFeatures |= YepX86SimdFeatureSSE;
		simdFeatures |= YepX86SimdFeatureSSE2;
	#else
		// Intel, AMD: edx[bit 29] in extended info.
		if YEP_LIKELY(basicInfo[3] & 0x20000000u) {
			isaFeatures |= YepX86IsaFeatureX64;
		}
		// Intel, AMD: edx[bit 25] in basic info.
		if YEP_LIKELY(basicInfo[3] & 0x02000000u) {
			simdFeatures |= YepX86SimdFeatureSSE;
		}
		// Intel, AMD: edx[bit 26] in basic info.
		if YEP_LIKELY(basicInfo[3] & 0x04000000u) {
			simdFeatures |= YepX86SimdFeatureSSE2;
		}
	#endif
		// Intel, AMD: ecx[bit 0] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00000001u) {
			simdFeatures |= YepX86SimdFeatureSSE3;
		}
		// Intel, AMD: ecx[bit 9] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x0000200u) {
			simdFeatures |= YepX86SimdFeatureSSSE3;
		}
		// Intel, AMD: ecx[bit 19] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00080000u) {
			simdFeatures |= YepX86SimdFeatureSSE4_1;
		}
		// Intel: ecx[bit 20] in basic info (reserved bit on AMD CPUs).
		if YEP_LIKELY(basicInfo[2] & 0x00100000u) {
			simdFeatures |= YepX86SimdFeatureSSE4_2;
		}
		// AMD: ecx[bit 6] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00000040u) {
			simdFeatures |= YepX86SimdFeatureSSE4A;
		}
		// AMD: ecx[bit 7] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00000080u) {
			systemFeatures |= YepX86SystemFeatureMisalignedSSE;
		}

		// Intel, AMD: ecx[bit 28] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x10000000u) {
			simdFeatures |= YepX86SimdFeatureAVX;
		}
		// Intel: ebx[bit 5] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000020u) {
			simdFeatures |= YepX86SimdFeatureAVX2;
		}
		// Intel: ebx[bit 16] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00010000u) {
			simdFeatures |= YepX86SimdFeatureAVX512F;
		}
		// Intel: ebx[bit 26] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x04000000u) {
			simdFeatures |= YepX86SimdFeatureAVX512PF;
		}
		// Intel: ebx[bit 27] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x08000000u) {
			simdFeatures |= YepX86SimdFeatureAVX512ER;
		}
		// Intel: ebx[bit 28] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x10000000u) {
			simdFeatures |= YepX86SimdFeatureAVX512CD;
		}

		// Intel: ecx[bit 12] in basic info (reserved bit on AMD CPUs).
		if YEP_LIKELY(basicInfo[2] & 0x00001000u) {
			simdFeatures |= YepX86SimdFeatureFMA3;
		}
		// AMD: ecx[bit 16] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00010000u) {
			simdFeatures |= YepX86SimdFeatureFMA4;
		}

		// AMD: ecx[bit 11] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00000800u) {
			simdFeatures |= YepX86SimdFeatureXOP;
		}
		// Intel, AMD: ecx[bit 29] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x20000000u) {
			simdFeatures |= YepX86SimdFeatureF16C;
		}

		// Intel: ebx[bit 4] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000010u) {
			isaFeatures |= YepX86IsaFeatureHLE;
		}
		// Intel: ebx[bit 11] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000800u) {
			isaFeatures |= YepX86IsaFeatureRTM;
		}
		// Intel: either HLE or RTM is supported.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000810u) {
			isaFeatures |= YepX86IsaFeatureXtest;
		}
		// Intel: ebx[bit 14] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00004000u) {
			isaFeatures |= YepX86IsaFeatureMPX;
		}

		// Intel, AMD: ecx[bit 26] in basic info = XSAVE.
		// Intel, AMD: ecx[bit 27] in basic info = OSXSAVE.
		if YEP_LIKELY((basicInfo[2] & 0x0C000000u) == 0x0C000000u) {
			Yep64u xcr0ValidBits = 0ull;
			if YEP_LIKELY(maxBaseCpuidIndex >= 0x0000000Du) {
				int extendedStateEnumerationMainParameters[4];
				__cpuidex(extendedStateEnumerationMainParameters, 0x0000000Du, 0x00000000u);
				xcr0ValidBits = yepBuiltin_CombineParts_32u32u_64u(extendedStateEnumerationMainParameters[3], extendedStateEnumerationMainParameters[0]);
			}

			#if !defined(YEP_K1OM_X64_ABI)
				Yep64u xfeatureEnabledMask = _xgetbv(0);
				// Intel, AMD: XFEATURE_ENABLED_MASK[bit 0] for FPU XSAVE
				if YEP_LIKELY(isaFeatures & YepX86IsaFeatureFPU) {
					if YEP_LIKELY(xcr0ValidBits & 0x0000000000000001ull) {
						if YEP_LIKELY(xfeatureEnabledMask & 0x0000000000000001ull) {
							systemFeatures |= YepX86SystemFeatureFPU;
						}
					} else {
						systemFeatures |= YepX86SystemFeatureFPU;
					}
				}
				// Intel, AMD: XFEATURE_ENABLED_MASK[bit 1] for SSE XSAVE
				if YEP_LIKELY(simdFeatures & YepX86SimdFeatureSSE) {
					if YEP_LIKELY(xcr0ValidBits & 0x0000000000000002ull) {
						if YEP_LIKELY(xfeatureEnabledMask & 0x0000000000000002ull) {
							systemFeatures |= YepX86SystemFeatureXMM;
						}
					} else {
						systemFeatures |= YepX86SystemFeatureXMM;
					}
				}
				// Intel, AMD: XFEATURE_ENABLED_MASK[bit 2] for AVX XSAVE
				if YEP_LIKELY(simdFeatures & YepX86SimdFeatureAVX) {
					if YEP_LIKELY(((xcr0ValidBits & xfeatureEnabledMask) & 0x0000000000000006ull) == 0x0000000000000006ull) {
						systemFeatures |= YepX86SystemFeatureYMM;
					}
				}
				// Intel: XFEATURE_ENABLED_MASK[bit 5] for 8 64-bit OpMask registers (k0-k7)
				// Intel: XFEATURE_ENABLED_MASK[bit 6] for the high 256 bits of the zmm registers zmm0-zmm15
				// Intel: XFEATURE_ENABLED_MASK[bit 7] for the 512-bit wide zmm registers zmm16-zmm31
				if YEP_LIKELY(simdFeatures & YepX86SimdFeatureAVX512F) {
					if YEP_LIKELY(((xcr0ValidBits & xfeatureEnabledMask) & 0x00000000000000E6ull) == 0x00000000000000E6ull) {
						systemFeatures |= YepX86SystemFeatureZMM;
					}
				}
				// Intel: XFEATURE_ENABLED_MASK[bit 3] for BNDREGS
				// Intel: XFEATURE_ENABLED_MASK[bit 4] for BNDCSR
				if YEP_LIKELY(isaFeatures & YepX86IsaFeatureMPX) {
					if YEP_LIKELY(((xcr0ValidBits & xfeatureEnabledMask) & 0x0000000000000018ull) == 0x0000000000000018ull) {
						systemFeatures |= YepX86SystemFeatureBND;
					}
				}
			#endif
		} else {
			// If the OSXSAVE feature is not supported, speculate that OS supports FPU & SSE if the CPU does.
			if YEP_LIKELY(isaFeatures & YepX86IsaFeatureFPU) {
				systemFeatures |= YepX86SystemFeatureFPU;
			}
			if YEP_LIKELY(simdFeatures & YepX86SimdFeatureSSE) {
				systemFeatures |= YepX86SystemFeatureXMM;
			}
		}
		
	#if defined(YEP_MICROSOFT_X64_ABI) || defined(YEP_SYSTEMV_X64_ABI)
		isaFeatures |= YepX86IsaFeatureCMOV;
	#else
		// Intel, AMD: edx[bit 15] in basic info.
		// AMD: edx[bit 15] in extended info (zero bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00008000u) {
			isaFeatures |= YepX86IsaFeatureCMOV;
		}
	#endif

	#if defined(YEP_X64_ABI)
		isaFeatures |= YepX86IsaFeatureCmpxchg8b;
	#else
		// Intel, AMD: edx[bit 8] in basic info.
		// AMD: edx[bit 8] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY((basicInfo[3] | extendedInfo[3]) & 0x00000100u) {
			isaFeatures |= YepX86IsaFeatureCmpxchg8b;
		}
	#endif

		// Intel, AMD: ecx[bit 13] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00002000u) {
			isaFeatures |= YepX86IsaFeatureCmpxchg16b;
		}

		// Intel: ecx[bit 22] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00400000u) {
			isaFeatures |= YepX86IsaFeatureMovbe;
		}

		// Some early x86-64 CPUs lack LAHF & SAHF instructions.
		// A special CPU feature bit must be checked to ensure their availability.
		// Intel, AMD: ecx[bit 0] in extended info.
		if YEP_LIKELY(extendedInfo[2] & 0x00000001u) {
			isaFeatures |= YepX86IsaFeatureLahfSahf64;
		}
		// Intel: ebx[bit 0] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000001U) {
			isaFeatures |= YepX86IsaFeatureFsGsBase;
		}

		// Intel, AMD: ecx[bit 5] in extended info.
		if YEP_LIKELY(extendedInfo[2] & 0x00000020u) {
			isaFeatures |= YepX86IsaFeatureLzcnt;
		}
		// Intel, AMD: ecx[bit 23] in basic info.
		if YEP_LIKELY(basicInfo[2] & 0x00800000u) {
			isaFeatures |= YepX86IsaFeaturePopcnt;
		}
		// AMD: ecx[bit 21] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00200000u) {
			isaFeatures |= YepX86IsaFeatureTBM;
		}
		// Intel, AMD: ebx[bit 3] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000008u) {
			isaFeatures |= YepX86IsaFeatureBMI;
		}
		// Intel: ebx[bit 8] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00000100u) {
			isaFeatures |= YepX86IsaFeatureBMI2;
		}
		// Intel: ebx[bit 19] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00080000u) {
			isaFeatures |= YepX86IsaFeatureADX;
		}

		// Intel: ecx[bit 25] in basic info (reserved bit on AMD CPUs).
		if YEP_LIKELY(basicInfo[2] & 0x02000000u) {
			isaFeatures |= YepX86IsaFeatureAES;
		}
		// Intel: ecx[bit 1] in basic info (reserved bit on AMD CPUs).
		if YEP_LIKELY(basicInfo[2] & 0x00000002u) {
			isaFeatures |= YepX86IsaFeaturePclmulqdq;
		}
		// Intel: ecx[bit 30] in basic info (reserved bit on AMD CPUs).
		if YEP_LIKELY(basicInfo[2] & 0x40000000u) {
			isaFeatures |= YepX86IsaFeatureRdrand;
		}
		// Intel: ebx[bit 18] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x00040000u) {
			isaFeatures |= YepX86IsaFeatureRdseed;
		}
		// Intel: ebx[bit 29] in structured feature info.
		if YEP_LIKELY(structuredFeatureInfo[1] & 0x20000000u) {
			isaFeatures |= YepX86IsaFeatureSHA;
		}

		int padlockInfo[4] = { 0, 0, 0, 0 };
		__cpuid(padlockInfo, 0xC0000000u);
		Yep32u maxPadlockCpuidIndex = padlockInfo[0];
		if YEP_UNLIKELY(maxPadlockCpuidIndex >= 0xC0000001u) {
			__cpuid(padlockInfo, 0xC0000001);

			// VIA: edx[bit 2] in padlock info = RNG exists on chip flag.
			if YEP_LIKELY(padlockInfo[3] & 0x00000004u) {
				isaFeatures |= YepX86IsaFeatureRNG;
				// VIA: edx[bit 3] in padlock info = RNG enabled by OS.
				if YEP_LIKELY(padlockInfo[3] & 0x00000008u) {
					systemFeatures |= YepX86SystemFeatureRNG;
				}
			}
			// VIA: edx[bit 6] in padlock info = ACE exists on chip flag.
			if YEP_LIKELY(padlockInfo[3] & 0x00000040u) {
				isaFeatures |= YepX86IsaFeatureACE;
				// VIA: edx[bit 7] in padlock info = ACE enabled by OS.
				if YEP_LIKELY(padlockInfo[3] & 0x00000080u) {
					systemFeatures |= YepX86SystemFeatureACE;
				}
			}
			// VIA: edx[bit 8] in padlock info = ACE2 exists on chip flag.
			if YEP_LIKELY(padlockInfo[3] & 0x00000100u) {
				isaFeatures |= YepX86IsaFeatureACE2;
				// VIA: edx[bit 9] in padlock info = ACE 2 enabled by OS.
				if YEP_LIKELY(padlockInfo[3] & 0x00000200u) {
					systemFeatures |= YepX86SystemFeatureACE2;
				}
			}
			// VIA: edx[bit 10] in padlock info = PHE exists on chip flag.
			if YEP_LIKELY(padlockInfo[3] & 0x00000400u) {
				isaFeatures |= YepX86IsaFeaturePHE;
				// VIA: edx[bit 11] in padlock info = PHE enabled by OS.
				if YEP_LIKELY(padlockInfo[3] & 0x00000400u) {
					systemFeatures |= YepX86SystemFeaturePHE;
				}
			}
			// VIA: edx[bit 12] in padlock info = PMM exists on chip flag.
			if YEP_LIKELY(padlockInfo[3] & 0x00001000u) {
				isaFeatures |= YepX86IsaFeaturePMM;
				// VIA: edx[bit 13] in padlock info = PMM enabled by OS.
				if YEP_LIKELY(padlockInfo[3] & 0x00002000u) {
					systemFeatures |= YepX86SystemFeaturePMM;
				}
			}
		}

		// AMD: ecx[bit 15] in extended info (reserved bit on Intel CPUs).
		if YEP_LIKELY(extendedInfo[2] & 0x00008000u) {
			isaFeatures |= YepX86IsaFeatureLWP;
		}

		// Intel, AMD: edx[bit 27] in extended info.
		if YEP_LIKELY(extendedInfo[3] & 0x08000000u) {
			isaFeatures |= YepX86IsaFeatureRdtscp;
		}
	}

	static void decodeCacheDescriptor(CacheHierarchyInfo& cache, Yep8u descriptor, const ModelInfo& modelInfo, YepCpuVendor vendor);
	static void decodeDeterministicCacheParameters(CacheHierarchyInfo& cache, Yep32u cacheType, Yep32u cacheLevel, Yep32u cacheSize, Yep32u lineSize, Yep32u associativity);

	static void initCacheInfo(CacheHierarchyInfo& cache, const ModelInfo& modelInfo, YepCpuVendor vendor, Yep32u maxBaseCpuidIndex, Yep32u maxExtendedCpuidIndex) {
		if YEP_LIKELY(maxBaseCpuidIndex >= 1u) {
			int registers[4];
			__cpuid(registers, 1);
			const Yep32u clflushLineSizeInQwords = static_cast<Yep8u>(registers[1] >> 8);
			cache.clflushLineSize = static_cast<Yep16u>( clflushLineSizeInQwords * 8 );
		}
		if YEP_LIKELY(maxBaseCpuidIndex >= 2u) {
			int cacheDescriptors[4];
			__cpuid(cacheDescriptors, 2);
			Yep32u numberOfCpuidIterations = cacheDescriptors[0] & 0xFF;
			cacheDescriptors[0] &= 0xFFFFFF00;
			while (numberOfCpuidIterations != 0) {
				for (Yep32u registerIndex = 0; registerIndex < 4; registerIndex++) {
					if YEP_UNLIKELY(cacheDescriptors[registerIndex] < 0) {
						continue;
					} else {
						for (Yep32u descriptorIndex = 0; descriptorIndex < sizeof(cacheDescriptors[registerIndex]); descriptorIndex++) {
							const Yep8u cacheDescriptor = static_cast<Yep8u>(cacheDescriptors[registerIndex] >> (descriptorIndex * 8));
							decodeCacheDescriptor(cache, cacheDescriptor, modelInfo, vendor);
						}
					}
				}
				if (--numberOfCpuidIterations != 0) {
					__cpuid(cacheDescriptors, 2);
				}
			} ;
		}
		if YEP_LIKELY(maxBaseCpuidIndex >= 4u) {
			for (Yep32s index = 0; index >= 0; index++) {
				int deterministicCacheParameters[4];
				__cpuidex(deterministicCacheParameters, 4, index);
				const Yep32u cacheType = deterministicCacheParameters[0] & 0x1F;
				if YEP_LIKELY(cacheType == 0) {
					break;
				} else {
					const Yep32u cacheLevel = (deterministicCacheParameters[0] >> 5) & 0x7;
					const Yep32u isFullyAssociative = YepBoolean(deterministicCacheParameters[0] & 0x200);
					const Yep32u numberOfSets = 1 + deterministicCacheParameters[2];
					const Yep32u waysOfAssociativity = 1 + (Yep32u(deterministicCacheParameters[1]) >> 22);
					const Yep32u physicalLinePartitions = 1 + ((deterministicCacheParameters[1] >> 12) & 0x3FF);
					const Yep32u lineSize = 1 + (deterministicCacheParameters[1] & 0x0FFF);
					const Yep32u cacheSize = waysOfAssociativity * physicalLinePartitions * lineSize * numberOfSets;
					decodeDeterministicCacheParameters(cache, cacheType, cacheLevel, cacheSize, lineSize, isFullyAssociative ? 0xFFFFFFFFu : waysOfAssociativity);
				}
			}
		}
		if (YEP_LIKELY(maxExtendedCpuidIndex >= 0x80000005u) && YEP_UNLIKELY(_vendor == YepCpuVendorAMD)) {
			int cacheParameters[4];
			__cpuid(cacheParameters, 0x80000005u);
			
			// AMD: ecx[bits 24:31] = L1 data cache size in KB
			cache.L1DCacheInfo.cacheSize = (static_cast<Yep32u>(cacheParameters[2]) >> 24) * 1024;
			// AMD: ecx[bits 16:23] = L1 data cache associativity
			cache.L1DCacheInfo.associativity = static_cast<Yep16s>(static_cast<Yep8s>(cacheParameters[2] >> 16));
			// AMD: ecx[bits 0:7] = L1 data cache line size in bytes
			cache.L1DCacheInfo.lineSize = cacheParameters[2] & 0xFF;
			
			// AMD: edx[bits 24:31] = L1 instruction cache size in KB
			cache.L1ICacheInfo.cacheSize = (static_cast<Yep32u>(cacheParameters[3]) >> 24) * 1024;
			// AMD: edx[bits 16:23] = L1 instruction cache associativity
			cache.L1ICacheInfo.associativity = static_cast<Yep16s>(static_cast<Yep8s>(cacheParameters[3] >> 16));
			// AMD: edx[bits 0:7] = L1 instruction cache line size in bytes
			cache.L1ICacheInfo.lineSize = cacheParameters[3] & 0xFFu;
		}
		if YEP_LIKELY(maxExtendedCpuidIndex >= 0x80000006u) {
			int cacheParameters[4];
			__cpuid(cacheParameters, 0x80000006u);
			
			static const Yep8u associativityMap[16] = {
				 0,  1,   2,  0,
				 4,  0,   8,  0,
				16,  0,  32, 48,
				64, 96, 128, 0xFF
			};
			
			/* Knights Corner has a bug here: it reports 256K in this leaf, but 512K (the right value) in leaf 0x00000004 */
			if YEP_LIKELY((_vendor != YepCpuVendorIntel) || (cache.L2CacheInfo.cacheSize == 0u)) {
				// AMD, Intel: ecx[bits 16:31] = L2 cache size in KB
				cache.L2CacheInfo.cacheSize = ((cacheParameters[2] >> 16) & 0xFFFFu) * 1024;
				// AMD, Intel: ecx[bits 12:15] = L2 cache associativity
				cache.L2CacheInfo.associativity = associativityMap[(cacheParameters[2] >> 12) & 0xFU];
				// AMD, Intel: ecx[bits 0:7] = L2 cache line size in bytes
				cache.L2CacheInfo.lineSize = cacheParameters[2] & 0xFFU;
			}
			
			if YEP_UNLIKELY(_vendor == YepCpuVendorAMD) {
				// AMD: edx[bits 18:31] = L3 cache size in KB
				cache.L3CacheInfo.cacheSize = (Yep32u(cacheParameters[3]) >> 18) * 524288;
				// AMD: edx[bits 12:15] = L3 cache associativity
				cache.L3CacheInfo.associativity = associativityMap[(cacheParameters[3] >> 12) & 0xFu];
				// AMD: edx[bits 0:7] = L3 cache line size in bytes
				cache.L3CacheInfo.lineSize = cacheParameters[3] & 0xFFu;
			}
		}
		if (YEP_LIKELY(maxExtendedCpuidIndex >= 0x8000001Du) && YEP_UNLIKELY(_vendor == YepCpuVendorAMD)) {
			int extendedInfo[4];
			__cpuid(extendedInfo, 0x80000001u);
			
			// AMD: ecx[bit 22] in extended info = Topology Extensions
			if YEP_UNLIKELY(extendedInfo[2] & 0x00400000u) {
				int cacheParameters[4];
				
				int parameterIndex = 0;
				__cpuidex(cacheParameters, 0x8000001Du, parameterIndex);
				while ((cacheParameters[0] & 0x1F) != 0) {
					// AMD: eax[bits 0:4] = cache type:
					// - 0 = No more caches
					// - 1 = Data cache
					// - 2 = Instruction cache
					// - 3 = Unified cache
					const Yep32u cacheType = cacheParameters[0] & 0x1F;
					// AMD: eax[bits 5:7] = cache level (1, 2, or 3)
					const Yep32u cacheLevel = (cacheParameters[0] >> 5) & 0x7;

					// AMD: ebx[bits 22:31] = associativity - 1
					const Yep32u associativity = (Yep32u(cacheParameters[1]) >> 22) + 1;
					// AMD: ebx[bits 0:11] = line size - 1
					const Yep32u lineSize = (cacheParameters[1] & 0xFFF) + 1;
					// AMD: ecx[bits 0:31] = number of sets - 1
					const Yep32u numberOfSets = cacheParameters[2];
					
					if ((cacheLevel == 1) && (cacheType == 1)) {
						cache.L1DCacheInfo.cacheSize = associativity * lineSize * numberOfSets;
						cache.L1DCacheInfo.associativity = associativity;
						cache.L1DCacheInfo.lineSize = lineSize;
					} else if ((cacheLevel == 1) && (cacheType == 2)) {
						cache.L1ICacheInfo.cacheSize = associativity * lineSize * numberOfSets;
						cache.L1ICacheInfo.associativity = associativity;
						cache.L1ICacheInfo.lineSize = lineSize;
					} else if ((cacheLevel == 2) && (cacheType == 3)) {
						cache.L2CacheInfo.cacheSize = associativity * lineSize * numberOfSets;
						cache.L2CacheInfo.associativity = associativity;
						cache.L2CacheInfo.lineSize = lineSize;
					} else if ((cacheLevel == 3) && (cacheType == 3)) {
						cache.L3CacheInfo.cacheSize = associativity * lineSize * numberOfSets;
						cache.L3CacheInfo.associativity = associativity;
						cache.L3CacheInfo.lineSize = lineSize;
					}
					__cpuidex(cacheParameters, 0x8000001Du, ++parameterIndex);
				}
			}
		}
	}

	static void decodeCacheDescriptor(CacheHierarchyInfo& cache, Yep8u descriptor, const ModelInfo& modelInfo, YepCpuVendor vendor) {
		// Descriptors from
		// * Application Note 485: Intel Processor Indentification and CPUID Instruction, August 2009, Order Number 241618-036
		// * Intel 64 and IA-32 Architectures Software Developer's Manual, Volume 2A: Instruction Set Reference, A-M, March 2010, Order Number 253666-034US
		// * Cyrix CPU Detection Guide, Preliminary Revision 1.01
		switch (descriptor) {
			case 0x06:
				cache.L1ICacheInfo.cacheSize = 8*1024;
				cache.L1ICacheInfo.lineSize = 32;
				cache.L1ICacheInfo.associativity = 4;
				cache.L1ICacheInfo.isUnified = false;
				break;
			case 0x08:
				cache.L1ICacheInfo.cacheSize = 16*1024;
				cache.L1ICacheInfo.lineSize = 32;
				cache.L1ICacheInfo.associativity = 4;
				cache.L1ICacheInfo.isUnified = false;
				break;
			case 0x09:
				cache.L1ICacheInfo.cacheSize = 32*1024;
				cache.L1ICacheInfo.lineSize = 64;
				cache.L1ICacheInfo.associativity = 4;
				cache.L1ICacheInfo.isUnified = false;
				break;
			case 0x0A:
				cache.L1DCacheInfo.cacheSize = 8*1024;
				cache.L1DCacheInfo.lineSize = 32;
				cache.L1DCacheInfo.associativity = 2;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x0C:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.lineSize = 32;
				cache.L1DCacheInfo.associativity = 4;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x0D:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 4;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x0E:
				cache.L1DCacheInfo.cacheSize = 24*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 6;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x21:
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x22:
				cache.L3CacheInfo.cacheSize = 512*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 4;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x23:
				cache.L3CacheInfo.cacheSize = 1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x25:
				cache.L3CacheInfo.cacheSize = 2*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x29:
				cache.L3CacheInfo.cacheSize = 4*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x2C:
				cache.L1DCacheInfo.cacheSize = 32*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 8;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x30:
				cache.L1ICacheInfo.cacheSize = 32*1024;
				cache.L1ICacheInfo.lineSize = 64;
				cache.L1ICacheInfo.associativity = 8;
				cache.L1ICacheInfo.isUnified = false;
				break;
			case 0x39:
				cache.L2CacheInfo.cacheSize = 128*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x3A:
				cache.L2CacheInfo.cacheSize = 192*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 6;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x3B:
				cache.L2CacheInfo.cacheSize = 128*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 2;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x3C:
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x3D:
				cache.L2CacheInfo.cacheSize = 384*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 6;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x3E:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x41:
				cache.L2CacheInfo.cacheSize = 128*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x42:
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x43:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x44:
				cache.L2CacheInfo.cacheSize = 1024*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x45:
				cache.L2CacheInfo.cacheSize = 2*1024*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x46:
				cache.L3CacheInfo.cacheSize = 4*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 4;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x47:
				cache.L3CacheInfo.cacheSize = 8*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x48:
				cache.L2CacheInfo.cacheSize = 3*1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x49:
				if (YEP_LIKELY(vendor == YepCpuVendorIntel) && YEP_UNLIKELY(modelInfo.model == 0x06) && YEP_UNLIKELY(modelInfo.family == 0x0F)) {
					cache.L3CacheInfo.cacheSize = 4*1024*1024;
					cache.L3CacheInfo.lineSize = 64;
					cache.L3CacheInfo.associativity = 16;
					cache.L3CacheInfo.isUnified = true;
				} else {
					cache.L2CacheInfo.cacheSize = 4*1024*1024;
					cache.L2CacheInfo.lineSize = 64;
					cache.L2CacheInfo.associativity = 16;
					cache.L2CacheInfo.isUnified = true;
				}
				break;
			case 0x4A:
				cache.L3CacheInfo.cacheSize = 6*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x4B:
				cache.L3CacheInfo.cacheSize = 8*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 16;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x4C:
				cache.L3CacheInfo.cacheSize = 12*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x4D:
				cache.L3CacheInfo.cacheSize = 16*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 16;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0x4E:
				cache.L2CacheInfo.cacheSize = 6*1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 24;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x60:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 8;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x66:
				cache.L1DCacheInfo.cacheSize = 8*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 4;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x67:
				cache.L1DCacheInfo.cacheSize = 16*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 4;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x68:
				cache.L1DCacheInfo.cacheSize = 32*1024;
				cache.L1DCacheInfo.lineSize = 64;
				cache.L1DCacheInfo.associativity = 4;
				cache.L1DCacheInfo.isUnified = false;
				break;
			case 0x70:
				cache.traceCacheInfo.microops = 12*1024;
				cache.traceCacheInfo.associativity = 8;
				break;
			case 0x71:
				cache.traceCacheInfo.microops = 16*1024;
				cache.traceCacheInfo.associativity = 8;
				break;
			case 0x72:
				cache.traceCacheInfo.microops = 32*1024;
				cache.traceCacheInfo.associativity = 8;
				break;
			case 0x73:
				cache.traceCacheInfo.microops = 64*1024;
				cache.traceCacheInfo.associativity = 8;
				break;
			case 0x78:
				cache.L2CacheInfo.cacheSize = 1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x79:
				cache.L2CacheInfo.cacheSize = 128*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x7A:
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x7B:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x7C:
				cache.L2CacheInfo.cacheSize = 1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x7D:
				cache.L2CacheInfo.cacheSize = 2*1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x7F:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 2;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x80:
				if (vendor == YepCpuVendorCyrix) {
					cache.L1ICacheInfo.cacheSize = 16*1024;
					cache.L1DCacheInfo.cacheSize = 16*1024;
					cache.L1ICacheInfo.lineSize = 16;
					cache.L1DCacheInfo.lineSize = 16;
					cache.L1ICacheInfo.associativity = 4;
					cache.L1DCacheInfo.associativity = 4;
					cache.L1ICacheInfo.isUnified = true;
					cache.L1DCacheInfo.isUnified = true;
				} else {
					cache.L2CacheInfo.cacheSize = 512*1024;
					cache.L2CacheInfo.lineSize = 64;
					cache.L2CacheInfo.associativity = 8;
					cache.L2CacheInfo.isUnified = true;
				}
				break;
			case 0x82:
				cache.L2CacheInfo.cacheSize = 256*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x83:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x84:
				cache.L2CacheInfo.cacheSize = 1024*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x85:
				cache.L2CacheInfo.cacheSize = 2*1024*1024;
				cache.L2CacheInfo.lineSize = 32;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x86:
				cache.L2CacheInfo.cacheSize = 512*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 4;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0x87:
				cache.L2CacheInfo.cacheSize = 1024*1024;
				cache.L2CacheInfo.lineSize = 64;
				cache.L2CacheInfo.associativity = 8;
				cache.L2CacheInfo.isUnified = true;
				break;
			case 0xD0:
				cache.L3CacheInfo.cacheSize = 512*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 4;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xD1:
				cache.L3CacheInfo.cacheSize = 1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 4;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xD2:
				cache.L3CacheInfo.cacheSize = 2*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 4;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xD6:
				cache.L3CacheInfo.cacheSize = 1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xD7:
				cache.L3CacheInfo.cacheSize = 2*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xD8:
				cache.L3CacheInfo.cacheSize = 4*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 8;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xDC:
				cache.L3CacheInfo.cacheSize = 3*512*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xDD:
				cache.L3CacheInfo.cacheSize = 3*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xDE:
				cache.L3CacheInfo.cacheSize = 6*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 12;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xE2:
				cache.L3CacheInfo.cacheSize = 2*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 16;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xE3:
				cache.L3CacheInfo.cacheSize = 4*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 16;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xE4:
				cache.L3CacheInfo.cacheSize = 8*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 16;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xEA:
				cache.L3CacheInfo.cacheSize = 12*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 24;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xEB:
				cache.L3CacheInfo.cacheSize = 18*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 24;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xEC:
				cache.L3CacheInfo.cacheSize = 24*1024*1024;
				cache.L3CacheInfo.lineSize = 64;
				cache.L3CacheInfo.associativity = 24;
				cache.L3CacheInfo.isUnified = true;
				break;
			case 0xF0:
				cache.prefetchLineSize = 64;
				break;
			case 0xF1:
				cache.prefetchLineSize = 128;
				break;
		}
	}

	static void decodeDeterministicCacheParameters(CacheHierarchyInfo& cache, Yep32u cacheType, Yep32u cacheLevel, Yep32u cacheSize, Yep32u lineSize, Yep32u associativity) {
		if YEP_LIKELY(cacheLevel == 1) {
			// 1st level cache
			switch (cacheType) {
				case 1: // Data cache
					cache.L1DCacheInfo.cacheSize = cacheSize;
					cache.L1DCacheInfo.lineSize = static_cast<Yep16u>(lineSize);
					cache.L1DCacheInfo.associativity = static_cast<Yep16u>(associativity);
					cache.L1DCacheInfo.isUnified = false;
					break;
				case 2: // Instruction cache
					cache.L1ICacheInfo.cacheSize = cacheSize;
					cache.L1ICacheInfo.lineSize = static_cast<Yep16u>(lineSize);
					cache.L1ICacheInfo.associativity = static_cast<Yep16u>(associativity);
					cache.L1DCacheInfo.isUnified = false;
					break;
				case 3: // Unified cache
					cache.L1ICacheInfo.cacheSize = cacheSize;
					cache.L1DCacheInfo.cacheSize = cacheSize;
					cache.L1ICacheInfo.lineSize = static_cast<Yep16u>(lineSize);
					cache.L1DCacheInfo.lineSize = static_cast<Yep16u>(lineSize);
					cache.L1ICacheInfo.associativity = static_cast<Yep16u>(associativity);
					cache.L1DCacheInfo.associativity = static_cast<Yep16u>(associativity);
					cache.L1ICacheInfo.isUnified = true;
					cache.L1DCacheInfo.isUnified = true;
					break;
			}
		} else if YEP_LIKELY((cacheLevel == 2) && (cacheType == 3)) {
			// 2nd level unified cache
			cache.L2CacheInfo.cacheSize = cacheSize;
			cache.L2CacheInfo.lineSize = static_cast<Yep16u>(lineSize);
			cache.L2CacheInfo.associativity = static_cast<Yep16u>(associativity);
			cache.L2CacheInfo.isUnified = true;
		} else if YEP_LIKELY((cacheLevel == 3) && (cacheType == 3)) {
			// 3rd level unified cache
			cache.L3CacheInfo.cacheSize = cacheSize;
			cache.L3CacheInfo.lineSize = static_cast<Yep16u>(lineSize);
			cache.L3CacheInfo.associativity = static_cast<Yep16u>(associativity);
			cache.L3CacheInfo.isUnified = true;
	#if defined(YEP_K1OM_X64_ABI)
		} else if YEP_LIKELY((cacheLevel == 2) && (cacheType == 1)) {
			/* A bug in Knights Corner: L2 unified cache is reported as L2 data cache */
			cache.L2CacheInfo.cacheSize = cacheSize;
			cache.L2CacheInfo.lineSize = static_cast<Yep16u>(lineSize);
			cache.L2CacheInfo.associativity = static_cast<Yep16u>(associativity);
			cache.L2CacheInfo.isUnified = true;
	#endif
		}
	}

	/** @brief	The state of the parser to be preserved between parsing different words. */
	struct ParserState {
		/** @brief	Pointer to the start of the previous word if it is "model". Null if previous word is not "model". */
		char* previousWord_model;
		/** @brief	Pointer to the start of the previous word if it is a single-uppercase-letter word. Null if previous word is anything different. */
		char* previousWord_UpperCaseLetter;
		/** @brief	Pointer to the start of the previous word if it is "Dual". Null if previous word is not "Dual". */
		char* previousWord_Dual;
		/** @brief	Pointer to the start of the previous word if it is "MMX". Null if previous word is not "MMX". */
		char* previousWord_MMX;
		/** @brief	Pointer to the start of the previous word if it is "Core", "Dual-Core", "QuadCore", etc. Null if previous word is anything different. */
		char* previousWord_Core;
		/** @brief	Pointer to the start of the word "w/" in the brand string. Null if there is no "w/" word in the brand string. */
		char* word_with;
		/** @brief	Pointer to the end of the word "APU" in the brand string. Null if there is no "APU" word in the brand string. */
		char* word_APU;
		/** @brief	Pointer to the '@' symbol in the brand string (separates frequency specification). Null if there is no '@' symbol. */
		char* frequencySeparator;
		/** @brief	Indicates whether the processor model number was already parsed. */
		YepBoolean parsedModelNumber;

		/** @brief	Resets information about the previous word. Keeps all other state information. */
		void resetPreviousWord() {
			this->previousWord_model = 0;
			this->previousWord_UpperCaseLetter = 0;
			this->previousWord_Dual = 0;
			this->previousWord_MMX = 0;
			this->previousWord_Core = 0;
		}

		/** @brief	Resets all state information. */
		void resetAll() {
			this->resetPreviousWord();
			this->word_with = 0;
			this->word_APU = 0;
			this->frequencySeparator = 0;
			this->parsedModelNumber = false;
		}
	};

	/**
	 * @brief	Overwrites the supplied string with space characters if it exactly matches the given string.
	 * @param	string	The string to be compared against other string, and erased in case of matching.
	 * @param	length	The length of the two string to be compared against each other.
	 * @param	match	The string to compare against.
	 * @retval	true	If the two strings match and the first supplied string was erased (overwritten with space characters).
	 * @retval	false	If the two strings are different and the first supplied string remained unchanged.
	 */
	static YEP_INLINE YepBoolean eraseMatching(char* string, size_t length, const char* match) {
		if (memcmp(string, match, length) == 0) {
			memset(string, ' ', length);
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @brief	Checks if the supplied ASCII character is an uppercase latin letter.
	 * @param	character	The character to analyse.
	 * @retval	true	If the supplied character is an uppercase latin letter ('A' to 'Z').
	 * @retval	false	If the supplied character is anything different.
	 */
	static YEP_INLINE YepBoolean isUpperCaseLetter(char character) {
		return Yep32u(character - 'A') <= Yep32u('Z' - 'A');
	}

	/**
	 * @brief	Checks if the supplied ASCII character is a digit.
	 * @param	character	The character to analyse.
	 * @retval	true	If the supplied character is a digit ('0' to '9').
	 * @retval	false	If the supplied character is anything different.
	 */
	static YEP_INLINE YepBoolean isDigit(char character) {
		return Yep32u(character - '0') < 10u;
	}

	static YEP_INLINE YepBoolean isZeroNumber(const char* wordStart, const char* wordEnd) {
		for (const char* currentCharacter = wordStart; currentCharacter != wordEnd; currentCharacter++) {
			if (*currentCharacter != '0') {
				return false;
			}
		}
		return true;
	}

	static YEP_INLINE YepBoolean isSpace(const char* wordStart, const char* wordEnd) {
		for (const char* currentCharacter = wordStart; currentCharacter != wordEnd; currentCharacter++) {
			if (*currentCharacter != ' ') {
				return false;
			}
		}
		return true;
	}

	static YEP_INLINE YepBoolean isNumber(const char* wordStart, const char* wordEnd) {
		for (const char* currentCharacter = wordStart; currentCharacter != wordEnd; currentCharacter++) {
			if (!isDigit(*currentCharacter)) {
				return false;
			}
		}
		return true;
	}

	static YEP_INLINE YepBoolean isModelNumber(const char* wordStart, const char* wordEnd) {
		YepBoolean previousDigit = false;
		for (const char* currentCharacter = wordStart; currentCharacter != wordEnd; currentCharacter++) {
			if (isDigit(*currentCharacter)) {
				if (previousDigit) {
					return true;
				}
				previousDigit = true;
			} else {
				previousDigit = false;
			}
		}
		return false;
	}

	/**
	 * @warning	Input and output words can overlap
	 */
	static YepSize copyWordForward(const char* wordStart, const char* wordEnd, char* outputPointer) {
		const YepSize wordLength = wordEnd - wordStart;
		for (YepSize index = 0; index != wordLength; index++) {
			outputPointer[index] = wordStart[index];
		}
		return wordLength;
	}

	static void transformWord(char* wordStart, char* wordEnd, ParserState& state) {
		const ParserState previousState = state;
		state.resetPreviousWord();

		YepSize wordLength = wordEnd - wordStart;

		if (state.frequencySeparator != 0) {
			if (wordStart > state.frequencySeparator) {
				if (state.parsedModelNumber) {
					memset(wordStart, ' ', wordLength);
				}
			}
		}


		/* Early AMD and Cyrix processors have "tm" suffix for trademark, e.g.
		 *   "AMD-K6tm w/ multimedia extensions"
		 *   "Cyrix MediaGXtm MMXtm Enhanced"
		 */
		if (wordLength > 2) {
			const char preceedingCharacter = wordEnd[-3];
			if (isDigit(preceedingCharacter) || isUpperCaseLetter(preceedingCharacter)) {
				if (eraseMatching(wordEnd - 2, 2, "tm")) {
					wordEnd -= 2;
					wordLength -= 2;
				}
			}
		}
		if (wordLength > 4) {
			/* Some early AMD CPUs have "AMD-" at the beginning, e.g.
			 *   "AMD-K5(tm) Processor"
			 *   "AMD-K6tm w/ multimedia extensions"
			 *   "AMD-K6(tm) 3D+ Processor"
			 *   "AMD-K6(tm)-III Processor"
			 */
			if (eraseMatching(wordStart, 4, "AMD-")) {
				wordStart += 4;
				wordLength -= 4;
			}
		}
		switch (wordLength) {
			case 1:
				/* On some Intel processors there is a space between the first letter of
				 * the name and the number after it, e.g.
				 *   "Intel(R) Core(TM) i7 CPU X 990  @ 3.47GHz"
				 *   "Intel(R) Core(TM) CPU Q 820  @ 1.73GHz"
				 * We want to merge these parts together, i.e. "X 990" -> "X990", "Q 820" -> "Q820"
				 */
				if (isUpperCaseLetter(wordStart[0])) {
					state.previousWord_UpperCaseLetter = wordStart;
					return;
				}
				break;
			case 2:
				/* Remember to erase "w/ ..... extensions" in "AMD-K6tm w/ multimedia extensions" */
				if (memcmp(wordStart, "w/", wordLength) == 0) {
					state.word_with = wordStart;
					return;
				}
				break;
			case 3:
				/* Erase "CPU" in brand string on Intel processors, e.g.
				 *  "Intel(R) Core(TM) i5 CPU         650  @ 3.20GHz"
				 *  "Intel(R) Xeon(R) CPU           X3210  @ 2.13GHz"
				 *  "Intel(R) Atom(TM) CPU Z2760  @ 1.80GHz"
				 */
				if (eraseMatching(wordStart, wordLength, "CPU")) {
					return;
				}
				/* Erase "AMD" in brand string on AMD processors, e.g.
				 *  "AMD Athlon(tm) Processor"
				 *  "AMD Engineering Sample"
				 *  "Quad-Core AMD Opteron(tm) Processor 2344 HE"
				 */
				if (eraseMatching(wordStart, wordLength, "AMD")) {
					return;
				}
				/* Erase "VIA" in brand string on VIA processors, e.g.
				 *   "VIA C3 Ezra"
				 *   "VIA C7-M Processor 1200MHz"
				 *   "VIA Nano L3050@1800MHz"
				 */
				if (eraseMatching(wordStart, wordLength, "VIA")) {
					return;
				}
				/* Erase "IDT" in brand string on early Centaur processors, e.g.
				 *   "IDT WinChip 2-3D"
				 */
				if (eraseMatching(wordStart, wordLength, "IDT")) {
					return;
				}
				/* Remember to erase "MMX Enhanced" in "Cyrix MediaGXtm MMXtm Enhanced" ("tm" suffix is removed by this point) */
				if (memcmp(wordStart, "MMX", wordLength) == 0) {
					state.previousWord_MMX = wordStart;
					return;
				}
				/* Remember to erase "APU ..... Graphics" in "AMD A10-4600M APU with Radeon(tm) HD Graphics" */
				if (eraseMatching(wordStart, wordLength, "APU")) {
					state.word_APU = wordEnd;
					return;
				}
				break;
			case 4:
				/* Remember to erase "Dual Core" in "AMD Athlon(tm) 64 X2 Dual Core Processor 3800+" */
				if (memcmp(wordStart, "Dual", wordLength) == 0) {
					state.previousWord_Dual = wordStart;
				}
				/* Erase "Dual Core" in "AMD Athlon(tm) 64 X2 Dual Core Processor 3800+" */
				if (previousState.previousWord_Dual != 0) {
					if (memcmp(wordStart, "Core", wordLength) == 0) {
						memset(previousState.previousWord_Dual, ' ', wordEnd - previousState.previousWord_Dual);
						state.previousWord_Core = wordEnd;
						return;
					}
				}
				break;
			case 5:
				/* Erase "Intel" in brand string on Intel processors, e.g.
				 *   "Intel(R) Xeon(R) CPU X3210 @ 2.13GHz"
				 *   "Intel(R) Atom(TM) CPU D2700 @ 2.13GHz"
				 *   "Genuine Intel(R) processor 800MHz"
				 */
				if (eraseMatching(wordStart, wordLength, "Intel")) {
					return;
				}
				/* Erase "Cyrix" in brand string on Cyrix processors, e.g.
				 *   "Cyrix MediaGXtm MMXtm Enhanced"
				 */
				if (eraseMatching(wordStart, wordLength, "Cyrix")) {
					return;
				}
				/* Remember to erase "model unknown" in "AMD Processor model unknown" */
				if (memcmp(wordStart, "model", wordLength) == 0) {
					state.previousWord_model = wordStart;
					return;
				}
				break;
			case 6:
				/* Erase "Mobile" when it is not part of the processor name, 
				 * e.g. in "AMD Turion(tm) X2 Ultra Dual-Core Mobile ZM-82"
				 */
				if (previousState.previousWord_Core != 0) {
					if (memcmp(wordStart, "Mobile", wordLength) == 0) {
						memset(wordStart, ' ', wordLength);
					}
				}
				/* Erase "family" in "Intel(R) Pentium(R) III CPU family 1266MHz" */
				if (eraseMatching(wordStart, wordLength, "family")) {
					return;
				}
			case 7:
				/* Erase "Geniune" in brand string on Intel engineering samples, e.g.
				 *   "Genuine Intel(R) processor 800MHz"
				 *   "Genuine Intel(R) CPU @ 2.13GHz"
				 *   "Genuine Intel(R) CPU 0000 @ 1.73GHz"
				 */
				if (eraseMatching(wordStart, wordLength, "Genuine")) {
					return;
				}
				/* Erase "model unknown" in "AMD Processor model unknown" */
				if (previousState.previousWord_model != 0) {
					if (memcmp(wordStart, "unknown", wordLength) == 0) {
						memset(previousState.previousWord_model, ' ', wordEnd - previousState.previousWord_model);
						return;
					}
				}
				break;
			case 8:
				/* Erase "QuadCore" in "VIA QuadCore L4700 @ 1.2+ GHz" */
				if (eraseMatching(wordStart, wordLength, "QuadCore")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				/* Erase "Six-Core" in "AMD FX(tm)-6100 Six-Core Processor" */
				if (eraseMatching(wordStart, wordLength, "Six-Core")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				/* Erase "APU ..... Graphics" in "AMD A10-4600M APU with Radeon(tm) HD Graphics" */
				if (state.word_APU != 0) {
					if (memcmp(wordStart, "Graphics", wordLength) == 0) {
						memset(state.word_APU, ' ', wordEnd - state.word_APU);
						state.word_APU = 0;
						return;
					}
				}
				/* Erase "MMX Enhanced" in "Cyrix MediaGXtm MMXtm Enhanced" ("tm" suffix is removed by this point) */
				if (previousState.previousWord_MMX != 0) {
					if (memcmp(wordStart, "Enhanced", wordLength) == 0) {
						memset(previousState.previousWord_MMX, ' ', wordEnd - previousState.previousWord_MMX);
						return;
					}
				}
				break;
			case 9:
				if (eraseMatching(wordStart, wordLength, "Processor")) {
					return;
				}
				if (eraseMatching(wordStart, wordLength, "processor")) {
					return;
				}
				/* Erase "Dual-Core" in "Pentium(R) Dual-Core CPU T4200 @ 2.00GHz" */
				if (eraseMatching(wordStart, wordLength, "Dual-Core")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				/* Erase "Quad-Core" in AMD processors, e.g.
				 *   "Quad-Core AMD Opteron(tm) Processor 2347 HE"
				 *   "AMD FX(tm)-4170 Quad-Core Processor"
				 */
				if (eraseMatching(wordStart, wordLength, "Quad-Core")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				/* Erase "Transmeta" in brand string on Transmeta processors, e.g.
				 *   "Transmeta(tm) Crusoe(tm) Processor TM5800"
				 *   "Transmeta Efficeon(tm) Processor TM8000"
				 */
				if (eraseMatching(wordStart, wordLength, "Transmeta")) {
					return;
				}
			case 10:
				/* Erase "Eight-Core" in AMD processors, e.g.
				 *   "AMD FX(tm)-8150 Eight-Core Processor"
				 */
				if (eraseMatching(wordStart, wordLength, "Eight-Core")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				if (state.word_with != 0) {
					if (memcmp(wordStart, "extensions", wordLength) == 0) {
						memset(state.word_with, ' ', wordEnd - state.word_with);
						state.word_with = 0;
						return;
					}
				}
				break;
			case 11:
				/* Erase "Triple-Core" in AMD processors, e.g.
				 *   "AMD Phenom(tm) II N830 Triple-Core Processor"
				 *   "AMD Phenom(tm) 8650 Triple-Core Processor"
				 */
				if (eraseMatching(wordStart, wordLength, "Triple-Core")) {
					state.previousWord_Core = wordEnd;
					return;
				}
				break;
		}
		if (isZeroNumber(wordStart, wordEnd)) {
			memset(wordStart, ' ', wordLength);
			return;
		}
		/* On some Intel processors the last letter of the name is put before the number,
		 * and an additional space it added, e.g.
		 *   "Intel(R) Core(TM) i7 CPU X 990  @ 3.47GHz"
		 *   "Intel(R) Core(TM) CPU Q 820  @ 1.73GHz"
		 *   "Intel(R) Core(TM) i5 CPU M 480  @ 2.67GHz"
		 * We fix this issue, i.e. "X 990" -> "990X", "Q 820" -> "820Q"
		 */
		if (previousState.previousWord_UpperCaseLetter != 0) {
			/* A single letter word followed by 2-to-5 digit letter is merged together */
			if ((wordLength >= 2) && (wordLength <= 5)) {
				if (isNumber(wordStart, wordEnd)) {
					/* Load the previous single-letter word */
					char letter = *previousState.previousWord_UpperCaseLetter;
					/* Erase the previous single-letter word */
					*previousState.previousWord_UpperCaseLetter = ' ';
					/* Move the current word one position to the left */
					copyWordForward(wordStart, wordEnd, wordStart - 1);
					wordStart -= 1;
					/* Add the letter on the end */
					/* Note: accessing wordStart[-1] is safe because this is not the first word */
					wordEnd[-1] = letter;
				}
			}
		}
		if (state.frequencySeparator != 0) {
			if (isModelNumber(wordStart, wordEnd)) {
				state.parsedModelNumber = true;
			}
		}
	}

	static YepSize beautifyBrandString(char brandString[48]) {
		// First find the end of the string
		// Start search from the end because some brand strings contain zeroes in the middle
		char* stringLast = &brandString[47];
		while (stringLast == '\0') {
			stringLast--;
			// Check that we didn't reach the start of the brand string; this is only possible if all characters are zero
			if (stringLast == brandString) {
				// All characters are zeros
				return 0;
			}
		}
		/* The first character after the end of the string */
		char* const stringEnd = stringLast + 1;
		*stringEnd = '\0';

		ParserState parserState;
		parserState.resetAll();

		/* Now unify all whitespace characters: replace tabs and '\0' with spaces */
		{
			YepBoolean inCurlyBraces = false;
			for (char* currentCharacter = brandString; currentCharacter != stringEnd; currentCharacter++) {
				const char character = *currentCharacter;
				if (character == '(') {
					inCurlyBraces = true;
				}
				if ((character == '\t') || (character == '\0') || (character == '@') || inCurlyBraces) {
					*currentCharacter = ' ';
				}
				if (character == ')') {
					inCurlyBraces = false;
				} else if (character == '@') {
					parserState.frequencySeparator = currentCharacter;
				}
			}
		}

		/* Iterate through all words and erase redundant parts */
		{
			bool isWord = false;
			char* wordStart;
			for (char* currentCharacter = brandString; currentCharacter != stringEnd; currentCharacter++) {
				const char character = *currentCharacter;
				if (character == ' ') {
					if (isWord) {
						isWord = false;
						transformWord(wordStart, currentCharacter, parserState);
					}
				} else {
					if (!isWord) {
						isWord = true;
						wordStart = currentCharacter;
					}
				}
			}
			if (isWord) {
				transformWord(wordStart, stringEnd, parserState);
			}
		}

		/* Check if there is some string before the frequency separator.
		 * If only frequency is available, erase everything */
		if (parserState.frequencySeparator != 0) {
			if (isSpace(brandString, parserState.frequencySeparator)) {
				return 0; /* Empty string */
			}
		}

		/* Compact words: collapse multiple spacing into one */
		{
			char* outputPointer = brandString;
			char* wordStart;
			YepBoolean isWord = false;
			YepBoolean previousWordEndsWithDash = true;
			YepBoolean currentWordStartsWithDash = false;
			for (char* currentCharacter = brandString; currentCharacter != stringEnd; currentCharacter++) {
				const char character = *currentCharacter;
				if (character == ' ') {
					if (isWord) {
						isWord = false;
						if (!currentWordStartsWithDash && !previousWordEndsWithDash) {
							*outputPointer++ = ' ';
						}
						outputPointer += copyWordForward(wordStart, currentCharacter, outputPointer);
						/* Note: currentCharacter[-1] exists because there is a word before this space */
						previousWordEndsWithDash = (currentCharacter[-1] == '-');
					}
				} else {
					if (!isWord) {
						isWord = true;
						wordStart = currentCharacter;
						currentWordStartsWithDash = (character == '-');
					}
				}
			}
			if (isWord) {
				if (!currentWordStartsWithDash && !previousWordEndsWithDash) {
					*outputPointer++ = ' ';
				}
				outputPointer += copyWordForward(wordStart, stringEnd, outputPointer);
			}
			return outputPointer - brandString;
		}
	}

	static YepSize getCpuBrandString(char brandString[48]) {
		int registers[4];
		__cpuid(registers, 0x80000002u);
		*reinterpret_cast<int*>(&brandString[0]) = registers[0];
		*reinterpret_cast<int*>(&brandString[4]) = registers[1];
		*reinterpret_cast<int*>(&brandString[8]) = registers[2];
		*reinterpret_cast<int*>(&brandString[12]) = registers[3];
		__cpuid(registers, 0x80000003u);
		*reinterpret_cast<int*>(&brandString[16]) = registers[0];
		*reinterpret_cast<int*>(&brandString[20]) = registers[1];
		*reinterpret_cast<int*>(&brandString[24]) = registers[2];
		*reinterpret_cast<int*>(&brandString[28]) = registers[3];
		__cpuid(registers, 0x80000004u);
		*reinterpret_cast<int*>(&brandString[32]) = registers[0];
		*reinterpret_cast<int*>(&brandString[36]) = registers[1];
		*reinterpret_cast<int*>(&brandString[40]) = registers[2];
		*reinterpret_cast<int*>(&brandString[44]) = registers[3];
		return beautifyBrandString(brandString);
	}

	static void initBriefCpuName(ConstantString& briefCpuName, Yep32u maxExtendedCpuidIndex, Yep64u isaFeatures, Yep64u simdFeatures) {
		static char briefCpuNameBuffer[48];
		if YEP_LIKELY(maxExtendedCpuidIndex >= 0x80000004u) {
			const YepSize briefCpuNameLength = getCpuBrandString(briefCpuNameBuffer);
			if YEP_LIKELY(briefCpuNameLength != 0) {
				briefCpuName = ConstantString(briefCpuNameBuffer, briefCpuNameLength);
				return;
			}
		}
		
		#if defined(YEP_X64_ABI)
			#if defined(YEP_K1OM_X64_ABI)
				switch (_logicalCoresCount) {
					case 244:
						briefCpuName = YEP_MAKE_CONSTANT_STRING("Xeon Phi SE10P"); break;
					case 240:
						briefCpuName = YEP_MAKE_CONSTANT_STRING("Xeon Phi 5110P"); break;
					default:
						briefCpuName = YEP_MAKE_CONSTANT_STRING("Xeon Phi"); break;
				}
			#else
				briefCpuName = YEP_MAKE_CONSTANT_STRING("x86-64 compatible");
			#endif
		#else
			const Yep64u x64IsaMask = YepX86IsaFeatureCpuid | YepX86IsaFeatureRdtsc | YepX86IsaFeatureX64;
			const Yep64u x64SimdMask = YepX86SimdFeatureSSE | YepX86SimdFeatureSSE2;
			const Yep64u i586IsaMask = YepX86IsaFeatureCpuid | YepX86IsaFeatureRdtsc | YepX86IsaFeatureFPU;
			const Yep64u i686IsaMask = i586IsaMask | YepX86IsaFeatureCMOV;
			if YEP_LIKELY(YEP_LIKELY((isaFeatures & x64IsaMask) == x64IsaMask) && YEP_LIKELY((simdFeatures & x64SimdMask) == x64SimdMask)) {
				briefCpuName = YEP_MAKE_CONSTANT_STRING("x86-64 compatible");
			} else if YEP_LIKELY((isaFeatures & i686IsaMask) == i686IsaMask) {
				briefCpuName = YEP_MAKE_CONSTANT_STRING("i686 compatible");
			} else if YEP_LIKELY((isaFeatures & i586IsaMask) == i586IsaMask) {
				briefCpuName = YEP_MAKE_CONSTANT_STRING("i586 compatible");
			} else {
				briefCpuName = YEP_MAKE_CONSTANT_STRING("i486 compatible");
			}
		#endif
	}

	static void initCpuName(ConstantString& briefCpuName, ConstantString& fullCpuName, Yep32u maxExtendedCpuidIndex, YepCpuVendor vendor, Yep64u isaFeatures, Yep64u simdFeatures) {
		static char fullCpuNameBuffer[128];
		
		initBriefCpuName(briefCpuName, maxExtendedCpuidIndex, isaFeatures, simdFeatures);
		
		if YEP_UNLIKELY(vendor == YepCpuVendorUnknown) {
			fullCpuName = briefCpuName;
		} else {
			const ConstantString vendorString = _yepLibrary_GetCpuVendorDescription(vendor);
			memcpy(fullCpuNameBuffer, vendorString.pointer, vendorString.length);
			fullCpuNameBuffer[vendorString.length] = ' ';
			memcpy(fullCpuNameBuffer + vendorString.length + 1, briefCpuName.pointer, briefCpuName.length);
			fullCpuName = ConstantString(fullCpuNameBuffer, vendorString.length + 1 + briefCpuName.length);
		}
	}

	ModelInfo _modelInfo;

	YepStatus _yepLibrary_InitCpuInfo() {
		#if defined(YEP_LINUX_OS)
			_yepLibrary_InitLinuxLogicalCoresCount(_logicalCoresCount, _systemFeatures);
		#elif defined(YEP_MACOSX_OS)
			_yepLibrary_InitMacOSXLogicalCoresCount(_logicalCoresCount, _systemFeatures);
		#elif defined(YEP_WINDOWS_OS)
			_yepLibrary_InitWindowsLogicalCoresCount(_logicalCoresCount, _systemFeatures);
		#else
			#error "This OS is not supported yet"
		#endif

		Yep32u maxBaseCpuidIndex = 0, maxExtendedCpuidIndex = 0;
		getCpuidMaxIndex(maxBaseCpuidIndex, maxExtendedCpuidIndex);
		initVendor(_vendor);
		
		initModelInfo(_modelInfo, maxBaseCpuidIndex);
		initMicroarchitecture(_microarchitecture, _modelInfo, _vendor);

		initCacheInfo(_cache, _modelInfo, _vendor, maxBaseCpuidIndex, maxExtendedCpuidIndex);
		initIsaInfo(_isaFeatures, _simdFeatures, _systemFeatures, _vendor, _microarchitecture, maxBaseCpuidIndex, maxExtendedCpuidIndex);
		
		initCpuName(_briefCpuName, _fullCpuName, maxExtendedCpuidIndex, _vendor, _isaFeatures, _simdFeatures);
		
		_dispatchList = _yepLibrary_GetMicroarchitectureDispatchList(_microarchitecture);
		return YepStatusOk;
	}
#else
	#error "The functions in this file should only be used in and compiled for x86/x86-64"
#endif
