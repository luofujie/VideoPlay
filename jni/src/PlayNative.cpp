#include "JNILoad.h"
#include "JNIUtil.h"
extern "C"
{
	void Test(JNIEnv* env, jobject obj)
	{
		LOGI("Test JNI");
	}
	int Init(JNIEnv* env, jobject obj, jobject surface)
	{
		return 0;
	}
	void Destroye(JNIEnv* env, jobject obj)
	{
	}
	int Open(JNIEnv* env, jobject obj,jstring path)
	{
		return 0;
	}
	void Play(JNIEnv* env, jobject obj)
	{
	}
	void Pause(JNIEnv* env, jobject obj)
	{
	}
	void Stop(JNIEnv* env, jobject obj)
	{
	}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/PlayNative";
static JNINativeMethod VideoPlayMethods[] =
	{
		{ "Test", "()V", (void*) Test },
		{ "Init", "(Landroid/view/Surface;)I", (void*) Init },
		{ "Destroye", "()V", (void*) Destroye } };
int RegisterVideoPlayMethods(JNIEnv *env)
{
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
			sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

