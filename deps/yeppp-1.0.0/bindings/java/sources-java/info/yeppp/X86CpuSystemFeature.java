/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	x86-specific non-ISA processor or system features.
 * @see	Library#isSupported(CpuSystemFeature)
 */
public class X86CpuSystemFeature extends CpuSystemFeature {

	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine. */
	public static final X86CpuSystemFeature ACE           = new X86CpuSystemFeature(32);
	/** @brief Processor and the operating system support the Padlock Advanced Cryptography Engine 2. */
	public static final X86CpuSystemFeature ACE2          = new X86CpuSystemFeature(33);
	/** @brief Processor and the operating system support the Padlock Random Number Generator. */
	public static final X86CpuSystemFeature RNG           = new X86CpuSystemFeature(34);
	/** @brief Processor and the operating system support the Padlock Hash Engine. */
	public static final X86CpuSystemFeature PHE           = new X86CpuSystemFeature(35);
	/** @brief Processor and the operating system support the Padlock Montgomery Multiplier. */
	public static final X86CpuSystemFeature PMM           = new X86CpuSystemFeature(36);
	/** @brief Processor allows to use misaligned memory operands in SSE instructions other than loads and stores. */
	public static final X86CpuSystemFeature MisalignedSSE = new X86CpuSystemFeature(37);
	/** @brief The CPU has x87 registers, and the operating system preserves them during context switch. */
	public static final X86CpuSystemFeature FPU           = new X86CpuSystemFeature(52);
	/** @brief The CPU has xmm (SSE) registers, and the operating system preserves them during context switch. */
	public static final X86CpuSystemFeature XMM           = new X86CpuSystemFeature(53);
	/** @brief The CPU has ymm (AVX) registers, and the operating system preserves them during context switch. */
	public static final X86CpuSystemFeature YMM           = new X86CpuSystemFeature(54);
	/** @brief The CPU has zmm (MIC or AVX-512) registers, and the operating system preserves them during context switch. */
	public static final X86CpuSystemFeature ZMM           = new X86CpuSystemFeature(55);
	/** @brief The CPU has bnd (MPX) registers, and the operating system preserved them during context switch. */
	public static final X86CpuSystemFeature BND           = new X86CpuSystemFeature(56);

	protected X86CpuSystemFeature(int id) {
		super(id, CpuArchitecture.X86.getId());
	}

};
