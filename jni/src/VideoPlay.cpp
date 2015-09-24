#include "JNIUtil.h"
#include "VideoPlay.h"
VideoPlay::~VideoPlay()
{
}
VideoPlay::VideoPlay() :
		m_playNative(NULL), m_nativeWindow(NULL), m_bInit(false), m_pFormatCtx(
				NULL)
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

	return 0;
}
void VideoPlay::Play()
{
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
		return;
	}

	AVCodecContext *pCodecCtx = NULL;
	AVCodec *pCodec = NULL;
	pCodecCtx = m_pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	//AV_CODEC_ID_H264
	if (pCodec == NULL)
	{
		LOGE("Couldn't find Codec.\n");
		return;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return;
	}
	ANativeWindow_setBuffersGeometry(m_nativeWindow, pCodecCtx->width,
			pCodecCtx->height, WINDOW_FORMAT_RGBX_8888);
	int nfomat = ANativeWindow_getFormat(m_nativeWindow);
	AVFrame *pFrameRGB = av_frame_alloc();
	AVFrame *pFrame = av_frame_alloc();
	uint8_t *out_buffer;
	out_buffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
			pCodecCtx->height)];
	avpicture_fill((AVPicture *) pFrameRGB, out_buffer, PIX_FMT_RGB24,
			pCodecCtx->width, pCodecCtx->height);
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

	SwsContext* img_convert_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width,
			pCodecCtx->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
	clock_t time_start = clock();
	int got_picture;
	int ret = 0;
	LOGI("pCodecCtx->width=%d,pCodecCtx->height=%d", pCodecCtx->width,
			pCodecCtx->height);
	while (av_read_frame(m_pFormatCtx, packet) >= 0)
	{
		if (packet->stream_index == videoindex)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
					packet);
			if (ret < 0)
			{
				LOGE("Decode Error.\n");
				return;
			}
			if (got_picture)
			{
				sws_scale(img_convert_ctx,
						(const uint8_t* const *) pFrame->data, pFrame->linesize,
						0, pCodecCtx->height, pFrameRGB->data,
						pFrameRGB->linesize);
				int y_size = pCodecCtx->width * pCodecCtx->height;
				ANativeWindow_Buffer windowBuffer;
				if (ANativeWindow_lock(m_nativeWindow, &windowBuffer, NULL) < 0)
				{
					LOGE("cannot lock window");
				}
				else
				{
					unsigned char* tmp =(unsigned char*)( pFrameRGB->data[0]);
					unsigned int* data = (unsigned int*) windowBuffer.bits;
					for (int i = 0; i < pCodecCtx->height; i++)
					{
						for (int j = 0; j < pCodecCtx->width; j++)
						{
							int nFlag = 0;
							memcpy(&nFlag, tmp + (pCodecCtx->width * i + j) * 3, 3);
							data[i * windowBuffer.stride + j] = nFlag;
						}
					}
					ANativeWindow_unlockAndPost(m_nativeWindow);
				}
			}
		}
		av_free_packet(packet);
	}

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
