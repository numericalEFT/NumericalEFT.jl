/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	IA64-specific ISA extensions.
 * @see	Library#isSupported(CpuIsaFeature)
 */
public class IA64CpuIsaFeature extends CpuIsaFeature {
	
	/** @brief Long branch instruction. */
	public static final IA64CpuIsaFeature Brl       = new IA64CpuIsaFeature(0);
	/** @brief Atomic 128-bit (16-byte) loads, stores, and CAS. */
	public static final IA64CpuIsaFeature Atomic128 = new IA64CpuIsaFeature(1);
	/** @brief CLZ (count leading zeros) instruction. */
	public static final IA64CpuIsaFeature Clz       = new IA64CpuIsaFeature(2);
	/** @brief MPY4 and MPYSHL4 (Truncated 32-bit multiplication) instructions. */
	public static final IA64CpuIsaFeature Mpy4      = new IA64CpuIsaFeature(3);

	protected IA64CpuIsaFeature(int id) {
		super(id, CpuArchitecture.IA64.getId());
	}

};
