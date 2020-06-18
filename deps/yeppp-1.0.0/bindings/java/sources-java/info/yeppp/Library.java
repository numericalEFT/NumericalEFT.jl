/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

package info.yeppp;

/**
 * @brief	Non-computational functions for checking library version, quering information about processor, and benchmarking.
 */
public class Library {
	static {
		Library.loadOnce();

		final int[] versionNumbers = new int[4];
		final String releaseName = Library.getVersionInfo(versionNumbers);
		Library.version = new Version(versionNumbers[0], versionNumbers[1], versionNumbers[2], versionNumbers[3], releaseName);
	}

	/**
	 * @brief	Queries the ticks count of the high-resolution system timer.
	 * @details	The difference in ticks between two time moments divided by timer frequency gives the number of seconds between two time moments.
	 * @return	The current ticks count of the high-resolution system timer.
	 *        	This value should be interpreted as unsigned 64-bit integer.
	 * @throws	SystemException	If the attempt to read the high-resolution timer failed inside the OS kernel.
	 */
	public static native long getTimerTicks();

	/**
	 * @brief	Queries the frequency (number of ticks per second) of the high-resolution system timer.
	 * @details	The difference in ticks between two time moments divided by timer frequency gives the number of seconds between two time moments.
	 * @return	The frequency of the high-resolution system timer.
	 *        	This value should be interpreted as unsigned 64-bit integer.
	 * @throws	SystemException	If the attempt to read the high-resolution timer frequency failed inside the OS kernel.
	 */
	public static native long getTimerFrequency();

	/**
	 * @brief	Detects the minimum time difference in nanoseconds which can be measured by the high-resolution system timer.
	 * @return	The accuracy (in nanoseconds) of the high-resolution system timer.
	 *        	This value should be interpreted as unsigned 64-bit integer.
	 * @throws	SystemException	If the attempt to measure the accuracy of high-resolution timer failed inside the OS kernel.
	 */
	public static native long getTimerAccuracy();

	/**
	 * @brief	Returns information about the vendor of the processor.
	 * @return	A CpuVendor object with information about the company which designed the CPU core.
	 * @see	CpuVendor
	 */
	public static CpuVendor getCpuVendor() {
		return Library.vendor;
	}

	/**
	 * @brief	Provides information about the architecture of the processor.
	 * @return	A CpuArchitecture instance with information about the architecture of the CPU.
	 * @see	CpuArchitecture
	 */
	public static CpuArchitecture getCpuArchitecture() {
		return Library.architecture;
	}

	/**
	 * @brief	Provides information about the microarchitecture of the processor.
	 * @return	A CpuMicroarchitecture instance with information about the microarchitecture of the CPU core.
	 * @see	CpuMicroarchitecture
	 */
	public static CpuMicroarchitecture getCpuMicroarchitecture() {
		return Library.microarchitecture;
	}

	/**
	 * @brief	Initializes the processor cycle counter and starts counting the processor cycles.
	 * @details	Call #releaseCycleCounter() to get the number of processor cycles passed.
	 * @warning	This function may allocate system resources.
	 *         	To avoid resource leak, always match a successfull call to #acquireCycleCounter() with a call to #releaseCycleCounter().
	 * @warning	The cycle counters are not guaranteed to be syncronized across different processors/cores in a multiprocessor/multicore system.
	 *         	It is recommended to bind the current thread to a particular logical processor before using this function.
	 * @return	An object representing the state of the processor cycle counter. Pass this object to #releaseCycleCounter() to get the number of cycles passed.
	 * @throws	UnsupportedHardwareException	If the processor does not have cycle counter.
	 * @throws	UnsupportedSoftwareException	If the operating system does not provide access to the CPU cycle counter.
	 * @throws	SystemException	If the attempt to initialize cycle counter failed inside the OS kernel.
	 * @see	#releaseCycleCounter()
	 */
	public static CpuCycleCounterState acquireCycleCounter() {
		final long state = Library.getCpuCyclesAcquire();
		return new CpuCycleCounterState(state);
	}

	/**
	 * @ingroup yepLibrary
	 * @brief	Stops counting the processor cycles, releases the system resources associated with the cycle counter, and returns the number of cycles elapsed.
	 * @param[in,out]	cycleCounter	An object representing the state of the cycle counter returned by #acquireCycleCounter().
	 *               	            	The cycle counter should be released only once, and this function invalidates the state object.
	 * @return	The number of cycles elapsed since the call to #acquireCycleCounter().
	 *        	This value should be interpreted as unsigned 64-bit integer.
	 * @throws	IllegalStateException	The cycleCounter object is not a valid state of the cycle counter.
	 *        	                               	This can happen if the cycleCounter object was released previously.
	 * @throws	UnsupportedHardwareException	If the processor does not have cycle counter.
	 * @throws	UnsupportedSoftwareException	If the operating system does not provide access to the CPU cycle counter.
	 * @throws	SystemException	If the attempt to read the cycle counter or release the OS resources failed inside the OS kernel.
	 * @see	#acquireCycleCounter()
	 */
	public static long releaseCycleCounter(CpuCycleCounterState cycleCounter) {
		try {
			final long cycles = Library.getCpuCyclesRelease(cycleCounter.state);
			cycleCounter.state = 0l;
			return cycles;
		} catch (IllegalStateException e) {
			cycleCounter.state = 0l;
			throw e;
		}
	}

	/**
	 * @brief	Checks if the specified ISA extension is supported by the processor.
	 * @param[in]	isaFeature	An object specifying the ISA extension of interest.
	 * @retval	true	If the processor supports the specified ISA extension.
	 * @retval	false	If the processor does not support the specificed ISA extension.
	 * @see	CpuIsaFeature, X86CpuIsaFeature, ArmCpuIsaFeature, MipsCpuIsaFeature, IA64CpuIsaFeature
	 */
	public static boolean isSupported(CpuIsaFeature isaFeature) {
		if ((isaFeature.getArchitectureId() == Library.architectureId) || (isaFeature.getArchitectureId() == CpuArchitecture.Unknown.getId())) {
			final long mask = 1l << isaFeature.getId();
			return (Library.isaFeatures & mask) != 0l;
		} else {
			return false;
		}
	}

	/**
	 * @brief	Checks if the specified SIMD extension is supported by the processor.
	 * @param[in]	simdFeature	An object specifying the SIMD extension of interest.
	 * @retval	true	If the processor supports the specified SIMD extension.
	 * @retval	false	If the processor does not support the specificed SIMD extension.
	 * @see	CpuSimdFeature, X86CpuSimdFeature, ArmCpuSimdFeature, MipsCpuSimdFeature
	 */
	public static boolean isSupported(CpuSimdFeature simdFeature) {
		if ((simdFeature.getArchitectureId() == Library.architectureId) || (simdFeature.getArchitectureId() == CpuArchitecture.Unknown.getId())) {
			final long mask = 1l << simdFeature.getId();
			return (Library.simdFeatures & mask) != 0l;
		} else {
			return false;
		}
	}

	/**
	 * @brief	Checks if processor or system support the specified non-ISA feature.
	 * @param[in]	systemFeature	An object specifying the non-ISA processor or system feature of interest.
	 * @retval	true	If the specified processor or system extension is supported on this machine.
	 * @retval	false	If the specified processor or system extension is not supported on this machine.
	 * @see	CpuSystemFeature, X86CpuSystemFeature, ArmCpuSystemFeature
	 */
	public static boolean isSupported(CpuSystemFeature systemFeature) {
		if ((systemFeature.getArchitectureId() == Library.architectureId) || (systemFeature.getArchitectureId() == CpuArchitecture.Unknown.getId())) {
			final long mask = 1l << systemFeature.getId();
			return (Library.systemFeatures & mask) != 0l;
		} else {
			return false;
		}
	}

	/**
	 * @brief	Provides information about @Yeppp library version.
	 * @return	An object describing @Yeppp library version.
	 * @see	Version
	 */
	public static Version getVersion() {
		return Library.version;
	}

	/* EXPERIMENTAL! Will be removed in future versions. */
	public static native String getCpuName();
	public static native int getCpuLogicalCoresCount();
	public static native int getCpuL0ICacheSize();
	public static native int getCpuL0DCacheSize();
	public static native int getCpuL1ICacheSize();
	public static native int getCpuL1DCacheSize();
	public static native int getCpuL2CacheSize();
	public static native int getCpuL3CacheSize();

	private static native long getCpuIsaFeatures();
	private static native long getCpuSimdFeatures();
	private static native long getCpuSystemFeatures();
	private static native int getCpuVendorId();
	private static native int getCpuArchitectureId();
	private static native int getCpuMicroarchitectureId();
	private static native long getCpuCyclesAcquire();
	private static native long getCpuCyclesRelease(long state);

	/* Constants for ELF headers */
	private static final int EI_MAG0 = 0;
	private static final int EI_MAG1 = 1;
	private static final int EI_MAG2 = 2;
	private static final int EI_MAG3 = 3;
	private static final int EI_CLASS = 4;
	private static final int EI_DATA = 5;

	private static final int ELFCLASS32 = 1;
	private static final int ELFCLASS64 = 2;

	private static final int ELFDATA2LSB = 1;
	private static final int ELFDATA2MSB = 2;

	private static final int EM_386    = 3;
	private static final int EM_X86_64 = 62;
	private static final int EM_K1OM   = 181;
	private static final int EM_ARM    = 40;
	private static final int EM_ARM64  = 183;

	private static final int EF_ARM_ABIMASK        = 0xFF000000;
	private static final int EF_ARM_ABI_FLOAT_SOFT = 0x00000200;
	private static final int EF_ARM_ABI_FLOAT_HARD = 0x00000400;

	private static final int SHT_ARM_ATTRIBUTES = 0x70000003;

	/* ARM EABI build attributes */
	private static final int Tag_File = 1;
	private static final int Tag_CPU_raw_name = 4;
	private static final int Tag_CPU_name = 5;
	private static final int Tag_ABI_VFP_args = 28;
	private static final int Tag_compatibility = 32;

	private static boolean isTabNTBS(int tag) {
		switch (tag) {
			case Tag_CPU_raw_name:
			case Tag_CPU_name:
			case Tag_compatibility:
				return true;
			default:
				if (tag < 32) {
					return false;
				} else {
					return (tag % 2) == 1;
				}
		}
	}

	public static byte[] readBytes(java.io.RandomAccessFile file, int bytesToRead) {
		try {
			byte[] buffer = new byte[bytesToRead];
			int offset = 0;
			int bytesRead;
			while ((bytesRead = file.read(buffer, offset, bytesToRead - offset)) != -1) {
				offset += bytesRead;
				if (offset == bytesToRead)
					return buffer;
			}
			return null;
		} catch (Exception e) {
			return null;
		}
	}

	private static String getLibraryResource() {
		final String osArch = System.getProperty("os.arch");
		final String osName = System.getProperty("os.name");
		if ((osName != null) && (osArch != null)) {
			if (osName.startsWith("Windows")) {
				if (osArch.equals("x86")) {
					final String sunDataModel = System.getProperty("sun.arch.data.model");
					if ((sunDataModel != null) && (sunDataModel.equals("64"))) {
						return "/windows/amd64/yeppp.dll";
					} else {
						return "/windows/i586/yeppp.dll";
					}
				} else if (osArch.equals("amd64")) {
					final String sunDataModel = System.getProperty("sun.arch.data.model");
					if ((sunDataModel != null) && (sunDataModel.equals("32"))) {
						return "/windows/i586/yeppp.dll";
					} else {
						return "/windows/amd64/yeppp.dll";
					}
				}
			} else if (osName.equals("Linux")) {
				try {
					final java.io.RandomAccessFile file = new java.io.RandomAccessFile("/proc/self/exe", "r");
					byte[] identification = readBytes(file, 16);
					if ((identification[EI_MAG0] == 0x7F) && (identification[EI_MAG1] == 'E') && (identification[EI_MAG2] == 'L') && (identification[EI_MAG3] == 'F')) {
						/* Detected ELF signature  */
						final int elfEndianess = identification[EI_DATA];
						if (elfEndianess == ELFDATA2LSB) {
							/* Little endian headers and ABI */
							final int elfClass = identification[EI_CLASS];
							if (elfClass == ELFCLASS32) {
								/* ELF-32 file format */
								final byte[] header = readBytes(file, 36);
								final int machine = header[2] | (header[3] << 8);
								if (machine == EM_386) {
									return "/linux/x86/libyeppp.so";
								} else if (machine == EM_ARM) {
									final int flags = header[20] | (header[21] << 8) | (header[22] << 16) | (header[23] << 24);
									if ((flags & EF_ARM_ABIMASK) != 0) {
										/* ARM EABI header */
										final int fpFlags = flags & (EF_ARM_ABI_FLOAT_SOFT | EF_ARM_ABI_FLOAT_HARD);
										if (fpFlags == EF_ARM_ABI_FLOAT_SOFT) {
											/* Soft-float ARM EABI (armel) */
											return "/linux/armel/libyeppp.so";
										} else if (fpFlags == EF_ARM_ABI_FLOAT_HARD) {
											/* Hard-float ARM EABI (armhf) */
											return "/linux/armhf/libyeppp.so";
										} else {
											/* ARM EABI version (armel or armhf) is not specified here: need to parse sections */
											final int sectionHeadersOffset = header[16] | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
											final int sectionHeaderSize = header[30] | (header[31] << 8);
											final int sectionCount = header[32] | (header[33] << 8);

											/* Check the header size */
											if (sectionHeaderSize == 40) {
												/* Skip the null section */
												file.seek(sectionHeadersOffset + sectionHeaderSize);
												for (int sectionIndex = 1; sectionIndex < sectionCount; sectionIndex++) {
													/* Read section header */
													final byte[] sectionHeader = readBytes(file, sectionHeaderSize);
													if (sectionHeader != null) {
														final int sectionType = sectionHeader[4] | (sectionHeader[5] << 8) | (sectionHeader[6] << 16) | (sectionHeader[7] << 24);
														if (sectionType == SHT_ARM_ATTRIBUTES) {
															/* Found .ARM.attributes section. Now read it into memory. */
															final int sectionOffset = sectionHeader[16] | (sectionHeader[17] << 8) |
																(sectionHeader[18] << 16) | (sectionHeader[19] << 24);
															file.seek(sectionOffset);
															final int sectionSize = sectionHeader[20] | (sectionHeader[21] << 8) |
																(sectionHeader[22] << 16) | (sectionHeader[23] << 24);
															final byte[] section = readBytes(file, sectionSize);
															if (section != null) {
																/* Verify that it has known format version */
																final int formatVersion = section[0];
																if (formatVersion == 'A') {
																	/* Iterate build attribute sections. We look for "aeabi" attributes section. */
																	int attributesSectionOffset = 1;
																	while (attributesSectionOffset < sectionSize) {
																		final int attributesSectionLength = section[attributesSectionOffset] |
																			(section[attributesSectionOffset+1] << 8) |
																			(section[attributesSectionOffset+2] << 16) |
																			(section[attributesSectionOffset+3] << 24);
																		if (attributesSectionLength > 10) {
																			/* Check if attributes section name if "aeabi" */
																			if ((section[attributesSectionOffset+4] == 'a') &&
																				(section[attributesSectionOffset+5] == 'e') &&
																				(section[attributesSectionOffset+6] == 'a') &&
																				(section[attributesSectionOffset+7] == 'b') &&
																				(section[attributesSectionOffset+8] == 'i') &&
																				(section[attributesSectionOffset+9] == 0))
																			{

																				/* Iterate build attribute subsections. */
																				int attributesSubsectionOffset = attributesSectionOffset + 10;
																				while (attributesSubsectionOffset < attributesSectionOffset + attributesSectionLength) {
																					final int attributesSubsectionLength = section[attributesSubsectionOffset+1] |
																						(section[attributesSubsectionOffset+2] << 8) |
																						(section[attributesSubsectionOffset+3] << 16) |
																						(section[attributesSubsectionOffset+4] << 24);
																					/* We look for subsection of attributes for the whole file. */
																					final int attributesSubsectionTag = section[attributesSubsectionOffset];
																					if (attributesSubsectionTag == Tag_File) {
																						/* Now read tag: value pairs */
																						int tagOffset = attributesSubsectionOffset + 5;
																						while (tagOffset < attributesSubsectionOffset + attributesSubsectionLength) {
																							/* Read ULEB128-encoded integer */
																							int tagByte = section[tagOffset++];
																							int tag = (tagByte & 0x7F);
																							while (tagByte < 0) {
																								tagByte = section[tagOffset++];
																								tag = (tag << 7) | (tagByte & 0x7F);
																							}
																							if (isTabNTBS(tag)) {
																								/* Null-terminated string. Skip. */
																								while (section[tagOffset++] != 0);
																							} else {
																								/* ULEB128-encoded integer. Parse. */
																								int valueByte = section[tagOffset++];
																								int value = (valueByte & 0x7F);
																								while (tagByte < 0) {
																									valueByte = section[tagOffset++];
																									value = (value << 7) | (valueByte & 0x7F);
																								}
																								if (tag == Tag_ABI_VFP_args) {
																									switch (value) {
																										case 0:
																											/* The user intended FP parameter/result passing to conform to AAPCS, base variant. */
																											return "/linux/armel/libyeppp.so";
																										case 1:
																											/* The user intended FP parameter/result passing to conform to AAPCS, VFP variant. */
																											return "/linux/armhf/libyeppp.so";
																										case 2:
																											/* The user intended FP parameter/result passing to conform to tool chain-specific conventions. */
																											return null;
																										case 3:
																											/* Code is compatible with both the base and VFP variants; the user did not permit non-variadic functions to pass FP parameters/result. */
																											return "/linux/armel/libyeppp.so";
																										default:
																											return null;
																									}
																								}
																							}
																						}
																					}
																					attributesSubsectionOffset += attributesSubsectionLength;
																				}
																			}
																		}
																		attributesSectionOffset += attributesSectionLength;
																	}
																}
															}
														}
													}
												}
											}
											/* If no Tag_ABI_VFP_args is present, assume default value (soft-float). */
											return "/linux/armel/libyeppp.so";
										}
									}
								}
							} else if (elfClass == ELFCLASS64) {
								/* ELF-64 file format */
								final byte[] header = readBytes(file, 48);
								final int machine = header[2] | (header[3] << 8);
								if (machine == EM_X86_64) {
									return "/linux/x86_64/libyeppp.so";
								}
							}
						}
					}
				} catch (Exception e) {
				}
			} else if (osName.equals("Darwin") || osName.equals("Mac OS X")) {
				if (osArch.equals("i386")) {
					return "/macosx/x86/libyeppp.dylib";
				} else  if (osArch.equals("x86_64")) {
					return "/macosx/x86_64/libyeppp.dylib";
				}
			}
		}
		return null;
	}

	private static void loadOnce() {
		final String javaVendorUrl = System.getProperty("java.vendor.url");
		if (javaVendorUrl != null) {
			if (!javaVendorUrl.contains("android")) {
				final String libraryResourceName = Library.getLibraryResource();
				if (libraryResourceName != null) {
					final String libraryName = libraryResourceName.substring(libraryResourceName.lastIndexOf('/') + 1);
					final int libraryNameExtensionPosition = libraryName.lastIndexOf('.');
					if (libraryNameExtensionPosition != -1) {
						final String libraryPrefix = libraryName.substring(0, libraryNameExtensionPosition);
						final String librarySuffix = libraryName.substring(libraryNameExtensionPosition);
						final java.io.InputStream libraryResourceStream = Library.class.getResourceAsStream(libraryResourceName);
						if (libraryResourceStream != null) {
							java.io.File libraryFile = null;
							java.io.FileOutputStream libraryFileStream = null;
							try {
								libraryFile = java.io.File.createTempFile(libraryPrefix, librarySuffix);
								libraryFileStream = new java.io.FileOutputStream(libraryFile);
								byte[] buffer = new byte[131072];
								int bytesRead;
								while ((bytesRead = libraryResourceStream.read(buffer)) != -1) {
									libraryFileStream.write(buffer, 0, bytesRead);
								}
								libraryFileStream.close();
								libraryFile.deleteOnExit();
								System.load(libraryFile.getAbsolutePath());
								return;
							} catch (Exception e) {
								if (libraryFile != null) {
									libraryFile.delete();
								}
							}
						}
					}
				}
			}
		}
		System.loadLibrary("yeppp");
	}

	static void load() {
	}

	private static long isaFeatures = Library.getCpuIsaFeatures();
	private static long simdFeatures = Library.getCpuSimdFeatures();
	private static long systemFeatures = Library.getCpuSystemFeatures();
	private static CpuVendor vendor = new CpuVendor(Library.getCpuVendorId());
	private static int architectureId = Library.getCpuArchitectureId();
	private static CpuArchitecture architecture = new CpuArchitecture(architectureId);
	private static CpuMicroarchitecture microarchitecture = new CpuMicroarchitecture(Library.getCpuMicroarchitectureId());

	private static native String getVersionInfo(int[] versionNumbers);
	private static Version version;

}
