#                    Yeppp! library implementation
#
# This file is part of Yeppp! library and licensed under the New BSD license.
# See LICENSE.txt for the full text of the license.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := yeppp
LOCAL_SRC_FILES := $(LOCAL_PATH)/../$(TARGET_ARCH_ABI)/libyeppp.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../library/headers
include $(PREBUILT_SHARED_LIBRARY)
