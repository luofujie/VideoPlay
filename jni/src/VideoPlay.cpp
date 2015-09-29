#include "JNIUtil.h"
#include "VideoPlay.h"
#include <assert.h>

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
		SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;
static SLMuteSoloItf fdPlayerMuteSolo;
static SLVolumeItf fdPlayerVolume;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	assert(bq == bqPlayerBufferQueue);
	assert(NULL == context);
	// for streaming playback, replace this test by logic to find and fill the next buffer
	if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize)
	{
		SLresult result;
		// enqueue another buffer
		result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
				nextBuffer, nextSize);
		// the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
		// which for this code example would indicate a programming error
		assert(SL_RESULT_SUCCESS == result);
	}
}

void createEngine()
{
	SLresult result;

	// create engine
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result);

	// realize the engine
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	// get the engine interface, which is needed in order to create other objects
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
			&engineEngine);
	assert(SL_RESULT_SUCCESS == result);

	// create output mix, with environmental reverb specified as a non-required interface
	const SLInterfaceID ids[1] =
	{	SL_IID_ENVIRONMENTALREVERB};
	const SLboolean req[1] =
	{	SL_BOOLEAN_FALSE};
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1,
			ids, req);
	assert(SL_RESULT_SUCCESS == result);

	// realize the output mix
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	// get the environmental reverb interface
	// this could fail if the environmental reverb effect is not available,
	// either because the feature is not present, excessive CPU load, or
	// the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
	result = (*outputMixObject)->GetInterface(outputMixObject,
			SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
	if (SL_RESULT_SUCCESS == result)
	{
		result =
		(*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
				outputMixEnvironmentalReverb, &reverbSettings);
	}
	// ignore unsuccessful result codes for environmental reverb, as it is optional for this example
}

void createBufferQueueAudioPlayer(int rate, int channel, int bitsPerSample)
{
	SLresult result;

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq =
	{	SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
//    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_16,
//        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
//        SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN};
	SLDataFormat_PCM format_pcm;
	format_pcm.formatType = SL_DATAFORMAT_PCM;
	format_pcm.numChannels = channel;
	format_pcm.samplesPerSec = rate * 1000;
	format_pcm.bitsPerSample = bitsPerSample;
	format_pcm.containerSize = 16;
	if (channel == 2)
	format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	else
	format_pcm.channelMask = SL_SPEAKER_FRONT_CENTER;
	format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
	SLDataSource audioSrc =
	{	&loc_bufq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix =
	{	SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
	SLDataSink audioSnk =
	{	&loc_outmix, NULL};

	// create audio player
	const SLInterfaceID ids[3] =
	{	SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
		/*SL_IID_MUTESOLO,*/SL_IID_VOLUME};
	const SLboolean req[3] =
	{	SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
		/*SL_BOOLEAN_TRUE,*/SL_BOOLEAN_TRUE};
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject,
			&audioSrc, &audioSnk, 3, ids, req);
	assert(SL_RESULT_SUCCESS == result);
// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY,
			&bqPlayerPlay);
	assert(SL_RESULT_SUCCESS == result);

	// get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
			&bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);

	// register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,
			bqPlayerCallback, NULL);
	assert(SL_RESULT_SUCCESS == result);

	// get the effect send interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
			&bqPlayerEffectSend);
	assert(SL_RESULT_SUCCESS == result);

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is					// get the mute/solo interface	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);	assert(SL_RESULT_SUCCESS == result);
#endif

	// get the volume interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME,
			&bqPlayerVolume);
	assert(SL_RESULT_SUCCESS == result);

// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);

}

void AudioWrite(const void*buffer, int size)
{
	(*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, size);
}
VideoPlay::~VideoPlay()
{
}
VideoPlay::VideoPlay() :
m_playNative(NULL),
m_nativeWindow(NULL), m_bInit(false), m_pFormatCtx(
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
void VideoPlay::Play()
{
	AVFrame *pFrameRGB = av_frame_alloc();
	AVFrame *pFrame = av_frame_alloc();
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	uint8_t *out_buffer;
	out_buffer = new uint8_t[avpicture_get_size(PIX_FMT_RGB24, m_nWidth,
			m_height)];
	avpicture_fill((AVPicture *) pFrameRGB, out_buffer, PIX_FMT_RGB24,
			m_nWidth, m_height);
	SwsContext* img_convert_ctx = sws_getContext(m_nWidth, m_height,
			m_pVideoCodecCtx->pix_fmt, m_nWidth, m_height, PIX_FMT_RGB24,
			SWS_BICUBIC, NULL, NULL, NULL);
	int ret = 0;
	LOGI("pCodecCtx->width=%d,pCodecCtx->height=%d", m_nWidth, m_height);
	LOGI("m_audioindex=%d,m_videoindex=%d", m_audioindex, m_videoindex);
	LOGI("m_audioCodeID=%d,m_videoCodeID=%d", m_audioCodeID, m_videoCodeID);

	AVFrame* AudioFrame = av_frame_alloc();
	SwrContext* au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, AV_CH_LAYOUT_STEREO,
			AV_SAMPLE_FMT_S16, 44100, m_pAudioCodecCtx->channel_layout,
			m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0,
			NULL);
	swr_init(au_convert_ctx);
	int audio_linesize = 0;
	int audio_buffer_size = av_samples_get_buffer_size(&audio_linesize,
			m_pAudioCodecCtx->channels, m_pAudioCodecCtx->frame_size,
			m_pAudioCodecCtx->sample_fmt, 1);
	uint8_t *audio_buffer = new uint8_t[audio_buffer_size];
	int flag_start = 0;
	createEngine();
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
				ANativeWindow_Buffer windowBuffer;
				if (ANativeWindow_lock(m_nativeWindow, &windowBuffer, NULL) < 0)
				{
					LOGE("cannot lock window");
				}
				else
				{
					unsigned char* tmp = (unsigned char*) (pFrameRGB->data[0]);
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
					ANativeWindow_unlockAndPost(m_nativeWindow);
				}
			}
		}
		int AudioFinished = 0;
		if (packet->stream_index == m_audioindex)
		{
			if (flag_start == 0)
			{
				flag_start = 1;
				createBufferQueueAudioPlayer(m_pAudioCodecCtx->sample_rate,
						m_pAudioCodecCtx->channels,
						SL_PCMSAMPLEFORMAT_FIXED_16);
			}

			ret = avcodec_decode_audio4(m_pAudioCodecCtx, AudioFrame,
					&AudioFinished, packet);
			if (ret > 0 && AudioFinished)
			{
				swr_convert(au_convert_ctx, &audio_buffer, audio_linesize,
						(const uint8_t **) AudioFrame->data,
						AudioFrame->nb_samples);
				(*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
						audio_buffer, audio_buffer_size);
			}
		}

	}
	av_free_packet(packet);
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

