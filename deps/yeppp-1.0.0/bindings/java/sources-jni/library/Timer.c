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

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getTimerTicks(JNIEnv *env, jclass class) {
	Yep64u ticks = 0ull;
	enum YepStatus status;
	status = yepLibrary_GetTimerTicks(&ticks);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return ticks;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getTimerFrequency(JNIEnv *env, jclass class) {
	Yep64u frequency = 0ull;
	enum YepStatus status;
	status = yepLibrary_GetTimerFrequency(&frequency);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return frequency;
}

JNIEXPORT jlong JNICALL Java_info_yeppp_Library_getTimerAccuracy(JNIEnv *env, jclass class) {
	Yep64u accuracy = 0ull;
	enum YepStatus status;
	status = yepLibrary_GetTimerAccuracy(&accuracy);
	if (status != YepStatusOk) {
		yepJNI_ThrowSuitableException(env, status);
	}
	return accuracy;
}
