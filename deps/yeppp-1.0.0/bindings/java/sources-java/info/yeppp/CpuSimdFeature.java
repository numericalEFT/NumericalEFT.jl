/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	SIMD extensions.
 * @see	CpuArchitecture#iterateSimdFeatures(), Library#isSupported(CpuSimdFeature), X86CpuSimdFeature, ArmCpuSimdFeature, MipsCpuSimdFeature
 */
public class CpuSimdFeature {
	static {
		Library.load();
	}

	private final int architectureId;
	private final int id;

	protected CpuSimdFeature(int id, int architectureId) {
		this.id = id;
		this.architectureId = architectureId;
	}

	protected CpuSimdFeature(int id) {
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

	public final boolean equals(CpuSimdFeature other) {
		if (other == null) {
			return false;
		} else {
			return (this.id == other.id) && (this.architectureId == other.architectureId);
		}
	}

	@Override
	public final boolean equals(Object other) {
		if (other instanceof CpuSimdFeature) {
			return this.equals((CpuSimdFeature)other);
		} else {
			return false;
		}
	}

	@Override
	public final int hashCode() {
		return this.id ^ this.architectureId;
	}

	/**
	 * @brief	Provides a string ID for this SIMD extension.
	 * @return	A string which starts with a Latin letter and contains only Latin letters, digits, and underscore symbol.
	 * @see	getDescription()
	 */
	@Override
	public final String toString() {
		return CpuSimdFeature.toString(this.id, this.architectureId);
	}

	/**
	 * @brief	Provides a text description for this ISA extension.
	 * @return	A string description which can contain spaces and non-ASCII characters.
	 * @see	toString()
	 */
	public final String getDescription() {
		return CpuSimdFeature.getDescription(this.id, this.architectureId);
	}
};
