/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <yepPredefines.h>
#include <yepTypes.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <yepVersion.h>

#include <yepCore.h>
#include <core/functions.h>
#include <math/functions.h>
#include <library/functions.h>

YepStatus YEPABI yepLibrary_Init() {
	YepStatus status = _yepLibrary_InitCpuInfo();
	if YEP_UNLIKELY(status != YepStatusOk) {
		return status;
	}
	
	status = _yepCore_Init();
	if YEP_UNLIKELY(status != YepStatusOk) {
		return status;
	}
	status = _yepMath_Init();
	if YEP_UNLIKELY(status != YepStatusOk) {
		return status;
	}

	return YepStatusOk;
}

FunctionPointer _yepLibrary_InitFunction(const FunctionDescriptor<YepStatus (*)()>* dispatchTable) {
	typedef FunctionDescriptor<YepStatus (*)()> GeneralizedFunctionDescriptor;
	const Yep64u unsupportedIsaFeatures = ~_isaFeatures;
	const Yep64u unsupportedSimdFeatures = ~_simdFeatures;
	const Yep64u unsupportedSystemFeatures = ~_systemFeatures;
	const GeneralizedFunctionDescriptor* startFunction = dispatchTable;
	const GeneralizedFunctionDescriptor* endFunction = startFunction;

	YepBoolean defaultImplementation;
	do {
		const YepBoolean defaultMicroarchitecture = (endFunction->microarchitecture == YepCpuMicroarchitectureUnknown);
		const YepBoolean defaultIsaFeatures = (endFunction->isaFeatures == YepIsaFeaturesDefault);
		const YepBoolean defaultSimdFeatures = (endFunction->simdFeatures == YepSimdFeaturesDefault);
		const YepBoolean defaultSystemFeatures = (endFunction->systemFeatures == YepSystemFeaturesDefault);
		defaultImplementation = defaultMicroarchitecture && defaultIsaFeatures && defaultSimdFeatures && defaultSystemFeatures;
		endFunction += 1;
	} while (!defaultImplementation);
	
	for (const YepCpuMicroarchitecture *targetMicroarchitecturePointer = _dispatchList; ; targetMicroarchitecturePointer++) {
		const YepCpuMicroarchitecture targetMicroarchitecture = *targetMicroarchitecturePointer;
		for (const GeneralizedFunctionDescriptor* currentFunction = startFunction; currentFunction != endFunction; currentFunction++) {
			if YEP_UNLIKELY(currentFunction->microarchitecture == targetMicroarchitecture) {
				const YepBoolean hasSupportedIsaFeatures = ((currentFunction->isaFeatures & unsupportedIsaFeatures) == 0);
				const YepBoolean hasSupportedSimdFeatures = ((currentFunction->simdFeatures & unsupportedSimdFeatures) == 0);
				const YepBoolean hasSupportedSystemFeatures = ((currentFunction->systemFeatures & unsupportedSystemFeatures) == 0);
				const YepBoolean hasSupportedFeatures = hasSupportedIsaFeatures && hasSupportedSimdFeatures && hasSupportedSystemFeatures;
				if (hasSupportedFeatures) {
					return currentFunction->function;
				}
			}
		}
	}
}

YepStatus YEPABI yepLibrary_Release() {
	return YepStatusOk;
}

static const YepLibraryVersion _version = { YEP_MAJOR_VERSION, YEP_MINOR_VERSION, YEP_PATCH_VERSION, YEP_BUILD_VERSION, YEP_RELEASE_NAME };

const YepLibraryVersion* YEPABI yepLibrary_GetVersion() {
	return &_version;
}

#if defined(YEP_WINDOWS_OS) && !defined(YEP_STATIC_LIBRARY)
	#include <windows.h>

	extern "C" BOOL WINAPI _DllMainCRTStartup(HINSTANCE instance, DWORD reason, LPVOID) {
		return TRUE;
	} 
#endif

#if defined(YEP_LINUX_OS)
	#if defined(YEP_ARM_CPU)
		asm (
			".section .version, \"S\",%progbits\n"
			".string \"" YEP_FULL_VERSION_STR "\"\n"
		);
	#else
		asm (
			".section .version, \"S\",@progbits\n"
			".string \"" YEP_FULL_VERSION_STR "\"\n"
		);
	#endif
#endif
