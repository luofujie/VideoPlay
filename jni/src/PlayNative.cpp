#include <jni.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "Stdafx.h"
#include "JNILoad.h"
ANativeWindow* gPlayView;
extern "C"
{
	void Test(JNIEnv* env, jobject obj)
	{
		LOGI("Test JNI");
	}
	int Init(JNIEnv* env, jobject obj, jobject surface)
	{
		gPlayView = ANativeWindow_fromSurface(env, surface);
		if(gPlayView == NULL)
		{
			LOGE("Get ANativeWindow ERROR!");
			return -1;
		}
		LOGI("init ANativeWindow success!");
		return 0;
	}
	void Destroye()
	{
		ANativeWindow_release(gPlayView);
		gPlayView = NULL;
	}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/PlayNative";
static JNINativeMethod VideoPlayMethods[] = {
		{ "Test", "()V", (void*) Test },
		{"Init", "(Landroid/view/Surface;)I", (void*) Init },
		{ "Destroye", "()V", (void*) Destroye }
	};
int RegisterVideoPlayMethods(JNIEnv *env) {
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
			sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

