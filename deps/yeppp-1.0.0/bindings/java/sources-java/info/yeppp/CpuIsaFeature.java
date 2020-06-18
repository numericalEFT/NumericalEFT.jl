/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	ISA extensions.
 * @see	CpuArchitecture#iterateIsaFeatures(), Library#isSupported(CpuIsaFeature), X86CpuIsaFeature, ArmCpuIsaFeature, MipsCpuIsaFeature, IA64CpuIsaFeature
 */
public class CpuIsaFeature {
	static {
		Library.load();
	}

	private final int architectureId;
	private final int id;

	protected CpuIsaFeature(int id, int architectureId) {
		this.id = id;
		this.architectureId = architectureId;
	}

	protected CpuIsaFeature(int id) {
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

	public final boolean equals(CpuIsaFeature other) {
		if (other == null) {
			return false;
		} else {
			return (this.id == other.id) && (this.architectureId == other.architectureId);
		}
	}

	@Override
	public final boolean equals(Object other) {
		if (other instanceof CpuIsaFeature) {
			return this.equals((CpuIsaFeature)other);
		} else {
			return false;
		}
	}

	@Override
	public final int hashCode() {
		return this.id ^ this.architectureId;
	}

	/**
	 * @brief	Provides a string ID for this ISA extension.
	 * @return	A string which starts with a Latin letter and contains only Latin letters, digits, and underscore symbol.
	 * @see	getDescription()
	 */
	@Override
	public final String toString() {
		return CpuIsaFeature.toString(this.id, this.architectureId);
	}

	/**
	 * @brief	Provides a text description for this ISA extension.
	 * @return	A string description which can contain spaces and non-ASCII characters.
	 * @see	toString()
	 */
	public final String getDescription() {
		return CpuIsaFeature.getDescription(this.id, this.architectureId);
	}
};
