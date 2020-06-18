/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	ARM-specific non-ISA processor or system features.
 * @see	Library#isSupported(CpuSystemFeature)
 */
public class ArmCpuSystemFeature extends CpuSystemFeature {

	/** @brief VFP vector mode is supported in hardware. */
	public static final ArmCpuSystemFeature VFPVectorMode = new ArmCpuSystemFeature(32);
	/** @brief The CPU has FPA registers (f0-f7), and the operating system preserves them during context switch. */
	public static final ArmCpuSystemFeature FPA           = new ArmCpuSystemFeature(56);
	/** @brief The CPU has WMMX registers (wr0-wr15), and the operating system preserves them during context switch. */
	public static final ArmCpuSystemFeature WMMX          = new ArmCpuSystemFeature(57);
	/** @brief The CPU has s0-s31 VFP registers, and the operating system preserves them during context switch. */
	public static final ArmCpuSystemFeature S32           = new ArmCpuSystemFeature(58);
	/** @brief The CPU has d0-d31 VFP registers, and the operating system preserves them during context switch. */
	public static final ArmCpuSystemFeature D32           = new ArmCpuSystemFeature(59);

	protected ArmCpuSystemFeature(int id) {
		super(id, CpuArchitecture.ARM.getId());
	}

};
