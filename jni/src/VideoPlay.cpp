#include <unistd.h>

#include "JNIUtil.h"
#include "VideoPlay.h"

#include <semaphore.h>
#include <assert.h>
VideoPlay::~VideoPlay()
{
}
VideoPlay::VideoPlay()
{
	m_playNative = NULL;
	m_nativeWindow = NULL;
	m_bInit = false;
	m_pFormatCtx = NULL;
	m_pVideoCodecCtx = NULL;
	m_pVideoCodec = NULL;
	m_height = 0;
	m_eState = State_Stop;
	m_audioempty = true;
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

	//视频信息解析
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
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoindex = i;
			break;
		}
	if (m_videoindex == -1)
	{
		LOGE("Couldn't find a video stream.\n");
		return -1;
	}
	m_pVideoCodecCtx = m_pFormatCtx->streams[m_videoindex]->codec;
	m_pVideoCodec = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);
	m_videoCodeID = m_pVideoCodecCtx->codec_id;
	//AV_CODEC_ID_H264
	if (m_pVideoCodec < 0)
	{
		LOGE("Couldn't find Codec.\n");
		return -1;
	}
	if (avcodec_open2(m_pVideoCodecCtx, m_pVideoCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return -1;
	}

	//音频信息解析
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioindex = i;
			break;
		}
	}
	if (m_audioindex < 0)
	{
		LOGE("Couldn't find a audio stream.\n");
		return -1;
	}
	m_pAudioCodecCtx = m_pFormatCtx->streams[m_audioindex]->codec;
	m_pAudioCodec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);
	m_audioCodeID = m_pAudioCodecCtx->codec_id;
	//AV_CODEC_ID_AAC
	if (m_pAudioCodec < 0)
	{
		LOGE("Couldn't find Codec.\n");
		return -1;
	}
	if (avcodec_open2(m_pAudioCodecCtx, m_pAudioCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return -1;
	}

	m_nWidth = m_pVideoCodecCtx->width;
	m_height = m_pVideoCodecCtx->height;
	ANativeWindow_setBuffersGeometry(m_nativeWindow, m_nWidth, m_height,
			WINDOW_FORMAT_RGBX_8888);
	return 0;
}
sem_t semPlay;
//视频处理消费者生产者
sem_t semVideoEmpty; // 同步信号量， 当满了时阻止生产者放产品
sem_t semVideoFull;   // 同步信号量， 当没产品时阻止消费者消费
pthread_mutex_t mutexVideo;   // 互斥信号量， 一次只有一个线程访问缓冲

//音频处理消费者生产者
sem_t semAudioEmpty; // 同步信号量， 当满了时阻止生产者放产品
sem_t semAudioFull;   // 同步信号量， 当没产品时阻止消费者消费
pthread_mutex_t mutexAudio;   // 互斥信号量， 一次只有一个线程访问缓冲

void VideoPlay::Play()
{
	LOGI("m_eState=%d", m_eState);
	if (m_eState == State_Stop)
	{
		sem_init(&semVideoEmpty, 0, MAX_BUFF_SIZE);
		sem_init(&semVideoFull, 0, 0);
		sem_init(&semAudioEmpty, 0, MAX_BUFF_SIZE);
		sem_init(&semAudioFull, 0, 0);
		sem_init(&semPlay, 0, 0);
		m_decodeThreadID = -1;
		m_playThreadID = -1;
		m_eState = State_Playing;
		pthread_mutex_init(&mutexVideo, NULL);
		pthread_mutex_init(&mutexAudio, NULL);
		pthread_create(&m_decodeThreadID, NULL, DecodeThread, this);
		pthread_create(&m_playThreadID, NULL, PlayThread, this);
	}
	else if (m_eState == State_Pause)
	{
		m_eState = State_Playing;
		sem_post(&semPlay);
	}
}

void* VideoPlay::PlayThread(void *args)
{
	VideoPlay* play = (VideoPlay*) args;
	play->PlayAudioVideo();
	pthread_exit(0);
}
void* VideoPlay::DecodeThread(void *args)
{
	VideoPlay* play = (VideoPlay*) args;
	play->Decode();
	pthread_exit(0);
}
void VideoPlay::PlayAudioVideo()
{
	clock_t start, end;
	int us = 0;
	int n = 0;
	int x = 0;
	while (1)
	{
		if (m_eState == State_Pause)
		{
			sem_wait(&semPlay);
		}
		start = clock();
		ANativeWindow_Buffer windowBuffer;
		if (ANativeWindow_lock(m_nativeWindow, &windowBuffer, NULL) < 0)
		{
			LOGE("cannot lock window");
		}
		else
		{
			sem_wait(&semVideoFull);
			pthread_mutex_lock(&mutexVideo);
			unsigned char* tmp = (unsigned char*) m_videoBuff.front();
			m_videoBuff.pop();
			pthread_mutex_unlock(&mutexVideo);
			sem_post(&semVideoEmpty);
			unsigned int* data = (unsigned int*) windowBuffer.bits;
			for (int i = 0; i < m_height; i++)
			{
				for (int j = 0; j < m_nWidth; j++)
				{
					int nFlag = 0;
					memcpy(&nFlag, tmp + (m_nWidth * i + j) * 3, 3);
					data[i * windowBuffer.stride + j] = nFlag;
				}
			}
			delete[] tmp;
			tmp = NULL;
			ANativeWindow_unlockAndPost(m_nativeWindow);
			end = clock();
			us += m_ptm - (end - start);
			n++;
			if (n % 25 == 0)
			{
				x++;
				LOGI("m_ptm=%d,第%d秒", m_ptm, x);
			}
			if (us > 0)
			{
				usleep(us);
				us = 0;
			}
		}
	}

	//	createBufferQueueAudioPlayer(44100, m_pAudioCodecCtx->channels,
	//					SL_PCMSAMPLEFORMAT_FIXED_16);
	//	(*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
	//			audio_buffer, audio_buffer_size);
	//	createEngine();
}
extern SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static const char tmp[] =
#include "android_clip.h"
;
#define MAX_AUDIO_FRAME_SIZE 192000
void VideoPlay::Decode()
{
	AVFrame *pFrameRGB = av_frame_alloc();
	AVFrame *pFrame = av_frame_alloc();
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	uint8_t *out_buffer;
	out_buffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, m_nWidth,
			m_height)];
	avpicture_fill((AVPicture *) pFrameRGB, out_buffer, PIX_FMT_RGB24, m_nWidth,
			m_height);
	SwsContext* img_convert_ctx = sws_getContext(m_nWidth, m_height,
			m_pVideoCodecCtx->pix_fmt, m_nWidth, m_height, PIX_FMT_RGB24,
			SWS_BICUBIC, NULL, NULL, NULL);
	int ret = 0;
	LOGI("pCodecCtx->width=%d,pCodecCtx->height=%d", m_nWidth, m_height);
	LOGI("m_audioindex=%d,m_videoindex=%d", m_audioindex, m_videoindex);
	LOGI("m_audioCodeID=%d,m_videoCodeID=%d", m_audioCodeID, m_videoCodeID);

	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_FRONT_CENTER;
	//nb_samples: AAC-1024 MP3-1152
	int out_nb_samples = m_pAudioCodecCtx->frame_size;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 44100;
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels,
			out_nb_samples, out_sample_fmt, 1);
	out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	//FIX:Some Codec's Context Information is missing
	int64_t in_channel_layout;
	struct SwrContext *au_convert_ctx;
	in_channel_layout = av_get_default_channel_layout(
			m_pAudioCodecCtx->channels);
	//Swr
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout,
			out_sample_fmt, out_sample_rate, in_channel_layout,
			m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0,
			NULL);
	swr_init(au_convert_ctx);
	AVFrame *pAudioFrame;
	pAudioFrame = av_frame_alloc();

	m_audioLen = out_buffer_size;
	LOGI("m_pFormatCtx->duration=%d", m_pFormatCtx->duration);
	LOGI("den=%d,num=%d", m_pVideoCodecCtx->time_base.den,
			m_pVideoCodecCtx->time_base.num);
	m_ptm = 40000;	//av_q2d(m_pVideoCodecCtx->time_base)*1000000;
	int flag_start = 0;
	createEngine();
	LOGI(" bit_rate = %d ", m_pAudioCodecCtx->bit_rate);
	LOGI(" sample_rate = %d ", m_pAudioCodecCtx->sample_rate);
	LOGI(" channels = %d ", m_pAudioCodecCtx->channels);
	LOGI(" code_name = %s ", m_pAudioCodecCtx->codec->name);
	LOGI(" block_align = %d", m_pAudioCodecCtx->block_align);
	while (av_read_frame(m_pFormatCtx, packet) >= 0)
	{
		int got_picture = 0;
		if (packet->stream_index == m_videoindex)
		{

			ret = avcodec_decode_video2(m_pVideoCodecCtx, pFrame, &got_picture,
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
						0, m_height, pFrameRGB->data, pFrameRGB->linesize);

				sem_wait(&semVideoEmpty);
				pthread_mutex_lock(&mutexVideo);
				unsigned char* tmp = new unsigned char[m_nWidth * m_height * 3];
				memcpy(tmp, pFrameRGB->data[0], m_nWidth * m_height * 3);
				m_videoBuff.push((int) tmp);
				pthread_mutex_unlock(&mutexVideo);
				sem_post(&semVideoFull);
			}
		}

		int AudioFinished = 0;
		if (packet->stream_index == m_audioindex)
		{
			ret = avcodec_decode_audio4(m_pAudioCodecCtx, pAudioFrame,
					&AudioFinished, packet);
			if (ret > 0 && AudioFinished)
			{

				swr_convert(au_convert_ctx, &out_buffer, MAX_AUDIO_FRAME_SIZE,
						(const uint8_t **) pAudioFrame->data,
						pAudioFrame->nb_samples);
				if (flag_start == 0)
				{
					flag_start = 1;
					createBufferQueueAudioPlayer(out_sample_rate, out_channels,
					SL_PCMSAMPLEFORMAT_FIXED_16, bqPlayerCallback);
					(*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
							out_buffer, out_buffer_size);

				}
				uint8_t *tmp = (uint8_t *) av_malloc(
				MAX_AUDIO_FRAME_SIZE * 2);
				memcpy(tmp, out_buffer, out_buffer_size);
				m_audioBuff.push((int) tmp);
				tmp = NULL;
			}
		}
	}
	av_frame_free(&pAudioFrame);
	av_free_packet(packet);
	m_bDecodeFinish = true;
	LOGI("Decode File Finish!");
}
// this callback handler is called every time a buffer finishes playing
void VideoPlay::bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq,
		void *context)
{
	VideoPlay& play = VideoPlay::GetObject();
	assert(bq == bqPlayerBufferQueue);
	assert(NULL == context);
	static short * buffer = NULL;
	if (buffer != NULL)
	{
		av_free(buffer);
		buffer = NULL;
	}
	while (1)
	{
		short *nextBuffer = NULL;
		unsigned nextSize = 0;
		int count = play.m_audioBuff.size();
		if (count > 0)
		{
			nextBuffer = (short*) play.m_audioBuff.front();
			play.m_audioBuff.pop();
			nextSize = play.m_audioLen;
		}
		// for streaming playback, replace this test by logic to find and fill the next buffer
		if (count > 0 && NULL != nextBuffer && 0 != nextSize)
		{
			SLresult result;
			// enqueue another buffer
			buffer = nextBuffer;
			result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
					nextBuffer, nextSize);
			// the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
			// which for this code example would indicate a programming error
			assert(SL_RESULT_SUCCESS == result);
			break;
		}
		usleep(1000);
	}
}
void VideoPlay::Pause()
{
	if (m_eState == State_Playing)
		m_eState = State_Pause;
}
void VideoPlay::Stop()
{
	if (m_eState != State_Stop)
		m_eState = State_Stop;
}
void VideoPlay::Destroye()
{
	JNIUtil util;
	JNIEnv* env = util.GetJNIEnv();
	ANativeWindow_release(m_nativeWindow);
	m_nativeWindow = NULL;
	env->DeleteGlobalRef(m_playNative);
	avformat_close_input(&m_pFormatCtx);
	(*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
}

