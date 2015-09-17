LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libavcodec-56
LOCAL_SRC_FILES := lib/libavcodec-56.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libavdevice-56
LOCAL_SRC_FILES := lib/libavdevice-56.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libavfilter-5
LOCAL_SRC_FILES := lib/libavfilter-5.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libavformat-56
LOCAL_SRC_FILES := lib/libavformat-56.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libavutil-54
LOCAL_SRC_FILES := lib/libavutil-54.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libswscale-3
LOCAL_SRC_FILES := lib/libswscale-3.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libswresample-1
LOCAL_SRC_FILES := lib/libswresample-1.so
LOCAL_PRELINK_MODULE := true
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_MODULE := VideoPlay

LOCAL_SRC_FILES := src/Main.cpp \
	src/VideoPlay.cpp \
	src/JNILoad.cpp

LOCAL_SHARED_LIBRARIES := libavcodec-56\
	libavdevice-56 \
	libavfilter-5 \
	libavformat-56 \
	libavutil-54 \
	libswresample-1\
	libswscale-3
LOCAL_LDLIBS := -llog -landroid -lm\
	$(LOCAL_PATH)/lib/libavcodec-56.so \
	$(LOCAL_PATH)/lib/libavdevice-56.so \
	$(LOCAL_PATH)/lib/libavfilter-5.so \
	$(LOCAL_PATH)/lib/libavformat-56.so \
	$(LOCAL_PATH)/lib/libavutil-54.so \
	$(LOCAL_PATH)/lib/libswresample-1.so \
	$(LOCAL_PATH)/lib/libswscale-3.so 
include $(BUILD_SHARED_LIBRARY)
