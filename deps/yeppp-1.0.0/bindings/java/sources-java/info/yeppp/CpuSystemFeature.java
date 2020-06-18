/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	Non-ISA processor or system features.
 * @see	CpuArchitecture#iterateSystemFeatures(), Library#isSupported(CpuSystemFeature), X86CpuSystemFeature, ArmCpuSystemFeature
 */
public class CpuSystemFeature {
	static {
		Library.load();
	}

	/** @brief The processor has a built-in cycle counter, and the operating system provides a way to access it. */
	public static final CpuSystemFeature CycleCounter      = new CpuSystemFeature(0);
	/** @brief The processor has a 64-bit cycle counter, or the operating system provides an abstraction of a 64-bit cycle counter. */
	public static final CpuSystemFeature CycleCounter64Bit = new CpuSystemFeature(1);
	/** @brief The processor and the operating system allows to use 64-bit pointers. */
	public static final CpuSystemFeature AddressSpace64Bit = new CpuSystemFeature(2);
	/** @brief The processor and the operating system allows to do 64-bit arithmetical operations on general-purpose registers. */
	public static final CpuSystemFeature GPRegisters64Bit  = new CpuSystemFeature(3);
	/** @brief The processor and the operating system allows misaligned memory reads and writes. */
	public static final CpuSystemFeature MisalignedAccess  = new CpuSystemFeature(4);
	/** @brief The processor or the operating system support at most one hardware thread. */
	public static final CpuSystemFeature SingleThreaded    = new CpuSystemFeature(5);

	private final int architectureId;
	private final int id;

	protected CpuSystemFeature(int id, int architectureId) {
		this.id = id;
		this.architectureId = architectureId;
	}

	protected CpuSystemFeature(int id) {
		this.id = id;
		this.architectureId = CpuArchitecture.Unknown.getId();
	}

	protected final int getId() {
		return this.id;
	}

	protected final int getArchitectureId() {
		return this.architectureId;
	}

	protected static native boolean isDefined(int id, int architectureId);
	private static native String toString(int id, int architectureId);
	private static native String getDescription(int id, int architectureId);

	public final boolean equals(CpuSystemFeature other) {
		if (other == null) {
			return false;
		} else {
			return (this.id == other.id) && (this.architectureId == other.architectureId);
		}
	}

	@Override
	public final boolean equals(Object other) {
		if (other instanceof CpuSystemFeature) {
			return this.equals((CpuSystemFeature)other);
		} else {
			return false;
		}
	}

	@Override
	public final int hashCode() {
		return this.id ^ this.architectureId;
	}

	/**
	 * @brief	Provides a string ID for this non-ISA processor or system feature.
	 * @return	A string which starts with a Latin letter and contains only Latin letters, digits, and underscore symbol.
	 * @see	getDescription()
	 */
	@Override
	public final String toString() {
		return CpuSystemFeature.toString(this.id, this.architectureId);
	}

	/**
	 * @brief	Provides a text description for this non-ISA processor or system feature.
	 * @return	A string description which can contain spaces and non-ASCII characters.
	 * @see	toString()
	 */
	public final String getDescription() {
		return CpuSystemFeature.getDescription(this.id, this.architectureId);
	}
};
