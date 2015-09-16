#ifndef STDAFX_H_
#define STDAFX_H_

#ifndef INT64_C
	#define INT64_C
	#define UINT64_C
#endif

#include <jni.h>
#include <android/log.h>
#define LOG_TAG "JNI_LOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


#endif /* STDAFX_H_ */
