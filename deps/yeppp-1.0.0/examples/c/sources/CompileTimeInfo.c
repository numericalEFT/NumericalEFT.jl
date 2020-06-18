#include <stdio.h>
#include <yepPredefines.h>

int main(int argc, char **argv) {
	#if defined(YEP_X86_CPU)
		#if defined(YEP_X86_ABI)
			printf("Target architecture: x86 (standard x86 ABI)\n");
		#elif defined(YEP_MICROSOFT_X64_ABI)
			printf("Target architecture: x86 (Microsoft x64 ABI)\n");
		#elif defined(YEP_SYSTEMV_X64_ABI)
			printf("Target architecture: x86 (System V x86-64 ABI)\n");
		#elif defined(YEP_K1OM_X64_ABI)
			printf("Target architecture: x86 (K1OM x86-64 ABI)\n");
		#elif defined(YEP_X64_ABI)
			printf("Target architecture: x86 (unknown variant of x86-64 ABI)\n");
		#else
			printf("Target architecture: x86 (unknown ABI)\n");
		#endif
	#elif defined(YEP_ARM_CPU)
		#if defined(YEP_HARDEABI_ARM_ABI)
			printf("Target architecture: ARM (Hard-Float EABI)\n");
		#elif defined(YEP_SOFTEABI_ARM_ABI)
			printf("Target architecture: ARM (Soft-Float EABI)\n");
		#elif defined(YEP_EABI_ARM_ABI)
			printf("Target architecture: ARM (unknown variant of EABI)\n");
		#else
			printf("Target architecture: ARM (unknown ABI)\n");
		#endif
	#elif defined(YEP_MIPS_CPU)
		#if defined(YEP_HARDO32_MIPS_ABI)
			printf("Target architecture: MIPS (Hard-Float O32 ABI)\n");
		#elif defined(YEP_O32_MIPS_ABI)
			printf("Target architecture: MIPS (unknown variant of O32 ABI)\n");
		#elif defined(YEP_MIPS32_ABI)
			printf("Target architecture: MIPS (unknown variant of MIPS32 ABI)\n");
		#else
			printf("Target architecture: MIPS (unknown ABI)\n");
		#endif
	#elif defined(YEP_IA64_CPU)
		printf("Target architecture: IA64 (unknown ABI)\n");
	#elif defined(YEP_POWERPC_CPU)
		printf("Target architecture: PowerPC (unknown ABI)\n");
	#elif defined(YEP_SPARC_CPU)
		printf("Target architecture: SPARC (unknown ABI)\n");
	#else
		printf("Target architecture: unknown\n");
	#endif

	#if defined(YEP_LITTLE_ENDIAN_BYTE_ORDER)
		printf("Target endianness: little endian\n");
	#elif defined(YEP_BIG_ENDIAN_BYTE_ORDER)
		printf("Target endianness: big endian\n");
	#else
		printf("Target endianness: unknown\n");
	#endif

	#if defined(YEP_X86_CPU) || defined(YEP_X64_CPU)
		printf("Target processor suppport for x86/x86-64 ISA extensions:\n");
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_MMX_EXTENSION)
			printf("\tMMX:     Yes\n");
		#else
			printf("\tMMX:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_MMXPLUS_EXTENSION)
			printf("\tMMX+:    Yes\n");
		#else
			printf("\tMMX+:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_3DNOW_EXTENSION)
			printf("\t3dnow!:  Yes\n");
		#else
			printf("\t3dnow!:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_3DNOWPLUS_EXTENSION)
			printf("\t3dnow!+: Yes\n");
		#else
			printf("\t3dnow!+: No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE_EXTENSION)
			printf("\tSSE:     Yes\n");
		#else
			printf("\tSSE:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE2_EXTENSION)
			printf("\tSSE2:    Yes\n");
		#else
			printf("\tSSE2:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE3_EXTENSION)
			printf("\tSSE3:    Yes\n");
		#else
			printf("\tSSE3:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSSE3_EXTENSION)
			printf("\tSSSE3:   Yes\n");
		#else
			printf("\tSSSE3:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE4A_EXTENSION)
			printf("\tSSE4A:   Yes\n");
		#else
			printf("\tSSE4A:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE4_1_EXTENSION)
			printf("\tSSE4.1:  Yes\n");
		#else
			printf("\tSSE4.1:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_SSE4_2_EXTENSION)
			printf("\tSSE4.2:  Yes\n");
		#else
			printf("\tSSE4.2:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_AVX_EXTENSION)
			printf("\tAVX:     Yes\n");
		#else
			printf("\tAVX:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_AVX2_EXTENSION)
			printf("\tAVX2:    Yes\n");
		#else
			printf("\tAVX2:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_F16C_EXTENSION)
			printf("\tF16C:    Yes\n");
		#else
			printf("\tF16C:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_FMA4_EXTENSION)
			printf("\tFMA4:    Yes\n");
		#else
			printf("\tFMA4:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_FMA3_EXTENSION)
			printf("\tFMA3:    Yes\n");
		#else
			printf("\tFMA3:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_XOP_EXTENSION)
			printf("\tXOP:     Yes\n");
		#else
			printf("\tXOP:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_KNC_EXTENSION)
			printf("\tKNC:     Yes\n");
		#else
			printf("\tKNC:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_LZCNT_EXTENSION)
			printf("\tLZCNT:   Yes\n");
		#else
			printf("\tLZCNT:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_POPCNT_EXTENSION)
			printf("\tPOPCNT:  Yes\n");
		#else
			printf("\tPOPCNT:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_TBM_EXTENSION)
			printf("\tTBM:     Yes\n");
		#else
			printf("\tTBM:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION)
			printf("\tBMI:     Yes\n");
		#else
			printf("\tBMI:     No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_X86_BMI_EXTENSION)
			printf("\tBMI2:    Yes\n");
		#else
			printf("\tBMI2:    No\n");
		#endif
	#elif defined(YEP_ARM_CPU)
		printf("Target processor suppport for ARM ISA extensions:\n");
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V4_INSTRUCTIONS)
			printf("\tARMv4:    Yes\n");
		#else
			printf("\tARMv4:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V5_INSTRUCTIONS)
			printf("\tARMv5:    Yes\n");
		#else
			printf("\tARMv5:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V5E_INSTRUCTIONS)
			printf("\tARMv5E:   Yes\n");
		#else
			printf("\tARMv5E:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V6_INSTRUCTIONS)
			printf("\tARMv6:    Yes\n");
		#else
			printf("\tARMv6:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V6K_INSTRUCTIONS)
			printf("\tARMv6K:   Yes\n");
		#else
			printf("\tARMv6K:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_V7_INSTRUCTIONS)
			printf("\tARMv7:    Yes\n");
		#else
			printf("\tARMv7:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_DIV_EXTENSION)
			printf("\tDIV:      Yes\n");
		#else
			printf("\tDIV:      No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP_EXTENSION)
			printf("\tVFP:      Yes\n");
		#else
			printf("\tVFP:      No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP2_EXTENSION)
			printf("\tVFPv2:    Yes\n");
		#else
			printf("\tVFPv2:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP3_EXTENSION)
			printf("\tVFPv3:    Yes\n");
		#else
			printf("\tVFPv3:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP3_D32_EXTENSION)
			printf("\tVFP-D32:  Yes\n");
		#else
			printf("\tVFP-D32:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP3HP_EXTENSION)
			printf("\tVFP-FP16: Yes\n");
		#else
			printf("\tVFP-FP16: No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_VFP4_EXTENSION)
			printf("\tVFPv4:    Yes\n");
		#else
			printf("\tVFPv4:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_XSCALE_EXTENSION)
			printf("\tXScale:  Yes\n");
		#else
			printf("\tXScale:  No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_WMMX_EXTENSION)
			printf("\tWMMX:    Yes\n");
		#else
			printf("\tWMMX:    No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_WMMX2_EXTENSION)
			printf("\tWMMX2:   Yes\n");
		#else
			printf("\tWMMX2:   No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_ARM_NEON_EXTENSION)
			printf("\tNEON:    Yes\n");
		#else
			printf("\tNEON:    No\n");
		#endif
	#elif defined(YEP_MIPS_CPU)
		printf("Target processor suppport for MIPS ISA extensions:\n");
		#if defined(YEP_PROCESSOR_SUPPORTS_MIPS_R2_INSTRUCTIONS)
			printf("\tMIPS R2:       Yes\n");
		#else
			printf("\tMIPS R2:       No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_MIPS_3D_INSTRUCTIONS)
			printf("\tMIPS 3D:       Yes\n");
		#else
			printf("\tMIPS 3D:       No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_MIPS_PAIREDSINGLE_INSTRUCTIONS)
			printf("\tPaired Single: Yes\n");
		#else
			printf("\tPaired Single: No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_MIPS_DSP_INSTRUCTIONS)
			printf("\tMIPS DSP:      Yes\n");
		#else
			printf("\tMIPS DSP:      No\n");
		#endif
		#if defined(YEP_PROCESSOR_SUPPORTS_MIPS_DSP2_INSTRUCTIONS)
			printf("\tMIPS DSPr2:    Yes\n");
		#else
			printf("\tMIPS DSPr2:    No\n");
		#endif
	#endif

	#if defined(YEP_LINUX_OS)
		#if defined(YEP_GNU_LINUX_OS)
			printf("Target OS: GNU/Linux\n");
		#elif defined(YEP_ANDROID_LINUX_OS)
			printf("Target OS: Android\n");
		#else
			printf("Target OS: Linux-based (unknown userland)\n");
		#endif
	#elif defined(YEP_MACOSX_OS)
		printf("Target OS: Mac OS X\n");
	#elif defined(YEP_WINDOWS_OS)
		printf("Target OS: Windows\n");
	#else
		printf("Target OS: unknown\n");
	#endif

	#if defined(YEP_GNU_COMPILER)
		printf("Compiler: gcc\n");
	#elif defined(YEP_CLANG_COMPILER)
		printf("Compiler: clang\n");
	#elif defined(YEP_INTEL_COMPILER_FOR_UNIX)
		printf("Compiler: icc\n");
	#elif defined(YEP_ARM_COMPILER)
		printf("Compiler: armcc\n");
	#elif defined(YEP_IBM_COMPILER)
		printf("Compiler: xlc\n");
	#elif defined(YEP_PGI_COMPILER)
		printf("Compiler: pgcc\n");
	#elif defined(YEP_PATHSCALE_COMPILER)
		printf("Compiler: pathcc\n");
	#elif defined(YEP_MICROSOFT_COMPILER)
		printf("Compiler: cl.exe\n");
	#elif defined(YEP_ARM_COMPILER)
		printf("Compiler: armcc\n");
	#elif defined(YEP_EMBARCADERO_COMPILER)
		printf("Compiler: bcc.exe\n");
	#else
		printf("Compiler: unknown\n");
	#endif
	#if defined(YEP_COMPILER_SUPPORTS_HEXADECIMAL_FLOATING_POINT_CONSTANTS)
		printf("Compiler supports hexadecimal floating-point constants: Yes\n");
	#else
		printf("Compiler supports hexadecimal floating-point constants: No\n");
	#endif

	#if defined(YEP_PIC)
		printf("Position-independent code generation: Yes\n");
	#else
		printf("Position-independent code generation: No\n");
	#endif

	#if defined(YEP_X86_CPU) || defined(YEP_X64_CPU)
		printf("Compiler suppport for x86/x86-64 intrinsics:\n");
		#if defined(YEP_COMPILER_SUPPORTS_X86_MMX_EXTENSION)
			printf("\tMMX:     Yes\n");
		#else
			printf("\tMMX:     No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_MMXPLUS_EXTENSION)
			printf("\tMMX+:    Yes\n");
		#else
			printf("\tMMX+:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_3DNOW_EXTENSION)
			printf("\t3dnow!:  Yes\n");
		#else
			printf("\t3dnow!:  No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_3DNOWPLUS_EXTENSION)
			printf("\t3dnow!+: Yes\n");
		#else
			printf("\t3dnow!+: No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE_EXTENSION)
			printf("\tSSE:     Yes\n");
		#else
			printf("\tSSE:     No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE2_EXTENSION)
			printf("\tSSE2:    Yes\n");
		#else
			printf("\tSSE2:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE3_EXTENSION)
			printf("\tSSE3:    Yes\n");
		#else
			printf("\tSSE3:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSSE3_EXTENSION)
			printf("\tSSSE3:   Yes\n");
		#else
			printf("\tSSSE3:   No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE4A_EXTENSION)
			printf("\tSSE4A:   Yes\n");
		#else
			printf("\tSSE4A:   No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE4_1_EXTENSION)
			printf("\tSSE4.1:  Yes\n");
		#else
			printf("\tSSE4.1:  No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_SSE4_2_EXTENSION)
			printf("\tSSE4.2:  Yes\n");
		#else
			printf("\tSSE4.2:  No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_AVX_EXTENSION)
			printf("\tAVX:     Yes\n");
		#else
			printf("\tAVX:     No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_AVX2_EXTENSION)
			printf("\tAVX2:    Yes\n");
		#else
			printf("\tAVX2:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_F16C_EXTENSION)
			printf("\tF16C:    Yes\n");
		#else
			printf("\tF16C:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_FMA4_EXTENSION)
			printf("\tFMA4:    Yes\n");
		#else
			printf("\tFMA4:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_FMA3_EXTENSION)
			printf("\tFMA3:    Yes\n");
		#else
			printf("\tFMA3:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_XOP_EXTENSION)
			printf("\tXOP:     Yes\n");
		#else
			printf("\tXOP:     No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_X86_KNC_EXTENSION)
			printf("\tKNC:     Yes\n");
		#else
			printf("\tKNC:     No\n");
		#endif
	#elif defined(YEP_ARM_CPU)
		printf("Compiler suppport for ARM intrinsics:\n");
		#if defined(YEP_COMPILER_SUPPORTS_ARM_WMMX_EXTENSION)
			printf("\tWMMX:    Yes\n");
		#else
			printf("\tWMMX:    No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_ARM_WMMX2_EXTENSION)
			printf("\tWMMX2:   Yes\n");
		#else
			printf("\tWMMX2:   No\n");
		#endif
		#if defined(YEP_COMPILER_SUPPORTS_ARM_NEON_EXTENSION)
			printf("\tNEON:    Yes\n");
		#else
			printf("\tNEON:    No\n");
		#endif
	#endif

	return 0;
}
