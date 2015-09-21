#include <jni.h>
#include "Stdafx.h"
#include "JNILoad.h"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
}


int add(int x, int y);
extern "C" {
void Test(JNIEnv* env, jobject obj) {
	LOGI("Test JNI");
}
int Init(JNIEnv* env, jobject obj) {
	LOGI("Init");
	av_register_all();
	int version = avcodec_version();
	LOGI("version=%d",version);
	return 0;
}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/VideoPlayNative";
static JNINativeMethod VideoPlayMethods[] = { { "Test", "()V", (void*) Test }, {
		"Init", "()I", (void*) Init } };
int RegisterVideoPlayMethods(JNIEnv *env) {
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
			sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

