#include <stdlib.h>
#include <assert.h>
#include "JNIUtil.h"
#include "AudioPlay.h"
// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// create the engine and output mix objects
void createEngine()
{
	SLresult result;

	// create engine
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// realize the engincreateEnginee
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// get the engine interface, which is needed in order to create other objects
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
			&engineEngine);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// create output mix, with environmental reverb specified as a non-required interface
	const SLInterfaceID ids[1] =
	{ SL_IID_ENVIRONMENTALREVERB };
	const SLboolean req[1] =
	{ SL_BOOLEAN_FALSE };
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1,
			ids, req);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// realize the output mix
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

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
		(void) result;
	}
	// ignore unsuccessful result codes for environmental reverb, as it is optional for this example

}

// create buffer queue audio player
void createBufferQueueAudioPlayer(int rate, int channel, int bitsPerSample,PlayCallBack callback)
{
	SLresult result;

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq =
	{ SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm =
	{ SL_DATAFORMAT_PCM, channel, SL_SAMPLINGRATE_44_1,
	SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
	SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN };
	SLDataSource audioSrc =
	{ &loc_bufq, &format_pcm };

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix =
	{ SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
	SLDataSink audioSnk =
	{ &loc_outmix, NULL };

	// create audio player
	const SLInterfaceID ids[3] =
	{ SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
	/*SL_IID_MUTESOLO,*/SL_IID_VOLUME };
	const SLboolean req[3] =
	{ SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
	/*SL_BOOLEAN_TRUE,*/SL_BOOLEAN_TRUE };
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject,
			&audioSrc, &audioSnk, 3, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// realize the player
	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// get the play interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY,
			&bqPlayerPlay);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// get the buffer queue interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
			&bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// register callback on the buffer queue
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,
			callback, NULL);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// get the effect send interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
			&bqPlayerEffectSend);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is	// get the mute/solo interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
#endif

	// get the volume interface
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME,
			&bqPlayerVolume);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;

	// set the player's state to playing
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void) result;
}

