# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dumptool
LOCAL_SRC_FILES := dumptool.c
include $(BUILD_EXECUTABLE)
