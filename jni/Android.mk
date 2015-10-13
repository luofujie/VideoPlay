LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)  
LOCAL_MODULE := ffmpeg  
LOCAL_SRC_FILES := lib/libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)  

include $(CLEAR_VARS)
LOCAL_MODULE := VideoPlay

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := src/Main.cpp \
	src/PlayNative.cpp \
	src/JNILoad.cpp \
	src/VideoPlay.cpp \
	src/JNIUtil.cpp \
	src/AudioPlay.cpp
	
LOCAL_SHARED_LIBRARIES := ffmpeg
LOCAL_LDLIBS := -llog -landroid -lOpenSLES 
include $(BUILD_SHARED_LIBRARY)
