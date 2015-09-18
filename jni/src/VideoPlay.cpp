#include <jni.h>
#include "Stdafx.h"
#include "JNILoad.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
int add(int x,int y);
extern "C"
{

	void Test(JNIEnv* env,jobject obj)
	{
		LOGI("Test JNI");
	}
	int Init(JNIEnv* env,jobject obj)
	{
		av_register_all();
		return 0;
	}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/VideoPlayNative";
static JNINativeMethod VideoPlayMethods[] =
{
		{ "Test", "()V", (void*) Test },
		{ "Init", "()I", (void*) Init }
};
int RegisterVideoPlayMethods(JNIEnv *env)
{
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
            sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

