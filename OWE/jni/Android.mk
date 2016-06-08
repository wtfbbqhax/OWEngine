LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS    := -ldl -llog
LOCAL_MODULE    := oweloader
LOCAL_SRC_FILES := owe.c

include $(BUILD_SHARED_LIBRARY)
