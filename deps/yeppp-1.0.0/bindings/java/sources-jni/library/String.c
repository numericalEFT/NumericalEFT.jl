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
#if defined(YEP_WINDOWS_OS)
	#include <windows.h>
#endif

#define BUFFER_SIZE 1024

static void numberToString(char* string, Yep32u number) {
	char buffer[16];
	char* cur;
#if defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
	Yep32u newNumber;
#endif
	if (number == 0) {
		*string++ = '0';
		*string = 0;
	} else {
		buffer[15] = '\0';
		cur = &buffer[15];
		while (number != 0) {
			#if defined(YEP_ARM_CPU) || defined(YEP_MIPS_CPU)
				newNumber = ((Yep64u)(number) * 3435973837u) >> 3;
				*(--cur) = '0' + (number - newNumber * 10u);
				number = newNumber;
			#else
				*(--cur) = '0' + (number % 10);
				number /= 10;
			#endif
		}
		while ((*string++ = *cur++));
	}
}

YEP_PRIVATE_SYMBOL void yepJNI_ThrowSpecificException(JNIEnv *env, enum YepStatus errorStatus, jclass exceptionClass) {
	enum YepStatus status;
	YepSize bufferLength = BUFFER_SIZE - 1;
	char onStackBuffer[BUFFER_SIZE];
	status = yepLibrary_GetString(YepEnumerationStatus, errorStatus, YepStringTypeDescription, onStackBuffer, &bufferLength);
	if (status == YepStatusOk) {
		onStackBuffer[bufferLength] = '\0';
		(*env)->ThrowNew(env, exceptionClass, onStackBuffer);
		return;
	} else if (status == YepStatusInsufficientBuffer) {
		/* Dynamically allocate memory for string buffer */
#if defined(YEP_WINDOWS_OS)
		HANDLE heap = GetProcessHeap();
		char *heapBuffer = HeapAlloc(heap, 0, bufferLength + 1);
		if (heapBuffer != NULL) {
			status = yepLibrary_GetString(YepEnumerationStatus, errorStatus, YepStringTypeDescription, heapBuffer, &bufferLength);
			if (status == YepStatusOk) {
				heapBuffer[bufferLength] = '\0';
				(*env)->ThrowNew(env, exceptionClass, heapBuffer);
				HeapFree(heap, 0, heapBuffer);
				return;
			}
		}
#endif
	}
	/* Everything failed, but we don't */

	#if BUFFER_SIZE < 32
		#error "BUFFER_SIZE must be at least 32 to avoid buffer overflow"
	#endif
	/* A bit of junk code to avoid linking to LibC */
	onStackBuffer[0] = 'Y';
	onStackBuffer[1] = 'e';
	onStackBuffer[2] = 'p';
	onStackBuffer[3] = 'p';
	onStackBuffer[4] = 'p';
	onStackBuffer[5] = '!';
	onStackBuffer[6] = ' ';
	onStackBuffer[7] = 'e';
	onStackBuffer[8] = 'r';
	onStackBuffer[9] = 'r';
	onStackBuffer[10] = 'o';
	onStackBuffer[11] = 'r';
	onStackBuffer[12] = ' ';
	onStackBuffer[13] = '#';
	numberToString(&onStackBuffer[14], errorStatus);

	(*env)->ThrowNew(env, exceptionClass, onStackBuffer);
}

YEP_PRIVATE_SYMBOL void yepJNI_ThrowSuitableException(JNIEnv *env, enum YepStatus errorStatus) {
	switch (errorStatus) {
		case YepStatusNullPointer:
			yepJNI_ThrowSpecificException(env, errorStatus, NullPointerException);
			break;
		case YepStatusMisalignedPointer:
			yepJNI_ThrowSpecificException(env, errorStatus, MisalignedPointerError);
			break;
		case YepStatusInvalidArgument:
			yepJNI_ThrowSpecificException(env, errorStatus, IllegalArgumentException);
			break;
		case YepStatusInvalidState:
			yepJNI_ThrowSpecificException(env, errorStatus, IllegalStateException);
			break;
		case YepStatusUnsupportedHardware:
			yepJNI_ThrowSpecificException(env, errorStatus, UnsupportedHardwareException);
			break;
		case YepStatusUnsupportedSoftware:
			yepJNI_ThrowSpecificException(env, errorStatus, UnsupportedSoftwareException);
			break;
		case YepStatusOutOfMemory:
			yepJNI_ThrowSpecificException(env, errorStatus, OutOfMemoryError);
			break;
		case YepStatusSystemError:
			yepJNI_ThrowSpecificException(env, errorStatus, SystemException);
			break;
		default:
			yepJNI_ThrowSpecificException(env, errorStatus, RuntimeException);
			break;
	}
}

static jstring getString(JNIEnv *env, const enum YepEnumeration enumeration, const Yep32u value, const enum YepStringType stringType) {
	enum YepStatus status;
	jstring string = NULL;
	YepSize bufferLength = BUFFER_SIZE - 1;
	char onStackBuffer[BUFFER_SIZE];
	status = yepLibrary_GetString(enumeration, value, stringType, onStackBuffer, &bufferLength);
	if (status == YepStatusOk) {
		onStackBuffer[bufferLength] = '\0';
		/* If not enough memory, will throw OutOfMemoryError and return NULL. Bypass it to JVM without any handling. */
		string = (*env)->NewStringUTF(env, onStackBuffer);
	} else if (status == YepStatusInsufficientBuffer) {
		/* Dynamically allocate memory for string buffer */
#if defined(YEP_WINDOWS_OS)
		HANDLE heap = GetProcessHeap();
		char *heapBuffer = HeapAlloc(heap, 0, bufferLength + 1);
		if (heapBuffer != NULL) {
			status = yepLibrary_GetString(enumeration, value, stringType, heapBuffer, &bufferLength);
			if (status == YepStatusOk) {
				heapBuffer[bufferLength] = '\0';
				/* If not enough memory, will throw OutOfMemoryError and return NULL. Pass it back to JVM without any handling. */
				string = (*env)->NewStringUTF(env, heapBuffer);
				HeapFree(heap, 0, heapBuffer);
			}
		} else {
			yepJNI_ThrowSpecificException(env, YepStatusOutOfMemory, OutOfMemoryError);
		}
#endif
	} else {
		yepJNI_ThrowSuitableException(env, status);
	}
	return string;
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuArchitecture_toString(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuArchitecture, id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuArchitecture_getDescription(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuArchitecture, id, YepStringTypeDescription);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuVendor_toString(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuVendor, id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuVendor_getDescription(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuVendor, id, YepStringTypeDescription);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuMicroarchitecture_toString(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuMicroarchitecture, id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuMicroarchitecture_getDescription(JNIEnv *env, jclass class, jint id) {
	return getString(env, YepEnumerationCpuMicroarchitecture, id, YepStringTypeDescription);
}

JNIEXPORT jboolean JNICALL Java_info_yeppp_CpuIsaFeature_isDefined(JNIEnv *env, jclass class, jint id, jint architectureId) {
	YepSize dummySize = 0;
	enum YepStatus status;

	status = yepLibrary_GetString(YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription, NULL, &dummySize);
	return (status == YepStatusInsufficientBuffer) ? JNI_TRUE: JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuIsaFeature_toString(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuIsaFeature_getDescription(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_ISA_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription);
}

JNIEXPORT jboolean JNICALL Java_info_yeppp_CpuSimdFeature_isDefined(JNIEnv *env, jclass class, jint id, jint architectureId) {
	YepSize dummySize = 0;
	enum YepStatus status;

	status = yepLibrary_GetString(YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription, NULL, &dummySize);
	return (status == YepStatusInsufficientBuffer) ? JNI_TRUE: JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuSimdFeature_toString(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuSimdFeature_getDescription(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_SIMD_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription);
}

JNIEXPORT jboolean JNICALL Java_info_yeppp_CpuSystemFeature_isDefined(JNIEnv *env, jclass class, jint id, jint architectureId) {
	YepSize dummySize = 0;
	enum YepStatus status;

	status = yepLibrary_GetString(YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription, NULL, &dummySize);
	return (status == YepStatusInsufficientBuffer) ? JNI_TRUE: JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuSystemFeature_toString(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeID);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_CpuSystemFeature_getDescription(JNIEnv *env, jclass class, jint id, jint architectureId) {
	return getString(env, YEP_ENUMERATION_SYSTEM_FEATURE_FOR_ARCHITECTURE(architectureId), id, YepStringTypeDescription);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_Library_getVersionInfo(JNIEnv *env, jclass class, jintArray versionNumberArray) {
	jint versionNumbers[4];
	const struct YepLibraryVersion *version = yepLibrary_GetVersion();

	versionNumbers[0] = version->major;
	versionNumbers[1] = version->minor;
	versionNumbers[2] = version->patch;
	versionNumbers[3] = version->build;
	(*env)->SetIntArrayRegion(env, versionNumberArray, 0, 4, versionNumbers);
	if ((*env)->ExceptionCheck(env) != JNI_FALSE) {
		return NULL;
	}
	
	return (*env)->NewStringUTF(env, version->releaseName);
}

JNIEXPORT jstring JNICALL Java_info_yeppp_Library_getCpuName(JNIEnv *env, jclass class) {
	return getString(env, YepEnumerationCpuFullName, 0, YepStringTypeDescription);
}
