/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <jni.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <yepJavaPrivate.h>

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getCpuIsaFeatures(JNIEnv *env, jclass class) {
	Yep64u isaFeatures = YepIsaFeaturesDefault;
	enum YepStatus status;
	status = yepLibrary_GetCpuIsaFeatures(&isaFeatures);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return isaFeatures;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getCpuSimdFeatures(JNIEnv *env, jclass class) {
	Yep64u simdFeatures = YepSimdFeaturesDefault;
	enum YepStatus status;
	status = yepLibrary_GetCpuSimdFeatures(&simdFeatures);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return simdFeatures;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getCpuSystemFeatures(JNIEnv *env, jclass class) {
	Yep64u systemFeatures = YepSystemFeaturesDefault;
	enum YepStatus status;
	status = yepLibrary_GetCpuSystemFeatures(&systemFeatures);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return systemFeatures;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuVendorId(JNIEnv *env, jclass class) {
	enum YepCpuVendor vendor = YepCpuVendorUnknown;
	enum YepStatus status;
	status = yepLibrary_GetCpuVendor(&vendor);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return vendor;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuArchitectureId(JNIEnv *env, jclass class) {
	enum YepCpuArchitecture architecture = YepCpuArchitectureUnknown;
	enum YepStatus status;
	status = yepLibrary_GetCpuArchitecture(&architecture);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return architecture;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuMicroarchitectureId(JNIEnv *env, jclass class) {
	enum YepCpuMicroarchitecture microarchitecture = YepCpuMicroarchitectureUnknown;
	enum YepStatus status;
	status = yepLibrary_GetCpuMicroarchitecture(&microarchitecture);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return microarchitecture;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getCpuCyclesAcquire(JNIEnv *env, jclass class) {
	Yep64u state = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuCyclesAcquire(&state);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return state;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getCpuCyclesRelease(JNIEnv *env, jclass class, jlong jstate) {
	Yep64u state = jstate;
	Yep64u cycles = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuCyclesRelease(&state, &cycles);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cycles;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuLogicalCoresCount(JNIEnv *env, jclass class) {
	Yep32u logicalCores = 0;
	enum YepStatus status;

	status = yepLibrary_GetLogicalCoresCount(&logicalCores);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return logicalCores;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL0ICacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuInstructionCacheSize(0, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL0DCacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuDataCacheSize(0, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL1ICacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuInstructionCacheSize(1, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL1DCacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuDataCacheSize(1, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL2CacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuDataCacheSize(2, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}

JNIEXPORT jint JNICALL Java_info_yeppp_Library_getCpuL3CacheSize(JNIEnv *env, jclass class) {
	Yep32u cacheSize = 0;
	enum YepStatus status;

	status = yepLibrary_GetCpuDataCacheSize(3, &cacheSize);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return cacheSize;
}
