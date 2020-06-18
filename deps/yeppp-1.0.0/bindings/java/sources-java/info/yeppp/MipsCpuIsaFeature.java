/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	MIPS-specific ISA extensions.
 * @see	Library#isSupported(CpuIsaFeature)
 */
public class MipsCpuIsaFeature extends CpuIsaFeature {

	/** @brief MIPS I instructions. */
	public static final MipsCpuIsaFeature MIPS_I    = new MipsCpuIsaFeature(0);
	/** @brief MIPS II instructions. */
	public static final MipsCpuIsaFeature MIPS_II   = new MipsCpuIsaFeature(1);
	/** @brief MIPS III instructions. */
	public static final MipsCpuIsaFeature MIPS_III  = new MipsCpuIsaFeature(2);
	/** @brief MIPS IV instructions. */
	public static final MipsCpuIsaFeature MIPS_IV   = new MipsCpuIsaFeature(3);
	/** @brief MIPS V instructions. */
	public static final MipsCpuIsaFeature MIPS_V    = new MipsCpuIsaFeature(4);
	/** @brief MIPS32/MIPS64 Release 1 instructions. */
	public static final MipsCpuIsaFeature R1        = new MipsCpuIsaFeature(5);
	/** @brief MIPS32/MIPS64 Release 2 instructions. */
	public static final MipsCpuIsaFeature R2        = new MipsCpuIsaFeature(6);
	/** @brief FPU with S, D, and W formats and instructions. */
	public static final MipsCpuIsaFeature FPU       = new MipsCpuIsaFeature(24);
	/** @brief MIPS16 extension. */
	public static final MipsCpuIsaFeature MIPS16    = new MipsCpuIsaFeature(25);
	/** @brief SmartMIPS extension. */
	public static final MipsCpuIsaFeature SmartMIPS = new MipsCpuIsaFeature(26);
	/** @brief Multi-threading extension. */
	public static final MipsCpuIsaFeature MT        = new MipsCpuIsaFeature(27);
	/** @brief MicroMIPS extension. */
	public static final MipsCpuIsaFeature MicroMIPS = new MipsCpuIsaFeature(28);
	/** @brief MIPS virtualization extension. */
	public static final MipsCpuIsaFeature VZ        = new MipsCpuIsaFeature(29);

	protected MipsCpuIsaFeature(int id) {
		super(id, CpuArchitecture.MIPS.getId());
	}

};
