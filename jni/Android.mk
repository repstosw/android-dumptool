LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dumptool
LOCAL_SRC_FILES := dumptool.c  util.c 
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := dumpmem
LOCAL_SRC_FILES := dumpmem.c  util.c 
include $(BUILD_EXECUTABLE)
