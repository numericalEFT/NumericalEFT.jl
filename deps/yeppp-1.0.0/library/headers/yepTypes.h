/*
 *                          Yeppp! library header
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 *
 * Copyright (C) 2010-2012 Marat Dukhan
 * Copyright (C) 2012-2013 Georgia Institute of Technology
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Georgia Institute of Technology nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

/** @defgroup yepTypes	yepTypes.h: common Yeppp! data types. */

#include <yepPredefines.h>
#include <stddef.h>

#ifdef YEP_MICROSOFT_COMPILER
	/* stdint.h is not supported before Visual Studio 2010 */
	typedef unsigned __int8  Yep8u;
	typedef unsigned __int16 Yep16u;
	typedef unsigned __int32 Yep32u;
	typedef unsigned __int64 Yep64u;

	typedef signed __int8    Yep8s;
	typedef signed __int16   Yep16s;
	typedef signed __int32   Yep32s;
	typedef signed __int64   Yep64s;

#else
	#include <stdint.h>

	/** @name	Integral types */
	/**@{*/
	/**
	 * @ingroup	yepTypes
	 * @brief	8-bit unsigned integer type.
	 */
	typedef uint8_t                        Yep8u;
	/**
	 * @ingroup	yepTypes
	 * @brief	16-bit unsigned integer type.
	 */
	typedef uint16_t                       Yep16u;
	/**
	 * @ingroup	yepTypes
	 * @brief	32-bit unsigned integer type.
	 */
	typedef uint32_t                       Yep32u;
	/**
	 * @ingroup	yepTypes
	 * @brief	64-bit unsigned integer type.
	 */
	typedef uint64_t                       Yep64u;
	/**
	 * @ingroup	yepTypes
	 * @brief	8-bit signed integer type.
	 */
	typedef int8_t                         Yep8s;
	/**
	 * @ingroup	yepTypes
	 * @brief	16-bit signed integer type.
	 */
	typedef int16_t                        Yep16s;
	/**
	 * @ingroup	yepTypes
	 * @brief	32-bit signed integer type.
	 */
	typedef int32_t                        Yep32s;
	/**
	 * @ingroup	yepTypes
	 * @brief	64-bit signed integer type.
	 */
	typedef int64_t                        Yep64s;
#endif
/**
 * @ingroup	yepTypes
 * @brief	Unsigned integer type of pointer width.
 * @details	YepSize is 64-bit wide on systems with 64-bit pointers and 32-bit wide on systems with 32-bit pointers.
 */
typedef size_t                         YepSize;

#ifdef __DOXYGEN__
	/**
	 * @ingroup	yepTypes
	 * @brief	Half-precision (16-bit) IEEE floating point type.
	 */
	typedef compiler_specific<half>        Yep16f;
#else
	typedef Yep16u Yep16f;
#endif
/**
 * @ingroup	yepTypes
 * @brief	Single-precision (32-bit) IEEE floating point type.
 */
typedef float                          Yep32f;
/**
 * @ingroup	yepTypes
 * @brief	Double-precision (64-bit) IEEE floating point type.
 */
typedef double                         Yep64f;
#ifdef __DOXYGEN__
	/**
	 * @ingroup	yepTypes
	 * @brief	Extended-precision (80-bit) IEEE floating point type.
	 */
	typedef compiler_specific<long double> Yep80f;
#else
	#if defined(YEP_X86_CPU)
		#if defined(YEP_GCC_COMPATIBLE_COMPILER) || (defined(YEP_INTEL_COMPILER_FOR_WINDOWS) && (__LONG_DOUBLE_SIZE__ == 80))
			#define YEP_COMPILER_SUPPORTS_YEP80F_TYPE
			typedef long double Yep80f;
		#endif
	#endif
#endif

#ifdef __DOXYGEN__
	/**
	 * @ingroup	yepTypes
	 * @brief	Boolean type.
	 * @details	The only valid values for YepBoolean type are YepBooleanTrue and YepBooleanFalse.
	 */
	typedef compiler_specific<bool>        YepBoolean;
	/**
	 * @ingroup	yepTypes
	 * @brief	Boolean true value.
	 */
	#define YepBooleanTrue                 true
	/**
	 * @ingroup	yepTypes
	 * @brief	Boolean false value.
	 */
	#define YepBooleanFalse                false
	/**@}*/
#else
	#ifndef __cplusplus
		#if defined(YEP_MICROSOFT_COMPILER)
			typedef unsigned __int8    YepBoolean;
			#define YepBooleanTrue     1
			#define YepBooleanFalse    0
		#else
			#include <stdbool.h>
			typedef bool               YepBoolean;
			#define YepBooleanTrue     true
			#define YepBooleanFalse    false
		#endif
	#else
		typedef bool                       YepBoolean;
		const YepBoolean YepBooleanTrue  = true;
		const YepBoolean YepBooleanFalse = false;
	#endif
#endif

#pragma pack(push, 1)

/**
 * @ingroup	yepTypes
 * @brief	Complex half-precision (16-bit) IEEE floating point type.
 */
typedef struct {
	/** @brief	Real part of the complex number. */
	Yep16f re;
	/** @brief	Imaginary part of the complex number. */
	Yep16f im;
} Yep16fc;

/**
 * @ingroup	yepTypes
 * @brief	Complex single-precision (32-bit) IEEE floating point type.
 */
typedef struct {
	/** @brief	Real part of the complex number. */
	Yep32f re;
	/** @brief	Imaginary part of the complex number. */
	Yep32f im;
} Yep32fc;

/**
 * @ingroup	yepTypes
 * @brief	Complex double-precision (64-bit) IEEE floating point type.
 */
typedef struct {
	/** @brief	Real part of the complex number. */
	Yep64f re;
	/** @brief	Imaginary part of the complex number. */
	Yep64f im;
} Yep64fc;

/**
 * @ingroup	yepTypes
 * @brief	Dual single-precision (32-bit) IEEE floating point type.
 * @details	A number of this type is represented as an unevaluated sum of two Yep32f numbers.
 */
typedef struct {
	Yep32f high;
	Yep32f low;
} Yep32df;

/**
 * @ingroup	yepTypes
 * @brief	Dual double-precision (64-bit) IEEE floating point type.
 * @details	A number of this type is represented as an unevaluated sum of two Yep64f numbers.
 */
typedef struct {
	Yep64f high;
	Yep64f low;
} Yep64df;

#if defined(YEP_LITTLE_ENDIAN_BYTE_ORDER) || defined(__DOXYGEN__)
	/**
	 * @ingroup	yepTypes
	 * @brief	128-bit unsigned integer type.
	 */
	typedef struct {
		Yep64u low;
		Yep64u high;
	} Yep128u;

	/**
	 * @ingroup	yepTypes
	 * @brief	128-bit signed integer type.
	 */
	typedef struct {
		Yep64u low;
		Yep64s high;
	} Yep128s;
#elif defined(YEP_BIG_ENDIAN_BYTE_ORDER)
	typedef struct {
		Yep64u high;
		Yep64u low;
	} Yep128u;

	typedef struct {
		Yep64s high;
		Yep64u low;
	} Yep128s;
#else
	#error "Unknown or unsupported byte order"
#endif

#pragma pack(pop)

/**
 * @ingroup	yepTypes
 * @brief	Indicates success or failure of @Yeppp functions.
 */
enum YepStatus {
	/** @brief Operation finished successfully. */
	YepStatusOk = 0,
	/** @brief Function call failed because one of the pointer arguments is null. */
	YepStatusNullPointer = 1,
	/** @brief Function call failed because one of the pointer arguments is not properly aligned. */
	YepStatusMisalignedPointer = 2,
	/** @brief Function call failed because one of the integer arguments has unsupported value. */
	YepStatusInvalidArgument = 3,
	/** @brief Function call failed because some of the data passed to the function has invalid format or values. */
	YepStatusInvalidData = 4,
	/** @brief Function call failed because one of the state objects passed is corrupted. */
	YepStatusInvalidState = 5,
	/** @brief Function call failed because the system hardware does not support the requested operation. */
	YepStatusUnsupportedHardware = 6,
	/** @brief Function call failed because the operating system does not support the requested operation. */
	YepStatusUnsupportedSoftware = 7,
	/** @brief Function call failed because the provided output buffer is too small or exhausted. */
	YepStatusInsufficientBuffer = 8,
	/** @brief Function call failed because the library could not allocate the memory. */
	YepStatusOutOfMemory = 9,
	/** @brief Function call failed because some of the system calls inside the function failed. */
	YepStatusSystemError = 10,
	/** @brief Function call failed because access to the requested resource is not allowed for this user. */
	YepStatusAccessDenied = 11
};
