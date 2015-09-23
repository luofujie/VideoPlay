#ifndef VIDEO_PLAY
#define VIDEO_PLAY

class VideoPlay
{
public:
	VideoPlay();
	~VideoPlay();
	int Init();
	int OpenFile();
	void Play();
	void Pause();
	void Stop();
	void Destroye();
private:
};
#endif

