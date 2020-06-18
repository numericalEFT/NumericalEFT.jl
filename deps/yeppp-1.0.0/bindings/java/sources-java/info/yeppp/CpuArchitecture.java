/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

import java.util.Iterator;
import java.util.NoSuchElementException;

/** @brief	The basic instruction set architecture of the processor. */
/** @see	Library#getCpuArchitecture */
public final class CpuArchitecture {
	static {
		Library.load();
	}

	/** @brief	Instruction set architecture is not known to the library. */
	/** @details	This value is never returned on supported architectures. */
	public static final CpuArchitecture Unknown = new CpuArchitecture(0);
	/** @brief	x86 or x86-64 ISA. */
	public static final CpuArchitecture X86     = new CpuArchitecture(1);
	/** @brief	ARM ISA. */
	public static final CpuArchitecture ARM     = new CpuArchitecture(2);
	/** @brief	MIPS ISA. */
	public static final CpuArchitecture MIPS    = new CpuArchitecture(3);
	/** @brief	PowerPC ISA. */
	public static final CpuArchitecture PowerPC = new CpuArchitecture(4);
	/** @brief	IA64 ISA. */
	public static final CpuArchitecture IA64    = new CpuArchitecture(5);
	/** @brief	SPARC ISA. */
	public static final CpuArchitecture SPARC   = new CpuArchitecture(6);

	private final int id;

	protected CpuArchitecture(int id) {
		this.id = id;
	}

	protected int getId() {
		return this.id;
	}

	private final class CpuIsaFeaturesIterator implements Iterator<CpuIsaFeature> {
		@Override
		public CpuIsaFeature next() {
			if (this.nextId < 64) {
				final int currentId = this.nextId;
				this.nextId = this.findNext(this.nextId);
				if (CpuArchitecture.this.equals(CpuArchitecture.X86)) {
					return new X86CpuIsaFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.ARM)) {
					return new ArmCpuIsaFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.MIPS)) {
					return new MipsCpuIsaFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.IA64)) {
					return new IA64CpuIsaFeature(currentId);
				} else {
					return new CpuIsaFeature(currentId, CpuArchitecture.this.getId());
				}
			} else {
				throw new NoSuchElementException(String.format("No more CPU ISA Extensions for architecture %s", CpuArchitecture.this.toString()));
			}
		}

		@Override
		public boolean hasNext() {
			return this.nextId < 64;
		}

		@Override
		public void remove() {
			throw new UnsupportedOperationException();
		}

		private CpuIsaFeaturesIterator() {
			this.nextId = findNext(-1);
		}

		private final int findNext(int id) {
			while (++id < 64) {
				if (CpuIsaFeature.isDefined(id, CpuArchitecture.this.getId())) {
					return id;
				}
			}
			return id;
		}

		private int nextId;
	}

	private final class CpuSimdFeaturesIterator implements Iterator<CpuSimdFeature> {
		@Override
		public CpuSimdFeature next() {
			if (this.nextId < 64) {
				final int currentId = this.nextId;
				this.nextId = this.findNext(this.nextId);
				if (CpuArchitecture.this.equals(CpuArchitecture.X86)) {
					return new X86CpuSimdFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.ARM)) {
					return new ArmCpuSimdFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.MIPS)) {
					return new MipsCpuSimdFeature(currentId);
				} else {
					return new CpuSimdFeature(currentId, CpuArchitecture.this.getId());
				}
			} else {
				throw new NoSuchElementException(String.format("No more CPU SIMD Extensions for architecture %s", CpuArchitecture.this.toString()));
			}
		}

		@Override
		public boolean hasNext() {
			return this.nextId < 64;
		}

		@Override
		public void remove() {
			throw new UnsupportedOperationException();
		}

		private CpuSimdFeaturesIterator() {
			this.nextId = findNext(-1);
		}

		private final int findNext(int id) {
			while (++id < 64) {
				if (CpuSimdFeature.isDefined(id, CpuArchitecture.this.getId())) {
					return id;
				}
			}
			return id;
		}

		private int nextId;
	}

	private final class CpuSystemFeaturesIterator implements Iterator<CpuSystemFeature> {
		@Override
		public CpuSystemFeature next() {
			if (this.nextId < 64) {
				final int currentId = this.nextId;
				this.nextId = this.findNext(this.nextId);
				if (CpuArchitecture.this.equals(CpuArchitecture.X86)) {
					return new X86CpuSystemFeature(currentId);
				} else if (CpuArchitecture.this.equals(CpuArchitecture.ARM)) {
					return new ArmCpuSystemFeature(currentId);
				} else {
					return new CpuSystemFeature(currentId, CpuArchitecture.this.getId());
				}
			} else {
				throw new NoSuchElementException(String.format("No more non-ISA CPU and System Features for architecture %s", CpuArchitecture.this.toString()));
			}
		}

		@Override
		public boolean hasNext() {
			return this.nextId < 64;
		}

		@Override
		public void remove() {
			throw new UnsupportedOperationException();
		}

		private CpuSystemFeaturesIterator() {
			this.nextId = findNext(-1);
		}

		private final int findNext(int id) {
			while (++id < 64) {
				if (CpuSystemFeature.isDefined(id, CpuArchitecture.this.getId())) {
					return id;
				}
			}
			return id;
		}

		private int nextId;
	}

	/**
	 * @brief	Provides an iterator over ISA features potentially available on this architecture.
	 * @details	For #Unknown architecture provides an iterator over common ISA features.
	 * @return	An iterator over ISA features of this architecture.
	 */
	public final Iterator<CpuIsaFeature> iterateIsaFeatures() {
		return this.new CpuIsaFeaturesIterator();
	}

	/**
	 * @brief	Provides an iterator over SIMD features potentially available on this architecture.
	 * @details	For #Unknown architecture provides an iterator over common SIMD features.
	 * @return	An iterator over SIMD features of this architecture.
	 */
	public final Iterator<CpuSimdFeature> iterateSimdFeatures() {
		return this.new CpuSimdFeaturesIterator();
	}

	/**
	 * @brief	Provides an iterator over non-ISA processor and system features potentially available on this architecture.
	 * @details	For #Unknown architecture provides an iterator over common non-ISA CPU and system features.
	 * @return	An iterator over non-ISA processor and system features of this architecture.
	 */
	public final Iterator<CpuSystemFeature> iterateSystemFeatures() {
		return this.new CpuSystemFeaturesIterator();
	}

	private static native String toString(int id);
	private static native String getDescription(int id);

	public final boolean equals(CpuArchitecture other) {
		if (other == null) {
			return false;
		} else {
			return this.id == other.id;
		}
	}

	@Override
	public final boolean equals(Object other) {
		if (other instanceof CpuArchitecture) {
			return this.equals((CpuArchitecture)other);
		} else {
			return false;
		}
	}

	@Override
	public final int hashCode() {
		return this.id;
	}

	/**
	 * @brief	Provides a string ID for this CPU architecture.
	 * @return	A string which starts with a Latin letter and contains only Latin letters, digits, and underscore symbol.
	 * @see	getDescription()
	 */
	@Override
	public final String toString() {
		return CpuArchitecture.toString(this.id);
	}

	/**
	 * @brief	Provides a text description for this CPU architecture.
	 * @return	A string description which can contain spaces and non-ASCII characters.
	 * @see	toString()
	 */
	public final String getDescription() {
		return CpuArchitecture.getDescription(this.id);
	}
};
