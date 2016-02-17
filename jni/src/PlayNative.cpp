#include "JNILoad.h"
#include "JNIUtil.h"
#include "VideoPlay.h"
#include <pthread.h>
extern "C"
{
	int Init(JNIEnv* env, jobject obj, jobject surface)
	{
		LOGI("---------------Init!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		video.Init(obj,surface);
		LOGI("~~~~~~~~~~~~~Init end!~~~~~~~~~~~~");
		return 0;
	}
	void Destroye(JNIEnv* env, jobject obj)
	{
		LOGI("---------------Destroye!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		video.Destroye();
		LOGI("~~~~~~~~~~~~~Destroye end!~~~~~~~~~~~~");
	}
	int Open(JNIEnv* env, jobject obj,jstring path)
	{
		LOGI("---------------Open!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		JNIUtil util;
		char* strPath = util.Jstring2String(path);
		video.OpenFile(strPath);
		LOGI("~~~~~~~~~~~~~Open end!~~~~~~~~~~~~");
		return 0;
	}

	void Play(JNIEnv* env, jobject obj)
	{
		LOGI("---------------Play!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		video.SetPlayState(State_Play);
		LOGI("~~~~~~~~~~~~~Play end!~~~~~~~~~~~~");

	}
	void Pause(JNIEnv* env, jobject obj)
	{
		LOGI("---------------Pause!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		video.SetPlayState(State_Pause);
		LOGI("~~~~~~~~~~~~~Pause end!~~~~~~~~~~~~");
	}
	void Stop(JNIEnv* env, jobject obj)
	{
		LOGI("---------------Stop!--------------");
		VideoPlay& video = VideoPlay::GetObject();
		video.SetPlayState(State_Stop);
		LOGI("~~~~~~~~~~~~~Stop end!~~~~~~~~~~~~");
	}
}
const char VideoPlayClassName[] = "com/hail/videoplay/jni/PlayNative";
static JNINativeMethod VideoPlayMethods[] =
	{
		{ "Init", "(Landroid/view/Surface;)I", (void*) Init },
		{ "Open", "(Ljava/lang/String;)I", (void*) Open },
		{ "Play", "()V", (void*) Play },
		{ "Pause", "()V", (void*) Pause },
		{ "Stop", "()V", (void*) Stop },
		{ "Destroye", "()V", (void*) Destroye } };
int RegisterVideoPlayMethods(JNIEnv *env)
{
	return jniRegisterNativeMethods(env, VideoPlayClassName, VideoPlayMethods,
			sizeof(VideoPlayMethods) / sizeof(JNINativeMethod));
}

