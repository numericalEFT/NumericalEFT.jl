/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

module yeppp.types;

/** @ingroup	yepLibrary */
/** @brief	Indicates success or failure of @Yeppp functions. */
enum Status : uint {
	/** @brief Operation finished successfully. */
	Ok = 0,
	/** @brief Function call failed because one of the pointer arguments is null. */
	NullPointer = 1,
	/** @brief Function call failed because one of the pointer arguments is not properly aligned. */
	MisalignedPointer = 2,
	/** @brief Function call failed because one of the integer arguments has unsupported value. */
	InvalidArgument = 3,
	/** @brief Function call failed because some of the data passed to the function has invalid format or values. */
	InvalidData = 4,
	/** @brief Function call failed because one of the state objects passed is corrupted. */
	InvalidState = 5,
	/** @brief Function call failed because the system hardware does not support the requested operation. */
	UnsupportedHardware = 6,
	/** @brief Function call failed because the operating system does not support the requested operation. */
	UnsupportedSoftware = 7,
	/** @brief Function call failed because the provided output buffer is too small or exhausted. */
	InsufficientBuffer = 8,
	/** @brief Function call failed becuase the library could not allocate the memory. */
	OutOfMemory = 9,
	/** @brief Function call failed because some of the system calls inside the function failed. */
	SystemError = 10
};
