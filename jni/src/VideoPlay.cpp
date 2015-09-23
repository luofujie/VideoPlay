#include "JNIUtil.h"
#include "VideoPlay.h"

VideoPlay::~VideoPlay()
{
}
VideoPlay::VideoPlay() :
		m_playNative(NULL), m_nativeWindow(NULL), m_bInit(false)
{
}
int VideoPlay::Init(jobject play, jobject surface)
{
	JNIUtil util;
	JNIEnv* env = util.GetJNIEnv();
	m_playNative = env->NewGlobalRef(play);
	m_nativeWindow = ANativeWindow_fromSurface(env, surface);
	if (m_nativeWindow == NULL)
	{
		LOGE("Get ANativeWindow ERROR!");
		return -1;
	}
	LOGI("init ANativeWindow success!");
	av_register_all();
	return 0;
}

int VideoPlay::OpenFile(const char* path)
{
	LOGI("VideoPath: %s", path);
	m_pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&m_pFormatCtx, path, NULL, NULL) != 0)
	{
		LOGE("couldn't open file!");
		return -1;
	}
	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		LOGE("Couldn't find stream information.\n");
		return -1;
	}

	int videoindex = -1;
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	if (videoindex == -1)
	{
		LOGE("Couldn't find a video stream.\n");
		return -1;
	}

	AVCodecContext *pCodecCtx = NULL;
	AVCodec *pCodec = NULL;
	pCodecCtx = m_pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		LOGE("Couldn't find Codec.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return -1;
	}

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
	JNIUtil util;
	JNIEnv* env = util.GetJNIEnv();
	ANativeWindow_release(m_nativeWindow);
	m_nativeWindow = NULL;
	env->DeleteGlobalRef(m_playNative);
	avformat_close_input(&m_pFormatCtx);
}
