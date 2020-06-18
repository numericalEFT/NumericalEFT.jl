/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	MIPS-specific SIMD extensions.
 * @see	Library#isSupported(CpuSimdFeature)
 */
public class MipsCpuSimdFeature extends CpuSimdFeature {

	/** @brief MDMX instruction set. */
	public static final MipsCpuSimdFeature MDMX         = new MipsCpuSimdFeature(0);
	/** @brief Paired-single instructions. */
	public static final MipsCpuSimdFeature PairedSingle = new MipsCpuSimdFeature(1);
	/** @brief MIPS3D instruction set. */
	public static final MipsCpuSimdFeature MIPS3D       = new MipsCpuSimdFeature(2);
	/** @brief MIPS DSP extension. */
	public static final MipsCpuSimdFeature DSP          = new MipsCpuSimdFeature(3);
	/** @brief MIPS DSP Release 2 extension. */
	public static final MipsCpuSimdFeature DSP2         = new MipsCpuSimdFeature(4);
	/** @brief Loongson (Godson) MMX instruction set. */
	public static final MipsCpuSimdFeature GodsonMMX    = new MipsCpuSimdFeature(5);
	/** @brief Ingenic Media Extension. */
	public static final MipsCpuSimdFeature MXU          = new MipsCpuSimdFeature(6);
	/** @brief Ingenic Media Extension 2. */
	public static final MipsCpuSimdFeature MXU2         = new MipsCpuSimdFeature(7);

	protected MipsCpuSimdFeature(int id) {
		super(id, CpuArchitecture.MIPS.getId());
	}

};
