/*
 *                          Yeppp! library header
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 *
 * Copyright (C) 2010-2012 Marat Dukhan
 * Copyright (C) 2012-2013 Georgia Institute of Technology
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Georgia Institute of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <yepPredefines.h>
#include <yepTypes.h>

/** @defgroup yepLibrary yepLibrary.h: library initialization, information, and support functions. */

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_Init	Library initialization, deinitialization, and version information
	 */

	/**
	 * @ingroup yepLibrary_Init
	 * @brief	Initialized the @Yeppp library.
	 * @retval	#YepStatusOk	The library is successfully initialized.
	 * @retval	#YepStatusSystemError	An uncoverable error inside the OS kernel occurred during library initialization.
	 * @see	yepLibrary_Release
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_Init();
	/**
	 * @ingroup yepLibrary_Init
	 * @brief	Deinitialized the @Yeppp library and releases the consumed system resources.
	 * @retval	#YepStatusOk	The library is successfully deinitialized.
	 * @retval	#YepStatusSystemError	The library failed to release some of the resources due to a failed call to the OS kernel.
	 * @see	yepLibrary_Init
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_Release();
	/**
	 * @ingroup	yepLibrary_Init
	 * @brief	Contains information about @Yeppp library version.
	 * @see	yepLibrary_GetVersion
	 */
	struct YepLibraryVersion {
		/** @brief The major version. Library releases with the same major versions are guaranteed to be API- and ABI-compatible. */
		Yep32u major;
		/** @brief The minor version. A change in minor versions indicates addition of new features, and major bug-fixes. */
		Yep32u minor;
		/** @brief The patch level. A version with a higher patch level indicates minor bug-fixes. */
		Yep32u patch;
		/** @brief The build number. The build number is unique for the fixed combination of major, minor, and patch-level versions. */
		Yep32u build;
		/** @brief A UTF-8 string with a human-readable name of this release. May contain non-ASCII characters. */
		const char* releaseName;
	};
	/**
	 * @ingroup yepLibrary_Init
	 * @brief	Returns basic information about the library version.
	 * @note	It is safe to call this function without initializing the library.
	 * @return	A pointer to a structure describing @Yeppp library version.
	 */
	YEP_PUBLIC_SYMBOL const struct YepLibraryVersion *YEPABI yepLibrary_GetVersion();

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_CpuFeatures Processor extensions information
	 */

	/**
	 * @ingroup yepLibrary_CpuFeatures
	 * @brief	Returns information about the supported ISA extensions (excluding SIMD extensions)
	 * @param[out]	isaFeatures	Pointer to a 64-bit mask where information about the supported ISA extensions will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a isaFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a isaFeatures pointer is null.
	 * @see	\ref x86_ISA_Extensions "x86 and x86-64 ISA extensions", \ref ARM_ISA_Extensions "ARM ISA extensions",
	 *     	\ref MIPS_ISA_Extensions "MIPS ISA extensions", \ref IA64_ISA_Extensions "IA64 ISA extensions",
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuIsaFeatures(Yep64u *isaFeatures);
	/**
	 * @ingroup yepLibrary_CpuFeatures
	 * @brief	Returns information about the supported SIMD extensions
	 * @param[out]	simdFeatures	Pointer to a 64-bit mask where information about the supported SIMD extensions will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a simdFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a simdFeatures pointer is null.
	 * @see	\ref x86_SIMD_Extensions "x86 and x86-64 SIMD extensions", \ref ARM_SIMD_Extensions "ARM SIMD extensions",
	 *     	\ref MIPS_SIMD_Extensions "MIPS SIMD extensions"
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuSimdFeatures(Yep64u *simdFeatures);
	/**
	 * @ingroup yepLibrary_CpuFeatures
	 * @brief	Returns information about processor features other than ISA extensions, and OS features related to CPU.
	 * @param[out]	systemFeatures	Pointer to a 64-bit mask where information about extended processor and system features will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a systemFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a systemFeatures pointer is null.
	 * @see	\ref Common_CPU_and_System_Features "Common CPU and system features",
	 *     	\ref x86_CPU_and_System_Features "x86 and x86-64 CPU and system features",
	 *     	\ref ARM_CPU_and_System_Features "ARM CPU and system features",
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuSystemFeatures(Yep64u *systemFeatures);

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_CpuInfo	Processor architecture, microarchitecture, and vendor information
	 */

	/** @name	Processor vendor information */
	/**@{*/
	/**
	 * @ingroup	yepLibrary_CpuInfo
	 * @brief	The company which designed the processor microarchitecture.
	 * @see	yepLibrary_GetCpuVendor
	 */
	enum YepCpuVendor {
		/** @brief	Processor vendor is not known to the library, or the library failed to get vendor information from the OS. */
		YepCpuVendorUnknown = 0,
		
		/* x86/x86-64 CPUs */
		
		/** @brief	Intel Corporation. Vendor of x86, x86-64, IA64, and ARM processor microarchitectures. */
		/** @details	Sold its ARM design subsidiary in 2006. The last ARM processor design was released in 2004. */
		YepCpuVendorIntel = 1,
		/** @brief	Advanced Micro Devices, Inc. Vendor of x86 and x86-64 processor microarchitectures. */
		YepCpuVendorAMD = 2,
		/** @brief	VIA Technologies, Inc. Vendor of x86 and x86-64 processor microarchitectures. */
		/** @details	Processors are designed by Centaur Technology, a subsidiary of VIA Technologies. */
		YepCpuVendorVIA = 3,
		/** @brief	Transmeta Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2004. */
		/**         	Transmeta processors implemented VLIW ISA and used binary translation to execute x86 code. */
		YepCpuVendorTransmeta = 4,
		/** @brief	Cyrix Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1996. */
		YepCpuVendorCyrix = 5,
		/** @brief	Rise Technology. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1999. */
		YepCpuVendorRise = 6,
		/** @brief	National Semiconductor. Vendor of x86 processor microarchitectures. */
		/** @details	Sold its x86 design subsidiary in 1999. The last processor design was released in 1998. */
		YepCpuVendorNSC = 7,
		/** @brief	Silicon Integrated Systems. Vendor of x86 processor microarchitectures. */
		/** @details	Sold its x86 design subsidiary in 2001. The last processor design was released in 2001. */
		YepCpuVendorSiS = 8,
		/** @brief	NexGen. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1994. */
		/**         	NexGen designed the first x86 microarchitecture which decomposed x86 instructions into simple microoperations. */
		YepCpuVendorNexGen = 9,
		/** @brief	United Microelectronics Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Ceased x86 in the early 1990s. The last processor design was released in 1991. */
		/**         	Designed U5C and U5D processors. Both are 486 level. */
		YepCpuVendorUMC = 10,
		/** @brief	RDC Semiconductor Co., Ltd. Vendor of x86 processor microarchitectures. */
		/** @details	Designes embedded x86 CPUs. */
		YepCpuVendorRDC = 11,
		/** @brief	DM&P Electronics Inc. Vendor of x86 processor microarchitectures. */
		/** @details	Mostly embedded x86 designs. */
		YepCpuVendorDMP = 12,
		
		/* ARM CPUs */
		
		/** @brief	ARM Holdings plc. Vendor of ARM processor microarchitectures. */
		YepCpuVendorARM       = 20,
		/** @brief	Marvell Technology Group Ltd. Vendor of ARM processor microarchitectures. */
		YepCpuVendorMarvell   = 21,
		/** @brief	Qualcomm Incorporated. Vendor of ARM processor microarchitectures. */
		YepCpuVendorQualcomm  = 22,
		/** @brief	Digital Equipment Corporation. Vendor of ARM processor microarchitecture. */
		/** @details	Sold its ARM designs in 1997. The last processor design was released in 1997. */
		YepCpuVendorDEC       = 23,
		/** @brief	Texas Instruments Inc. Vendor of ARM processor microarchitectures. */
		YepCpuVendorTI        = 24,
		/** @brief	Apple Inc. Vendor of ARM processor microarchitectures. */
		YepCpuVendorApple     = 25,
		
		/* MIPS CPUs */
		
		/** @brief	Ingenic Semiconductor. Vendor of MIPS processor microarchitectures. */
		YepCpuVendorIngenic   = 40,
		/** @brief	Institute of Computing Technology of the Chinese Academy of Sciences. Vendor of MIPS processor microarchitectures. */
		YepCpuVendorICT       = 41,
		/** @brief	MIPS Technologies, Inc. Vendor of MIPS processor microarchitectures. */
		YepCpuVendorMIPS      = 42,
		
		/* PowerPC CPUs */
		
		/** @brief	International Business Machines Corporation. Vendor of PowerPC processor microarchitectures. */
		YepCpuVendorIBM       = 50,
		/** @brief	Motorola, Inc. Vendor of PowerPC and ARM processor microarchitectures. */
		YepCpuVendorMotorola  = 51,
		/** @brief	P. A. Semi. Vendor of PowerPC processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2007. */
		YepCpuVendorPASemi    = 52,
		
		/* SPARC CPUs */
		
		/** @brief	Sun Microsystems, Inc. Vendor of SPARC processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2008. */
		YepCpuVendorSun       = 60,
		/** @brief	Oracle Corporation. Vendor of SPARC processor microarchitectures. */
		YepCpuVendorOracle    = 61,
		/** @brief	Fujitsu Limited. Vendor of SPARC processor microarchitectures. */
		YepCpuVendorFujitsu   = 62,
		/** @brief	Moscow Center of SPARC Technologies CJSC. Vendor of SPARC processor microarchitectures. */
		YepCpuVendorMCST      = 63
	};
	/**
	 * @ingroup yepLibrary_CpuInfo
	 * @brief	Returns information about the vendor of the processor.
	 * @param[out]	vendor	Pointer to a variable where information about the processor vendor will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a vendor parameter.
	 * @retval	#YepStatusNullPointer	The @a vendor pointer is null.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuVendor(enum YepCpuVendor *vendor);
	/**@}*/

	/** @name	Processor architecture information */
	/**@{*/
	/**
	 * @ingroup	yepLibrary_CpuInfo
	 * @brief	The basic instruction set architecture of the processor.
	 * @details	The ISA is always known at compile-time.
	 * @see	yepLibrary_GetCpuArchitecture
	 */
	enum YepCpuArchitecture {
		/** @brief	Instruction set architecture is not known to the library. */
		/** @details	This value is never returned on supported architectures. */
		YepCpuArchitectureUnknown = 0,
		/** @brief	x86 or x86-64 ISA. */
		YepCpuArchitectureX86 = 1,
		/** @brief	ARM ISA. */
		YepCpuArchitectureARM = 2,
		/** @brief	MIPS ISA. */
		YepCpuArchitectureMIPS = 3,
		/** @brief	PowerPC ISA. */
		YepCpuArchitecturePowerPC = 4,
		/** @brief	IA64 ISA. */
		YepCpuArchitectureIA64 = 5,
		/** @brief	SPARC ISA. */
		YepCpuArchitectureSPARC = 6
	};
	/**
	 * @ingroup yepLibrary_CpuInfo
	 * @brief	Returns the type of processor architecture.
	 * @param[out]	architecture	Pointer to a variable where information about the processor architecture will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a architecture parameter.
	 * @retval	#YepStatusNullPointer	The @a architecture pointer is null.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuArchitecture(enum YepCpuArchitecture *architecture);
	/**@}*/
	
	/** @name	Processor microarchitecture information */
	/**@{*/
	/**
	 * @ingroup	yepLibrary_CpuInfo
	 * @brief	Type of processor microarchitecture.
	 * @details	Low-level instruction performance characteristics, such as latency and throughput, are constant within microarchitecture.
	 *         	Processors of the same microarchitecture can differ in supported instruction sets and other extensions.
	 * @see	yepLibrary_GetCpuMicroarchitecture
	 */
	enum YepCpuMicroarchitecture {
		/** @brief Microarchitecture is unknown, or the library failed to get information about the microarchitecture from OS */
		YepCpuMicroarchitectureUnknown       = 0,
		
		/** @brief Pentium and Pentium MMX microarchitecture. */
		YepCpuMicroarchitectureP5            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0001,
		/** @brief Pentium Pro, Pentium II, and Pentium III. */
		YepCpuMicroarchitectureP6            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0002,
		/** @brief Pentium 4 with Willamette, Northwood, or Foster cores. */
		YepCpuMicroarchitectureWillamette    = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0003,
		/** @brief Pentium 4 with Prescott and later cores. */
		YepCpuMicroarchitecturePrescott      = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0004,
		/** @brief Pentium M. */
		YepCpuMicroarchitectureDothan        = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0005,
		/** @brief Intel Core microarchitecture. */
		YepCpuMicroarchitectureYonah         = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0006,
		/** @brief Intel Core 2 microarchitecture on 65 nm process. */
		YepCpuMicroarchitectureConroe        = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0007,
		/** @brief Intel Core 2 microarchitecture on 45 nm process. */
		YepCpuMicroarchitecturePenryn        = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0008,
		/** @brief Intel Atom on 45 nm process. */
		YepCpuMicroarchitectureBonnell       = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0009,
		/** @brief Intel Nehalem and Westmere microarchitectures (Core i3/i5/i7 1st gen). */
		YepCpuMicroarchitectureNehalem       = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000A,
		/** @brief Intel Sandy Bridge microarchitecture (Core i3/i5/i7 2nd gen). */
		YepCpuMicroarchitectureSandyBridge   = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000B,
		/** @brief Intel Atom on 32 nm process. */
		YepCpuMicroarchitectureSaltwell      = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000C,
		/** @brief Intel Ivy Bridge microarchitecture (Core i3/i5/i7 3rd gen). */
		YepCpuMicroarchitectureIvyBridge     = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000D,
		/** @brief Intel Haswell microarchitecture (Core i3/i5/i7 4th gen). */
		YepCpuMicroarchitectureHaswell       = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000E,
		/** @brief Intel Silvermont microarchitecture (22 nm out-of-order Atom). */
		YepCpuMicroarchitectureSilvermont    = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x000F,
		
		/** @brief Intel Knights Ferry HPC boards. */
		YepCpuMicroarchitectureKnightsFerry  = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0100,
		/** @brief Intel Knights Corner HPC boards (aka Xeon Phi). */
		YepCpuMicroarchitectureKnightsCorner = (YepCpuArchitectureX86 << 24) + (YepCpuVendorIntel << 16) + 0x0101,
		
		/** @brief AMD K5. */
		YepCpuMicroarchitectureK5            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0001,
		/** @brief AMD K6 and alike. */
		YepCpuMicroarchitectureK6            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0002,
		/** @brief AMD Athlon and Duron. */
		YepCpuMicroarchitectureK7            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0003,
		/** @brief AMD Geode GX and LX. */
		YepCpuMicroarchitectureGeode         = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0004,
		/** @brief AMD Athlon 64, Opteron 64. */
		YepCpuMicroarchitectureK8            = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0005,
		/** @brief AMD K10 (Barcelona, Istambul, Magny-Cours). */
		YepCpuMicroarchitectureK10           = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0006,
		/** @brief AMD Bobcat mobile microarchitecture. */
		YepCpuMicroarchitectureBobcat        = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0007,
		/** @brief AMD Bulldozer microarchitecture (1st gen K15). */
		YepCpuMicroarchitectureBulldozer     = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0008,
		/** @brief AMD Piledriver microarchitecture (2nd gen K15). */
		YepCpuMicroarchitecturePiledriver    = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x0009,
		/** @brief AMD Jaguar mobile microarchitecture. */
		YepCpuMicroarchitectureJaguar        = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x000A,
		/** @brief AMD Steamroller microarchitecture (3rd gen K15). */
		YepCpuMicroarchitectureSteamroller   = (YepCpuArchitectureX86 << 24) + (YepCpuVendorAMD   << 16) + 0x000B,
		
		/** @brief DEC/Intel StrongARM processors. */
		YepCpuMicroarchitectureStrongARM     = (YepCpuArchitectureARM << 24) + (YepCpuVendorIntel   << 16) + 0x0001,
		/** @brief Intel/Marvell XScale processors. */
		YepCpuMicroarchitectureXScale        = (YepCpuArchitectureARM << 24) + (YepCpuVendorIntel   << 16) + 0x0002,
		
		/** @brief ARM7 series. */
		YepCpuMicroarchitectureARM7          = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0001,
		/** @brief ARM9 series. */
		YepCpuMicroarchitectureARM9          = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0002,
		/** @brief ARM 1136, ARM 1156, ARM 1176, or ARM 11MPCore. */
		YepCpuMicroarchitectureARM11         = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0003,
		/** @brief ARM Cortex-A5. */
		YepCpuMicroarchitectureCortexA5      = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0004,
		/** @brief ARM Cortex-A7. */
		YepCpuMicroarchitectureCortexA7      = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0005,
		/** @brief ARM Cortex-A8. */
		YepCpuMicroarchitectureCortexA8      = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0006,
		/** @brief ARM Cortex-A9. */
		YepCpuMicroarchitectureCortexA9      = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0007,
		/** @brief ARM Cortex-A15. */
		YepCpuMicroarchitectureCortexA15     = (YepCpuArchitectureARM << 24) + (YepCpuVendorARM     << 16) + 0x0008,
		
		/** @brief Qualcomm Scorpion. */
		YepCpuMicroarchitectureScorpion      = (YepCpuArchitectureARM << 24) + (YepCpuVendorQualcomm << 16) + 0x0001,
		/** @brief Qualcomm Krait. */
		YepCpuMicroarchitectureKrait         = (YepCpuArchitectureARM << 24) + (YepCpuVendorQualcomm << 16) + 0x0002,
		
		/** @brief Marvell Sheeva PJ1. */
		YepCpuMicroarchitecturePJ1           = (YepCpuArchitectureARM << 24) + (YepCpuVendorMarvell << 16) + 0x0001,
		/** @brief Marvell Sheeva PJ4. */
		YepCpuMicroarchitecturePJ4           = (YepCpuArchitectureARM << 24) + (YepCpuVendorMarvell << 16) + 0x0002,
		
		/** @brief Apple A6 and A6X processors. */
		YepCpuMicroarchitectureSwift         = (YepCpuArchitectureARM << 24) + (YepCpuVendorApple   << 16) + 0x0001,

		/** @brief Intel Itanium. */
		YepCpuMicroarchitectureItanium       = (YepCpuArchitectureIA64 << 24) + (YepCpuVendorIntel << 16) + 0x0001,
		/** @brief Intel Itanium 2. */
		YepCpuMicroarchitectureItanium2      = (YepCpuArchitectureIA64 << 24) + (YepCpuVendorIntel << 16) + 0x0002,
		
		/** @brief MIPS 24K. */
		YepCpuMicroarchitectureMIPS24K       = (YepCpuArchitectureMIPS << 24) + (YepCpuVendorMIPS << 16) + 0x0001,
		/** @brief MIPS 34K. */
		YepCpuMicroarchitectureMIPS34K       = (YepCpuArchitectureMIPS << 24) + (YepCpuVendorMIPS << 16) + 0x0002,
		/** @brief MIPS 74K. */
		YepCpuMicroarchitectureMIPS74K       = (YepCpuArchitectureMIPS << 24) + (YepCpuVendorMIPS << 16) + 0x0003,
		
		/** @brief Ingenic XBurst. */
		YepCpuMicroarchitectureXBurst        = (YepCpuArchitectureMIPS << 24) + (YepCpuVendorIngenic << 16) + 0x0001,
		/** @brief Ingenic XBurst 2. */
		YepCpuMicroarchitectureXBurst2       = (YepCpuArchitectureMIPS << 24) + (YepCpuVendorIngenic << 16) + 0x0002
	};
	/**
	 * @ingroup yepLibrary_CpuInfo
	 * @brief	Returns the type of processor microarchitecture used.
	 * @param[out]	microarchitecture	Pointer to a variable where information about the processor microarchitecture will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a microarchitecture parameter.
	 * @retval	#YepStatusNullPointer	The @a microarchitecture pointer is null.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuMicroarchitecture(enum YepCpuMicroarchitecture *microarchitecture);
	/**@}*/

	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuDataCacheSize(Yep32u level, Yep32u *cacheSize);
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuInstructionCacheSize(Yep32u level, Yep32u *cacheSize);
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetLogicalCoresCount(Yep32u *logicalCoresCount);

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_CycleCounter	Cycle counter access
	 */

	/**
	 * @ingroup yepLibrary_CycleCounter
	 * @brief	Initializes the processor cycle counter and starts counting the processor cycles.
	 * @details	In the current implementation this function can use:
	 *         	 - RDTSC or RDTSCP instructions on x86 and x86-64.
	 *         	 - ITC register on IA64.
	 *         	 - Linux perf events subsystem on ARM and MIPS. This option requires unrestricted access to perf events subsystem (file /proc/sys/kernel/perf_event_paranoid should contain 0 or -1, if this file does not exist, the kernel is compiled without perf events subsystem).
	 *         	 - PMCCNTR register on ARM if user-mode access to performance counters is enabled and the counter is properly configured. This option is intended for use with the kernel-mode driver in drivers/arm_pmu directory in @Yeppp distribution. This option provides only 32-bit cycle counter.
	 * @warning	The state is not guaranteed to be the current processor cycle counter value, and should not be used as such.
	 * @warning	This function may allocate system resources.
	 *         	To avoid resource leak, always match a successfull call to #yepLibrary_GetCpuCyclesAcquire with a call to #yepLibrary_GetCpuCyclesRelease.
	 * @warning	The cycle counters are not guaranteed to be syncronized across different processors/cores in a multiprocessor/multicore system.
	 *         	It is recommended to bind the current thread to a particular logical processor before using this function.
	 * @param[out]	state	Pointer to a variable where the state of the cycle counter will be stored.
	 *            	     	If the function fails, the value of the state variable is not changed.
	 * @retval	#YepStatusOk	The cycle counter successfully initialized and its state is stored to the variable pointed by @a state parameter.
	 * @retval	#YepStatusNullPointer	The @a state pointer is null.
	 * @retval	#YepStatusUnsupportedHardware	The processor does not have cycle counter.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the CPU cycle counter.
	 * @retval	#YepStatusSystemError	An attempt to initialize cycle counter failed inside the OS kernel.
	 * @see	yepLibrary_GetCpuCyclesRelease
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuCyclesAcquire(Yep64u *state);
	/**
	 * @ingroup yepLibrary_CycleCounter
	 * @brief	Stops counting the processor cycles, releases the system resources associated with the cycle counter, and returns the number of cycles elapsed.
	 * @param[in,out]	state	Pointer to a variable with the state of the cycle counter saved by #yepLibrary_GetCpuCyclesAcquire.
	 *               	     	The cycle counter should be released only once, and the function zeroes out the state variable.
	 * @param[out]	cycles	Pointer to a variable where the number of cycles elapsed will be stored.
	 *            	      	The pointer can be the same as @a state pointer.
	 * @retval	#YepStatusOk	The number of cycles elapsed is saved to the variable pointed by @a cycles parameter, and the system resources are successfully released.
	 * @retval	#YepStatusNullPointer	Either the @a state pointer or the @a cycles pointer is null.
	 * @retval	#YepStatusInvalidState	The @a state variable does not specify a valid state of the cycle counter.
	 *        	                     	This can happen if the @a state variable was not initialized, or it was released previously.
	 * @retval	#YepStatusUnsupportedHardware	The processor does not have cycle counter.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the CPU cycle counter.
	 * @retval	#YepStatusSystemError	An attempt to read the cycle counter or release the OS resources failed inside the OS kernel.
	 * @see yepLibrary_GetCpuCyclesAcquire
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetCpuCyclesRelease(Yep64u *state, Yep64u *cycles);

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_EnergyCounter	Energy counter access
	 */

	/**
	 * @ingroup	yepLibrary_EnergyCounter
	 * @brief	Energy counter state.
	 * @see	yepLibrary_GetEnergyCounterAcquire, yepLibrary_GetEnergyCounterRelease
	 */
	struct YepEnergyCounter {
		Yep64u state[6];
	};
	/**
	 * @ingroup	yepLibrary_EnergyCounter
	 * @brief	Energy counter type.
	 * @see	yepLibrary_GetEnergyCounterAcquire
	 */
	enum YepEnergyCounterType {
		/** @brief	Intel RAPL per-package energy counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the energy (in Joules) consumed by all chips in the CPU package. */
		YepEnergyCounterTypeRaplPackageEnergy = 1,
		/** @brief	Intel RAPL power plane 0 energy counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the energy (in Joules) consumed by power plane 0 (includes CPU cores and caches). */
		YepEnergyCounterTypeRaplPowerPlane0Energy = 2,
		/** @brief	Intel RAPL power plane 1 energy counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the energy (in Joules) consumed by power plane 1 (includes GPU cores). */
		YepEnergyCounterTypeRaplPowerPlane1Energy = 3,
		/** @brief	Intel RAPL DRAM energy counter.
		 *  @details	This counter is supported on Intel Sandy Bridge E processors, and estimates the energy (in Joules) consumed by DRAM modules.
		 *          	Motherboard support is required to use this counter. */
		YepEnergyCounterTypeRaplDRAMEnergy = 4,
		/** @brief	Intel RAPL per-package power counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the average power (in Watts) consumed by all chips in the CPU package.
		 *          	This counter is implemented as a combination of RAPL per-package energy counter and system timer. */
		YepEnergyCounterTypeRaplPackagePower = 5,
		/** @brief	Intel RAPL power plane 0 power counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the average power (in Watts) consumed by power plane 0 (includes CPU cores and caches).
		 *          	This counter is implemented as a combination of RAPL power plane 0 energy counter and system timer. */
		YepEnergyCounterTypeRaplPowerPlane0Power = 6,
		/** @brief	Intel RAPL power plane 1 power counter.
		 *  @details	This counter is supported on Intel Sandy Bridge and Ivy Bridge processors, and estimates the average power (in Watts) consumed by power plane 1 (includes GPU cores).
		 *          	This counter is implemented as a combination of RAPL power plane 1 energy counter and system timer. */
		YepEnergyCounterTypeRaplPowerPlane1Power = 7,
		/** @brief	Intel RAPL DRAM power counter.
		 *  @details	This counter is supported on Intel Sandy Bridge E processors, and estimates the average power (in Watts) consumed by DRAM modules.
		 *          	This counter is implemented as a combination of RAPL DRAM energy counter and system timer.
		 *          	Motherboard support is required to use this counter. */
		YepEnergyCounterTypeRaplDRAMPower = 8
	};
	/**
	 * @ingroup yepLibrary_EnergyCounter
	 * @brief	Initializes the specified energy counter and starts energy measurements.
	 * @param[in]	type	The type of the energy counter to initialize.
	 * @param[out]	state	The state variable corresponding to the initialized energy counter.
	 *            	     	If the function fails, the value of the state variable is not changed.
	 *            	     	It is recommended to initialize the state variables to all zeroes before calling this function.
	 * @retval	#YepStatusOk	The energy counter successfully initialized and its state is store to the variable pointed by @a state parameter.
	 * @retval	#YepStatusNullPointer	The @a state pointer is null.
	 * @retval	#YepStatusInvalidArgument	The @a type parameter does not specify a valid energy counter type.
	 * @retval	#YepStatusUnsupportedHardware	The hardware does not support the requested energy counter type.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the specified energy counter.
	 * @retval	#YepStatusSystemError	An attempt to read the energy counter or release the OS resources failed inside the OS kernel.
	 * @retval	#YepStatusAccessDenied	The user does not possess the required access rights to read the energy counter.
	 * @see yepLibrary_GetEnergyCounterRelease
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetEnergyCounterAcquire(enum YepEnergyCounterType type, struct YepEnergyCounter *state);
	/**
	 * @ingroup yepLibrary_EnergyCounter
	 * @brief	Stops the energy counter, releases the system resources associated with the energy counter, and reads the counter measurement.
	 * @param[in,out]	state	Pointer to a variable with the state of the energy counter saved by #yepLibrary_GetEnergyCounterAcquire.
	 *               	     	The energy counter should be released only once, and the function zeroes out the state variable.
	 * @param[out]	measurement	Pointer to a variable where the number of cycles elapsed will be stored.
	 *            	      	The pointer can be the same as @a state pointer.
	 * @retval	#YepStatusOk	The energy counter measurement is saved to the variable pointed by @a measurement parameter, and the system resources are successfully released.
	 * @retval	#YepStatusNullPointer	Either the @a state pointer or the @a measurement pointer is null.
	 * @retval	#YepStatusInvalidState	The @a state variable does not specify a valid state of the energy counter.
	 *        	                      	This can happen if the @a state variable was not initialized, or it was released previously.
	 * @retval	#YepStatusUnsupportedHardware	The hardware does not support the requested energy counter type.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the specified energy counter.
	 * @retval	#YepStatusSystemError	An attempt to read the energy counter or release the OS resources failed inside the OS kernel.
	 * @retval	#YepStatusAccessDenied	The user does not possess the required access rights to read the energy counter.
	 * @see yepLibrary_GetEnergyCounterAcquire
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetEnergyCounterRelease(struct YepEnergyCounter *state, Yep64f *measurement);

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_Timer	System timer access
	 */

	/**
	 * @ingroup yepLibrary_Timer
	 * @brief	Returns the number of ticks of the high-resolution system timer.
	 * @param[out]	ticks	Pointer to a variable where the number of timer ticks will be stored.
	 *            	     	If the function fails, the value of the variable at this address is not changed.
	 * @retval	#YepStatusOk	The number of timer ticks is successfully stored to the variable pointed by @a ticks parameter.
	 * @retval	#YepStatusNullPointer	The @a ticks pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to read the high-resolution timer failed inside the OS kernel.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetTimerTicks(Yep64u *ticks);
	/**
	 * @ingroup yepLibrary_Timer
	 * @brief	Returns the number of ticks of the system timer per second.
	 * @param[out]	frequency	Pointer to a variable where the number of timer ticks per second will be stored.
	 * @retval	#YepStatusOk	The number of timer ticks is successfully stored to the variable pointed by @a frequency parameter.
	 * @retval	#YepStatusNullPointer	The @a frequency pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to query the high-resolution timer parameters failed inside the OS kernel.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetTimerFrequency(Yep64u *frequency);
	/**
	 * @ingroup yepLibrary_Timer
	 * @brief	Returns the minimum time difference in nanoseconds which can be measured by the high-resolution system timer.
	 * @param[out]	accuracy	Pointer to a variable where the timer accuracy will be stored.
	 * @retval	#YepStatusOk	The accuracy of the timer is successfully stored to the variable pointed by @a accuracy parameter.
	 * @retval	#YepStatusNullPointer	The @a accuracy pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to measure the accuracy of high-resolution timer failed inside the OS kernel.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetTimerAccuracy(Yep64u *accuracy);

	/**
	 * @ingroup yepLibrary
	 * @defgroup yepLibrary_ToString	String representation for enumeration values
	 */
	/** @ingroup	yepLibrary_ToString */
	/** @brief	Returns the #YepEnumeration value corresponding to ISA features of the given architecture */
	/** @see	YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE, YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE, YepEnumeration */
	#define YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(architecture) (256 + (architecture))
	/** @ingroup	yepLibrary_ToString */
	/** @brief	Returns the #YepEnumeration value corresponding to SIMD features of the given architecture */
	/** @see	YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE, YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE, YepEnumeration */
	#define YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(architecture) (512 + (architecture))
	/** @ingroup	yepLibrary_ToString */
	/** @brief	Returns the #YepEnumeration value corresponding to non-ISA or system features of the given architecture */
	/** @see	YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE, YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE, YepEnumeration */
	#define YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(architecture) (768 + (architecture))
	/** @ingroup	yepLibrary_ToString */
	/** @brief	Indicates how to interpret integer value of @Yeppp enumeration. */
	/** @see	yepLibrary_GetString */
	enum YepEnumeration {
		/** @brief	The enumeration type is #YepStatus */
		YepEnumerationStatus = 0,
		/** @brief The enumeration type is #YepCpuArchitecture */
		YepEnumerationCpuArchitecture = 1,
		/** @brief The enumeration type is #YepCpuVendor */
		YepEnumerationCpuVendor = 2,
		/** @brief The enumeration type is #YepCpuMicroarchitecture */
		YepEnumerationCpuMicroarchitecture = 3,
		/** @brief The enumeration type is one of the processor packages for which a brief name (without vendor name) will be requested */
		YepEnumerationCpuBriefName = 4,
		/** @brief The enumeration type is one of the processor packages for which a full name (including vendor name) will be requested */
		YepEnumerationCpuFullName = 5,
		/** @brief The enumeration type is one of the common ISA features constants */
		YepEnumerationGenericIsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureUnknown),
		/** @brief The enumeration type is one of the common SIMD features constants */
		YepEnumerationGenericSimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureUnknown),
		/** @brief The enumeration type is one of the common non-ISA or system features constants */
		YepEnumerationGenericSystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureUnknown),
		/** @brief The enumeration type is one of the x86 ISA features constants */
		YepEnumerationX86IsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureX86),
		/** @brief The enumeration type is one of the x86 SIMD features constants */
		YepEnumerationX86SimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureX86),
		/** @brief The enumeration type is one of the x86 non-ISA or system features constants */
		YepEnumerationX86SystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureX86),
		/** @brief The enumeration type is one of the ARM ISA features constants */
		YepEnumerationARMIsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureARM),
		/** @brief The enumeration type is one of the ARM SIMD features constants */
		YepEnumerationARMSimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureARM),
		/** @brief The enumeration type is one of the ARM non-ISA or system features constants */
		YepEnumerationARMSystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureARM),
		/** @brief The enumeration type is one of the MIPS ISA features constants */
		YepEnumerationMIPSIsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureMIPS),
		/** @brief The enumeration type is one of the MIPS SIMD features constants */
		YepEnumerationMIPSSimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureMIPS),
		/** @brief The enumeration type is one of the MIPS non-ISA or system features constants */
		YepEnumerationMIPSSystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureMIPS),
		/** @brief The enumeration type is one of the PowerPC ISA features constants */
		YepEnumerationPowerPCIsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitecturePowerPC),
		/** @brief The enumeration type is one of the PowerPC SIMD features constants */
		YepEnumerationPowerPCSimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitecturePowerPC),
		/** @brief The enumeration type is one of the PowerPC non-ISA or system features constants */
		YepEnumerationPowerPCSystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitecturePowerPC),
		/** @brief The enumeration type is one of the IA64 ISA features constants */
		YepEnumerationIA64IsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureIA64),
		/** @brief The enumeration type is one of the IA64 SIMD features constants */
		YepEnumerationIA64SimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureIA64),
		/** @brief The enumeration type is one of the IA64 non-ISA or system features constants */
		YepEnumerationIA64SystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureIA64),
		/** @brief The enumeration type is one of the SPARC ISA features constants */
		YepEnumerationSPARCIsaFeature = YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureSPARC),
		/** @brief The enumeration type is one of the SPARC SIMD features constants */
		YepEnumerationSPARCSimdFeature = YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureSPARC),
		/** @brief The enumeration type is one of the SPARC non-ISA or system features constants */
		YepEnumerationSPARCSystemFeature = YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(YepCpuArchitectureSPARC)
	};
	/** @ingroup	yepLibrary_ToString */
	/** @brief	Indicates the type of requested string representation. */
	/** @see	yepLibrary_GetString */
	enum YepStringType {
		/** @brief Description string. Description can contain spaces, typography symbols, and non-ASCII characters */
		YepStringTypeDescription = 0,
		/** @brief ID string. ID must start with a Latin letter, and can contain only Latin letter (uppercase or lowercase), digits, and underscore symbol */
		YepStringTypeID = 1
	};
	/**
	 * @ingroup yepLibrary_ToString
	 * @brief	Returns a string representation of an integer value in a enumeration.
	 * @param[in]	enumerationType	Indicates the type of integer value passed to the function in @a enumerationValue parameter.
	 * @param[in]	enumerationValue	The enumeration value of type specified in @a enumerationType which must be converted to string.
	 *          	The interpretation of @a enumerationValue parameter depends on @a enumerationType as follows:
	 *          	<table>
	 *          		<tr>
	 *          			<th>enumerationType</th>
	 *          			<th>enumerationValue</th>
	 *          			<th>Valid stringType</th>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>#YepEnumerationStatus</td>
	 *          			<td>Numerical value in #YepStatus enumeration</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>#YepEnumerationCpuArchitecture</td>
	 *          			<td>Numerical value in #YepCpuArchitecture enumeration</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>#YepEnumerationCpuVendor</td>
	 *          			<td>Numerical value in #YepCpuVendor enumeration</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>#YepEnumerationCpuBriefName</td>
	 *          			<td>Processor package id (must be 0)</td>
	 *          			<td>@ref YepStringTypeDescription "Description"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>#YepEnumerationCpuFullName</td>
	 *          			<td>Processor package id (must be 0)</td>
	 *          			<td>@ref YepStringTypeDescription "Description"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>@ref YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE "YepEnumeration*IsaFeature"</td>
	 *          			<td>Position of non-zero bit in ISA feature flag</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>@ref YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE "YepEnumeration*SimdFeature"</td>
	 *          			<td>Position of non-zero bit in SIMD feature flag</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          		<tr>
	 *          			<td>@ref YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE "YepEnumeration*SystemFeature"</td>
	 *          			<td>Position of non-zero bit in system feature flag</td>
	 *          			<td>@ref YepStringTypeDescription "Description", @ref YepStringTypeID "ID"</td>
	 *          		</tr>
	 *          	</table>
	 * @param[in]	stringType	Indicates the type of requiested string.
	 * @param[out]	buffer	An output buffer of size specified by the @a length parameter. If this pointer is null, required buffer size will be stored in length variable. On successfull return the buffer will contain the string representation of @a enumerationValue. The string representation does not include the terminating zero. If the function fails, the content of the buffer is not changed.
	 * @param[in,out]	length	On function call this variable must contain the length (in bytes) of the buffer. On successfull return this variable will contain the length (in bytes) of the string written to the buffer. If the function fails with YepStatusInsufficientBuffer error, on return the @a length variable will contain the required size of the buffer. In the function fails with any other error, this variable is unchanged.
	 * @retval	#YepStatusOk	The string is successfully stored in the @a buffer.
	 * @retval	#YepStatusNullPointer	Length pointer is null.
	 * @retval	#YepStatusInvalidArgument	The values of @a enumerationType, @a enumerationValue or @a stringType are unknown to the library, or they are provided in invalid combination.
	 * @retval	#YepStatusInsufficientBuffer	The buffer pointer is null or the output buffer is too small for the string. The content of the output buffer is unchanged, and the required size of the buffer is returned in the length variable.
	 */
	YEP_PUBLIC_SYMBOL enum YepStatus YEPABI yepLibrary_GetString(enum YepEnumeration enumerationType, Yep32u enumerationValue, enum YepStringType stringType, void *buffer, YepSize *length);

#ifdef __cplusplus
	const Yep64u YepIsaFeaturesDefault        = 0x0000000000000000ull;
	const Yep64u YepSimdFeaturesDefault       = 0x0000000000000000ull;
	const Yep64u YepSystemFeaturesDefault     = 0x0000000000000000ull;
#else
	#define YepIsaFeaturesDefault               0x0000000000000000ull
	#define YepSimdFeaturesDefault              0x0000000000000000ull
	#define YepSystemFeaturesDefault            0x0000000000000000ull
#endif

#ifdef __cplusplus
	const Yep64u YepSystemFeatureCycleCounter      = 0x0000000000000001ull;
	const Yep64u YepSystemFeatureCycleCounter64Bit = 0x0000000000000002ull;
	const Yep64u YepSystemFeatureAddressSpace64Bit = 0x0000000000000004ull;
	const Yep64u YepSystemFeatureGPRegisters64Bit  = 0x0000000000000008ull;
	const Yep64u YepSystemFeatureMisalignedAccess  = 0x0000000000000010ull;
	const Yep64u YepSystemFeatureSingleThreaded    = 0x0000000000000020ull;
#else
	/** @anchor	Common_CPU_and_System_Features
	 *  @name	Common CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor has a built-in cycle counter, and the operating system provides a way to access it. */
	#define YepSystemFeatureCycleCounter       0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor has a 64-bit cycle counter, or the operating system provides an abstraction of a 64-bit cycle counter. */
	#define YepSystemFeatureCycleCounter64Bit  0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor and the operating system allows to use 64-bit pointers. */
	#define YepSystemFeatureAddressSpace64Bit  0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor and the operating system allows to do 64-bit arithmetical operations on general-purpose registers. */
	#define YepSystemFeatureGPRegisters64Bit   0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor and the operating system allows misaligned memory reads and writes. */
	#define YepSystemFeatureMisalignedAccess   0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The processor or the operating system support at most one hardware thread. */
	#define YepSystemFeatureSingleThreaded     0x0000000000000020ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepX86IsaFeatureFPU          = 0x0000000000000001ull;
	const Yep64u YepX86IsaFeatureCpuid        = 0x0000000000000002ull;
	const Yep64u YepX86IsaFeatureRdtsc        = 0x0000000000000004ull;
	const Yep64u YepX86IsaFeatureCMOV         = 0x0000000000000008ull;
	const Yep64u YepX86IsaFeatureSYSENTER     = 0x0000000000000010ull;
	const Yep64u YepX86IsaFeatureSYSCALL      = 0x0000000000000020ull;
	const Yep64u YepX86IsaFeatureMSR          = 0x0000000000000040ull;
	const Yep64u YepX86IsaFeatureClflush      = 0x0000000000000080ull;
	const Yep64u YepX86IsaFeatureMONITOR      = 0x0000000000000100ull;
	const Yep64u YepX86IsaFeatureFXSAVE       = 0x0000000000000200ull;
	const Yep64u YepX86IsaFeatureXSAVE        = 0x0000000000000400ull;
	const Yep64u YepX86IsaFeatureCmpxchg8b    = 0x0000000000000800ull;
	const Yep64u YepX86IsaFeatureCmpxchg16b   = 0x0000000000001000ull;
	const Yep64u YepX86IsaFeatureX64          = 0x0000000000002000ull;
	const Yep64u YepX86IsaFeatureLahfSahf64   = 0x0000000000004000ull;
	const Yep64u YepX86IsaFeatureFsGsBase     = 0x0000000000008000ull;
	const Yep64u YepX86IsaFeatureMovbe        = 0x0000000000010000ull;
	const Yep64u YepX86IsaFeaturePopcnt       = 0x0000000000020000ull;
	const Yep64u YepX86IsaFeatureLzcnt        = 0x0000000000040000ull;
	const Yep64u YepX86IsaFeatureBMI          = 0x0000000000080000ull;
	const Yep64u YepX86IsaFeatureBMI2         = 0x0000000000100000ull;
	const Yep64u YepX86IsaFeatureTBM          = 0x0000000000200000ull;
	const Yep64u YepX86IsaFeatureRdrand       = 0x0000000000400000ull;
	const Yep64u YepX86IsaFeatureACE          = 0x0000000000800000ull;
	const Yep64u YepX86IsaFeatureACE2         = 0x0000000001000000ull;
	const Yep64u YepX86IsaFeatureRNG          = 0x0000000002000000ull;
	const Yep64u YepX86IsaFeaturePHE          = 0x0000000004000000ull;
	const Yep64u YepX86IsaFeaturePMM          = 0x0000000008000000ull;
	const Yep64u YepX86IsaFeatureAES          = 0x0000000010000000ull;
	const Yep64u YepX86IsaFeaturePclmulqdq    = 0x0000000020000000ull;
	const Yep64u YepX86IsaFeatureRdtscp       = 0x0000000040000000ull;
	const Yep64u YepX86IsaFeatureLWP          = 0x0000000080000000ull;
	const Yep64u YepX86IsaFeatureHLE          = 0x0000000100000000ull;
	const Yep64u YepX86IsaFeatureRTM          = 0x0000000200000000ull;
	const Yep64u YepX86IsaFeatureXtest        = 0x0000000400000000ull;
	const Yep64u YepX86IsaFeatureRdseed       = 0x0000000800000000ull;
	const Yep64u YepX86IsaFeatureADX          = 0x0000001000000000ull;
	const Yep64u YepX86IsaFeatureSHA          = 0x0000002000000000ull;
	const Yep64u YepX86IsaFeatureMPX          = 0x0000004000000000ull;
#else
	/** @anchor	x86_ISA_Extensions
	 *  @name	x86 and x86-64 ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief x87 FPU integrated on chip. */
	#define YepX86IsaFeatureFPU                   0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief x87 CPUID instruction. */
	#define YepX86IsaFeatureCpuid                 0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDTSC instruction. */
	#define YepX86IsaFeatureRdtsc                 0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief CMOV, FCMOV, and FCOMI/FUCOMI instructions. */
	#define YepX86IsaFeatureCMOV                  0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SYSENTER and SYSEXIT instructions. */
	#define YepX86IsaFeatureSYSENTER              0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SYSCALL and SYSRET instructions. */
	#define YepX86IsaFeatureSYSCALL               0x0000000000000020ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDMSR and WRMSR instructions. */
	#define YepX86IsaFeatureMSR                   0x0000000000000040ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief CLFLUSH instruction. */
	#define YepX86IsaFeatureClflush               0x0000000000000080ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MONITOR and MWAIT instructions. */
	#define YepX86IsaFeatureMONITOR               0x0000000000000100ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief FXSAVE and FXRSTOR instructions. */
	#define YepX86IsaFeatureFXSAVE                0x0000000000000200ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief XSAVE, XRSTOR, XGETBV, and XSETBV instructions. */
	#define YepX86IsaFeatureXSAVE                 0x0000000000000400ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief CMPXCHG8B instruction. */
	#define YepX86IsaFeatureCmpxchg8b             0x0000000000000800ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief CMPXCHG16B instruction. */
	#define YepX86IsaFeatureCmpxchg16b            0x0000000000001000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Support for 64-bit mode. */
	#define YepX86IsaFeatureX64                   0x0000000000002000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Support for LAHF and SAHF instructions in 64-bit mode. */
	#define YepX86IsaFeatureLahfSahf64            0x0000000000004000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions. */
	#define YepX86IsaFeatureFsGsBase              0x0000000000008000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MOVBE instruction. */
	#define YepX86IsaFeatureMovbe                 0x0000000000010000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief POPCNT instruction. */
	#define YepX86IsaFeaturePopcnt                0x0000000000020000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief LZCNT instruction. */
	#define YepX86IsaFeatureLzcnt                 0x0000000000040000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief BMI instruction set. */
	#define YepX86IsaFeatureBMI                   0x0000000000080000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief BMI 2 instruction set. */
	#define YepX86IsaFeatureBMI2                  0x0000000000100000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief TBM instruction set. */
	#define YepX86IsaFeatureTBM                   0x0000000000200000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDRAND instruction. */
	#define YepX86IsaFeatureRdrand                0x0000000000400000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Padlock Advanced Cryptography Engine on chip. */
	#define YepX86IsaFeatureACE                   0x0000000000800000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Padlock Advanced Cryptography Engine 2 on chip. */
	#define YepX86IsaFeatureACE2                  0x0000000001000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Padlock Random Number Generator on chip. */
	#define YepX86IsaFeatureRNG                   0x0000000002000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Padlock Hash Engine on chip. */
	#define YepX86IsaFeaturePHE                   0x0000000004000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Padlock Montgomery Multiplier on chip. */
	#define YepX86IsaFeaturePMM                   0x0000000008000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AES instruction set. */
	#define YepX86IsaFeatureAES                   0x0000000010000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief PCLMULQDQ instruction. */
	#define YepX86IsaFeaturePclmulqdq             0x0000000020000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDTSCP instruction. */
	#define YepX86IsaFeatureRdtscp                0x0000000040000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Lightweight Profiling extension. */
	#define YepX86IsaFeatureLWP                   0x0000000080000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Hardware Lock Elision extension. */
	#define YepX86IsaFeatureHLE                   0x0000000100000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Restricted Transactional Memory extension. */
	#define YepX86IsaFeatureRTM                   0x0000000200000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief XTEST instruction. */
	#define YepX86IsaFeatureXtest                 0x0000000400000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief RDSEED instruction. */
	#define YepX86IsaFeatureRdseed                0x0000000800000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ADCX and ADOX instructions. */
	#define YepX86IsaFeatureADX                   0x0000001000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SHA instruction set. */
	#define YepX86IsaFeatureSHA                   0x0000002000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Memory Protection Extension. */
	#define YepX86IsaFeatureMPX                   0x0000004000000000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepX86SimdFeatureMMX                  = 0x0000000000000001ull;
	const Yep64u YepX86SimdFeatureMMXPlus              = 0x0000000000000002ull;
	const Yep64u YepX86SimdFeatureEMMX                 = 0x0000000000000004ull;
	const Yep64u YepX86SimdFeature3dnow                = 0x0000000000000008ull;
	const Yep64u YepX86SimdFeature3dnowPlus            = 0x0000000000000010ull;
	const Yep64u YepX86SimdFeature3dnowPrefetch        = 0x0000000000000020ull;
	const Yep64u YepX86SimdFeature3dnowGeode           = 0x0000000000000040ull;
	const Yep64u YepX86SimdFeatureSSE                  = 0x0000000000000080ull;
	const Yep64u YepX86SimdFeatureSSE2                 = 0x0000000000000100ull;
	const Yep64u YepX86SimdFeatureSSE3                 = 0x0000000000000200ull;
	const Yep64u YepX86SimdFeatureSSSE3                = 0x0000000000000400ull;
	const Yep64u YepX86SimdFeatureSSE4_1               = 0x0000000000000800ull;
	const Yep64u YepX86SimdFeatureSSE4_2               = 0x0000000000001000ull;
	const Yep64u YepX86SimdFeatureSSE4A                = 0x0000000000002000ull;
	const Yep64u YepX86SimdFeatureAVX                  = 0x0000000000004000ull;
	const Yep64u YepX86SimdFeatureAVX2                 = 0x0000000000008000ull;
	const Yep64u YepX86SimdFeatureXOP                  = 0x0000000000010000ull;
	const Yep64u YepX86SimdFeatureF16C                 = 0x0000000000020000ull;
	const Yep64u YepX86SimdFeatureFMA3                 = 0x0000000000040000ull;
	const Yep64u YepX86SimdFeatureFMA4                 = 0x0000000000080000ull;
	const Yep64u YepX86SimdFeatureKNF                  = 0x0000000000100000ull;
	const Yep64u YepX86SimdFeatureKNC                  = 0x0000000000200000ull;
	const Yep64u YepX86SimdFeatureAVX512F              = 0x0000000000400000ull;
	const Yep64u YepX86SimdFeatureAVX512CD             = 0x0000000000800000ull;
	const Yep64u YepX86SimdFeatureAVX512ER             = 0x0000000001000000ull;
	const Yep64u YepX86SimdFeatureAVX512PF             = 0x0000000002000000ull;
#else
	/** @anchor	x86_SIMD_Extensions
	 *  @name	x86 and x86-64 SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MMX instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION */
	#define YepX86SimdFeatureMMX                  0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MMX+ instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION */
	#define YepX86SimdFeatureMMXPlus              0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief EMMX instruction set. */
	#define YepX86SimdFeatureEMMX                 0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief 3dnow! instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION */
	#define YepX86SimdFeature3dnow                0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief 3dnow!+ instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION */
	#define YepX86SimdFeature3dnowPlus            0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief 3dnow! prefetch instructions. */
	#define YepX86SimdFeature3dnowPrefetch        0x0000000000000020ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Geode 3dnow! instructions. */
	#define YepX86SimdFeature3dnowGeode           0x0000000000000040ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION */
	#define YepX86SimdFeatureSSE                  0x0000000000000080ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE 2 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION */
	#define YepX86SimdFeatureSSE2                 0x0000000000000100ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE 3 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION */
	#define YepX86SimdFeatureSSE3                 0x0000000000000200ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSSE 3 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION */
	#define YepX86SimdFeatureSSSE3                0x0000000000000400ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE 4.1 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION */
	#define YepX86SimdFeatureSSE4_1               0x0000000000000800ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE 4.2 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION */
	#define YepX86SimdFeatureSSE4_2               0x0000000000001000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SSE 4A instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION */
	#define YepX86SimdFeatureSSE4A                0x0000000000002000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION */
	#define YepX86SimdFeatureAVX                  0x0000000000004000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX 2 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION */
	#define YepX86SimdFeatureAVX2                 0x0000000000008000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief XOP instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION */
	#define YepX86SimdFeatureXOP                  0x0000000000010000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief F16C instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION */
	#define YepX86SimdFeatureF16C                 0x0000000000020000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief FMA3 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION */
	#define YepX86SimdFeatureFMA3                 0x0000000000040000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief FMA4 instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION */
	#define YepX86SimdFeatureFMA4                 0x0000000000080000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Knights Ferry (aka Larrabee) instruction set. */
	#define YepX86SimdFeatureKNF                  0x0000000000100000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Knights Corner (aka Xeon Phi) instruction set. */
	/** @see YEP_COMPILER_SUPPORTS_X86_KNC_EXTENSION */
	#define YepX86SimdFeatureKNC                  0x0000000000200000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX-512 Foundation instruction set. */
	#define YepX86SimdFeatureAVX512F              0x0000000000400000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX-512 Conflict Detection instruction set. */
	#define YepX86SimdFeatureAVX512CD             0x0000000000800000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX-512 Exponential and Reciprocal instruction set. */
	#define YepX86SimdFeatureAVX512ER             0x0000000001000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief AVX-512 Prefetch instruction set. */
	#define YepX86SimdFeatureAVX512PF             0x0000000002000000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepX86SystemFeatureACE                = 0x0000000100000000ull;
	const Yep64u YepX86SystemFeatureACE2               = 0x0000000200000000ull;
	const Yep64u YepX86SystemFeatureRNG                = 0x0000000400000000ull;
	const Yep64u YepX86SystemFeaturePHE                = 0x0000000800000000ull;
	const Yep64u YepX86SystemFeaturePMM                = 0x0000001000000000ull;
	const Yep64u YepX86SystemFeatureMisalignedSSE      = 0x0000002000000000ull;
	const Yep64u YepX86SystemFeatureFPU                = 0x0010000000000000ull;
	const Yep64u YepX86SystemFeatureXMM                = 0x0020000000000000ull;
	const Yep64u YepX86SystemFeatureYMM                = 0x0040000000000000ull;
	const Yep64u YepX86SystemFeatureZMM                = 0x0080000000000000ull;
	const Yep64u YepX86SystemFeatureBND                = 0x0100000000000000ull;
#else
	/** @anchor	x86_CPU_and_System_Features
	 *  @name	x86 and x86-64 CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine. */
	#define YepX86SystemFeatureACE                0x0000000100000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine 2. */
	#define YepX86SystemFeatureACE2               0x0000000200000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor and the operating system support the Padlock Random Number Generator. */
	#define YepX86SystemFeatureRNG                0x0000000400000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor and the operating system support the Padlock Hash Engine. */
	#define YepX86SystemFeaturePHE                0x0000000800000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor and the operating system support the Padlock Montgomery Multiplier. */
	#define YepX86SystemFeaturePMM                0x0000001000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Processor allows to use misaligned memory operands in SSE instructions other than loads and stores. */
	#define YepX86SystemFeatureMisalignedSSE      0x0000002000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has x87 FPU registers, and the operating system preserves them during context switch. */
	#define YepX86SystemFeatureFPU                0x0010000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has xmm (SSE) registers, and the operating system preserves them during context switch. */
	#define YepX86SystemFeatureSSE                0x0020000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has ymm (AVX) registers, and the operating system preserves them during context switch. */
	#define YepX86SystemFeatureAVX                0x0040000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has zmm (MIC or AVX-512) registers, and the operating system preserves them during context switch. */
	#define YepX86SystemFeatureZMM                0x0080000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has bnd (MPX) registers, and the operating system preserves them during context switch. */
	#define YepX86SystemFeatureBND                0x0100000000000000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepIA64IsaFeatureBrl                  = 0x0000000000000001ull;
	const Yep64u YepIA64IsaFeatureAtomic128            = 0x0000000000000002ull;
	const Yep64u YepIA64IsaFeatureClz                  = 0x0000000000000004ull;
	const Yep64u YepIA64IsaFeatureMpy4                 = 0x0000000000000008ull;
#else
	/** @anchor	IA64_ISA_Extensions
	 *  @name	IA64 ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Long branch instruction. */
	#define YepIA64IsaFeatureBrl                  0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Atomic 128-bit (16-byte) loads, stores, and CAS. */
	#define YepIA64IsaFeatureAtomic128            0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief CLZ (count leading zeros) instruction. */
	#define YepIA64IsaFeatureClz                  0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MPY4 and MPYSHL4 (Truncated 32-bit multiplication) instructions. */
	#define YepIA64IsaFeatureMpy4                 0x0000000000000008ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepARMIsaFeatureV4                    = 0x0000000000000001ull;
	const Yep64u YepARMIsaFeatureV5                    = 0x0000000000000002ull;
	const Yep64u YepARMIsaFeatureV5E                   = 0x0000000000000004ull;
	const Yep64u YepARMIsaFeatureV6                    = 0x0000000000000008ull;
	const Yep64u YepARMIsaFeatureV6K                   = 0x0000000000000010ull;
	const Yep64u YepARMIsaFeatureV7                    = 0x0000000000000020ull;
	const Yep64u YepARMIsaFeatureV7MP                  = 0x0000000000000040ull;
	const Yep64u YepARMIsaFeatureThumb                 = 0x0000000000000080ull;
	const Yep64u YepARMIsaFeatureThumb2                = 0x0000000000000100ull;
	const Yep64u YepARMIsaFeatureThumbEE               = 0x0000000000000200ull;
	const Yep64u YepARMIsaFeatureJazelle               = 0x0000000000000400ull;
	const Yep64u YepARMIsaFeatureFPA                   = 0x0000000000000800ull;
	const Yep64u YepARMIsaFeatureVFP                   = 0x0000000000001000ull;
	const Yep64u YepARMIsaFeatureVFP2                  = 0x0000000000002000ull;
	const Yep64u YepARMIsaFeatureVFP3                  = 0x0000000000004000ull;
	const Yep64u YepARMIsaFeatureVFPd32                = 0x0000000000008000ull;
	const Yep64u YepARMIsaFeatureVFP3HP                = 0x0000000000010000ull;
	const Yep64u YepARMIsaFeatureVFP4                  = 0x0000000000020000ull;
	const Yep64u YepARMIsaFeatureDiv                   = 0x0000000000040000ull;
	const Yep64u YepARMIsaFeatureArmada                = 0x0000000000080000ull;
#else
	/** @anchor	ARM_ISA_Extensions
	 *  @name	ARM ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv4 instruction set. */
	#define YepARMIsaFeatureV4                    0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv5 instruciton set. */
	#define YepARMIsaFeatureV5                    0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv5 DSP instructions. */
	#define YepARMIsaFeatureV5E                   0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv6 instruction set. */
	#define YepARMIsaFeatureV6                    0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv6 Multiprocessing extensions. */
	#define YepARMIsaFeatureV6K                   0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv7 instruction set. */
	#define YepARMIsaFeatureV7                    0x0000000000000020ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief ARMv7 Multiprocessing extensions. */
	#define YepARMIsaFeatureV7MP                  0x0000000000000040ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Thumb mode. */
	#define YepARMIsaFeatureThumb                 0x0000000000000080ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Thumb 2 mode. */
	#define YepARMIsaFeatureThumb2                0x0000000000000100ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Thumb EE mode. */
	#define YepARMIsaFeatureThumbEE               0x0000000000000200ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Jazelle extensions. */
	#define YepARMIsaFeatureJazelle               0x0000000000000400ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief FPA instructions. */
	#define YepARMIsaFeatureFPA                   0x0000000000000800ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFP instruction set. */
	#define YepARMIsaFeatureVFP                   0x0000000000001000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFPv2 instruction set. */
	#define YepARMIsaFeatureVFP2                  0x0000000000002000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFPv3 instruction set. */
	#define YepARMIsaFeatureVFP3                  0x0000000000004000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFP implementation with 32 double-precision registers. */
	#define YepARMIsaFeatureVFPd32                0x0000000000008000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFPv3 half precision extension. */
	#define YepARMIsaFeatureVFP3HP                0x0000000000010000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFPv4 instruction set. */
	#define YepARMIsaFeatureVFP4                  0x0000000000020000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SDIV and UDIV instructions. */
	#define YepARMIsaFeatureDiv                   0x0000000000040000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Marvell Armada instruction extensions. */
	#define YepARMIsaFeatureArmada                0x0000000000080000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepARMSimdFeatureXScale               = 0x0000000000000001ull;
	const Yep64u YepARMSimdFeatureWMMX                 = 0x0000000000000002ull;
	const Yep64u YepARMSimdFeatureWMMX2                = 0x0000000000000004ull;
	const Yep64u YepARMSimdFeatureNEON                 = 0x0000000000000008ull;
	const Yep64u YepARMSimdFeatureNEONHP               = 0x0000000000000010ull;
	const Yep64u YepARMSimdFeatureNEON2                = 0x0000000000000020ull;
#else
	/** @anchor	ARM_SIMD_Extensions
	 *  @name	ARM SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief XScale instructions. */
	#define YepARMSimdFeatureXScale               0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Wireless MMX instruction set. */
	#define YepARMSimdFeatureWMMX                 0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Wireless MMX 2 instruction set. */
	#define YepARMSimdFeatureWMMX2                0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief NEON (Advanced SIMD) instruction set. */
	#define YepARMSimdFeatureNEON                 0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief NEON (Advanced SIMD) half-precision extension. */
	#define YepARMSimdFeatureNEONHP               0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief NEON (Advanced SIMD) v2 instruction set. */
	#define YepARMSimdFeatureNEON2                0x0000000000000020ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepARMSystemFeatureVFPVectorMode      = 0x0000000100000000ull;
	const Yep64u YepARMSystemFeatureFPA                = 0x0100000000000000ull;
	const Yep64u YepARMSystemFeatureWMMX               = 0x0200000000000000ull;
	const Yep64u YepARMSystemFeatureS32                = 0x0400000000000000ull;
	const Yep64u YepARMSystemFeatureD32                = 0x0800000000000000ull;
#else
	/** @anchor	ARM_CPU_and_System_Features
	 *  @name	ARM CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief VFP vector mode is supported in hardware. */
	#define YepARMSystemFeatureVFPVectorMode      0x0000000100000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has FPA registers (f0-f7), and the operating system preserves them during context switch. */
	#define YepARMSystemFeatureFPA                0x0100000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has WMMX registers (wr0-wr15), and the operating system preserves them during context switch. */
	#define YepARMSystemFeatureWMMX               0x0200000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has s0-s31 VFP registers, and the operating system preserves them during context switch. */
	#define YepARMSystemFeatureS32                0x0400000000000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief The CPU has d0-d31 VFP registers, and the operating system preserves them during context switch. */
	#define YepARMSystemFeatureD32                0x0800000000000000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepMIPSIsaFeatureMIPS_I        = 0x0000000000000001ull;
	const Yep64u YepMIPSIsaFeatureMIPS_II       = 0x0000000000000002ull;
	const Yep64u YepMIPSIsaFeatureMIPS_III      = 0x0000000000000004ull;
	const Yep64u YepMIPSIsaFeatureMIPS_IV       = 0x0000000000000008ull;
	const Yep64u YepMIPSIsaFeatureMIPS_V        = 0x0000000000000010ull;
	const Yep64u YepMIPSIsaFeatureR1            = 0x0000000000000020ull;
	const Yep64u YepMIPSIsaFeatureR2            = 0x0000000000000040ull;
	const Yep64u YepMIPSIsaFeatureFPU           = 0x0000000001000000ull;
	const Yep64u YepMIPSIsaFeatureMIPS16        = 0x0000000002000000ull;
	const Yep64u YepMIPSIsaFeatureSmartMIPS     = 0x0000000004000000ull;
	const Yep64u YepMIPSIsaFeatureMT            = 0x0000000008000000ull;
	const Yep64u YepMIPSIsaFeatureMicroMIPS     = 0x0000000010000000ull;
	const Yep64u YepMIPSIsaFeatureVZ            = 0x0000000020000000ull;
#else
	/** @anchor	MIPS_ISA_Extensions
	 *  @name	MIPS ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS I instructions. */
	#define YepMIPSIsaFeatureMIPS_I               0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS II instructions. */
	#define YepMIPSIsaFeatureMIPS_II              0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS III instructions. */
	#define YepMIPSIsaFeatureMIPS_III             0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS IV instructions. */
	#define YepMIPSIsaFeatureMIPS_IV              0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS V instructions. */
	#define YepMIPSIsaFeatureMIPS_V               0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS32/MIPS64 Release 1 instructions. */
	#define YepMIPSIsaFeatureR1                   0x0000000000000020ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS32/MIPS64 Release 2 instructions. */
	#define YepMIPSIsaFeatureR2                   0x0000000000000040ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief FPU with S, D, and W formats and instructions. */
	#define YepMIPSIsaFeatureFPU                  0x0000000001000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS16 extension. */
	#define YepMIPSIsaFeatureMIPS16               0x0000000002000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief SmartMIPS extension. */
	#define YepMIPSIsaFeatureSmartMIPS            0x0000000004000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Multi-threading extension. */
	#define YepMIPSIsaFeatureMT                   0x0000000008000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MicroMIPS extension. */
	#define YepMIPSIsaFeatureMicroMIPS            0x0000000010000000ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS virtualization extension. */
	#define YepMIPSIsaFeatureVZ                   0x0000000020000000ull
	/**@}*/
#endif

#ifdef __cplusplus
	const Yep64u YepMIPSSimdFeatureMDMX         = 0x0000000000000001ull;
	const Yep64u YepMIPSSimdFeaturePairedSingle = 0x0000000000000002ull;
	const Yep64u YepMIPSSimdFeatureMIPS3D       = 0x0000000000000004ull;
	const Yep64u YepMIPSSimdFeatureDSP          = 0x0000000000000008ull;
	const Yep64u YepMIPSSimdFeatureDSP2         = 0x0000000000000010ull;
	const Yep64u YepMIPSSimdFeatureGodsonMMX    = 0x0000000000000020ull;
	const Yep64u YepMIPSSimdFeatureMXU          = 0x0000000000000040ull;
	const Yep64u YepMIPSSimdFeatureMXU2         = 0x0000000000000080ull;
#else
	/** @anchor	MIPS_SIMD_Extensions
	 *  @name	MIPS SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MDMX instruction set. */
	#define YepMIPSSimdFeatureMDMX                0x0000000000000001ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Paired-single instructions. */
	#define YepMIPSSimdFeaturePairedSingle        0x0000000000000002ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS3D instruction set. */
	#define YepMIPSSimdFeatureMIPS3D              0x0000000000000004ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS DSP extension. */
	#define YepMIPSSimdFeatureDSP                 0x0000000000000008ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief MIPS DSP Release 2 extension. */
	#define YepMIPSSimdFeatureDSP2                0x0000000000000010ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Loongson (Godson) MMX instruction set. */
	/** @bug Not detected in this @Yeppp release. */
	#define YepMIPSSimdFeatureGodsonMMX           0x0000000000000020ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Ingenic Media Extension. */
	#define YepMIPSSimdFeatureMXU                 0x0000000000000040ull
	/** @ingroup yepLibrary_CpuFeatures */
	/** @brief Ingenic Media Extension 2. */
	#define YepMIPSSimdFeatureMXU2                0x0000000000000080ull
	/**@}*/
#endif
	
#ifdef __cplusplus
}
#endif
