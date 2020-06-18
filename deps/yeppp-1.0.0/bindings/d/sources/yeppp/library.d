/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

module yeppp.library;
import yeppp.types;

extern (C) {
	/** @ingroup	yepLibrary */
	/** @brief	Contains information about @Yeppp library version. */
	/** @see	yepLibrary_GetVersion */
	struct LibraryVersion {
		/** @brief The major version. Library releases with the same major versions are guaranteed to be API- and ABI-compatible. */
		uint major;
		/** @brief The minor version. A change in minor versions indicates addition of new features, and major bug-fixes. */
		uint minor;
		/** @brief The patch level. A version with a higher patch level indicates minor bug-fixes. */
		uint patchLevel;
		/** @brief The build number. The build number is unique for the fixed combination of major, minor, and patch-level versions. */
		uint build;
		/** @brief A UTF-8 string with a human-readable name of this release. May contain non-ASCII characters. */
		immutable(char)* releaseName;
	};

	/** @ingroup	yepLibrary */
	/** @brief	The basic instruction set architecture of the processor. */
	/** @details	The ISA is always known at compile-time. */
	/** @see	yepLibrary_GetCpuArchitecture */
	enum CpuArchitecture : uint {
		/** @brief	Instruction set architecture is not known to the library. */
		/** @details	This value is never returned on supported architectures. */
		Unknown = 0,
		/** @brief	x86 or x86-64 ISA. */
		X86 = 1,
		/** @brief	ARM ISA. */
		ARM = 2,
		/** @brief	MIPS ISA. */
		MIPS = 3,
		/** @brief	PowerPC ISA. */
		PowerPC = 4,
		/** @brief	IA64 ISA. */
		IA64 = 5,
		/** @brief	SPARC ISA. */
		SPARC = 6
	};
	
	/** @ingroup	yepLibrary */
	/** @brief	The company which designed the processor microarchitecture. */
	/** @see	yepLibrary_GetCpuVendor */
	enum CpuVendor : uint {
		/** @brief	Processor vendor is not known to the library, or the library failed to get vendor information from the OS. */
		Unknown = 0,
		
		/* x86/x86-64 CPUs */
		
		/** @brief	Intel Corporation. Vendor of x86, x86-64, IA64, and ARM processor microarchitectures. */
		/** @details	Sold its ARM design subsidiary in 2006. The last ARM processor design was released in 2004. */
		Intel = 1,
		/** @brief	Advanced Micro Devices, Inc. Vendor of x86 and x86-64 processor microarchitectures. */
		AMD = 2,
		/** @brief	VIA Technologies, Inc. Vendor of x86 and x86-64 processor microarchitectures. */
		/** @details	Processors are designed by Centaur Technology, a subsidiary of VIA Technologies. */
		VIA = 3,
		/** @brief	Transmeta Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2004. */
		/**         	Transmeta processors implemented VLIW ISA and used binary translation to execute x86 code. */
		Transmeta = 4,
		/** @brief	Cyrix Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1996. */
		Cyrix = 5,
		/** @brief	Rise Technology. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1999. */
		Rise = 6,
		/** @brief	National Semiconductor. Vendor of x86 processor microarchitectures. */
		/** @details	Sold its x86 design subsidiary in 1999. The last processor design was released in 1998. */
		NSC = 7,
		/** @brief	Silicon Integrated Systems. Vendor of x86 processor microarchitectures. */
		/** @details	Sold its x86 design subsidiary in 2001. The last processor design was released in 2001. */
		SiS = 8,
		/** @brief	NexGen. Vendor of x86 processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 1994. */
		/**         	NexGen designed the first x86 microarchitecture which decomposed x86 instructions into simple microoperations. */
		NexGen = 9,
		/** @brief	United Microelectronics Corporation. Vendor of x86 processor microarchitectures. */
		/** @details	Ceased x86 in the early 1990s. The last processor design was released in 1991. */
		/**         	Designed U5C and U5D processors. Both are 486 level. */
		UMC = 10,
		/** @brief	RDC Semiconductor Co., Ltd. Vendor of x86 processor microarchitectures. */
		/** @details	Designes embedded x86 CPUs. */
		RDC = 11,
		/** @brief	DM&P Electronics Inc. Vendor of x86 processor microarchitectures. */
		/** @details	Mostly embedded x86 designs. */
		DMP = 12,
		
		/* ARM CPUs */
		
		/** @brief	ARM Holdings plc. Vendor of ARM processor microarchitectures. */
		ARM       = 20,
		/** @brief	Marvell Technology Group Ltd. Vendor of ARM processor microarchitectures. */
		Marvell   = 21,
		/** @brief	Qualcomm Incorporated. Vendor of ARM processor microarchitectures. */
		Qualcomm  = 22,
		/** @brief	Digital Equipment Corporation. Vendor of ARM processor microarchitecture. */
		/** @details	Sold its ARM designs in 1997. The last processor design was released in 1997. */
		DEC       = 23,
		/** @brief	Texas Instruments Inc. Vendor of ARM processor microarchitectures. */
		TI        = 24,
		/** @brief	Apple Inc. Vendor of ARM processor microarchitectures. */
		Apple     = 25,
		
		/* MIPS CPUs */
		
		/** @brief	Ingenic Semiconductor. Vendor of MIPS processor microarchitectures. */
		Ingenic   = 40,
		/** @brief	Institute of Computing Technology of the Chinese Academy of Sciences. Vendor of MIPS processor microarchitectures. */
		ICT       = 41,
		/** @brief	MIPS Technologies, Inc. Vendor of MIPS processor microarchitectures. */
		MIPS      = 42,
		
		/* PowerPC CPUs */
		
		/** @brief	International Business Machines Corporation. Vendor of PowerPC processor microarchitectures. */
		IBM       = 50,
		/** @brief	Motorola, Inc. Vendor of PowerPC and ARM processor microarchitectures. */
		Motorola  = 51,
		/** @brief	P. A. Semi. Vendor of PowerPC processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2007. */
		PASemi    = 52,
		
		/* SPARC CPUs */
		
		/** @brief	Sun Microsystems, Inc. Vendor of SPARC processor microarchitectures. */
		/** @details	Now defunct. The last processor design was released in 2008. */
		Sun       = 60,
		/** @brief	Oracle Corporation. Vendor of SPARC processor microarchitectures. */
		Oracle    = 61,
		/** @brief	Fujitsu Limited. Vendor of SPARC processor microarchitectures. */
		Fujitsu   = 62,
		/** @brief	Moscow Center of SPARC Technologies CJSC. Vendor of SPARC processor microarchitectures. */
		MCST      = 63
	};
	
	/**
	 * @ingroup	yepLibrary
	 * @brief	Type of processor microarchitecture.
	 * @details	Low-level instruction performance characteristics, such as latency and throughput, are constant within microarchitecture.
	 *         	Processors of the same microarchitecture can differ in supported instruction sets and other extensions.
	 * @see	yepLibrary_GetCpuMicroarchitecture
	 */
	enum CpuMicroarchitecture : uint {
		/** @brief Microarchitecture is unknown, or the library failed to get information about the microarchitecture from OS */
		Unknown       = 0,
		
		/** @brief Pentium and Pentium MMX microarchitecture. */
		P5            = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0001,
		/** @brief Pentium Pro, Pentium II, and Pentium III. */
		P6            = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0002,
		/** @brief Pentium 4 with Willamette, Northwood, or Foster cores. */
		Willamette    = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0003,
		/** @brief Pentium 4 with Prescott and later cores. */
		Prescott      = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0004,
		/** @brief Pentium M. */
		Dothan        = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0005,
		/** @brief Intel Core microarchitecture. */
		Yonah         = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0006,
		/** @brief Intel Core 2 microarchitecture on 65 nm process. */
		Conroe        = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0007,
		/** @brief Intel Core 2 microarchitecture on 45 nm process. */
		Penryn        = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0008,
		/** @brief Intel Atom on 45 nm process. */
		Bonnell       = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0009,
		/** @brief Intel Nehalem and Westmere microarchitectures (Core i3/i5/i7 1st gen). */
		Nehalem       = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000A,
		/** @brief Intel Sandy Bridge microarchitecture (Core i3/i5/i7 2nd gen). */
		SandyBridge   = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000B,
		/** @brief Intel Atom on 32 nm process. */
		Saltwell      = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000C,
		/** @brief Intel Ivy Bridge microarchitecture (Core i3/i5/i7 3rd gen). */
		IvyBridge     = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000D,
		/** @brief Intel Haswell microarchitecture (Core i3/i5/i7 4th gen). */
		Haswell       = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000E,
		/** @brief Intel Silvermont microarchitecture (22 nm out-of-order Atom). */
		Silvermont    = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x000F,
		
		/** @brief Intel Knights Ferry HPC boards. */
		KnightsFerry  = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0100,
		/** @brief Intel Knights Corner HPC boards (aka Xeon Phi). */
		KnightsCorner = (CpuArchitecture.X86 << 24) + (CpuVendor.Intel << 16) + 0x0101,
		
		/** @brief AMD K5. */
		K5            = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0001,
		/** @brief AMD K6 and alike. */
		K6            = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0002,
		/** @brief AMD Athlon and Duron. */
		K7            = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0003,
		/** @brief AMD Geode GX and LX. */
		Geode         = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0004,
		/** @brief AMD Athlon 64, Opteron 64. */
		K8            = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0005,
		/** @brief AMD K10 (Barcelona, Istambul, Magny-Cours). */
		K10           = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0006,
		/** @brief AMD Bobcat mobile microarchitecture. */
		Bobcat        = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0007,
		/** @brief AMD Bulldozer microarchitecture (1st gen K15). */
		Bulldozer     = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0008,
		/** @brief AMD Piledriver microarchitecture (2nd gen K15). */
		Piledriver    = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x0009,
		/** @brief AMD Jaguar mobile microarchitecture. */
		Jaguar        = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x000A,
		/** @brief AMD Steamroller microarchitecture (3rd gen K15). */
		Steamroller   = (CpuArchitecture.X86 << 24) + (CpuVendor.AMD   << 16) + 0x000B,
		
		/** @brief DEC/Intel StrongARM processors. */
		StrongARM     = (CpuArchitecture.ARM << 24) + (CpuVendor.Intel   << 16) + 0x0001,
		/** @brief Intel/Marvell XScale processors. */
		XScale        = (CpuArchitecture.ARM << 24) + (CpuVendor.Intel   << 16) + 0x0002,
		
		/** @brief ARM7 series. */
		ARM7          = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0001,
		/** @brief ARM9 series. */
		ARM9          = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0002,
		/** @brief ARM 1136, ARM 1156, ARM 1176, or ARM 11MPCore. */
		ARM11         = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0003,
		/** @brief ARM Cortex-A5. */
		CortexA5      = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0004,
		/** @brief ARM Cortex-A7. */
		CortexA7      = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0005,
		/** @brief ARM Cortex-A8. */
		CortexA8      = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0006,
		/** @brief ARM Cortex-A9. */
		CortexA9      = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0007,
		/** @brief ARM Cortex-A15. */
		CortexA15     = (CpuArchitecture.ARM << 24) + (CpuVendor.ARM     << 16) + 0x0008,
		
		/** @brief Qualcomm Scorpion. */
		Scorpion      = (CpuArchitecture.ARM << 24) + (CpuVendor.Qualcomm << 16) + 0x0001,
		/** @brief Qualcomm Krait. */
		Krait         = (CpuArchitecture.ARM << 24) + (CpuVendor.Qualcomm << 16) + 0x0002,
		
		/** @brief Marvell Sheeva PJ1. */
		PJ1           = (CpuArchitecture.ARM << 24) + (CpuVendor.Marvell << 16) + 0x0001,
		/** @brief Marvell Sheeva PJ4. */
		PJ4           = (CpuArchitecture.ARM << 24) + (CpuVendor.Marvell << 16) + 0x0002,
		
		/** @brief Apple A6 and A6X processors. */
		Swift         = (CpuArchitecture.ARM << 24) + (CpuVendor.Apple   << 16) + 0x0001,

		/** @brief Intel Itanium. */
		Itanium       = (CpuArchitecture.IA64 << 24) + (CpuVendor.Intel << 16) + 0x0001,
		/** @brief Intel Itanium 2. */
		Itanium2      = (CpuArchitecture.IA64 << 24) + (CpuVendor.Intel << 16) + 0x0002,
		
		/** @brief MIPS 24K. */
		MIPS24K       = (CpuArchitecture.MIPS << 24) + (CpuVendor.MIPS << 16) + 0x0001,
		/** @brief MIPS 34K. */
		MIPS34K       = (CpuArchitecture.MIPS << 24) + (CpuVendor.MIPS << 16) + 0x0002,
		/** @brief MIPS 74K. */
		MIPS74K       = (CpuArchitecture.MIPS << 24) + (CpuVendor.MIPS << 16) + 0x0003,
		
		/** @brief Ingenic XBurst. */
		XBurst        = (CpuArchitecture.MIPS << 24) + (CpuVendor.Ingenic << 16) + 0x0001,
		/** @brief Ingenic XBurst 2. */
		XBurst2       = (CpuArchitecture.MIPS << 24) + (CpuVendor.Ingenic << 16) + 0x0002
	};

	const ulong YepIsaFeaturesDefault                 = 0x0000000000000000UL;
	const ulong YepSimdFeaturesDefault                = 0x0000000000000000UL;
	const ulong YepSystemFeaturesDefault              = 0x0000000000000000UL;

	/** @name	Common CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief The processor has a built-in cycle counter, and the operating system provides a way to access it. */
	const ulong YepSystemFeatureCycleCounter       = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief The processor has a 64-bit cycle counter, or the operating system provides an abstraction of a 64-bit cycle counter. */
	const ulong YepSystemFeatureCycleCounter64Bit  = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief The processor and the operating system allows to use 64-bit pointers. */
	const ulong YepSystemFeatureAddressSpace64Bit  = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief The processor and the operating system allows to do 64-bit arithmetical operations on general-purpose registers. */
	const ulong YepSystemFeatureGPRegisters64Bit   = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief The processor and the operating system allows misaligned memory reads and writes. */
	const ulong YepSystemFeatureMisalignedAccess   = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief The processor or the operating system support at most one hardware thread. */
	const ulong YepSystemFeatureSingleThreaded     = 0x0000000000000020UL;
	/**@}*/


	/** @name	x86 and x86-64 ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief x87 FPU integrated on chip. */
	const ulong YepX86IsaFeatureFPU                   = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief x87 CPUID instruction. */
	const ulong YepX86IsaFeatureCpuid                 = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief RDTSC instruction. */
	const ulong YepX86IsaFeatureRdtsc                 = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief CMOV, FCMOV, and FCOMI/FUCOMI instructions. */
	const ulong YepX86IsaFeatureCMOV                  = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief SYSENTER and SYSEXIT instructions. */
	const ulong YepX86IsaFeatureSYSENTER              = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief SYSCALL and SYSRET instructions. */
	const ulong YepX86IsaFeatureSYSCALL               = 0x0000000000000020UL;
	/** @ingroup yepLibrary */
	/** @brief RDMSR and WRMSR instructions. */
	const ulong YepX86IsaFeatureMSR                   = 0x0000000000000040UL;
	/** @ingroup yepLibrary */
	/** @brief CLFLUSH instruction. */
	const ulong YepX86IsaFeatureClflush               = 0x0000000000000080UL;
	/** @ingroup yepLibrary */
	/** @brief MONITOR and MWAIT instructions. */
	const ulong YepX86IsaFeatureMONITOR               = 0x0000000000000100UL;
	/** @ingroup yepLibrary */
	/** @brief FXSAVE and FXRSTOR instructions. */
	const ulong YepX86IsaFeatureFXSAVE                = 0x0000000000000200UL;
	/** @ingroup yepLibrary */
	/** @brief XSAVE, XRSTOR, XGETBV, and XSETBV instructions. */
	const ulong YepX86IsaFeatureXSAVE                 = 0x0000000000000400UL;
	/** @ingroup yepLibrary */
	/** @brief CMPXCHG8B instruction. */
	const ulong YepX86IsaFeatureCmpxchg8b             = 0x0000000000000800UL;
	/** @ingroup yepLibrary */
	/** @brief CMPXCHG16B instruction. */
	const ulong YepX86IsaFeatureCmpxchg16b            = 0x0000000000001000UL;
	/** @ingroup yepLibrary */
	/** @brief Support for 64-bit mode. */
	const ulong YepX86IsaFeatureX64                   = 0x0000000000002000UL;
	/** @ingroup yepLibrary */
	/** @brief Support for LAHF and SAHF instructions in 64-bit mode. */
	const ulong YepX86IsaFeatureLahfSahf64            = 0x0000000000004000UL;
	/** @ingroup yepLibrary */
	/** @brief RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions. */
	const ulong YepX86IsaFeatureFsGsBase              = 0x0000000000008000UL;
	/** @ingroup yepLibrary */
	/** @brief MOVBE instruction. */
	const ulong YepX86IsaFeatureMovbe                 = 0x0000000000010000UL;
	/** @ingroup yepLibrary */
	/** @brief POPCNT instruction. */
	const ulong YepX86IsaFeaturePopcnt                = 0x0000000000020000UL;
	/** @ingroup yepLibrary */
	/** @brief LZCNT instruction. */
	const ulong YepX86IsaFeatureLzcnt                 = 0x0000000000040000UL;
	/** @ingroup yepLibrary */
	/** @brief BMI instruction set. */
	const ulong YepX86IsaFeatureBMI                   = 0x0000000000080000UL;
	/** @ingroup yepLibrary */
	/** @brief BMI 2 instruction set. */
	const ulong YepX86IsaFeatureBMI2                  = 0x0000000000100000UL;
	/** @ingroup yepLibrary */
	/** @brief TBM instruction set. */
	const ulong YepX86IsaFeatureTBM                   = 0x0000000000200000UL;
	/** @ingroup yepLibrary */
	/** @brief RDRAND instruction. */
	const ulong YepX86IsaFeatureRdrand                = 0x0000000000400000UL;
	/** @ingroup yepLibrary */
	/** @brief Padlock Advanced Cryptography Engine on chip. */
	const ulong YepX86IsaFeatureACE                   = 0x0000000000800000UL;
	/** @ingroup yepLibrary */
	/** @brief Padlock Advanced Cryptography Engine 2 on chip. */
	const ulong YepX86IsaFeatureACE2                  = 0x0000000001000000UL;
	/** @ingroup yepLibrary */
	/** @brief Padlock Random Number Generator on chip. */
	const ulong YepX86IsaFeatureRNG                   = 0x0000000002000000UL;
	/** @ingroup yepLibrary */
	/** @brief Padlock Hash Engine on chip. */
	const ulong YepX86IsaFeaturePHE                   = 0x0000000004000000UL;
	/** @ingroup yepLibrary */
	/** @brief Padlock Montgomery Multiplier on chip. */
	const ulong YepX86IsaFeaturePMM                   = 0x0000000008000000UL;
	/** @ingroup yepLibrary */
	/** @brief AES instruction set. */
	const ulong YepX86IsaFeatureAES                   = 0x0000000010000000UL;
	/** @ingroup yepLibrary */
	/** @brief PCLMULQDQ instruction. */
	const ulong YepX86IsaFeaturePclmulqdq             = 0x0000000020000000UL;
	/** @ingroup yepLibrary */
	/** @brief RDTSCP instruction. */
	const ulong YepX86IsaFeatureRdtscp                = 0x0000000040000000UL;
	/** @ingroup yepLibrary */
	/** @brief Lightweight Profiling extension. */
	const ulong YepX86IsaFeatureLWP                   = 0x0000000080000000UL;
	/** @ingroup yepLibrary */
	/** @brief Hardware Lock Elision extension. */
	const ulong YepX86IsaFeatureHLE                   = 0x0000000100000000UL;
	/** @ingroup yepLibrary */
	/** @brief Restricted Transactional Memory extension. */
	const ulong YepX86IsaFeatureRTM                   = 0x0000000200000000UL;
	/** @ingroup yepLibrary */
	/** @brief XTEST instruction. */
	const ulong YepX86IsaFeatureXtest                 = 0x0000000400000000UL;
	/** @ingroup yepLibrary */
	/** @brief RDSEED instruction. */
	const ulong YepX86IsaFeatureRdseed                = 0x0000000800000000UL;
	/** @ingroup yepLibrary */
	/** @brief ADCX and ADOX instructions. */
	const ulong YepX86IsaFeatureADX                   = 0x0000001000000000UL;
	/**@}*/

	/** @name	x86 and x86-64 SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief MMX instruction set. */
	const ulong YepX86SimdFeatureMMX                  = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief MMX+ instruction set. */
	const ulong YepX86SimdFeatureMMXPlus              = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief EMMX instruction set. */
	const ulong YepX86SimdFeatureEMMX                 = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief 3dnow! instruction set. */
	const ulong YepX86SimdFeature3dnow                = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief 3dnow!+ instruction set. */
	const ulong YepX86SimdFeature3dnowPlus            = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief 3dnow! prefetch instructions. */
	const ulong YepX86SimdFeature3dnowPrefetch        = 0x0000000000000020UL;
	/** @ingroup yepLibrary */
	/** @brief Geode 3dnow! instructions. */
	const ulong YepX86SimdFeature3dnowGeode           = 0x0000000000000040UL;
	/** @ingroup yepLibrary */
	/** @brief SSE instruction set. */
	const ulong YepX86SimdFeatureSSE                  = 0x0000000000000080UL;
	/** @ingroup yepLibrary */
	/** @brief SSE 2 instruction set. */
	const ulong YepX86SimdFeatureSSE2                 = 0x0000000000000100UL;
	/** @ingroup yepLibrary */
	/** @brief SSE 3 instruction set. */
	const ulong YepX86SimdFeatureSSE3                 = 0x0000000000000200UL;
	/** @ingroup yepLibrary */
	/** @brief SSSE 3 instruction set. */
	const ulong YepX86SimdFeatureSSSE3                = 0x0000000000000400UL;
	/** @ingroup yepLibrary */
	/** @brief SSE 4.1 instruction set. */
	const ulong YepX86SimdFeatureSSE4_1               = 0x0000000000000800UL;
	/** @ingroup yepLibrary */
	/** @brief SSE 4.2 instruction set. */
	const ulong YepX86SimdFeatureSSE4_2               = 0x0000000000001000UL;
	/** @ingroup yepLibrary */
	/** @brief SSE 4A instruction set. */
	const ulong YepX86SimdFeatureSSE4A                = 0x0000000000002000UL;
	/** @ingroup yepLibrary */
	/** @brief AVX instruction set. */
	const ulong YepX86SimdFeatureAVX                  = 0x0000000000004000UL;
	/** @ingroup yepLibrary */
	/** @brief AVX 2 instruction set. */
	const ulong YepX86SimdFeatureAVX2                 = 0x0000000000008000UL;
	/** @ingroup yepLibrary */
	/** @brief XOP instruction set. */
	const ulong YepX86SimdFeatureXOP                  = 0x0000000000010000UL;
	/** @ingroup yepLibrary */
	/** @brief F16C instruction set. */
	const ulong YepX86SimdFeatureF16C                 = 0x0000000000020000UL;
	/** @ingroup yepLibrary */
	/** @brief FMA3 instruction set. */
	const ulong YepX86SimdFeatureFMA3                 = 0x0000000000040000UL;
	/** @ingroup yepLibrary */
	/** @brief FMA4 instruction set. */
	const ulong YepX86SimdFeatureFMA4                 = 0x0000000000080000UL;
	/**@}*/

	/** @name	x86 CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief The CPU has x87 FPU registers, and the operating systems preserves them during context switch. */
	const ulong YepX86SystemFeatureFPU                = 0x0000000100000000UL;
	/** @ingroup yepLibrary */
	/** @brief The CPU has SSE registers, and the operating systems preserves them during context switch. */
	const ulong YepX86SystemFeatureSSE                = 0x0000000200000000UL;
	/** @ingroup yepLibrary */
	/** @brief The CPU has AVX registers, and the operating systems preserves them during context switch. */
	const ulong YepX86SystemFeatureAVX                = 0x0000000400000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor allows to use misaligned memory operands in SSE instructions other than loads and stores. */
	const ulong YepX86SystemFeatureMisalignedSSE      = 0x0000000800000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine. */
	const ulong YepX86SystemFeatureACE                = 0x0000001000000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine 2. */
	const ulong YepX86SystemFeatureACE2               = 0x0000002000000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor and the operating system support the Padlock Random Number Generator. */
	const ulong YepX86SystemFeatureRNG                = 0x0000004000000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor and the operating system support the Padlock Hash Engine. */
	const ulong YepX86SystemFeaturePHE                = 0x0000008000000000UL;
	/** @ingroup yepLibrary */
	/** @brief Processor and the operating system support the Padlock Montgomery Multiplier. */
	const ulong YepX86SystemFeaturePMM                = 0x0000010000000000UL;
	/**@}*/

	/** @name	IA64 ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief Long branch instruction. */
	const ulong YepIA64IsaFeatureBrl                  = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief Atomic 128-bit (16-byte) loads, stores, and CAS. */
	const ulong YepIA64IsaFeatureAtomic128            = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief CLZ (count leading zeros) instruction. */
	const ulong YepIA64IsaFeatureClz                  = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief MPY4 and MPYSHL4 (Truncated 32-bit multiplication) instructions. */
	const ulong YepIA64IsaFeatureMpy4                 = 0x0000000000000008UL;
	/**@}*/

	/** @name	ARM ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief ARMv4 instruction set. */
	const ulong YepARMIsaFeatureV4                    = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv5 instruciton set. */
	const ulong YepARMIsaFeatureV5                    = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv5 DSP instructions. */
	const ulong YepARMIsaFeatureV5E                   = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv6 instruction set. */
	const ulong YepARMIsaFeatureV6                    = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv6 Multiprocessing extensions. */
	const ulong YepARMIsaFeatureV6K                   = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv7 instruction set. */
	const ulong YepARMIsaFeatureV7                    = 0x0000000000000020UL;
	/** @ingroup yepLibrary */
	/** @brief ARMv7 Multiprocessing extensions. */
	const ulong YepARMIsaFeatureV7MP                  = 0x0000000000000040UL;
	/** @ingroup yepLibrary */
	/** @brief Thumb mode. */
	const ulong YepARMIsaFeatureThumb                 = 0x0000000000000080UL;
	/** @ingroup yepLibrary */
	/** @brief Thumb 2 mode. */
	const ulong YepARMIsaFeatureThumb2                = 0x0000000000000100UL;
	/** @ingroup yepLibrary */
	/** @brief Thumb EE mode. */
	const ulong YepARMIsaFeatureThumbEE               = 0x0000000000000200UL;
	/** @ingroup yepLibrary */
	/** @brief Jazelle extensions. */
	const ulong YepARMIsaFeatureJazelle               = 0x0000000000000400UL;
	/** @ingroup yepLibrary */
	/** @brief FPA instructions. */
	const ulong YepARMIsaFeatureFPA                   = 0x0000000000000800UL;
	/** @ingroup yepLibrary */
	/** @brief VFP instruction set. */
	const ulong YepARMIsaFeatureVFP                   = 0x0000000000001000UL;
	/** @ingroup yepLibrary */
	/** @brief VFPv2 instruction set. */
	const ulong YepARMIsaFeatureVFP2                  = 0x0000000000002000UL;
	/** @ingroup yepLibrary */
	/** @brief VFPv3 instruction set. */
	const ulong YepARMIsaFeatureVFP3                  = 0x0000000000004000UL;
	/** @ingroup yepLibrary */
	/** @brief VFP implementation with 32 double-precision registers. */
	const ulong YepARMIsaFeatureVFPd32                = 0x0000000000008000UL;
	/** @ingroup yepLibrary */
	/** @brief VFPv3 half precision extension. */
	const ulong YepARMIsaFeatureVFP3HP                = 0x0000000000010000UL;
	/** @ingroup yepLibrary */
	/** @brief VFPv4 instruction set. */
	const ulong YepARMIsaFeatureVFP4                  = 0x0000000000020000UL;
	/** @ingroup yepLibrary */
	/** @brief SDIV and UDIV instructions. */
	const ulong YepARMIsaFeatureDiv                   = 0x0000000000040000UL;
	/** @ingroup yepLibrary */
	/** @brief Marvell Armada instruction extensions. */
	const ulong YepARMIsaFeatureArmada                = 0x0000000000080000UL;
	/**@}*/

	/** @name	ARM SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief XScale instructions. */
	const ulong YepARMSimdFeatureXScale               = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief Wireless MMX instruction set. */
	const ulong YepARMSimdFeatureWMMX                 = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief Wireless MMX 2 instruction set. */
	const ulong YepARMSimdFeatureWMMX2                = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief NEON (Advanced SIMD) instruction set. */
	const ulong YepARMSimdFeatureNEON                 = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief NEON (Advanced SIMD) half-precision extension. */
	const ulong YepARMSimdFeatureNEONHP               = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief NEON (Advanced SIMD) v2 instruction set. */
	const ulong YepARMSimdFeatureNEON2                = 0x0000000000000020UL;
	/**@}*/


	/** @name	ARM CPU and System Features
	 *  @see	yepLibrary_GetCpuSystemFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief VFP vector mode is supported in hardware. */
	const ulong YepARMSystemFeatureVFPVectorMode      = 0x0000000100000000UL;
	/**@}*/



	/** @name	MIPS ISA Extensions
	 *  @see	yepLibrary_GetCpuIsaFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief MIPS32/MIPS64 Release 2 architecture. */
	const ulong YepMIPSIsaFeatureR2                   = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief MicroMIPS extension. */
	/** @bug Not detected in this @Yeppp release. */
	const ulong YepMIPSIsaFeatureMicroMIPS            = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief FPU with S, D, and W formats and instructions. */
	const ulong YepMIPSIsaFeatureFPU                  = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief Multi-threading extension. */
	const ulong YepMIPSIsaFeatureMT                   = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief MIPS16 extension. */
	const ulong YepMIPSIsaFeatureMIPS16               = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief SmartMIPS extension. */
	const ulong YepMIPSIsaFeatureSmartmips            = 0x0000000000000020UL;
	/**@}*/

	/** @name	MIPS SIMD Extensions
	 *  @see	yepLibrary_GetCpuSimdFeatures */
	/**@{*/
	/** @ingroup yepLibrary */
	/** @brief MDMX instruction set. */
	const ulong YepMIPSSimdFeatureMDMX                = 0x0000000000000001UL;
	/** @ingroup yepLibrary */
	/** @brief MIPS3D instruction set. */
	const ulong YepMIPSSimdFeatureMIPS3D              = 0x0000000000000002UL;
	/** @ingroup yepLibrary */
	/** @brief Paired-single instructions. */
	const ulong YepMIPSSimdFeaturePairedSingle        = 0x0000000000000004UL;
	/** @ingroup yepLibrary */
	/** @brief MIPS DSP extension. */
	const ulong YepMIPSSimdFeatureDSP                 = 0x0000000000000008UL;
	/** @ingroup yepLibrary */
	/** @brief MIPS DSP Release 2 extension. */
	const ulong YepMIPSSimdFeatureDSP2                = 0x0000000000000010UL;
	/** @ingroup yepLibrary */
	/** @brief Loongson (Godson) MMX instruction set. */
	/** @bug Not detected in this @Yeppp release. */
	const ulong YepMIPSSimdFeatureGodsonMMX           = 0x0000000000000020UL;
	/** @ingroup yepLibrary */
	/** @brief Ingenic Media Extension. */
	const ulong YepMIPSSimdFeatureIMX                 = 0x0000000000000040UL;
	/**@}*/

	/**
	 * @ingroup yepLibrary
	 * @brief	Initialized the @Yeppp library.
	 * @retval	#YepStatusOk	The library is successfully initialized.
	 * @retval	#YepStatusSystemError	An uncoverable error inside the OS kernel occurred during library initialization.
	 * @see	yepLibrary_Release
	 */
	Status yepLibrary_Init();
	/**
	 * @ingroup yepLibrary
	 * @brief	Deinitialized the @Yeppp library and releases the consumed system resources.
	 * @retval	#YepStatusOk	The library is successfully initialized.
	 * @retval	#YepStatusSystemError	The library failed to release some of the resources due to a failed call to the OS kernel.
	 * @see	yepLibrary_Init
	 */
	Status yepLibrary_Release();
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns basic information about the library version.
	 * @return	A pointer to a structure describing @Yeppp library version.
	 */
	immutable(LibraryVersion)* yepLibrary_GetVersion();
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns information about the supported ISA extensions (excluding SIMD extensions)
	 * @param[out]	isaFeatures	Pointer to a 64-bit mask where information about the supported ISA extensions will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a isaFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a isaFeatures pointer is null.
	 * @see	yepLibrary_GetCpuSimdFeatures, yepLibrary_GetCpuSystemFeatures
	 */
	Status yepLibrary_GetCpuIsaFeatures(out ulong isaFeatures);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns information about the supported SIMD extensions
	 * @param[out]	simdFeatures	Pointer to a 64-bit mask where information about the supported SIMD extensions will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a simdFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a simdFeatures pointer is null.
	 * @see	yepLibrary_GetCpuIsaFeatures, yepLibrary_GetCpuSystemFeatures
	 */
	Status yepLibrary_GetCpuSimdFeatures(out ulong simdFeatures);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns information about processor features other than ISA extensions, and OS features related to CPU.
	 * @param[out]	systemFeatures	Pointer to a 64-bit mask where information about extended processor and system features will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the mask pointed by @a systemFeatures parameter.
	 * @retval	#YepStatusNullPointer	The @a systemFeatures pointer is null.
	 * @see	yepLibrary_GetCpuIsaFeatures, yepLibrary_GetCpuSimdFeatures
	 */
	Status yepLibrary_GetCpuSystemFeatures(out ulong systemFeatures);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns information about the vendor of the processor.
	 * @param[out]	vendor	Pointer to a variable where information about the processor vendor will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a vendor parameter.
	 * @retval	#YepStatusNullPointer	The @a vendor pointer is null.
	 */
	Status yepLibrary_GetCpuVendor(out CpuVendor vendor);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns the type of processor architecture.
	 * @param[out]	architecture	Pointer to a variable where information about the processor architecture will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a architecture parameter.
	 * @retval	#YepStatusNullPointer	The @a architecture reference is null.
	 */
	Status yepLibrary_GetCpuArchitecture(out CpuArchitecture architecture);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns the type of processor microarchitecture used.
	 * @param[out]	microarchitecture	Pointer to a variable where information about the processor microarchitecture will be stored.
	 * @retval	#YepStatusOk	The information successfully stored to the variable pointed by @a microarchitecture parameter.
	 * @retval	#YepStatusNullPointer	The @a microarchitecture pointer is null.
	 */
	Status yepLibrary_GetCpuMicroarchitecture(out CpuMicroarchitecture microarchitecture);
	/**
	 * @ingroup yepLibrary
	 * @brief	Initializes the processor cycle counter and starts counting the processor cycles.
	 * @details	In the current implementation this function uses:
	 *         	 - RDTSC or RDTSCP instructions on x86 and x86-64.
	 *         	 - ITC register on IA64.
	 *         	 - Linux perf events subsystem on ARM and MIPS.
	 * @warning	The state is not guaranteed to be the current processor cycle counter value, and should not be used as such.
	 * @warning	This function may allocate system resources.
	 *         	To avoid resource leak, always match a successfull call to #yepLibrary_GetCpuCyclesAcquire with a call to #yepLibrary_GetCpuCyclesRelease.
	 * @param[out]	state	Pointer to a variable where the state of the cycle counter will be stored.
	 *            	     	If the function fails, the value of the state variable is not changed.
	 * @retval	#YepStatusOk	The cycle counter successfully initialized and its state is stored to the variable pointed by @a state parameter.
	 * @retval	#YepStatusNullPointer	The @a state pointer is null.
	 * @retval	#YepStatusUnsupportedHardware	The processor does not have cycle counter.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the CPU cycle counter.
	 * @retval	#YepStatusSystemError	An attempt to initialize cycle counter failed inside the OS kernel.
	 * @see	yepLibrary_GetCpuCyclesRelease
	 */
	Status yepLibrary_GetCpuCyclesAcquire(out ulong state);
	/**
	 * @ingroup yepLibrary
	 * @brief	Stops counting the processor cycles, releases the system resources associated with the cycle counter, and returns the number of cycles elapsed.
	 * @param[in,out]	state	Pointer to a variable with the state of the cycle counter saved by #yepLibrary_GetCpuCyclesAcquire.
	 *               	     	The cycle counter should be released only once, and the function zeroes out the state variable.
	 * @param[out]	cycles	Pointer to a variable where the number of cycles elapsed will be stored.
	 *            	      	The pointer can be the same as @a state pointer.
	 * @retval	#YepStatusOk	The number of cycles elapsed is saved to the variable pointed by @a state parameter, and the system resources are successfully released.
	 * @retval	#YepStatusNullPointer	Either the @a state pointer or the @a cycles pointer is null.
	 * @retval	#YepStatusInvalidState	The @a state variable does not specify a valid state of the cycle counter.
	 *        	                     	This can happen if the @a state variable was not initialized, or it was released previously.
	 * @retval	#YepStatusUnsupportedHardware	The processor does not have cycle counter.
	 * @retval	#YepStatusUnsupportedSoftware	The operating system does not provide access to the CPU cycle counter.
	 * @retval	#YepStatusSystemError	An attempt to read the cycle counter or release the OS resources failed inside the OS kernel.
	 * @see yepLibrary_GetCpuCyclesAcquire
	 */
	Status yepLibrary_GetCpuCyclesRelease(ref ulong state, out ulong cycles);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns the number of ticks of the high-resolution system timer.
	 * @param[out]	ticks	Pointer to a variable where the number of timer ticks will be stored.
	 *            	     	If the function fails, the value of the variable at this address is not changed.
	 * @retval	#YepStatusOk	The number of timer ticks is successfully stored to the variable pointed by @a ticks parameter.
	 * @retval	#YepStatusNullPointer	The @a ticks pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to read the high-resolution timer failed inside the OS kernel.
	 */
	Status yepLibrary_GetTimerTicks(out ulong ticks);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns the number of ticks of the system timer per second.
	 * @param[out]	frequency	Pointer to a variable where the number of timer ticks per second will be stored.
	 * @retval	#YepStatusOk	The number of timer ticks is successfully stored to the variable pointed by @a frequency parameter.
	 * @retval	#YepStatusNullPointer	The @a frequency pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to quenry the high-resolution timer parameters failed inside the OS kernel.
	 */
	Status yepLibrary_GetTimerFrequency(out ulong frequency);
	/**
	 * @ingroup yepLibrary
	 * @brief	Returns the minimum time difference in nanoseconds which can be measured by the high-resolution system timer.
	 * @param[out]	accuracy	Pointer to a variable where the timer accuracy will be stored.
	 * @retval	#YepStatusOk	The accuracy of the timer is successfully stored to the variable pointed by @a accuracy parameter.
	 * @retval	#YepStatusNullPointer	The @a accuracy pointer is null.
	 * @retval	#YepStatusSystemError	An attempt to measure the accuracy of high-resolution timer failed inside the OS kernel.
	 */
	Status yepLibrary_GetTimerAccuracy(out ulong accuracy);
}
