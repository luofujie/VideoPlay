#ifndef AUDIO_PLAY_H
#define AUDIO_PLAY_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#define MAX_AUDIO_FRAME_SIZE 192000
typedef void PlayCallBack(SLAndroidSimpleBufferQueueItf bq, void *context);


const SLEnvironmentalReverbSettings reverbSettings =
		SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
class AudioPlay
{
private:
	AudioPlay();
	AudioPlay(const AudioPlay& play);
	AudioPlay & operator=(const AudioPlay &);
public:
	static inline AudioPlay& GetObject()
	{
		static AudioPlay Audio;
		return Audio;
	}
	~AudioPlay();
	void init();
	void createBufferQueueAudioPlayer(int rate, int channel, int bitsPerSample,
			PlayCallBack callback);
	SLresult PlayBuff(void* buff, int len);
	void destroy();
private:
	void createEngine();
private:
	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
	// output mix interfaces
	SLObjectItf outputMixObject;
	SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
	// buffer queue player interfaces
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
	SLEffectSendItf bqPlayerEffectSend;
	SLMuteSoloItf bqPlayerMuteSolo;
	SLVolumeItf bqPlayerVolume;
	// aux effect on the output mix, used by the buffer queue player
};
#endif
