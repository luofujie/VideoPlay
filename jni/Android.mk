LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_MODULE    := VideoPlay

LOCAL_SRC_FILES := src/Main.cpp \
	src/VideoPlay.cpp
LOCAL_LDLIBS := -llog -landroid 
include $(BUILD_SHARED_LIBRARY)
