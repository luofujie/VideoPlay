LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_MODULE := VideoPlay
LOCAL_SRC_FILES := Main.cpp \
	VideoPlay.cpp \
	JNILoad.cpp

LOCAL_SHARED_LIBRARIES := avcodec\
	avdevice \
	avfilter \
	avformat \
	avutil \
	swresample \
	swscale
LOCAL_LDLIBS := -llog -landroid
include $(BUILD_SHARED_LIBRARY)