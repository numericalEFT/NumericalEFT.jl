/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under the New BSD license.
 * See LICENSE.txt for the full text of the license.
 */

#include <jni.h>
#include <yepPrivate.h>
#include <yepLibrary.h>
#include <yepVersion.h>
#include <yepJavaPrivate.h>

YEP_PRIVATE_SYMBOL jclass RuntimeException = NULL;
YEP_PRIVATE_SYMBOL jclass SystemException = NULL;
YEP_PRIVATE_SYMBOL jclass UnsupportedHardwareException = NULL;
YEP_PRIVATE_SYMBOL jclass UnsupportedSoftwareException = NULL;
YEP_PRIVATE_SYMBOL jclass NullPointerException = NULL;
YEP_PRIVATE_SYMBOL jclass MisalignedPointerError = NULL;
YEP_PRIVATE_SYMBOL jclass IndexOutOfBoundsException = NULL;
YEP_PRIVATE_SYMBOL jclass IllegalArgumentException = NULL;
YEP_PRIVATE_SYMBOL jclass IllegalStateException = NULL;
YEP_PRIVATE_SYMBOL jclass NegativeArraySizeException = NULL;
YEP_PRIVATE_SYMBOL jclass OutOfMemoryError = NULL;

static void cleanup(JNIEnv *env) {
	if (RuntimeException) {
		(*env)->DeleteGlobalRef(env, RuntimeException);
		RuntimeException = NULL;
	}
	if (SystemException) {
		(*env)->DeleteGlobalRef(env, SystemException);
		SystemException = NULL;
	}
	if (UnsupportedHardwareException) {
		(*env)->DeleteGlobalRef(env, UnsupportedHardwareException);
		UnsupportedHardwareException = NULL;
	}
	if (UnsupportedSoftwareException) {
		(*env)->DeleteGlobalRef(env, UnsupportedSoftwareException);
		UnsupportedSoftwareException = NULL;
	}
	if (NullPointerException) {
		(*env)->DeleteGlobalRef(env, NullPointerException);
		NullPointerException = NULL;
	}
	if (MisalignedPointerError) {
		(*env)->DeleteGlobalRef(env, MisalignedPointerError);
		MisalignedPointerError = NULL;
	}
	if (IndexOutOfBoundsException) {
		(*env)->DeleteGlobalRef(env, IndexOutOfBoundsException);
		IndexOutOfBoundsException = NULL;
	}
	if (IllegalArgumentException) {
		(*env)->DeleteGlobalRef(env, IllegalArgumentException);
		IllegalArgumentException = NULL;
	}
	if (IllegalStateException) {
		(*env)->DeleteGlobalRef(env, IllegalStateException);
		IllegalStateException = NULL;
	}
	if (NegativeArraySizeException) {
		(*env)->DeleteGlobalRef(env, NegativeArraySizeException);
		NegativeArraySizeException = NULL;
	}
	if (OutOfMemoryError) {
		(*env)->DeleteGlobalRef(env, OutOfMemoryError);
		OutOfMemoryError = NULL;
	}
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	JNIEnv *env;
	enum YepStatus status;
	jint jnicallResult;
	jclass localRuntimeException = NULL;
	jclass localSystemException = NULL;
	jclass localUnsupportedHardwareException = NULL;
	jclass localUnsupportedSoftwareException = NULL;
	jclass localNullPointerException = NULL;
	jclass localMisalignedPointerError = NULL;
	jclass localIllegalArgumentException = NULL;
	jclass localIllegalStateException = NULL;
	jclass localNegativeArraySizeException = NULL;
	jclass localIndexOutOfBoundsException = NULL;
	jclass localOutOfMemoryError = NULL;

	jnicallResult = (*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6);
	if (jnicallResult != JNI_OK) {
		return JNI_ERR;
	}

	localRuntimeException = (*env)->FindClass(env, "java/lang/RuntimeException");
	if (localRuntimeException) {
		RuntimeException = (jclass) (*env)->NewGlobalRef(env, localRuntimeException);
		(*env)->DeleteLocalRef(env, localRuntimeException);
		if (RuntimeException == NULL) goto errorHandler;
	} else goto errorHandler;

	localSystemException = (*env)->FindClass(env, "info/yeppp/SystemException");
	if (localSystemException) {
		SystemException = (jclass) (*env)->NewGlobalRef(env, localSystemException);
		(*env)->DeleteLocalRef(env, localSystemException);
		if (SystemException == NULL) goto errorHandler;
	} else goto errorHandler;

	localUnsupportedHardwareException = (*env)->FindClass(env, "info/yeppp/UnsupportedHardwareException");
	if (localUnsupportedHardwareException) {
		UnsupportedHardwareException = (jclass) (*env)->NewGlobalRef(env, localUnsupportedHardwareException);
		(*env)->DeleteLocalRef(env, localUnsupportedHardwareException);
		if (UnsupportedHardwareException == NULL) goto errorHandler;
	} else goto errorHandler;

	localUnsupportedSoftwareException = (*env)->FindClass(env, "info/yeppp/UnsupportedSoftwareException");
	if (localUnsupportedSoftwareException) {
		UnsupportedSoftwareException = (jclass) (*env)->NewGlobalRef(env, localUnsupportedSoftwareException);
		(*env)->DeleteLocalRef(env, localUnsupportedSoftwareException);
		if (UnsupportedSoftwareException == NULL) goto errorHandler;
	} else goto errorHandler;

	localNullPointerException = (*env)->FindClass(env, "java/lang/NullPointerException");
	if (localNullPointerException) {
		NullPointerException = (jclass) (*env)->NewGlobalRef(env, localNullPointerException);
		(*env)->DeleteLocalRef(env, localNullPointerException);
		if (NullPointerException == NULL) goto errorHandler;
	} else goto errorHandler;

	localMisalignedPointerError = (*env)->FindClass(env, "info/yeppp/MisalignedPointerError");
	if (localMisalignedPointerError) {
		MisalignedPointerError = (jclass) (*env)->NewGlobalRef(env, localMisalignedPointerError);
		(*env)->DeleteLocalRef(env, localMisalignedPointerError);
		if (MisalignedPointerError == NULL) goto errorHandler;
	} else goto errorHandler;

	localIndexOutOfBoundsException = (*env)->FindClass(env, "java/lang/IndexOutOfBoundsException");
	if (localIndexOutOfBoundsException) {
		IndexOutOfBoundsException = (jclass) (*env)->NewGlobalRef(env, localIndexOutOfBoundsException);
		(*env)->DeleteLocalRef(env, localIndexOutOfBoundsException);
		if (IndexOutOfBoundsException == NULL) goto errorHandler;
	} else goto errorHandler;

	localIllegalArgumentException = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
	if (localIllegalArgumentException) {
		IllegalArgumentException = (jclass) (*env)->NewGlobalRef(env, localIllegalArgumentException);
		(*env)->DeleteLocalRef(env, localIllegalArgumentException);
		if (IllegalArgumentException == NULL) goto errorHandler;
	} else goto errorHandler;

	localIllegalStateException = (*env)->FindClass(env, "java/lang/IllegalStateException");
	if (localIllegalStateException) {
		IllegalStateException = (jclass) (*env)->NewGlobalRef(env, localIllegalStateException);
		(*env)->DeleteLocalRef(env, localIllegalStateException);
		if (IllegalStateException == NULL) goto errorHandler;
	} else goto errorHandler;

	localNegativeArraySizeException = (*env)->FindClass(env, "java/lang/NegativeArraySizeException");
	if (localNegativeArraySizeException) {
		NegativeArraySizeException = (jclass) (*env)->NewGlobalRef(env, localNegativeArraySizeException);
		(*env)->DeleteLocalRef(env, localNegativeArraySizeException);
		if (NegativeArraySizeException == NULL) goto errorHandler;
	} else goto errorHandler;

	localOutOfMemoryError = (*env)->FindClass(env, "java/lang/OutOfMemoryError");
	if (localOutOfMemoryError) {
		OutOfMemoryError = (jclass) (*env)->NewGlobalRef(env, localOutOfMemoryError);
		(*env)->DeleteLocalRef(env, localOutOfMemoryError);
		if (OutOfMemoryError == NULL) goto errorHandler;
	} else goto errorHandler;

	status = yepLibrary_Init();
	if (status == YepStatusOk) {
		return JNI_VERSION_1_4;
	} else goto errorHandler;

errorHandler:
	cleanup(env);
	return JNI_ERR;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved) {
	JNIEnv *env;
	jint jnicallResult;

	yepLibrary_Release();

	jnicallResult = (*jvm)->GetEnv(jvm, (void**)&env, JNI_VERSION_1_6);
	if (jnicallResult != JNI_OK) {
		return;
	}

	cleanup(env);
}
