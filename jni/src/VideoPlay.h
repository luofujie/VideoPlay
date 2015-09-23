#ifndef VIDEO_PLAY
#define VIDEO_PLAY
#include <android/native_window.h>
#include <android/native_window_jni.h>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
}
class VideoPlay
{

private:
	VideoPlay();
	VideoPlay(const VideoPlay& play);
	VideoPlay & operator = (const VideoPlay &);
public:
	static inline VideoPlay& GetObject()
	{
		static VideoPlay Play;
		return Play;
	}
	~VideoPlay();
	int Init(jobject play,jobject surface);
	int OpenFile(const char* path);
	void Play();
	void Pause();
	void Stop();
	void Destroye();
private:
	jobject m_playNative;
	ANativeWindow* m_nativeWindow;
	bool m_bInit;
	AVFormatContext *m_pFormatCtx;
};
#endif

