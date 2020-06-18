/*
 *                      Yeppp! library implementation
 *
 * This file is part of Yeppp! library and licensed under 2-clause BSD license.
 * See LICENSE.txt for details.
 *
 */

#include <jni.h>
#include <yepPredefines.h>
#include <yepTypes.h>

extern YEP_PRIVATE_SYMBOL jclass RuntimeException;
extern YEP_PRIVATE_SYMBOL jclass SystemException;
extern YEP_PRIVATE_SYMBOL jclass UnsupportedHardwareException;
extern YEP_PRIVATE_SYMBOL jclass UnsupportedSoftwareException;
extern YEP_PRIVATE_SYMBOL jclass NullPointerException;
extern YEP_PRIVATE_SYMBOL jclass MisalignedPointerError;
extern YEP_PRIVATE_SYMBOL jclass IndexOutOfBoundsException;
extern YEP_PRIVATE_SYMBOL jclass IllegalArgumentException;
extern YEP_PRIVATE_SYMBOL jclass IllegalStateException;
extern YEP_PRIVATE_SYMBOL jclass NegativeArraySizeException;
extern YEP_PRIVATE_SYMBOL jclass OutOfMemoryError;

extern YEP_PRIVATE_SYMBOL void yepJNI_ThrowSpecificException(JNIEnv *env, enum YepStatus errorStatus, jclass exceptionClass);
extern YEP_PRIVATE_SYMBOL void yepJNI_ThrowSuitableException(JNIEnv *env, enum YepStatus errorStatus);
