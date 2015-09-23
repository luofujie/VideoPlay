#include <android/native_window_jni.h>
#include <android/native_window.h>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
}
#include "JNIUtil.h"
#include "VideoPlay.h"
VideoPlay::VideoPlay()
{
}
VideoPlay::~VideoPlay()
{
}
int VideoPlay::Init()
{
	//gPlayView = ANativeWindow_fromSurface(env, surface);
//	if (gPlayView == NULL)
//	{
//		LOGE("Get ANativeWindow ERROR!");
//		return -1;
//	}
	LOGI("init ANativeWindow success!");
	av_register_all();
	return 0;
}

int VideoPlay::OpenFile()
{
	return 0;
}
void VideoPlay::Play()
{
}
void VideoPlay::Pause()
{
}
void VideoPlay::Stop()
{
}
void VideoPlay::Destroye()
{
	//ANativeWindow_release(gPlayView);
	//gPlayView = NULL;
}
