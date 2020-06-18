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

#ifndef __cplusplus
	#error "This intrinsics should only be used in C++ code"
#endif

#include <yepPredefines.h>
#include <yepTypes.h>
#include <math.h>

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#include <intrin.h>
#endif
#if defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(YEP_X86_CPU)
	#include <x86intrin.h>
#endif

YEP_NATIVE_FUNCTION static YEP_INLINE void yepBuiltin_Break() {
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	__debugbreak();
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	__builtin_trap();
#else
	#error "Unsupported compiler"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE void yepBuiltin_AssumeUnreachable() {
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	__assume(0);
#elif defined(YEP_GNU_COMPILER)
	/* Supported since in gcc 4.5 */
	#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5))
		__builtin_unreachable();
	#else
		yepBuiltin_Break();
	#endif
#elif defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_UNIX)
	__builtin_unreachable();
#elif defined(YEP_NVIDIA_COMPILER)
	yepBuiltin_Break();
#else
	#error "Unsupported compiler"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE YepSize yepBuiltin_GetPointerMisalignment(const void* pointer, YepSize alignment) {
	return YepSize(pointer) % alignment;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_CombineParts_32u32u_64u(Yep32u hi, Yep32u lo) {
	return (Yep64u(hi) << 32) | Yep64u(lo);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_GetHighPart_64u_32u(Yep64u n) {
	return Yep32u(n >> 32);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_GetLowPart_64u_32u(Yep64u n) {
	return Yep32u(n);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_Cast_64f_64u(Yep64f x) {
#if defined(YEP_NVIDIA_COMPILER)
	return __double_as_longlong(x);
#elif defined(YEP_INTEL_COMPILER)
	return _castf64_u64(x);
#else
	union {
		Yep64f float64;
		Yep64u word64;
	} float64_word64;
	float64_word64.float64 = x;
	return float64_word64.word64;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Cast_64u_64f(Yep64u x) {
#if defined(YEP_NVIDIA_COMPILER)
	return __longlong_as_double(x);
#elif defined(YEP_INTEL_COMPILER)
	return _castu64_f64(x);
#else
	union {
		Yep64f float64;
		Yep64u word64;
	} float64_word64;
	float64_word64.word64 = x;
	return float64_word64.float64;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Cast_32f_32u(Yep32f x) {
#if defined(YEP_NVIDIA_COMPILER)
	return __float_as_int(x);
#elif defined(YEP_INTEL_COMPILER)
	return _castf32_u32(x);
#else
	union {
		Yep32f float32;
		Yep32u word32;
	} float32_word32;
	float32_word32.float32 = x;
	return float32_word32.word32;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Cast_32u_32f(Yep32u x) {
#if defined(YEP_NVIDIA_COMPILER)
	return __int_as_float(x);
#elif defined(YEP_INTEL_COMPILER)
	return _castu32_f32(x);
#else
	union {
		Yep32f float32;
		Yep32u word32;
	} float32_word32;
	float32_word32.word32 = x;
	return float32_word32.float32;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_Map_64f_64u(Yep64f x) {
	const Yep64u signMask = 0x8000000000000000ull;

	const Yep64u n = yepBuiltin_Cast_64f_64u(x);
	const Yep64u mask = Yep64u(Yep64s(n) >> 62) >> 1;
	return n ^ mask ^ signMask;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Map_64u_64f(Yep64u n) {
	const Yep64u signMask = 0x8000000000000000ull;
	
	const Yep64u m = n ^ signMask;
	const Yep64u mask = Yep64u(Yep64s(m) >> 62) >> 1;
	return yepBuiltin_Cast_64u_64f(m ^ mask);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Map_32f_32u(Yep32f x) {
	const Yep32u signMask = 0x80000000u;

	const Yep32u n = yepBuiltin_Cast_32f_32u(x);
	const Yep32u mask = Yep32u(Yep32s(n) >> 30) >> 1;
	return n ^ mask ^ signMask;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Map_32u_32f(Yep32u n) {
	const Yep32u signMask = 0x80000000u;
	
	const Yep32u m = n ^ signMask;
	const Yep32u mask = Yep32u(Yep32s(m) >> 30) >> 1;
	return yepBuiltin_Cast_32u_32f(m ^ mask);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep16u yepBuiltin_ByteSwap_16u_16u(Yep16u n) {
#if defined(YEP_GNU_COMPILER) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) || ((__GNUC__ == 4) && (__GNUC_MINOR__ == 7) && (__GNUC_PATCHLEVEL__ >= 3)))
	return __builtin_bswap16(n);
#elif defined(YEP_CLANG_COMPILER) && ((__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 2)))
	return __builtin_bswap16(n);
#elif defined(YEP_INTEL_COMPILER)
	return _rotwl(n, 8);
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	return Yep16u(__builtin_bswap32(n << 16));
#elif defined(YEP_MSVC_COMPATIBLE_COMPILER)
	return _byteswap_ushort(n);
#else
	return Yep16u(n >> 8) | Yep16u(n << 8);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_ByteSwap_32u_32u(Yep32u n) {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_LINUX)
	return __builtin_bswap32(n);
#elif defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS)
	return _byteswap_ulong(n);
#elif defined(YEP_NVIDIA_COMPILER)
	return __byte_perm(n, n, 0x3210);
#else
	return (n >> 24) | ((n >> 8) & 0x0000FF00u) | ((n << 8) & 0x00FF0000u) | (n << 24);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_ByteSwap_64u_64u(Yep64u n) {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_LINUX)
	return __builtin_bswap64(n);
#elif defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS)
	return _byteswap_uint64(n);
#else
	const Yep32u nLo = yepBuiltin_GetLowPart_64u_32u(n);
	const Yep32u nHi = yepBuiltin_GetHighPart_64u_32u(n);
	const Yep32u nLoSwapped = yepBuiltin_ByteSwap_32u_32u(nLo);
	const Yep32u nHiSwapped = yepBuiltin_ByteSwap_32u_32u(nHi);
	return yepBuiltin_CombineParts_32u32u_64u(nLoSwapped, nHiSwapped);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Abs_32f_32f(Yep32f x) {
#if defined(YEP_MICROSOFT_COMPILER)
	return abs(x);
#elif defined(YEP_INTEL_COMPILER)
	return fabsf(x);
#elif defined(YEP_ARM_COMPILER)
	return __fabsf(x);
#elif defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER)
	return __builtin_fabsf(x);
#elif defined(YEP_NVIDIA_COMPILER)
	return fabsf(x);
#else
	return x >= 0.0f ? x : -x;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Abs_64f_64f(Yep64f x) {
#if defined(YEP_MICROSOFT_COMPILER)
	return abs(x);
#elif defined(YEP_INTEL_COMPILER)
	return fabs(x);
#elif defined(YEP_ARM_COMPILER)
	return __fabs(x);
#elif defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER)
	return __builtin_fabs(x);
#elif defined(YEP_NVIDIA_COMPILER)
	return fabs(x);
#else
	return x >= 0.0 ? x : -x;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_8s_32f(Yep8s number) {
	return Yep32f(Yep32s(number));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_16s_32f(Yep16s number) {
	return Yep32f(Yep32s(number));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_32s_32f(Yep32s number) {
	return Yep32f(number);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_64s_32f(Yep64s number) {
	return Yep32f(number);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_8s_64f(Yep8s number) {
	return Yep64f(Yep32s(number));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_16s_64f(Yep16s number) {
	return Yep64f(Yep32s(number));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_32s_64f(Yep32s number) {
	return Yep64f(number);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_64s_64f(Yep64s number) {
	return Yep64f(number);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_8u_32f(Yep8u number) {
	return Yep32f(Yep32s(Yep32u(number)));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_16u_32f(Yep16u number) {
	return Yep32f(Yep32s(Yep32u(number)));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_32u_32f(Yep32u number) {
#if defined(YEP_ARM_CPU)
	return Yep32f(number);
#elif defined(YEP_MIPS_CPU)
	return Yep32f(Yep32s(number & 0x7FFFFFFFu)) - Yep32f(Yep32s(number & 0x80000000u));
#else
	return Yep32f(Yep64s(Yep64u(number)));
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Convert_64u_32f(Yep64u number) {
#if defined(YEP_ARM_CPU)
	return Yep32f(Yep64s(number & 0x7FFFFFFFFFFFFFFFull)) - Yep32f(Yep64s(number & 0x8000000000000000ull));
#else
	return Yep32f(number);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_8u_64f(Yep8u number) {
	return Yep64f(Yep32s(Yep32u(number)));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_16u_64f(Yep16u number) {
	return Yep64f(Yep32s(Yep32u(number)));
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_32u_64f(Yep32u number) {
#if defined(YEP_ARM_CPU)
	return Yep64f(number);
#else
	return Yep64f(Yep64s(Yep64u(number)));
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Convert_64u_64f(Yep64u number) {
#if defined(YEP_ARM_CPU)
	return Yep64f(number);
#else
	return Yep64f(Yep64s(number & 0x7FFFFFFFFFFFFFFFull)) - Yep64f(Yep64s(number & 0x8000000000000000ull));
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep8u yepBuiltin_Min_8u8u_8u(Yep8u a, Yep8u b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep8s yepBuiltin_Min_8s8s_8s(Yep8s a, Yep8s b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep16u yepBuiltin_Min_16u16u_16u(Yep16u a, Yep16u b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep16s yepBuiltin_Min_16s16s_16s(Yep16s a, Yep16s b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Min_32u32u_32u(Yep32u a, Yep32u b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32s yepBuiltin_Min_32s32s_32s(Yep32s a, Yep32s b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_Min_64u64u_64u(Yep64u a, Yep64u b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64s yepBuiltin_Min_64s64s_64s(Yep64s a, Yep64s b) {
	return (a > b) ? b : a;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Min_32f32f_32f(Yep32f a, Yep32f b) {
#if defined(YEP_NVIDIA_COMPILER)
	return fminf(a, b);
#elif defined(YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION) && defined(YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION)
	return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)));
#elif defined(YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS)
	if YEP_LIKELY(b == b) {
		return (a < b) ? a : b;
	} else {
		return a;
	}
#else
	Yep32u au = yepBuiltin_Cast_32f_32u(a);
	Yep32u bu = yepBuiltin_Cast_32f_32u(b);

	/* Check if b is NaN */
	const Yep32u twoBu = bu + bu;
	if YEP_UNLIKELY(twoBu > 0xFF000000u) {
		/* b is NaN, return a */
		bu = au;
	}

	/* Check if a is NaN */
	const Yep32u twoAu = au + au;
	if YEP_UNLIKELY(twoAu > 0xFF000000u) {
		/* a is NaN, return b */
		au = bu;
	}

	const Yep32s as = Yep32s(au) >= 0 ? au : 0x80000000 - au;
	const Yep32s bs = Yep32s(bu) >= 0 ? bu : 0x80000000 - bu;
	return as < bs ? yepBuiltin_Cast_32u_32f(au) : yepBuiltin_Cast_32u_32f(bu);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Min_64f64f_64f(Yep64f a, Yep64f b) {
#if defined(YEP_NVIDIA_COMPILER)
	return fmin(a, b);
#elif defined(YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION) && defined(YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION)
	return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(a), _mm_set_sd(b)));
#elif defined(YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS)
	if YEP_LIKELY(b == b) {
		return (a < b) ? a : b;
	} else {
		return a;
	}
#else
	Yep64u au = yepBuiltin_Cast_64f_64u(a);
	Yep64u bu = yepBuiltin_Cast_64f_64u(b);

	/* Check if b is NaN */
	const Yep64u negBu = bu | 0x8000000000000000ull;
	if YEP_UNLIKELY(negBu > 0xFFF0000000000000ull) {
		/* b is NaN, return a */
		bu = au;
	}

	/* Check if a is NaN */
	const Yep64u negAu = au | 0x8000000000000000ull;
	if YEP_UNLIKELY(negAu > 0xFFF0000000000000ull) {
		/* a is NaN, return b */
		au = bu;
	}

	const Yep64s as = Yep64s(au) >= 0ll ? au : 0x8000000000000000ll - au;
	const Yep64s bs = Yep64s(bu) >= 0ll ? bu : 0x8000000000000000ll - bu;
	return as < bs ? yepBuiltin_Cast_64u_64f(au) : yepBuiltin_Cast_64u_64f(bu);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep8u yepBuiltin_Max_8u8u_8u(Yep8u a, Yep8u b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep8s yepBuiltin_Max_8s8s_8s(Yep8s a, Yep8s b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep16u yepBuiltin_Max_16u16u_16u(Yep16u a, Yep16u b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep16s yepBuiltin_Max_16s16s_16s(Yep16s a, Yep16s b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Max_32u32u_32u(Yep32u a, Yep32u b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32s yepBuiltin_Max_32s32s_32s(Yep32s a, Yep32s b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_Max_64u64u_64u(Yep64u a, Yep64u b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64s yepBuiltin_Max_64s64s_64s(Yep64s a, Yep64s b) {
	return (a > b) ? a : b;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Max_32f32f_32f(Yep32f a, Yep32f b) {
#if defined(YEP_NVIDIA_COMPILER)
	return fmaxf(a, b);
#elif defined(YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION) && defined(YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION)
	return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)));
#elif defined(YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS)
	if YEP_LIKELY(b == b) {
		return (a > b) ? a : b;
	} else {
		return a;
	}
#else
	Yep32u au = yepBuiltin_Cast_32f_32u(a);
	Yep32u bu = yepBuiltin_Cast_32f_32u(b);

	/* Check if b is NaN */
	const Yep32u twoBu = bu + bu;
	if YEP_UNLIKELY(twoBu > 0xFF000000u) {
		/* b is NaN, return a */
		bu = au;
	}

	/* Check if a is NaN */
	const Yep32u twoAu = au + au;
	if YEP_UNLIKELY(twoAu > 0xFF000000u) {
		/* a is NaN, return b */
		au = bu;
	}

	const Yep32s as = Yep32s(au) >= 0 ? au : 0x80000000 - au;
	const Yep32s bs = Yep32s(bu) >= 0 ? bu : 0x80000000 - bu;
	return as > bs ? yepBuiltin_Cast_32u_32f(au) : yepBuiltin_Cast_32u_32f(bu);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Max_64f64f_64f(Yep64f a, Yep64f b) {
#if defined(YEP_NVIDIA_COMPILER)
	return fmax(a, b);
#elif defined(YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION) && defined(YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION)
	return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(a), _mm_set_sd(b)));
#elif defined(YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS)
	if YEP_LIKELY(b == b) {
		return (a > b) ? a : b;
	} else {
		return a;
	}
#else
	Yep64u au = yepBuiltin_Cast_64f_64u(a);
	Yep64u bu = yepBuiltin_Cast_64f_64u(b);

	/* Check if b is NaN */
	const Yep64u negBu = bu | 0x8000000000000000ull;
	if YEP_UNLIKELY(negBu > 0xFFF0000000000000ull) {
		/* b is NaN, return a */
		bu = au;
	}

	/* Check if a is NaN */
	const Yep64u negAu = au | 0x8000000000000000ull;
	if YEP_UNLIKELY(negAu > 0xFFF0000000000000ull) {
		/* a is NaN, return b */
		au = bu;
	}

	const Yep64s as = Yep64s(au) >= 0ll ? au : 0x8000000000000000ll - au;
	const Yep64s bs = Yep64s(bu) >= 0ll ? bu : 0x8000000000000000ll - bu;
	return as > bs ? yepBuiltin_Cast_64u_64f(au) : yepBuiltin_Cast_64u_64f(bu);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Clamp_32u32u32u_32u(Yep32u x, Yep32u xMin, Yep32u xMax) {
	return (x < xMin) ? xMin : (x > xMax) ? xMax : x;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32s yepBuiltin_Clamp_32s32s32s_32s(Yep32s x, Yep32s xMin, Yep32s xMax) {
	return (x < xMin) ? xMin : (x > xMax) ? xMax : x;
}

YEP_NATIVE_FUNCTION static YEP_INLINE YepBoolean yepBuiltin_IsNaN_64f(Yep64f n) {
	return !(n == n);
}

YEP_NATIVE_FUNCTION static YEP_INLINE YepBoolean yepBuiltin_IsNaN_32f(Yep32f n) {
	return !(n == n);
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_PositiveInfinity_32f() {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	return __builtin_inff();
#else
	const static Yep64f one = 1.0f;
	const static Yep64f zero = 0.0f;
	return one / zero;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_PositiveInfinity_64f() {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	return __builtin_inf();
#else
	const static Yep64f one = 1.0;
	const static Yep64f zero = 0.0;
	return one / zero;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_NegativeInfinity_32f() {
	return -yepBuiltin_PositiveInfinity_32f();
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_NegativeInfinity_64f() {
	return -yepBuiltin_PositiveInfinity_64f();
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_NaN_32f() {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	return __builtin_nanf("");
#else
	const static Yep32f zero = 0.0f;
	return zero / zero;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_NaN_64f() {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	return __builtin_nan("");
#else
	const static Yep64f zero = 0.0;
	return zero / zero;
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Nlz_64u_32u(Yep64u x) {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_UNIX)
	return __builtin_clzl(x);
#elif (defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS)) && (defined(YEP_IA64_ABI) || defined(YEP_X64_ABI))
	if (x == 0ull) {
		return 64u;
	} else {
		unsigned long bitPosition;
		_BitScanReverse64(&bitPosition, x);
		return 63u - bitPosition;
	}
#elif (defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS)) && defined(YEP_X86_CPU)
	const Yep32u xHi = yepBuiltin_GetHighPart_64u_32u(x);
	const Yep32u xLo = yepBuiltin_GetLowPart_64u_32u(x);
	unsigned long bitPositionHi, bitPositionLo;
	_BitScanReverse(&bitPositionLo, xLo);
	_BitScanReverse(&bitPositionHi, xHi);
	if YEP_UNLIKELY(xHi == 0u) {
		if YEP_UNLIKELY(xLo == 0u) {
			return 64u;
		} else {
			return 63u - bitPositionLo;
		}
	} else {
		return 31u - bitPositionHi;
	}
#elif defined(YEP_NVIDIA_COMPILER)
	return __clzll(x);
#else
	#error "Compiler-specific implementation needed"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_Nlz_32u_32u(Yep64u x) {
#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_UNIX)
	return __builtin_clz(x);
#elif (defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS))
	if (x == 0ull) {
		return 64u;
	} else {
		unsigned long bitPosition;
		_BitScanReverse(&bitPosition, x);
		return 63u - bitPosition;
	}
#elif defined(YEP_NVIDIA_COMPILER)
	return __clz(x);
#else
	#error "Compiler-specific implementation needed"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Ulp_64f_64f(Yep64f x) {
	const Yep64f absX = yepBuiltin_Abs_64f_64f(x);
	if YEP_LIKELY(absX < yepBuiltin_PositiveInfinity_64f()) {
		return yepBuiltin_Cast_64u_64f(yepBuiltin_Cast_64f_64u(absX) + 1ull) - absX;
	} else {
		return x;
	}
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Ulp_32f_32f(Yep32f x) {
	const Yep32f absX = yepBuiltin_Abs_32f_32f(x);
	if YEP_LIKELY(absX < yepBuiltin_PositiveInfinity_32f()) {
		return yepBuiltin_Cast_32u_32f(yepBuiltin_Cast_32f_32u(absX) + 1u) - absX;
	} else {
		return x;
	}
}

#if defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FMA_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __builtin_fma(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FMS_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __builtin_fma(a, b, -c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FNMA_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __builtin_fma(-a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FNMS_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __builtin_fma(-a, b, -c);
	}
#elif defined(YEP_NVIDIA_COMPILER) && defined(YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FMA_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __fma_rn(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FMS_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __fma_rn(a, b, -c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FNMA_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __fma_rn(-a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_FNMS_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return __fma_rn(-a, b, -c);
	}
#endif

#if defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FMA_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __builtin_fmaf(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FMS_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __builtin_fmaf(a, b, -c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FNMA_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __builtin_fmaf(-a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FNMS_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __builtin_fmaf(-a, b, -c);
	}
#elif defined(YEP_NVIDIA_COMPILER) && defined(YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FMA_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __fmaf_rn(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FMS_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __fmaf_rn(a, b, -c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FNMA_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __fmaf_rn(-a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_FNMS_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return __fmaf_rn(-a, b, -c);
	}
#endif

#if defined(YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Divide_64f64f64f_64f(Yep64f y, Yep64f c, Yep64f rcpC) {
		const Yep64f q = y * rcpC;
		const Yep64f r = yepBuiltin_FNMA_64f64f64f_64f(c, q, y);
		return yepBuiltin_FMA_64f64f64f_64f(r, rcpC, q);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_MultiplyAdd_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return yepBuiltin_FMA_64f64f64f_64f(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_MultiplySubtract_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return yepBuiltin_FMS_64f64f64f_64f(a, b, c);
	}
#else
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Divide_64f64f64f_64f(Yep64f y, Yep64f c, Yep64f rcpC) {
		return y / c;
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_MultiplyAdd_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return a * b + c;
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_MultiplySubtract_64f64f64f_64f(Yep64f a, Yep64f b, Yep64f c) {
		return a * b - c;
	}
#endif

#if defined(YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS)
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Divide_32f32f32f_32f(Yep32f y, Yep32f c, Yep32f rcpC) {
		const Yep32f q = y * rcpC;
		const Yep32f r = yepBuiltin_FNMA_32f32f32f_32f(c, q, y);
		return yepBuiltin_FMA_32f32f32f_32f(r, rcpC, q);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_MultiplyAdd_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return yepBuiltin_FMA_32f32f32f_32f(a, b, c);
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_MultiplySubtract_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return yepBuiltin_FMS_32f32f32f_32f(a, b, c);
	}
#else
	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Divide_32f32f32f_32f(Yep32f y, Yep32f c, Yep32f rcpC) {
		return y / c;
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_MultiplyAdd_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return a * b + c;
	}

	YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_MultiplySubtract_32f32f32f_32f(Yep32f a, Yep32f b, Yep32f c) {
		return a * b - c;
	}
#endif


/* See algorithm 4.3 in "Handbook of floating-point arithmetic" */
YEP_NATIVE_FUNCTION static YEP_INLINE Yep64df yepBuiltin_Add_64f64f_64df_AlgFast(Yep64f a, Yep64f b) {
#if defined(YEP_NVIDIA_COMPILER)
	Yep64df sum;
	sum.high = __dadd_rn(a, b);
	const Yep64f bCorrected = __dadd_rn(sum.high, -a);
	const Yep64f deltaB = __dadd_rn(b, -bCorrected);
	sum.low = deltaB;
	return sum;
#else
	Yep64df sum;
	sum.high = a + b;
	const Yep64f bCorrected = sum.high - a;
	const Yep64f deltaB = b - bCorrected;
	sum.low = deltaB;
	return sum;
#endif
}

/* See algorithm 4.4 in "Handbook of floating-point arithmetic" */
YEP_NATIVE_FUNCTION static YEP_INLINE Yep64df yepBuiltin_Add_64f64f_64df(Yep64f a, Yep64f b) {
#if defined(YEP_NVIDIA_COMPILER)
	Yep64df sum;
	sum.high = __dadd_rn(a, b);
	const Yep64f aCorrected = __dadd_rn(sum.high, -b);
	const Yep64f bCorrected = __dadd_rn(sum.high, -aCorrected);
	const Yep64f deltaA = __dadd_rn(a, -aCorrected);
	const Yep64f deltaB = __dadd_rn(b, -bCorrected);
	sum.low = __dadd_rn(deltaA, deltaB);
	return sum;
#else
	Yep64df sum;
	sum.high = a + b;
	const Yep64f aCorrected = sum.high - b;
	const Yep64f bCorrected = sum.high - aCorrected;
	const Yep64f deltaA = a - aCorrected;
	const Yep64f deltaB = b - bCorrected;
	sum.low = deltaA + deltaB;
	return sum;
#endif
}


YEP_NATIVE_FUNCTION static YEP_INLINE Yep64df yepBuiltin_Multiply_64f64f_64df(Yep64f a, Yep64f b) {
	Yep64df product;
	product.high = a * b;
#if defined(YEP_PROCESSOR_SUPPORTS_FMA_EXTENSION)
	product.low = yepBuiltin_FMS_64f64f64f_64f(a, b, product.high);
#else
	Yep64df da, db;
	/* Zeroes out 27 least significant bits */
	const Yep64u mask = 0xFFFFFFFFF8000000ull;
	da.high = yepBuiltin_Cast_64u_64f(yepBuiltin_Cast_64f_64u(a) & mask);
	da.low = a - da.high;
	db.high = yepBuiltin_Cast_64u_64f(yepBuiltin_Cast_64f_64u(b) & mask);
	db.low = b - da.high;
	const Yep64f t1 = -product.high + Yep64f(da.high * db.high);
	const Yep64f t2 = t1 + Yep64f(da.high * db.low);
	const Yep64f t3 = t2 + Yep64f(da.low * db.high);
	product.low = t3 + Yep64f(da.low * db.low);
#endif
	return product;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Log_32f_32f(Yep32f x) {
	const Yep32u defaultExponent = 0x3F800000u;
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep32f sqrt2 = 0x1.6A09E6p+0f;
	const Yep32f c2  = -0x1.FFFFF2p-2f;
	const Yep32f c3  =  0x1.55571Ep-2f;
	const Yep32f c4  = -0x1.0006B2p-2f;
	const Yep32f c5  =  0x1.98CB06p-3f;
	const Yep32f c6  = -0x1.530B6Ap-3f;
	const Yep32f c7  =  0x1.317FD6p-3f;
	const Yep32f c8  = -0x1.26F724p-3f;
	const Yep32f c9  =  0x1.6A66D0p-4f;

	const Yep32f ln2_hi = 0x1.62E400p-1f; /* The lowest 7 bits are zeros */
	const Yep32f ln2_lo = 0x1.7F7D1Cp-20f;
#else
	const Yep32f sqrt2 = 1.41421353816986083984375f;
	const Yep32f c2  = -0.4999997913837432861328125f;
	const Yep32f c3  =  0.3333401381969451904296875f;
	const Yep32f c4  = -0.2500255405902862548828125f;
	const Yep32f c5  =  0.19960598647594451904296875f;
	const Yep32f c6  = -0.16554911434650421142578125f;
	const Yep32f c7  =  0.14916960895061492919921875f;
	const Yep32f c8  = -0.1440260708332061767578125f;
	const Yep32f c9  =  8.8476955890655517578125e-2f;

	const Yep32f ln2_hi = 0.693145751953125f; /* The lowest 7 bits are zeros */
	const Yep32f ln2_lo = 1.428606765330187045037746429443359375e-6f;
#endif
	if YEP_UNLIKELY(yepBuiltin_IsNaN_32f(x)) {
		return x;
	} else {
		const Yep32s xWord = yepBuiltin_Cast_32f_32u(x);
		if YEP_UNLIKELY(xWord < 0) {
			// sign(x) == -1
			return yepBuiltin_NaN_32f();
		} else if YEP_UNLIKELY(xWord == 0) {
			// x == +0.0
			return yepBuiltin_NegativeInfinity_32f();
		} else if YEP_UNLIKELY(xWord == 0x7F800000) {
			// x == +inf
			return x;
		}
		Yep32s exponent;
		Yep32u mantissa;
		if YEP_LIKELY(xWord >= 0x00800000u) {
			// Normalized number
			exponent = Yep32s(Yep32u(xWord) >> 23) - 127;
			mantissa = xWord & 0x007FFFFFu;
		} else {
			// Denormalized number
			const Yep32u pointOffset = yepBuiltin_Nlz_32u_32u(xWord) - 7u;
			exponent = -126 - Yep32s(pointOffset);
			mantissa = (xWord << pointOffset) & 0x007FFFFFu;
		}
		x = yepBuiltin_Cast_32u_32f(defaultExponent | mantissa);
		if (x >= sqrt2) {
			exponent += 1;
			x = x * 0.5f;
		}
		const Yep32f t = x - 1.0f;
		const Yep32f dexp = Yep32f(exponent);
		return (t + t * (t * (c2 + t * (c3 + t * (c4 + t * (c5 + t * (c6 + t * (c7 + t * (c8 + t * c9)))))))) +
			dexp * ln2_lo) + dexp * ln2_hi;
	}
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Log_64f_64f(Yep64f x) {
	const Yep64u defaultExponent = 0x3FF0000000000000ull;
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f sqrt2 = 0x1.6A09E667F3BCDp+0;
	const Yep64f c2  = -0x1.FFFFFFFFFFFF2p-2;
	const Yep64f c3  =  0x1.5555555555103p-2;
	const Yep64f c4  = -0x1.00000000013C7p-2;
	const Yep64f c5  =  0x1.9999999A43E4Fp-3;
	const Yep64f c6  = -0x1.55555554A6A2Bp-3;
	const Yep64f c7  =  0x1.249248DAE4B2Ap-3;
	const Yep64f c8  = -0x1.FFFFFFBD8606Dp-4;
	const Yep64f c9  =  0x1.C71C90DB06248p-4;
	const Yep64f c10 = -0x1.9999C5BE751E3p-4;
	const Yep64f c11 =  0x1.745980F3FB889p-4;
	const Yep64f c12 = -0x1.554D5ACD502ABp-4;
	const Yep64f c13 =  0x1.3B4ED39194B87p-4;
	const Yep64f c14 = -0x1.25480A82633AFp-4;
	const Yep64f c15 =  0x1.0F23916A44515p-4;
	const Yep64f c16 = -0x1.EED2E2BB64B2Ep-5;
	const Yep64f c17 =  0x1.EA17E14773369p-5;
	const Yep64f c18 = -0x1.1654764F478ECp-4;
	const Yep64f c19 =  0x1.0266CD08DB2F2p-4;
	const Yep64f c20 = -0x1.CC4EC078138E3p-6;

	#if defined(YEP_PROCESSOR_SUPPORTS_FMA_EXTENSION)
		const Yep64df ln2 = { 0x1.62E42FEFA39EFp-1, 0x1.ABC9E3B39803Fp-56 };
	#else
		const Yep64df ln2 = { 0x1.62E42FEFA3800p-1, 0x1.EF35793C76730p-45 };
	#endif
#else
	const Yep64f sqrt2 = 1.4142135623730951;
	const Yep64f c2  = -0.4999999999999992;
	const Yep64f c3  =  0.3333333333332719;
	const Yep64f c4  = -0.25000000000028105;
	const Yep64f c5  = 0.20000000001936022;
	const Yep64f c6  = -0.16666666664680582;
	const Yep64f c7  =  0.14285714071282857;
	const Yep64f c8  = -0.12499999903264021;
	const Yep64f c9  =  0.11111122688488095;
	const Yep64f c10 = -0.10000016444912023;
	const Yep64f c11 =  0.09090566990173178;
	const Yep64f c12 = -0.08332572431118972;
	const Yep64f c13 =  0.07697947162641415;
	const Yep64f c14 = -0.0716019068260738;
	const Yep64f c15 =  0.06619602968955392;
	const Yep64f c16 = -0.060403292499492486;
	const Yep64f c17 =  0.059825839994664794;
	const Yep64f c18 = -0.06795164313050223;
	const Yep64f c19 =  0.06308631984365912;
	const Yep64f c20 = -0.028094947774939604;

	#if defined(YEP_PROCESSOR_SUPPORTS_FMA_EXTENSION)
		const Yep64df ln2 = { 0.6931471805599453, 2.3190468138462996e-17 };
	#else
		const Yep64df ln2 = { 0.6931471805598903, 5.497923018708371e-14 };
	#endif
#endif
	Yep32s exponent;
	Yep64u mantissa;
	const Yep64s xWord = yepBuiltin_Cast_64f_64u(x);
	if YEP_LIKELY(xWord >= 0x0010000000000000ull) {
		/* Normalized number */
		exponent = Yep32s((yepBuiltin_GetHighPart_64u_32u(Yep64u(xWord)) >> 20) & 0x7FFu) - 1023;
		mantissa = xWord & 0x000FFFFFFFFFFFFFull;
	} else {
		/* Denormalized number */
		const Yep32u pointOffset = yepBuiltin_Nlz_64u_32u(xWord) - 11u;
		exponent = -1022 - Yep32s(pointOffset);
		mantissa = (xWord << pointOffset) & 0x000FFFFFFFFFFFFFull;
	}
	x = yepBuiltin_Cast_64u_64f(defaultExponent | mantissa);
	if (x >= sqrt2) {
		exponent += 1;
		x = x * 0.5;
	}
	const Yep64f t = x - 1.0;
	const Yep64f dexp = yepBuiltin_Convert_32s_64f(exponent);
	const Yep64f pt = yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
			yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
				yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
					yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
						yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
							yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
								yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
									yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
										yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
											yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
												yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
													yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
														yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
															yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
																yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
																	yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
																		yepBuiltin_MultiplyAdd_64f64f64f_64f(t,
																			yepBuiltin_MultiplyAdd_64f64f64f_64f(t, c20, c19),
																		c18),
																	c17),
																c16),
															c15),
														c14),
													c13),
												c12),
											c11),
										c10),
									c9),
								c8),
							c7),
						c6),
					c5),
				c4),
			c3),
		c2);
	const Yep64f rf = yepBuiltin_MultiplyAdd_64f64f64f_64f(t, t * pt, t);
	Yep64f f = yepBuiltin_MultiplyAdd_64f64f64f_64f(dexp, ln2.high, yepBuiltin_MultiplyAdd_64f64f64f_64f(dexp, ln2.low, rf));
	if YEP_UNLIKELY(xWord < 0ll) {
		/* Fixup negative inputs */
		f = yepBuiltin_NaN_64f();
	} else if YEP_UNLIKELY(xWord == 0ll) {
		/* Fixup +0.0 */
		f = yepBuiltin_NegativeInfinity_64f();
	} else if YEP_UNLIKELY(!(x < yepBuiltin_PositiveInfinity_64f())) {
		/* Fixup +inf and NaN */
		f = x;
	}
	return f;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Exp_32f_32f(Yep32f x) {
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep32f magicBias = 0x1.000000p+23f;
	const Yep32f zeroCutoff = -0x1.9FE368p+6f; /* The smallest x for which expf(x) is non-zero */
	const Yep32f infCutoff = 0x1.62E42Ep+6f; /* The largest x for which expf(x) is finite */
	const Yep32f log2e  = 0x1.715476p+0f;
	const Yep32f ln2_hi = 0x1.62E400p-1f; /* The lowest 7 bits are zeros */
	const Yep32f ln2_lo = 0x1.7F7D1Cp-20f;

	const Yep32f c2 = 0x1.FFFFFCp-2f;
	const Yep32f c3 = 0x1.55548Cp-3f;
	const Yep32f c4 = 0x1.555834p-5f;
	const Yep32f c5 = 0x1.123CFEp-7f;
	const Yep32f c6 = 0x1.6ADCAEp-10f;
#else
	const Yep32f magicBias = 8388608.0f;
	const Yep32f zeroCutoff = -1.03972076416015625e+2f; /* The smallest x for which expf(x) is non-zero */
	const Yep32f infCutoff = 8.872283172607421875e+1f; /* The largest x for which expf(x) is finite */
	const Yep32f log2e  = 1.44269502162933349609375f;
	const Yep32f ln2_hi = 0.693145751953125f; /* The lowest 7 bits are zeros */
	const Yep32f ln2_lo = 1.428606765330187045037746429443359375e-6f;

	const Yep32f c2 = 0.499999940395355224609375f;
	const Yep32f c3 = 0.1666651666164398193359375f;
	const Yep32f c4 = 4.1668035089969635009765625e-2f;
	const Yep32f c5 = 8.369087241590023040771484375e-3f;
	const Yep32f c6 = 1.384208793751895427703857421875e-3f;
#endif
	
	if YEP_UNLIKELY(yepBuiltin_IsNaN_32f(x)) {
		return x;
	} else {
		Yep32f t = x * log2e + magicBias;
		Yep32u e1 = yepBuiltin_Cast_32f_32u(t) << 23;
		Yep32u e2 = e1;
		e1 = yepBuiltin_Clamp_32s32s32s_32s(e1, -126 << 23, 127 << 23);
		e2 -= e1;
		const Yep32f s1 = yepBuiltin_Cast_32u_32f(e1 + 0x3F800000u);
		const Yep32f s2 = yepBuiltin_Cast_32u_32f(e2 + 0x3F800000u);
		t -= magicBias;
		const Yep32f rx = (x - t * ln2_hi) - t * ln2_lo;
		const Yep32f rf = rx  + rx * rx * (c2 + rx * (c3 + rx * (c4 + rx * (c5 + rx * c6))));
		Yep32f f = s2 * (s1 * rf + s1);
		if YEP_UNLIKELY(x > infCutoff) {
			f = yepBuiltin_PositiveInfinity_32f();
		}
		if YEP_UNLIKELY(x < zeroCutoff) {
			f = 0.0f;
		}
		return f;
	}
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Exp_64f_64f(Yep64f x) {

#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f magicBias = 0x1.8000000000000p+52;
	const Yep64f log2e     = 0x1.71547652B82FEp+0;
	#if defined(YEP_PROCESSOR_SUPPORTS_FMA_EXTENSION)
		const Yep64df ln2 = { 0x1.62E42FEFA39EFp-1, 0x1.ABC9E3B39803Fp-56 };
	#else
		const Yep64df ln2 = { 0x1.62E42FEFA3800p-1, 0x1.EF35793C76730p-45 };
	#endif
	const Yep64f c2  = 0x1.0000000000005p-1;
	const Yep64f c3  = 0x1.5555555555540p-3;
	const Yep64f c4  = 0x1.5555555552115p-5;
	const Yep64f c5  = 0x1.11111111173CAp-7;
	const Yep64f c6  = 0x1.6C16C17F2BF99p-10;
	const Yep64f c7  = 0x1.A01A017EEB164p-13;
	const Yep64f c8  = 0x1.A019A6AC02A7Dp-16;
	const Yep64f c9  = 0x1.71DE71651CE7Ap-19;
	const Yep64f c10 = 0x1.28A284098D813p-22;
	const Yep64f c11 = 0x1.AE9043CA87A40p-26;

	const Yep64f zeroCutoff = -0x1.74910D52D3051p+9;
	const Yep64f infCutoff = 0x1.62E42FEFA39EFp+9;
#else
	const Yep64f magicBias = 6755399441055744.0;
	const Yep64f log2e     = 1.4426950408889634;
	#if defined(YEP_PROCESSOR_SUPPORTS_FMA_EXTENSION)
		const Yep64df ln2 = { 0.6931471805599453, 2.3190468138462996e-17 };
	#else
		const Yep64df ln2 = { 0.6931471805598903, 5.497923018708371e-14 };
	#endif
	const Yep64f c2  = 0.5000000000000006;
	const Yep64f c3  = 0.16666666666666607;
	const Yep64f c4  = 0.04166666666657385;
	const Yep64f c5  = 0.008333333333377175;
	const Yep64f c6  = 0.0013888888932278352;
	const Yep64f c7  = 0.0001984126974695729;
	const Yep64f c8  = 2.4801504579877947e-5;
	const Yep64f c9  = 2.755738182142102e-6;
	const Yep64f c10 = 2.762627110160372e-7;
	const Yep64f c11 = 2.5062096212675488e-8;

	const Yep64f zeroCutoff = -745.1332191019411;
	const Yep64f infCutoff = 709.7827128933840;
#endif
		
	if YEP_UNLIKELY(yepBuiltin_IsNaN_64f(x)) {
		return x;
	} else {
		Yep64f t = x * log2e + magicBias;
		Yep32u e1 = yepBuiltin_GetLowPart_64u_32u(yepBuiltin_Cast_64f_64u(t)) << 20;
		Yep32u e2 = e1;
		e1 = yepBuiltin_Clamp_32s32s32s_32s(e1, -1022 << 20, 1023 << 20);
		e2 -= e1;
		const Yep64f s1 = yepBuiltin_Cast_64u_64f(yepBuiltin_CombineParts_32u32u_64u(e1 + 0x3FF00000u, 0u));
		const Yep64f s2 = yepBuiltin_Cast_64u_64f(yepBuiltin_CombineParts_32u32u_64u(e2 + 0x3FF00000u, 0u));
		t -= magicBias;
		const Yep64f rx = (x - t * ln2.high) - t * ln2.low;
		const Yep64f px = yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
				yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
					yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
						yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
							yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
								yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
									yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
										yepBuiltin_MultiplyAdd_64f64f64f_64f(rx,
											yepBuiltin_MultiplyAdd_64f64f64f_64f(rx, c11, c10),
										c9),
									c8),
								c7),
							c6),
						c5),
					c4),
				c3),
			c2);
		const Yep64f rf = yepBuiltin_MultiplyAdd_64f64f64f_64f(rx, rx * px, rx);
		Yep64f f = s2 * yepBuiltin_MultiplyAdd_64f64f64f_64f(s1, rf, s1);
		if YEP_UNLIKELY(x > infCutoff) {
			f = yepBuiltin_PositiveInfinity_64f();
		}
		if YEP_UNLIKELY(x < zeroCutoff) {
			f = 0.0;
		}
		return f;
	}
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Sin_64f_64f(Yep64f x) {
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f minusPio2_hi = -0x1.921FB54440000p+0;
	const Yep64f minusPio2_me = -0x1.68C234C4C8000p-39;
	const Yep64f minusPio2_lo =  0x1.9D747F23E32EDp-79;
	const Yep64f twoOPi       =  0x1.45F306DC9C883p-1;
	const Yep64f magicBias    =  0x1.8000000000000p+52;

	const Yep64f c0           =  0x1.0000000000000p+0;
	const Yep64f c2           = -0x1.0000000000000p-1;
	const Yep64f c3           = -0x1.5555555555546p-3;
	const Yep64f c4           =  0x1.555555555554Bp-5;
	const Yep64f c5           =  0x1.111111110F51Ep-7;
	const Yep64f c6           = -0x1.6C16C16C15038p-10;
	const Yep64f c7           = -0x1.A01A019BB92C0p-13;
	const Yep64f c8           =  0x1.A01A019C94874p-16;
	const Yep64f c9           =  0x1.71DE3535C8A8Ap-19;
	const Yep64f c10          = -0x1.27E4F7F65104Fp-22;
	const Yep64f c11          = -0x1.AE5E38936D046p-26;
	const Yep64f c12          =  0x1.1EE9DF6693F7Ep-29;
	const Yep64f c13          =  0x1.5D8711D281543p-33;
	const Yep64f c14          = -0x1.8FA87EF79AE3Fp-37;
#else
	const Yep64f minusPio2_hi = -1.5707963267923333;
	const Yep64f minusPio2_me = -2.5633441515971907e-12;
	const Yep64f minusPio2_lo =  2.6718907338610155e-24;
	const Yep64f twoOPi       =  0.6366197723675814;
	const Yep64f magicBias    =  6755399441055744.0;

	const Yep64f c0           =  1.0;
	const Yep64f c2           = -0.5;
	const Yep64f c3           = -0.16666666666666624;
	const Yep64f c4           =  0.041666666666666595;
	const Yep64f c5           =  0.008333333333320921;
	const Yep64f c6           = -0.0013888888888873418;
	const Yep64f c7           = -0.0001984126982882608;
	const Yep64f c8           =  2.480158728907678e-05;
	const Yep64f c9           =  2.755731339913502e-06;
	const Yep64f c10          = -2.755731424340092e-07;
	const Yep64f c11          = -2.505071756776031e-08;
	const Yep64f c12          =  2.0875709384133097e-09;
	const Yep64f c13          =  1.58946757299646e-10;
	const Yep64f c14          = -1.135896887279365e-11;
#endif
	Yep64f t = x * twoOPi + magicBias;
	const Yep32u n = yepBuiltin_GetLowPart_64u_32u(yepBuiltin_Cast_64f_64u(t));
	t -= magicBias;
	x += t * minusPio2_hi;
	const Yep64f a = x;
	const Yep64f midProduct = t * minusPio2_me;
	x += midProduct;
	const Yep64f r = midProduct - (x - a);
	x += (t * minusPio2_lo + r);
	
	const Yep64f sqrX = x * x;
	Yep64f sinX = c13;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c11);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c9);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c7);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c5);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c3);
	sinX = sinX * sqrX;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, x, x);
	Yep64f cosX = c14;
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c12);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c10);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c8);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c6);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c4);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c2);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c0);
	
	const Yep64f f = (n & 1) ? cosX : sinX;
	return (n & 2) ? -f : f;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Cos_64f_64f(Yep64f x) {
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f minusPio2_hi = -0x1.921FB54440000p+0;
	const Yep64f minusPio2_me = -0x1.68C234C4C8000p-39;
	const Yep64f minusPio2_lo =  0x1.9D747F23E32EDp-79;
	const Yep64f twoOPi       =  0x1.45F306DC9C883p-1;
	const Yep64f magicBias    =  0x1.8000000000000p+52;

	const Yep64f c0           = -0x1.0000000000000p+0;
	const Yep64f c2           =  0x1.0000000000000p-1;
	const Yep64f c3           = -0x1.5555555555546p-3;
	const Yep64f c4           = -0x1.555555555554Bp-5;
	const Yep64f c5           =  0x1.111111110F51Ep-7;
	const Yep64f c6           =  0x1.6C16C16C15038p-10;
	const Yep64f c7           = -0x1.A01A019BB92C0p-13;
	const Yep64f c8           = -0x1.A01A019C94874p-16;
	const Yep64f c9           =  0x1.71DE3535C8A8Ap-19;
	const Yep64f c10          =  0x1.27E4F7F65104Fp-22;
	const Yep64f c11          = -0x1.AE5E38936D046p-26;
	const Yep64f c12          = -0x1.1EE9DF6693F7Ep-29;
	const Yep64f c13          =  0x1.5D8711D281543p-33;
	const Yep64f c14          =  0x1.8FA87EF79AE3Fp-37;
#else
	const Yep64f minusPio2_hi = -1.5707963267923333;
	const Yep64f minusPio2_me = -2.5633441515971907e-12;
	const Yep64f minusPio2_lo =  2.6718907338610155e-24;
	const Yep64f twoOPi       =  0.6366197723675814;
	const Yep64f magicBias    =  6755399441055744.0;

	const Yep64f c0           = -1.0;
	const Yep64f c2           =  0.5;
	const Yep64f c3           = -0.16666666666666624;
	const Yep64f c4           = -0.041666666666666595;
	const Yep64f c5           =  0.008333333333320921;
	const Yep64f c6           =  0.0013888888888873418;
	const Yep64f c7           = -0.0001984126982882608;
	const Yep64f c8           = -2.480158728907678e-05;
	const Yep64f c9           =  2.755731339913502e-06;
	const Yep64f c10          =  2.755731424340092e-07;
	const Yep64f c11          = -2.505071756776031e-08;
	const Yep64f c12          = -2.0875709384133097e-09;
	const Yep64f c13          =  1.58946757299646e-10;
	const Yep64f c14          =  1.135896887279365e-11;
#endif
	Yep64f t = x * twoOPi + magicBias;
	const Yep32u n = yepBuiltin_GetLowPart_64u_32u(yepBuiltin_Cast_64f_64u(t));
	t -= magicBias;
	x += t * minusPio2_hi;
	const Yep64f a = x;
	const Yep64f midProduct = t * minusPio2_me;
	x += midProduct;
	const Yep64f r = midProduct - (x - a);
	x += (t * minusPio2_lo + r);
	
	const Yep64f sqrX = x * x;
	Yep64f sinX = c13;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c11);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c9);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c7);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c5);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c3);
	sinX = sinX * sqrX;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, x, x);
	Yep64f minusCosX = c14;
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c12);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c10);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c8);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c6);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c4);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c2);
	minusCosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(minusCosX, sqrX, c0);
	
	const Yep64f f = (n & 1) ? sinX : minusCosX;
	return (n & 2) ? f : -f;
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Tan_64f_64f(Yep64f x) {
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f minusPio2_hi = -0x1.921FB54440000p+0;
	const Yep64f minusPio2_me = -0x1.68C234C4C8000p-39;
	const Yep64f minusPio2_lo =  0x1.9D747F23E32EDp-79;
	const Yep64f twoOPi       =  0x1.45F306DC9C883p-1;
	const Yep64f magicBias    =  0x1.8000000000000p+52;

	const Yep64f c0           =  0x1.0000000000000p+0;
	const Yep64f c2           = -0x1.0000000000000p-1;
	const Yep64f c3           = -0x1.5555555555546p-3;
	const Yep64f c4           =  0x1.555555555554Bp-5;
	const Yep64f c5           =  0x1.111111110F51Ep-7;
	const Yep64f c6           = -0x1.6C16C16C15038p-10;
	const Yep64f c7           = -0x1.A01A019BB92C0p-13;
	const Yep64f c8           =  0x1.A01A019C94874p-16;
	const Yep64f c9           =  0x1.71DE3535C8A8Ap-19;
	const Yep64f c10          = -0x1.27E4F7F65104Fp-22;
	const Yep64f c11          = -0x1.AE5E38936D046p-26;
	const Yep64f c12          =  0x1.1EE9DF6693F7Ep-29;
	const Yep64f c13          =  0x1.5D8711D281543p-33;
	const Yep64f c14          = -0x1.8FA87EF79AE3Fp-37;
#else
	const Yep64f minusPio2_hi = -1.5707963267923333;
	const Yep64f minusPio2_me = -2.5633441515971907e-12;
	const Yep64f minusPio2_lo =  2.6718907338610155e-24;
	const Yep64f twoOPi       =  0.6366197723675814;
	const Yep64f magicBias    =  6755399441055744.0;

	const Yep64f c0           =  1.0;
	const Yep64f c2           = -0.5;
	const Yep64f c3           = -0.16666666666666624;
	const Yep64f c4           =  0.041666666666666595;
	const Yep64f c5           =  0.008333333333320921;
	const Yep64f c6           = -0.0013888888888873418;
	const Yep64f c7           = -0.0001984126982882608;
	const Yep64f c8           =  2.480158728907678e-05;
	const Yep64f c9           =  2.755731339913502e-06;
	const Yep64f c10          = -2.755731424340092e-07;
	const Yep64f c11          = -2.505071756776031e-08;
	const Yep64f c12          =  2.0875709384133097e-09;
	const Yep64f c13          =  1.58946757299646e-10;
	const Yep64f c14          = -1.135896887279365e-11;
#endif

	Yep64f t = x * twoOPi + magicBias;
	const Yep32u n = yepBuiltin_GetLowPart_64u_32u(yepBuiltin_Cast_64f_64u(t));
	t -= magicBias;
	x += t * minusPio2_hi;
	const Yep64f a = x;
	const Yep64f midProduct = t * minusPio2_me;
	x += midProduct;
	const Yep64f r = midProduct - (x - a);
	x += (t * minusPio2_lo + r);
	
	const Yep64f sqrX = x * x;
	Yep64f sinX = c13;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c11);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c9);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c7);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c5);
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, sqrX, c3);
	sinX = sinX * sqrX;
	sinX = yepBuiltin_MultiplyAdd_64f64f64f_64f(sinX, x, x);
	Yep64f cosX = c14;
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c12);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c10);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c8);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c6);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c4);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c2);
	cosX = yepBuiltin_MultiplyAdd_64f64f64f_64f(cosX, sqrX, c0);

	return (n & 1) ? (-cosX / sinX) : (sinX / cosX);
}

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#pragma intrinsic(sqrt)
#endif

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32f yepBuiltin_Sqrt_32f_32f(Yep32f x) {
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	return sqrt(x);
#elif defined(YEP_NVIDIA_COMPILER)
	return __fsqrt_rn(x);
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	return __builtin_sqrtf(x);
#elif defined(YEP_ARM_COMPILER)
	return __sqrtf(x);
#else
	#error "Compiler-specific implementation needed"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_Sqrt_64f_64f(Yep64f x) {
#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	return sqrt(x);
#elif defined(YEP_NVIDIA_COMPILER)
	return __dsqrt_rn(x);
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	return __builtin_sqrt(x);
#elif defined(YEP_ARM_COMPILER)
	return __sqrt(x);
#else
	#error "Compiler-specific implementation needed"
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64f yepBuiltin_ArcSin_64f_64f(Yep64f x) {
#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
	const Yep64f half =  0x1.0000000000000p-1;
	const Yep64f ac3  =  0x1.5555555555332p-3;
	const Yep64f ac5  =  0x1.33333333768C7p-4;
	const Yep64f ac7  =  0x1.6DB6DB3E4DA8Ap-5;
	const Yep64f ac9  =  0x1.F1C72D5B739EFp-6;
	const Yep64f ac11 =  0x1.6E89DC94F7B19p-6;
	const Yep64f ac13 =  0x1.1C6D1EE2BF355p-6;
	const Yep64f ac15 =  0x1.C6E7A6CA04E0Dp-7;
	const Yep64f ac17 =  0x1.8F47A67BD13CFp-7;
	const Yep64f ac19 =  0x1.A7AC3B4A38FB8p-8;
	const Yep64f ac21 =  0x1.4296C857308B2p-6;
	const Yep64f ac23 = -0x1.0DB1C05152E38p-6;
	const Yep64f ac25 =  0x1.06AD1B749C8D4p-5;
	
	const Yep64f bc0  =  0x1.921FB54442D18p+0;
	const Yep64f bc1  =  0x1.6A09E667F3BC7p+1;
	const Yep64f bc3  = -0x1.E2B7DDDFF06ACp-2;
	const Yep64f bc5  =  0x1.B27247B01E1B8p-3;
	const Yep64f bc7  = -0x1.02995B468EBC5p-3;
	const Yep64f bc9  =  0x1.5FFB7742ECDC6p-4;
	const Yep64f bc11 = -0x1.032E1D4CDEC75p-4;
	const Yep64f bc13 =  0x1.924AF9192AF6Ap-5;
	const Yep64f bc15 = -0x1.41264A779EBFFp-5;
	const Yep64f bc17 =  0x1.1D9B9AF0438A1p-5;
	const Yep64f bc19 = -0x1.106A0643EEB6Cp-6;
	const Yep64f bc21 =  0x1.EBCC69FBEBEC2p-5;
	const Yep64f bc23 =  0x1.B2DE37FA33AAAp-5;
	const Yep64f bc25 =  0x1.8509940B63DD2p-4;
#else
	const Yep64f half =  0.5;
	const Yep64f ac3  =  0.16666666666665148;
	const Yep64f ac5  =  0.07500000000382832;
	const Yep64f ac7  =  0.044642856797897215;
	const Yep64f ac9  =  0.03038196019570621;
	const Yep64f ac11 =  0.02237173596574413;
	const Yep64f ac13 =  0.017360000764699752;
	const Yep64f ac15 =  0.013882595481880445;
	const Yep64f ac17 =  0.01218505505642922;
	const Yep64f ac19 =  0.006464733576851893;
	const Yep64f ac21 =  0.019689269681074158;
	const Yep64f ac23 = -0.016460836229539505;
	const Yep64f ac25 =  0.03206496584324872;
	
	const Yep64f bc0  =  1.5707963267948966;
	const Yep64f bc1  =  2.8284271247461876;
	const Yep64f bc3  = -0.4714045207912061;
	const Yep64f bc5  =  0.21213203436105998;
	const Yep64f bc7  = -0.12626906689714992;
	const Yep64f bc9  =  0.08593317591185387;
	const Yep64f bc11 = -0.0632764000455824;
	const Yep64f bc13 =  0.04910801555646922;
	const Yep64f bc15 = -0.03920282883060366;
	const Yep64f bc17 =  0.034864237417523876;
	const Yep64f bc19 = -0.01662684070445712;
	const Yep64f bc21 =  0.060033995628484785;
	const Yep64f bc23 =  0.053084477740062155;
	const Yep64f bc25 =  0.0949798377025595;
#endif
	const Yep64f absX = yepBuiltin_Abs_64f_64f(x);
	if YEP_LIKELY(absX <= 1.0) {
		if (absX <= half) {
			const Yep64f ax = x;
			const Yep64f ax2 = ax * ax;
			Yep64f af = ac25;
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac23);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac21);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac19);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac17);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac15);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac13);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac11);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac9);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac7);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac5);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af, ax2, ac3);
			af = yepBuiltin_MultiplyAdd_64f64f64f_64f(af * ax2, ax, ax);
			return af;
		} else {
			const Yep64f bx2 = absX * half - half;
			const Yep64f bx = yepBuiltin_Sqrt_64f_64f(bx2);
			Yep64f bf = bc25;
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc23);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc21);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc19);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc17);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc15);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc13);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc11);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc9);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc7);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc5);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc3);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx2, bc1);
			bf = yepBuiltin_MultiplyAdd_64f64f64f_64f(bf, bx, bc0);
			return x > 0.0 ? bf : -bf;
		}
	} else {
		if (yepBuiltin_IsNaN_64f(absX)) {
			return x;
		} else {
			return yepBuiltin_NaN_64f();
		}
	}
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64u yepBuiltin_Multiply_32u32u_64u(Yep32u x, Yep32u y) {
#if defined(YEP_MICROSOFT_COMPILER) && defined(YEP_X86_CPU)
	return __emulu(x, y);
#else
	return Yep64u(x) * Yep64u(y);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep64s yepBuiltin_Multiply_32s32s_64s(Yep32s x, Yep32s y) {
#if defined(YEP_MICROSOFT_COMPILER) && defined(YEP_X86_CPU)
	return __emul(x, y);
#else
	return Yep64s(x) * Yep64s(y);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32u yepBuiltin_MultiplyHigh_32u32u_32u(Yep32u x, Yep32u y) {
#if defined(YEP_MICROSOFT_COMPILER) && defined(YEP_X86_CPU)
	return Yep32u(__emulu(x, y) >> 32);
#elif defined(YEP_NVIDIA_COMPILER)
	return __umulhi(x, y);
#else
	return Yep32u(Yep64u(x) * Yep64u(y) >> 32);
#endif
}

YEP_NATIVE_FUNCTION static YEP_INLINE Yep32s yepBuiltin_MultiplyHigh_32s32s_32s(Yep32s x, Yep32s y) {
#if defined(YEP_MICROSOFT_COMPILER) && defined(YEP_X86_CPU)
	return Yep32s(Yep32u(Yep64u(__emul(x, y)) >> 32));
#elif defined(YEP_NVIDIA_COMPILER)
	return __mulhi(x, y);
#else
	return Yep32s(Yep32u(Yep64u(Yep64s(x) * Yep64s(y)) >> 32));
#endif
}

#if defined(YEP_MSVC_COMPATIBLE_COMPILER) && (defined(YEP_X64_ABI) || defined(YEP_IA64_ABI))
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128u yepBuiltin_Multiply_64u64u_128u(Yep64u x, Yep64u y) {
	Yep128u result;
	result.low = _umul128(x, y, &result.high);
	return result;
}
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(YEP_X64_ABI)
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128u yepBuiltin_Multiply_64u64u_128u(Yep64u x, Yep64u y) {
	const __uint128_t product = ((__uint128_t)x) * ((__uint128_t)y);
	Yep128u result;
	result.low = Yep64u(product);
	result.high = Yep64u(product >> 64);
	return result;
}
#elif defined(YEP_NVIDIA_COMPILER) && defined(YEP_CUDA_GPU)
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128u yepBuiltin_Multiply_64u64u_128u(Yep64u x, Yep64u y) {
	Yep128u result;
	result.low = x * y;
	result.high = __umul64hi(x, y);
	return result;
}
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER) && (defined(YEP_X64_ABI) || defined(YEP_IA64_ABI))
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128s yepBuiltin_Multiply_64s64s_128s(Yep64s x, Yep64s y) {
	Yep128s result;
	__int64 highPart;
	result.low = _mul128(x, y, &highPart);
	result.high = highPart;
	return result;
}
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(YEP_X64_ABI)
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128s yepBuiltin_Multiply_64s64s_128s(Yep64s x, Yep64s y) {
	const __int128_t product = ((__int128_t)x) * ((__int128_t)y);
	Yep128s result;
	result.low = Yep64u(product);
	result.high = Yep64u(((__uint128_t)product) >> 64);
	return result;
}
#elif defined(YEP_NVIDIA_COMPILER) && defined(YEP_CUDA_GPU)
YEP_NATIVE_FUNCTION static YEP_INLINE Yep128s yepBuiltin_Multiply_64s64s_128s(Yep64s x, Yep64s y) {
	Yep128s result;
	result.low = x * y;
	result.high = __mul64hi(x, y);
	return result;
}
#endif

/* Emulation of __cpuid, __cpuidex, and _xgetbv intrinsics on x86 and x86-64 */
#if defined(YEP_X86_CPU)
	#if defined(YEP_GCC_COMPATIBLE_COMPILER)
		#if defined(YEP_X86_ABI) && defined(YEP_PIC)
			static YEP_INLINE void __cpuid(int CPUInfo[4], int InfoType) {
				CPUInfo[0] = InfoType;
				asm volatile (
					"movl %%ebx, %%edi;"
					"cpuid;"
					"xchgl %%ebx, %%edi;"
					:"+a" (CPUInfo[0]), "=D" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
					:
					:
				);
			}

			static YEP_INLINE void __cpuidex(int CPUInfo[4], int InfoType, int ECXValue) {
				CPUInfo[0] = InfoType;
				CPUInfo[2] = ECXValue;
				asm volatile (
					"movl %%ebx, %%edi;"
					"cpuid;"
					"xchgl %%ebx, %%edi;"
					:"+a" (CPUInfo[0]), "=D" (CPUInfo[1]), "+c" (CPUInfo[2]), "=d" (CPUInfo[3])
					:
					:
				);
			}
		#else
			static YEP_INLINE void __cpuid(int CPUInfo[4], int InfoType) {
				CPUInfo[0] = InfoType;
				asm volatile (
					"cpuid;"
					:"+a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
					:
					:
				);
			}

			static YEP_INLINE void __cpuidex(int CPUInfo[4], int InfoType, int ECXValue) {
				CPUInfo[0] = InfoType;
				CPUInfo[2] = ECXValue;
				asm volatile (
					"cpuid;"
					:"+a" (CPUInfo[0]), "=b" (CPUInfo[1]), "+c" (CPUInfo[2]), "=d" (CPUInfo[3])
					:
					:
				);
			}
		#endif

		#if !defined(YEP_INTEL_COMPILER) && !defined(YEP_K1OM_X64_ABI)
			static YEP_INLINE Yep64u _xgetbv(Yep32u ext_ctrl_reg) {
				Yep32u lo, hi;
				asm volatile (
					"xgetbv"
					: "=a"(lo), "=d"(hi)
					: "c"(ext_ctrl_reg)
					:
				);
				return (Yep64u(hi) << 32) | Yep64u(lo);
			}
		#endif
	#elif defined(YEP_MSVC_COMPATIBLE_COMPILER)
		/* __cpuidex intrinsic is not suppored until Visual Studio 2008 SP1 */
		#if defined(YEP_MICROSOFT_COMPILER) && _MSC_FULL_VER < 150030729
			#pragma section(".text")
		
			#if defined(YEP_X86_CPU)
				/* fastcall: first argument in ecx, second in edx, third in [esp + 4] */
				
				__declspec(allocate(".text")) static const char __cpuidex_bytecode[] =
					"\x53\x56\x8B\x74\x24\x0C\x89\xD0\x0F\xA2\x89\x06\x89\x5E\x04\x89";

			#else
				/* x64: first argument in ecx, second in edx, third in r8 */
				
				__declspec(allocate(".text")) static const char __cpuidex_bytecode[] =
					"\x53\x89\xD0\x0F\xA2\x41\x89\x00\x41\x89\x58\x04\x41\x89\x48\x08\x41\x89\x50\x0C\x5B\xC3";

			#endif
				
			typedef void(__fastcall *CpuidexPointer)(int, int, int[4]);

			static YEP_INLINE void __cpuidex(int CPUInfo[4], int InfoType, int ECXValue) {
				(CpuidexPointer(&__cpuidex_bytecode))(ECXValue, InfoType, CPUInfo);
			}
		
		#endif
		/* _xgetbv intrinsic is not supported until Visual Studio 2010 SP1 */
		#if defined(YEP_MICROSOFT_COMPILER) && _MSC_FULL_VER < 160040219
			#pragma section(".text")
		
			#if defined(YEP_X86_CPU)
				/* fastcall: first argument in ecx, second in edx, third in [esp + 4] */
				
				__declspec(allocate(".text")) static const char _xgetbv_bytecode[] = 
					"\x0F\x01\xD0\xC3";

			#else
				/* x64: first argument in ecx, second in edx, third in r8 */
				__declspec(allocate(".text")) static const char _xgetbv_bytecode[] = 
					"\x0F\x01\xD0\x48\xC1\xE2\x20\x48\x09\xD0\xC3";

			#endif
				
			typedef Yep64u(__fastcall *XgetbvPointer)(Yep32u);
			typedef void(__fastcall *CpuidexPointer)(int, int, int[4]);

			static YEP_INLINE Yep64u _xgetbv(Yep32u ext_ctrl_reg) {
				return (XgetbvPointer(&_xgetbv_bytecode))(ext_ctrl_reg);
			}
		#elif !defined(YEP_INTEL_COMPILER)
			/* Visual Stidio 2010 SP1: _xgetbv intrinsic is supported, but not declared */
			extern "C" unsigned __int64 __cdecl _xgetbv(unsigned int ext_ctrl_reg);
			#pragma intrinsic(_xgetbv)
		#endif
	#endif
#endif
