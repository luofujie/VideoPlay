#ifndef AUDIO_PLAY_H
#define AUDIO_PLAY_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
typedef void PlayCallBack(SLAndroidSimpleBufferQueueItf bq, void *context);
void createBufferQueueAudioPlayer(int rate, int channel, int bitsPerSample,PlayCallBack callback);
void createEngine();
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
void SetNextBuffer(short *buffer, unsigned Size);

#endif
