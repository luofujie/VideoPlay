#include <jni.h>
#include "Stdafx.h"
#include "JNILoad.h"

extern "C"
{
	void Test(JNIEnv* env,jobject obj)
	{
		LOGI("Test JNI");
	}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/VideoPlayNative";
static JNINativeMethod VideoPlayMethods[] =
{
		{ "Test", "()V", (void*) Test }
};
int RegisterVideoPlayMethods(JNIEnv *env)
{
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
            sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

