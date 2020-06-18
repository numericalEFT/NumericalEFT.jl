/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	ARM-specific ISA extensions.
 * @see	Library#isSupported(CpuIsaFeature)
 */
public class ArmCpuIsaFeature extends CpuIsaFeature {

	/** @brief ARMv4 instruction set. */
	public static final ArmCpuIsaFeature V4         = new ArmCpuIsaFeature(0);
	/** @brief ARMv5 instruciton set. */
	public static final ArmCpuIsaFeature V5         = new ArmCpuIsaFeature(1);
	/** @brief ARMv5 DSP instructions. */
	public static final ArmCpuIsaFeature V5E        = new ArmCpuIsaFeature(2);
	/** @brief ARMv6 instruction set. */
	public static final ArmCpuIsaFeature V6         = new ArmCpuIsaFeature(3);
	/** @brief ARMv6 Multiprocessing extensions. */
	public static final ArmCpuIsaFeature V6K        = new ArmCpuIsaFeature(4);
	/** @brief ARMv7 instruction set. */
	public static final ArmCpuIsaFeature V7         = new ArmCpuIsaFeature(5);
	/** @brief ARMv7 Multiprocessing extensions. */
	public static final ArmCpuIsaFeature V7MP       = new ArmCpuIsaFeature(6);
	/** @brief Thumb mode. */
	public static final ArmCpuIsaFeature Thumb      = new ArmCpuIsaFeature(7);
	/** @brief Thumb 2 mode. */
	public static final ArmCpuIsaFeature Thumb2     = new ArmCpuIsaFeature(8);
	/** @brief Thumb EE mode. */
	public static final ArmCpuIsaFeature ThumbEE    = new ArmCpuIsaFeature(9);
	/** @brief Jazelle extensions. */
	public static final ArmCpuIsaFeature Jazelle    = new ArmCpuIsaFeature(10);
	/** @brief FPA instructions. */
	public static final ArmCpuIsaFeature FPA        = new ArmCpuIsaFeature(11);
	/** @brief VFP instruction set. */
	public static final ArmCpuIsaFeature VFP        = new ArmCpuIsaFeature(12);
	/** @brief VFPv2 instruction set. */
	public static final ArmCpuIsaFeature VFP2       = new ArmCpuIsaFeature(13);
	/** @brief VFPv3 instruction set. */
	public static final ArmCpuIsaFeature VFP3       = new ArmCpuIsaFeature(14);
	/** @brief VFP implementation with 32 double-precision registers. */
	public static final ArmCpuIsaFeature VFPd32     = new ArmCpuIsaFeature(15);
	/** @brief VFPv3 half precision extension. */
	public static final ArmCpuIsaFeature VFP3HP     = new ArmCpuIsaFeature(16);
	/** @brief VFPv4 instruction set. */
	public static final ArmCpuIsaFeature VFP4       = new ArmCpuIsaFeature(17);
	/** @brief SDIV and UDIV instructions. */
	public static final ArmCpuIsaFeature Div        = new ArmCpuIsaFeature(18);
	/** @brief Marvell Armada instruction extensions. */
	public static final ArmCpuIsaFeature Armada     = new ArmCpuIsaFeature(19);

	protected ArmCpuIsaFeature(int id) {
		super(id, CpuArchitecture.ARM.getId());
	}

};
