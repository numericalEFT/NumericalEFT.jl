/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	x86-specific ISA extensions.
 * @see	Library#isSupported(CpuIsaFeature)
 */
public class X86CpuIsaFeature extends CpuIsaFeature {

	/** @brief x87 FPU integrated on chip. */
	public static final X86CpuIsaFeature FPU        = new X86CpuIsaFeature(0);
	/** @brief x87 CPUID instruction. */
	public static final X86CpuIsaFeature Cpuid      = new X86CpuIsaFeature(1);
	/** @brief RDTSC instruction. */
	public static final X86CpuIsaFeature Rdtsc      = new X86CpuIsaFeature(2);
	/** @brief CMOV, FCMOV, and FCOMI/FUCOMI instructions. */
	public static final X86CpuIsaFeature CMOV       = new X86CpuIsaFeature(3);
	/** @brief SYSENTER and SYSEXIT instructions. */
	public static final X86CpuIsaFeature SYSENTER   = new X86CpuIsaFeature(4);
	/** @brief SYSCALL and SYSRET instructions. */
	public static final X86CpuIsaFeature SYSCALL    = new X86CpuIsaFeature(5);
	/** @brief RDMSR and WRMSR instructions. */
	public static final X86CpuIsaFeature MSR        = new X86CpuIsaFeature(6);
	/** @brief CLFLUSH instruction. */
	public static final X86CpuIsaFeature Clflush    = new X86CpuIsaFeature(7);
	/** @brief MONITOR and MWAIT instructions. */
	public static final X86CpuIsaFeature MONITOR    = new X86CpuIsaFeature(8);
	/** @brief FXSAVE and FXRSTOR instructions. */
	public static final X86CpuIsaFeature FXSAVE     = new X86CpuIsaFeature(9);
	/** @brief XSAVE, XRSTOR, XGETBV, and XSETBV instructions. */
	public static final X86CpuIsaFeature XSAVE      = new X86CpuIsaFeature(10);
	/** @brief CMPXCHG8B instruction. */
	public static final X86CpuIsaFeature Cmpxchg8b  = new X86CpuIsaFeature(11);
	/** @brief CMPXCHG16B instruction. */
	public static final X86CpuIsaFeature Cmpxchg16b = new X86CpuIsaFeature(12);
	/** @brief Support for 64-bit mode. */
	public static final X86CpuIsaFeature X64        = new X86CpuIsaFeature(13);
	/** @brief Support for LAHF and SAHF instructions in 64-bit mode. */
	public static final X86CpuIsaFeature LahfSahf64 = new X86CpuIsaFeature(14);
	/** @brief RDFSBASE, RDGSBASE, WRFSBASE, and WRGSBASE instructions. */
	public static final X86CpuIsaFeature FsGsBase   = new X86CpuIsaFeature(15);
	/** @brief MOVBE instruction. */
	public static final X86CpuIsaFeature Movbe      = new X86CpuIsaFeature(16);
	/** @brief POPCNT instruction. */
	public static final X86CpuIsaFeature Popcnt     = new X86CpuIsaFeature(17);
	/** @brief LZCNT instruction. */
	public static final X86CpuIsaFeature Lzcnt      = new X86CpuIsaFeature(18);
	/** @brief BMI instruction set. */
	public static final X86CpuIsaFeature BMI        = new X86CpuIsaFeature(19);
	/** @brief BMI 2 instruction set. */
	public static final X86CpuIsaFeature BMI2       = new X86CpuIsaFeature(20);
	/** @brief TBM instruction set. */
	public static final X86CpuIsaFeature TBM        = new X86CpuIsaFeature(21);
	/** @brief RDRAND instruction. */
	public static final X86CpuIsaFeature Rdrand     = new X86CpuIsaFeature(22);
	/** @brief Padlock Advanced Cryptography Engine on chip. */
	public static final X86CpuIsaFeature ACE        = new X86CpuIsaFeature(23);
	/** @brief Padlock Advanced Cryptography Engine 2 on chip. */
	public static final X86CpuIsaFeature ACE2       = new X86CpuIsaFeature(24);
	/** @brief Padlock Random Number Generator on chip. */
	public static final X86CpuIsaFeature RNG        = new X86CpuIsaFeature(25);
	/** @brief Padlock Hash Engine on chip. */
	public static final X86CpuIsaFeature PHE        = new X86CpuIsaFeature(26);
	/** @brief Padlock Montgomery Multiplier on chip. */
	public static final X86CpuIsaFeature PMM        = new X86CpuIsaFeature(27);
	/** @brief AES instruction set. */
	public static final X86CpuIsaFeature AES        = new X86CpuIsaFeature(28);
	/** @brief PCLMULQDQ instruction. */
	public static final X86CpuIsaFeature Pclmulqdq  = new X86CpuIsaFeature(29);
	/** @brief RDTSCP instruction. */
	public static final X86CpuIsaFeature Rdtscp     = new X86CpuIsaFeature(30);
	/** @brief Lightweight Profiling extension. */
	public static final X86CpuIsaFeature LPW        = new X86CpuIsaFeature(31);
	/** @brief Hardware Lock Elision extension. */
	public static final X86CpuIsaFeature HLE        = new X86CpuIsaFeature(32);
	/** @brief Restricted Transactional Memory extension. */
	public static final X86CpuIsaFeature RTM        = new X86CpuIsaFeature(33);
	/** @brief XTEST instruction. */
	public static final X86CpuIsaFeature Xtest      = new X86CpuIsaFeature(34);
	/** @brief RDSEED instruction. */
	public static final X86CpuIsaFeature Rdseed     = new X86CpuIsaFeature(35);
	/** @brief ADCX and ADOX instructions. */
	public static final X86CpuIsaFeature ADX        = new X86CpuIsaFeature(36);
	/** @brief SHA instruction set. */
	public static final X86CpuIsaFeature SHA        = new X86CpuIsaFeature(37);
	/** @brief Memory Protection Extension. */
	public static final X86CpuIsaFeature MPX        = new X86CpuIsaFeature(38);

	protected X86CpuIsaFeature(int id) {
		super(id, CpuArchitecture.X86.getId());
	}

};
