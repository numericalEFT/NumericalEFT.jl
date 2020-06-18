/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	x86-specific SIMD extensions.
 * @see	Library#isSupported(CpuSimdFeature)
 */
public class X86CpuSimdFeature extends CpuSimdFeature {
	
	/** @brief MMX instruction set. */
	public static final X86CpuSimdFeature MMX               = new X86CpuSimdFeature(0);
	/** @brief MMX+ instruction set. */
	public static final X86CpuSimdFeature MMXPlus           = new X86CpuSimdFeature(1);
	/** @brief EMMX instruction set. */
	public static final X86CpuSimdFeature EMMX              = new X86CpuSimdFeature(2);
	/** @brief 3dnow! instruction set. */
	public static final X86CpuSimdFeature ThreeDNow         = new X86CpuSimdFeature(3);
	/** @brief 3dnow!+ instruction set. */
	public static final X86CpuSimdFeature ThreeDNowPlus     = new X86CpuSimdFeature(4);
	/** @brief 3dnow! prefetch instructions. */
	public static final X86CpuSimdFeature ThreeDNowPrefetch = new X86CpuSimdFeature(5);
	/** @brief Geode 3dnow! instructions. */
	public static final X86CpuSimdFeature ThreeDNowGeode    = new X86CpuSimdFeature(6);
	/** @brief SSE instruction set. */
	public static final X86CpuSimdFeature SSE               = new X86CpuSimdFeature(7);
	/** @brief SSE 2 instruction set. */
	public static final X86CpuSimdFeature SSE2              = new X86CpuSimdFeature(8);
	/** @brief SSE 3 instruction set. */
	public static final X86CpuSimdFeature SSE3              = new X86CpuSimdFeature(9);
	/** @brief SSSE 3 instruction set. */
	public static final X86CpuSimdFeature SSSE3             = new X86CpuSimdFeature(10);
	/** @brief SSE 4.1 instruction set. */
	public static final X86CpuSimdFeature SSE4_1            = new X86CpuSimdFeature(11);
	/** @brief SSE 4.2 instruction set. */
	public static final X86CpuSimdFeature SSE4_2            = new X86CpuSimdFeature(12);
	/** @brief SSE 4A instruction set. */
	public static final X86CpuSimdFeature SSE4A             = new X86CpuSimdFeature(13);
	/** @brief AVX instruction set. */
	public static final X86CpuSimdFeature AVX               = new X86CpuSimdFeature(14);
	/** @brief AVX 2 instruction set. */
	public static final X86CpuSimdFeature AVX2              = new X86CpuSimdFeature(15);
	/** @brief XOP instruction set. */
	public static final X86CpuSimdFeature XOP               = new X86CpuSimdFeature(16);
	/** @brief F16C instruction set. */
	public static final X86CpuSimdFeature F16C              = new X86CpuSimdFeature(17);
	/** @brief FMA3 instruction set. */
	public static final X86CpuSimdFeature FMA3              = new X86CpuSimdFeature(18);
	/** @brief FMA4 instruction set. */
	public static final X86CpuSimdFeature FMA4              = new X86CpuSimdFeature(19);
	/** @brief Knights Ferry (aka Larrabee) instruction set. */
	public static final X86CpuSimdFeature KNF               = new X86CpuSimdFeature(20);
	/** @brief Knights Corner (aka Xeon Phi) instruction set. */
	public static final X86CpuSimdFeature KNC               = new X86CpuSimdFeature(21);
	/** @brief AVX-512 Foundation instruction set. */
	public static final X86CpuSimdFeature AVX512F           = new X86CpuSimdFeature(22);
	/** @brief AVX-512 Conflict Detection instruction set. */
	public static final X86CpuSimdFeature AVX512CD          = new X86CpuSimdFeature(23);
	/** @brief AVX-512 Exponential and Reciprocal instruction set. */
	public static final X86CpuSimdFeature AVX512ER          = new X86CpuSimdFeature(24);
	/** @brief AVX-512 Prefetch instruction set. */
	public static final X86CpuSimdFeature AVX512PF          = new X86CpuSimdFeature(25);

	protected X86CpuSimdFeature(int id) {
		super(id, CpuArchitecture.X86.getId());
	}

};
