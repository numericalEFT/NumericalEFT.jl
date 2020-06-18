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

#include <yepPredefines.h>
#include <yepTypes.h>

#if defined(YEP_PRIVATE_SYMBOL) && defined(YEP_LOCAL_SYMBOL) && defined(YEP_EXPORT_SYMBOL) && defined(YEP_IMPORT_SYMBOL) && defined(YEP_PUBLIC_SYMBOL)
	#undef YEP_PRIVATE_SYMBOL
	#undef YEP_LOCAL_SYMBOL
	#undef YEP_EXPORT_SYMBOL
	#undef YEP_IMPORT_SYMBOL
	#undef YEP_PUBLIC_SYMBOL
	#if defined(YEP_LINUX_OS)
		#if defined(YEP_STATIC_LIBRARY)
			#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("internal")))
			#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
			#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("hidden")))
			#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("hidden")))
		#else
			#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("internal")))
			#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
			#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("default")))
			#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("default")))
		#endif
	#elif defined(YEP_MACOSX_OS)
		#if defined(YEP_STATIC_LIBRARY)
			#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("hidden")))
			#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
			#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("hidden")))
			#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("hidden")))
		#else
			#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("hidden")))
			#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
			#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("default")))
			#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("default")))
		#endif
	#elif defined(YEP_WINDOWS_OS)
		#if defined(YEP_STATIC_LIBRARY)
			#define YEP_PRIVATE_SYMBOL
			#define YEP_LOCAL_SYMBOL
			#define YEP_EXPORT_SYMBOL
			#define YEP_IMPORT_SYMBOL
		#else
			#define YEP_PRIVATE_SYMBOL
			#define YEP_LOCAL_SYMBOL
			#define YEP_EXPORT_SYMBOL __declspec(dllexport)
			#define YEP_IMPORT_SYMBOL __declspec(dllimport)
		#endif
	#else
		#error "Unsupported output format"
	#endif
	#if defined(YEP_BUILD_LIBRARY)
		#define YEP_PUBLIC_SYMBOL YEP_EXPORT_SYMBOL
	#else
		#define YEP_PUBLIC_SYMBOL YEP_IMPORT_SYMBOL
	#endif
#else
	#error "Visibility symbols are not defined"
#endif

#if (defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER)) && defined(__ELF__)
	#define YEP_USE_DISPATCH_POINTER_SECTION  __attribute__ ((section(".data.DispatchPointer")))
	#define YEP_USE_DISPATCH_FUNCTION_SECTION __attribute__ ((section(".text.DispatchFunction")))
	#if defined(YEP_ARM_CPU)
		#define YEP_USE_DISPATCH_TABLE_SECTION __attribute__ ((section(".rodata.DispatchTable,\"a\",%progbits @")))
	#else
		#define YEP_USE_DISPATCH_TABLE_SECTION __attribute__ ((section(".rodata.DispatchTable,\"a\",@progbits #")))
	#endif
#elif (defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER)) && defined(__MACH__)
	#define YEP_USE_DISPATCH_POINTER_SECTION  __attribute__ ((section("__DATA,__data.DispPtr")))
	#define YEP_USE_DISPATCH_FUNCTION_SECTION __attribute__ ((section("__TEXT,__text.DispFun")))
	#define YEP_USE_DISPATCH_TABLE_SECTION    __attribute__ ((section("__DATA,__const.DispTbl")))
#elif defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_USE_DISPATCH_POINTER_SECTION __declspec(allocate(".data$DispatchPointer"))
	/* __declspec(allocate(...)) works with data only. Use pragma for code. */
	#define YEP_USE_DISPATCH_FUNCTION_SECTION
	#define YEP_USE_DISPATCH_TABLE_SECTION   __declspec(allocate(".rdata$DispatchTable"))
#else
	#error "Unsupported compiler or output format"
#endif

/* yepLibrary must be included after the new definitions of YEP_*_SYMBOL */
#include <yepLibrary.h>

#if defined(YEP_DESCRIBE_FUNCTION_IMPLEMENTATION)
	#error "YEP_DESCRIBE_FUNCTION_IMPLEMENTATION macro defined twice"
#else
	#if defined(YEP_DEBUG_LIBRARY)
		#define YEP_DESCRIBE_FUNCTION_IMPLEMENTATION(symbolName, isaFeatures, simdFeatures, systemFeatures, microarchitecture, sourceLanguage, algorithm, optimization) \
			{ symbolName, isaFeatures, simdFeatures, systemFeatures, microarchitecture, sourceLanguage, algorithm, optimization }
	#else
		#define YEP_DESCRIBE_FUNCTION_IMPLEMENTATION(symbolName, isaFeatures, simdFeatures, systemFeatures, microarchitecture, sourceLanguage, algorithm, optimization) \
			{ symbolName, isaFeatures, simdFeatures, systemFeatures, microarchitecture }
	#endif
#endif


#if defined(__cplusplus)
	typedef YepStatus (*FunctionPointer)();

	template <typename Function>
	struct FunctionDescriptor {
		Function function;
		Yep64u isaFeatures;
		Yep64u simdFeatures;
		Yep64u systemFeatures;
		YepCpuMicroarchitecture microarchitecture;
	#if defined(YEP_DEBUG_LIBRARY)
		const char language[4];
		const char* algorithm;
		const char* optimizations;
	#endif
	};

	template<class DescriptorType>
	static YEP_INLINE const DescriptorType* findDefaultDescriptor(const DescriptorType* descriptors) {
		const DescriptorType* defaultDescriptor = &descriptors[0];
		while ((defaultDescriptor ->isaFeatures != YepIsaFeaturesDefault) ||
			(defaultDescriptor ->simdFeatures != YepSimdFeaturesDefault) ||
			(defaultDescriptor ->systemFeatures != YepSystemFeaturesDefault) ||
			(defaultDescriptor ->microarchitecture != YepCpuMicroarchitectureUnknown))
		{
			defaultDescriptor++;
		}
		return defaultDescriptor;
	}
#endif
