#ifndef VIDEO_PLAY
#define VIDEO_PLAY
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include "AudioPlay.h"
using namespace std;
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
enum PlayState
{
	State_Stop, State_Pause, State_Play
};
class VideoPlay
{

private:
	VideoPlay();
	VideoPlay(const VideoPlay& play);
	VideoPlay & operator =(const VideoPlay &);
public:
	static inline VideoPlay& GetObject()
	{
		static VideoPlay Play;
		return Play;
	}
	~VideoPlay();
	int Init(jobject play, jobject surface);
	int OpenFile(const char* path);
	void SetPlayState(PlayState State);
	void Destroye();
private:
	void Play();
	void Pause();
	void Stop();
	void Decode();
	void PlayAudioVideo();
	static void *DecodeThread(void *args);
	static void* PlayThread(void *args);
	static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq,
			void *context);
private:
	jobject m_playNative;
	ANativeWindow* m_nativeWindow;
	AVFormatContext *m_pFormatCtx;
	AVCodecContext *m_pVideoCodecCtx;
	AVCodec *m_pVideoCodec;
	AVCodecID m_videoCodeID;
	AVCodecContext *m_pAudioCodecCtx;
	AVCodec *m_pAudioCodec;
	AVCodecID m_audioCodeID;
	int m_videoindex;
	int m_audioindex;
	int m_nWidth;
	int m_height;

	pthread_t m_decodeThreadID;
	pthread_t m_playThreadID;
	queue<int> m_videoBuff;
	queue<int> m_audioBuff;
	PlayState m_eState;
	pthread_mutex_t m_mutex;
	int m_ptm;
	bool m_bDecodeFinish;

	static const int MAX_BUFF_SIZE = 128;
	//线程同步异步操作
	sem_t semPlay;
	//视频处理消费者生产者
	sem_t semVideoEmpty; // 同步信号量， 当满了时阻止生产者放产品
	sem_t semVideoFull;   // 同步信号量， 当没产品时阻止消费者消费
	pthread_mutex_t mutexVideo;   // 互斥信号量， 一次只有一个线程访问缓冲

	AudioPlay& m_audioPlay;
	int m_audioLen;
//	//音频输出参数
//	uint64_t out_channel_layout;
//	int out_nb_samples;
//	AVSampleFormat out_sample_fmt;
//	int out_sample_rate;
//	int out_buffer_size;
//	int out_channels;

};
#endif

