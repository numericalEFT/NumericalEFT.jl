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

#if defined(_WIN32_WCE) || defined(WIN32_PLATFORM_HPC2000) || defined(WIN32_PLATFORM_HPCPRO) || defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	#define YEP_WINDOWSCE_OS
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
	#define YEP_WINDOWS_OS
#elif defined(__linux) || defined(__linux__) || defined(linux)
	#define YEP_LINUX_OS
	#if defined(ANDROID) || defined(__ANDROID__)
		#define YEP_ANDROID_LINUX_OS
	#endif
	#if defined(__gnu_linux__)
		#define YEP_GNU_LINUX_OS
	#endif
#elif defined(__APPLE__) && defined(__MACH__)
	#define YEP_MACOSX_OS
#endif

#define YEP_PREPROCESSOR_CONVERT_TO_STRING_HELPER(text) #text
#define YEP_PREPROCESSOR_CONVERT_TO_STRING(text) YEP_PREPROCESSOR_CONVERT_TO_STRING_HELPER(text)

#define YEP_PREPROCESSOR_CONCATENATE_STRINGS_HELPER(a, b) a##b
#define YEP_PREPROCESSOR_CONCATENATE_STRINGS(a, b) YEP_PREPROCESSOR_CONCATENATE_STRINGS_HELPER(a, b)

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
	#define YEP_MICROSOFT_COMPILER
#elif defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__CUDA_ARCH__)
	#define YEP_GNU_COMPILER
#elif defined(__INTEL_COMPILER)
	#define YEP_INTEL_COMPILER
	#if defined(_MSC_VER)
		#define YEP_INTEL_COMPILER_FOR_WINDOWS
	#else
		#define YEP_INTEL_COMPILER_FOR_UNIX
	#endif
#elif defined(__clang__)
	#define YEP_CLANG_COMPILER
#elif defined(__xlc__) || defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
	#define YEP_IBM_COMPILER
#elif defined(__PGI)
	#define YEP_PGI_COMPILER
#elif defined(__BORLANDC__) || defined(__CODEGEARC__)
	#define YEP_EMBARCADERO_COMPILER
#elif defined(__PATHCC__)
	#define YEP_PATHSCALE_COMPILER
#elif defined(__CC_ARM)
	#define YEP_ARM_COMPILER
#elif defined(__CUDA_ARCH__)
	#define YEP_NVIDIA_COMPILER
#endif

#if defined(YEP_MICROSOFT_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_WINDOWS)
	#define YEP_MSVC_COMPATIBLE_COMPILER
#endif

#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_INTEL_COMPILER_FOR_UNIX)
	#define YEP_GCC_COMPATIBLE_COMPILER
#endif

#if defined(__pic__) || defined(__PIC__)
	#define YEP_PIC
#endif

#if defined(_VC_NODEFAULTLIB)
	#if !defined(YEP_CUSTOM_RUNTIME)
		#define YEP_CUSTOM_RUNTIME
	#endif
#endif

#if defined(_M_IX86) || defined(i386) || defined(__i386) || defined(__i386__) || defined(_X86_) || defined(__X86__) || defined(__I86__) || defined(__INTEL__) || defined(__THW_INTEL__)
	#define YEP_X86_CPU
	#define YEP_X86_ABI
#elif defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
	#define YEP_X86_CPU
	#define YEP_X64_ABI
#elif defined(_M_IA64) || defined(__itanium__) || defined(__ia64) || defined(__ia64__) || defined(_IA64) || defined(__IA64__)
	#define YEP_IA64_CPU
	#define YEP_IA64_ABI
#elif defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) ||  defined(__thumb__) || defined(__arm) || defined(_ARM)
	#define YEP_ARM_CPU
	#define YEP_ARM_ABI
#elif defined(_M_MRX000) || defined(_MIPS_) || defined(_MIPS64) || defined(__mips__) || defined(__mips) || defined(__MIPS__)
	#define YEP_MIPS_CPU
	#if defined(__mips) && (__mips == 64)
		#define YEP_MIPS64_ABI
	#else
		#define YEP_MIPS32_ABI
	#endif
#elif defined(__sparc__) || defined(__sparc)
	#define YEP_SPARC_CPU
	#define YEP_SPARC_ABI
#elif defined(_M_PPC) || defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__)
	#define YEP_POWERPC_CPU
	#define YEP_POWERPC_ABI
#elif defined(__CUDA_ARCH__)
	#define YEP_CUDA_GPU
#elif defined(__OPENCL_VERSION__)
	#define YEP_OPENCL_DEVICE
	#if defined(__CPU__)
		#define YEP_OPENCL_CPU
	#elif defined(__GPU__)
		#define YEP_OPENCL_GPU
	#endif
#endif

#if defined(YEP_X86_CPU)
	#ifndef YEP_PROCESSOR_SUPPORTS_MISALIGNED_MEMORY_ACCESS
		#define YEP_PROCESSOR_SUPPORTS_MISALIGNED_MEMORY_ACCESS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
	#endif

	#if defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_INTEL_COMPILER)
		#if defined(__MMX__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION
			#endif
		#endif
		#if defined(__3dNOW__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_3DNOW_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_3DNOW_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION
			#endif
		#endif
		#if defined(__3dNOW_A__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_3DNOWPLUS_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_3DNOWPLUS_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION
			#endif
		#endif
		#if defined(__SSE__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION
			#endif
		#endif
		#if defined(__SSE2__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION	
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION
			#endif
		#endif
		#if defined(__SSE3__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE3_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE3_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION
			#endif
		#endif
		#if defined(__SSSE3__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSSE3_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSSE3_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION
			#endif
		#endif
		#if defined(__SSE4A__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE4A_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE4A_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION
			#endif
		#endif
		#if defined(__SSE4_1__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE4_1_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE4_1_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION
			#endif
		#endif
		#if defined(__SSE4_2__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE4_2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE4_2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION
			#endif
		#endif
		#if defined(__AVX__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_AVX_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_AVX_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION
			#endif
		#endif
		#if defined(__AVX2__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_AVX2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_AVX2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION
			#endif
		#endif
		#if defined(__F16C__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_F16C_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_F16C_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION
			#endif
		#endif
		#if defined(__FMA4__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_FMA4_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_FMA4_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION
			#endif
		#endif
		#if defined(__FMA__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
			#endif
		#endif
		#if defined(__XOP__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_XOP_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_XOP_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION
			#endif
		#endif
		#if defined(__ABM__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_LZCNT_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_LZCNT_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
			#endif
		#endif
		#if defined(__POPCNT__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_POPCNT_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_POPCNT_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
			#endif
		#endif
		#if defined(__BMI__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
			#endif
		#endif
		#if defined(__BMI2__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_BMI2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_BMI2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
			#endif
		#endif
		#if defined(__TBM__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_TBM_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_TBM_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_TBM_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_TBM_EXTENSION
			#endif
		#endif
		#if defined(__KNC__)
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_KNC_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_KNC_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_KNC_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_KNC_EXTENSION
			#endif
		#endif
	#endif
	#if defined(YEP_MICROSOFT_COMPILER)
		#if defined(YEP_X86_CPU)
			#if _M_IX86_FP >= 1
				#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
					#define YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
				#endif
				#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
					#define YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
				#endif
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION
			#endif
			#if _M_IX86_FP >= 2
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION
			#endif
		#else
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION
			#endif
		#endif
		#if _MSC_VER >= 1310
			/* Visual Studio 2003 or higher */
			#if defined(YEP_X86_CPU) || defined(YEP_INTEL_COMPILER)
				/* Microsoft compiler does not support MMX intrinsics for x64 builds */
				#ifndef YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION
				#endif
				#ifndef YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION
				#endif
			#endif
			#if defined(YEP_X86_CPU) && !defined(YEP_INTEL_COMPILER)
				/* Intel compiler does not support AMD's 3dnow! */
				#ifndef YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION
				#endif
				#ifndef YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION
				#endif
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION
			#endif
		#endif
		#if _MSC_VER >= 1400
			/* Visual Studio 2005 or higher */
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION
			#endif
		#endif
		#if _MSC_VER >= 1500
			/* Visual Studio 2008 or higher */
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION
			#endif
			#if !defined(YEP_INTEL_COMPILER)
				#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION
				#endif
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
			#endif
		#endif
		#if _MSC_VER >= 1600
			/* Visual Studio 2010 or higher */
			#ifndef YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION
			#endif
		#endif
		#if _MSC_FULL_VER >= 160040219
			/* Visual Studio 2010 SP1 or higher*/
			#if !defined(YEP_INTEL_COMPILER)
				#ifndef YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION
				#endif
				#ifndef YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION
				#endif
			#endif
		#endif
		#if _MSC_VER >= 1700
			/* Visual Studio 2012 or higher */
			#ifndef YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_HLE_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_HLE_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_RTM_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_RTM_EXTENSION
			#endif
			#if !defined(YEP_INTEL_COMPILER)
				#ifndef YEP_COMPILER_SUPPORTS_X86_TBM_EXTENSION
					#define YEP_COMPILER_SUPPORTS_X86_TBM_EXTENSION
				#endif
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
			#endif
		#endif
	#endif
	#if defined(YEP_INTEL_COMPILER)
		/* Intel Compiler has a very special understanding of instruction extensions */
		#if defined(__SSE4_2__)
			/* Nehalem. Also supports POPCNT */
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_POPCNT_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_POPCNT_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_POPCNT_EXTENSION
			#endif
		#endif
		#if defined(__AVX2__)
			/* Haswell. Also supports FMA3, F16C, LZCNT, BMI, and BMI2 */
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_LZCNT_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_LZCNT_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_LZCNT_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_X86_BMI2_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_X86_BMI2_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
				#define YEP_COMPILER_SUPPORTS_X86_BMI2_EXTENSION
			#endif
		#endif
	#endif
#elif defined(YEP_ARM_CPU)
	#if defined(YEP_GCC_COMPATIBLE_COMPILER)
		#if defined(__ARM_ARCH_7A__)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_ARCH_6ZK__) || defined(__ARM_ARCH_6K__) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V6K_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V6K_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6__) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V6K_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) || defined(__ARM_FEATURE_DSP) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V5E_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V5E_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5T__) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V5E_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V5_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V5_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V5_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_V4_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_ARM_V4_INSTRUCTIONS
			#endif
		#endif
		#if defined(__ARM_FEATURE_UNALIGNED) || defined(YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS)
			#ifndef YEP_PROCESSOR_SUPPORTS_MISALIGNED_MEMORY_ACCESS
				#define YEP_PROCESSOR_SUPPORTS_MISALIGNED_MEMORY_ACCESS
			#endif
		#endif
		#if defined(__XSCALE__)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_XSCALE_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_ARM_XSCALE_EXTENSION
			#endif
		#endif
		#if defined(__IWMMXT__) || defined(__ARM_WMMX)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_WMMX_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_ARM_WMMX_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_ARM_WMMX_EXTENSION
				#define YEP_COMPILER_SUPPORTS_ARM_WMMX_EXTENSION
			#endif
			#if defined(__IWMMXT2__) || (defined(__ARM_WMMX) && (__ARM_WMMX >= 2))
				#ifndef YEP_PROCESSOR_SUPPORTS_ARM_WMMX2_EXTENSION
					#define YEP_PROCESSOR_SUPPORTS_ARM_WMMX2_EXTENSION
				#endif
				#ifndef YEP_COMPILER_SUPPORTS_ARM_WMMX2_EXTENSION
					#define YEP_COMPILER_SUPPORTS_ARM_WMMX2_EXTENSION
				#endif
			#endif
		#endif
		#if defined(__ARM_NEON__)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_NEON_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_ARM_NEON_EXTENSION
			#endif
			#ifndef YEP_COMPILER_SUPPORTS_ARM_NEON_EXTENSION
				#define YEP_COMPILER_SUPPORTS_ARM_NEON_EXTENSION
			#endif
		#endif
		#if defined(__VFP_FP__) || defined(__ARM_FP)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_ARM_VFP_EXTENSION
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
			#endif
			#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
				#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP2_EXTENSION
					#define YEP_PROCESSOR_SUPPORTS_ARM_VFP2_EXTENSION
				#endif
				#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP3_EXTENSION
					#define YEP_PROCESSOR_SUPPORTS_ARM_VFP3_EXTENSION
				#endif
				#if defined(__ARM_FP) && ((__ARM_FP & 0x2) == 0x2)
					#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP3HP_EXTENSION
						#define YEP_PROCESSOR_SUPPORTS_ARM_VFP3HP_EXTENSION
					#endif
				#endif
				#if defined(YEP_PROCESSOR_SUPPORTS_ARM_NEON_EXTENSION)
					#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP3_D32_EXTENSION
						#define YEP_PROCESSOR_SUPPORTS_ARM_VFP3_D32_EXTENSION
					#endif
				#endif
				#if defined(__ARM_FEATURE_FMA) || (defined(__FP_FAST_FMA) && defined(__FP_FAST_FMAF))
					#ifndef YEP_PROCESSOR_SUPPORTS_ARM_VFP4_EXTENSION
						#define YEP_PROCESSOR_SUPPORTS_ARM_VFP4_EXTENSION
					#endif
				#endif
			#endif
		#endif
		#if defined(__ARM_ARCH_EXT_IDIV__)
			#ifndef YEP_PROCESSOR_SUPPORTS_ARM_DIV_EXTENSION
				#define YEP_PROCESSOR_SUPPORTS_ARM_DIV_EXTENSION
			#endif
		#endif
	#endif
#elif defined(YEP_MIPS_CPU)
	#if defined(YEP_GCC_COMPATIBLE_COMPILER)
		#if defined(__mips_isa_rev) && (__mips_isa_rev >= 2)
			#ifndef YEP_PROCESSOR_SUPPORTS_MIPS_R2_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_MIPS_R2_INSTRUCTIONS
			#endif
		#endif
		#if defined(__mips_dsp)
			#ifndef YEP_PROCESSOR_SUPPORTS_MIPS_DSP_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_MIPS_DSP_INSTRUCTIONS
			#endif
			#if defined(__mips_dsp_rev) && (__mips_dsp_rev >= 2)
				#ifndef YEP_PROCESSOR_SUPPORTS_MIPS_DSP2_INSTRUCTIONS
					#define YEP_PROCESSOR_SUPPORTS_MIPS_DSP2_INSTRUCTIONS
				#endif
			#endif
		#endif
		#if defined(__mips_paired_single_float)
			#ifndef YEP_PROCESSOR_SUPPORTS_MIPS_PAIREDSINGLE_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_MIPS_PAIREDSINGLE_INSTRUCTIONS
			#endif
		#endif
		#if defined(__mips3d)
			#ifndef YEP_PROCESSOR_SUPPORTS_MIPS_3D_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_MIPS_3D_INSTRUCTIONS
			#endif
		#endif
		#if defined(__mips_hard_float)
			#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
			#endif
			#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
				#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
			#endif
		#endif
	#endif
	#if defined(YEP_ANDROID_LINUX_OS)
		#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
		#endif
		#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
		#endif
	#endif
#elif defined(YEP_CUDA_GPU)
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
	#endif
	#if __CUDA_ARCH__ >= 130
		#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
		#endif
	#endif
#elif defined(YEP_OPENCL_DEVICE)
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FPU_INSTRUCTIONS
	#endif
	#if defined(cl_khr_fp64) || defined(cl_amd_fp64))
		#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FPU_INSTRUCTIONS
		#endif
	#endif
#endif
 
#if defined(YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION) || defined(YEP_PROCESSOR_SUPPORTS_X86_FMA4_EXTENSION)
	#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
	#endif
#elif defined(YEP_PROCESSOR_SUPPORTS_X86_KNC_EXTENSION) && defined(YEP_INTEL_COMPILER)
	#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
	#endif
#elif defined(YEP_IA64_CPU)
	#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
	#endif
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#if defined(__FP_FAST_FMA)
		#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#endif
	#endif
	#if defined(__FP_FAST_FMAF)
		#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#endif
	#endif
#elif defined(YEP_CUDA_GPU) && (__CUDA_ARCH__ >= 200)
	#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
	#endif
	#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
	#endif
#elif defined(YEP_OPENCL_DEVICE)
	#if defined(FP_FAST_FMA)
		#ifndef YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_DOUBLE_PRECISION_FMA_INSTRUCTIONS
		#endif
	#endif
	#if defined(FP_FAST_FMAF)
		#ifndef YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
			#define YEP_PROCESSOR_SUPPORTS_SINGLE_PRECISION_FMA_INSTRUCTIONS
		#endif
	#endif
#endif
 
#if defined(YEP_WINDOWS_OS)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(__LITTLE_ENDIAN__)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(__BIG_ENDIAN__)
	#define YEP_BIG_ENDIAN_BYTE_ORDER
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) && defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
	#define YEP_BIG_ENDIAN_BYTE_ORDER
#elif defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN__)
	#define YEP_BIG_ENDIAN_BYTE_ORDER
#elif defined(YEP_X86_CPU)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#elif (defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)) && defined(__ANDROID__)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#elif defined(YEP_CUDA_GPU)
	#define YEP_LITTLE_ENDIAN_BYTE_ORDER
#endif
 
#if defined(YEP_X64_ABI)
	#if defined(YEP_WINDOWS_OS)
		#define YEP_MICROSOFT_X64_ABI
	#elif defined(__KNC__)
		#define YEP_K1OM_X64_ABI
	#else
		#define YEP_SYSTEMV_X64_ABI
	#endif
#elif defined(YEP_ARM_ABI)
	#if defined(__ARM_EABI__)
		#define YEP_EABI_ARM_ABI
		#if defined(__ARM_PCS_VFP)
			#define YEP_HARDEABI_ARM_ABI
		#else
			#define YEP_SOFTEABI_ARM_ABI
		#endif
	#endif
#elif defined(YEP_MIPS32_ABI)
	#if defined(_ABIO32)
		#define YEP_O32_MIPS_ABI
		#if defined(__mips_hard_float)
			#define YEP_HARDO32_MIPS_ABI
		#endif
	#endif
#endif

#if defined(YEP_INTEL_COMPILER) || defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER) || defined(YEP_ARM_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	/* These compilers support hex floats even in C++ */
	#define YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
	/* C99 standard mandates support for hex floats */
	#define YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
	#define YEP_NULL_POINTER nullptr
#else
	#if defined(YEP_GNU_COMPILER) || defined(YEP_CLANG_COMPILER)
		#define YEP_NULL_POINTER __null
	#elif defined(__cplusplus)
		#define YEP_NULL_POINTER 0
	#else
		#define YEP_NULL_POINTER ((void*)0)
	#endif
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_RESTRICT __restrict
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#define YEP_RESTRICT __restrict__
#elif defined(YEP_ARM_COMPILER)
	#define YEP_RESTRICT __restrict__
#elif defined(YEP_NVIDIA_COMPILER)
	#define YEP_RESTRICT __restrict__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
	#define YEP_RESTRICT restrict
#else
	#define YEP_RESTRICT
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_NOINLINE __declspec(noinline)
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER)
	#define YEP_NOINLINE __attribute__((noinline))
#elif defined(YEP_NVIDIA_COMPILER)
	#define YEP_NOINLINE __noinline__
#else
	#define YEP_NOINLINE
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_INLINE __forceinline
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER)
	#define YEP_INLINE __attribute__((always_inline)) inline
#elif defined(YEP_NVIDIA_COMPILER)
	#define YEP_INLINE __forceinline__
#elif defined(__cplusplus) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
	#define YEP_INLINE inline
#else
	#define YEP_INLINE
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_NORETURN __declspec(noreturn)
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER) || defined(YEP_NVIDIA_COMPILER)
	#define YEP_NORETURN __attribute__((noreturn))
#else
	#define YEP_NORETURN
#endif

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_ALIGN(bytes) __declspec(align(bytes))
#elif defined(YEP_GCC_COMPATIBLE_COMPILER) || defined(YEP_ARM_COMPILER)
	#define YEP_ALIGN(bytes) __attribute__((aligned(bytes)))
#else
	/* Do nothing. Let the use of YEP_ALIGN generate compiler error. */
#endif

#define YEP_COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

#if defined(YEP_MSVC_COMPATIBLE_COMPILER)
	#define YEP_ALIGN_OF(type) __alignof(type)
#elif defined(YEP_GCC_COMPATIBLE_COMPILER)
	#define YEP_ALIGN_OF(type) __alignof__(type)
#elif defined(YE_ARM_COMPILER)
	#define YEP_ALIGN_OF(type) __ALIGNOF__(type)
#else
	/* Do nothing. Let the use of YEP_ALIGN_OF generate compiler error. */
#endif

#if defined(YEP_LINUX_OS)
	#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("internal")))
	#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
	#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("default")))
	#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("default")))
#elif defined(YEP_MACOSX_OS)
	#define YEP_PRIVATE_SYMBOL __attribute__((visibility ("hidden")))
	#define YEP_LOCAL_SYMBOL   __attribute__((visibility ("hidden")))
	#define YEP_EXPORT_SYMBOL  __attribute__((visibility ("default")))
	#define YEP_IMPORT_SYMBOL  __attribute__((visibility ("default")))
#elif defined(YEP_WINDOWS_OS)
	#define YEP_PRIVATE_SYMBOL
	#define YEP_LOCAL_SYMBOL
	#define YEP_EXPORT_SYMBOL __declspec(dllexport)
	#define YEP_IMPORT_SYMBOL __declspec(dllimport)
#else
	#define YEP_PRIVATE_SYMBOL
	#define YEP_LOCAL_SYMBOL
	#define YEP_EXPORT_SYMBOL
	#define YEP_IMPORT_SYMBOL
#endif
#define YEP_PUBLIC_SYMBOL YEP_IMPORT_SYMBOL

#if defined(YEP_NVIDIA_COMPILER)
	#define YEP_NATIVE_FUNCTION __device__
#else
	#define YEP_NATIVE_FUNCTION
#endif

#if defined(YEP_X86_ABI) && (defined(YEP_MSVC_COMPATIBLE_COMPILER))
	#define YEPABI __cdecl
#elif defined(YEP_X86_ABI) && (defined(YEP_GCC_COMPATIBLE_COMPILER))
	#define YEPABI __attribute__((cdecl))
#else
	#define YEPABI
#endif

#if defined(YEP_GCC_COMPATIBLE_COMPILER)
	#define YEP_LIKELY(x) (__builtin_expect(!!(x), 1))
	#define YEP_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
	#define YEP_LIKELY(x) (!!(x))
	#define YEP_UNLIKELY(x) (!!(x))
#endif
