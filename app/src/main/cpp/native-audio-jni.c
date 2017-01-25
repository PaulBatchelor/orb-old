/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/* This is a JNI example where we use native methods to play sounds
 * using OpenSL ES. See the corresponding Java source file located at:
 *
 *   src/com/example/nativeaudio/NativeAudio/NativeAudio.java
 */

#include <assert.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* android logging */

#include <android/log.h>

#define  LOG_TAG    "sporth"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifndef MAX
#define MAX(A, B) ((A > B) ? A : B)
#endif

#ifndef MIN
#define MIN(A, B) ((A < B) ? A : B)
#endif

// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
// #include <android/log.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


// for soundpipe 
#include "soundpipe.h"
#include "synth.h"

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
static SLmilliHertz bqPlayerSampleRate = 0;
static jint   bqPlayerBufSize = 0;
static short *resampleBuf = NULL;
// a mutext to guard against re-entrance to record & playback
// as well as make recording and playing back to be mutually exclusive
// this is to avoid crash at situations like:
//    recording is in session [not finished]
//    user presses record button and another recording coming in
// The action: when recording/playing back is not finished, ignore the new request
static pthread_mutex_t  audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// URI player interfaces
static SLObjectItf uriPlayerObject = NULL;
static SLPlayItf uriPlayerPlay;
static SLSeekItf uriPlayerSeek;
static SLMuteSoloItf uriPlayerMuteSolo;
static SLVolumeItf uriPlayerVolume;

// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;
static SLMuteSoloItf fdPlayerMuteSolo;
static SLVolumeItf fdPlayerVolume;

// recorder interfaces
static SLObjectItf recorderObject = NULL;
static SLRecordItf recorderRecord;
static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000
static short sawtoothBuffer[SAWTOOTH_FRAMES];

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)
static short recorderBuffer[RECORDER_FRAMES];
static unsigned recorderSize = 0;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;

typedef struct {
    sp_data *sp;
    sp_ftbl *ft;
    sp_fosc *fosc;
    sp_randh *randh;
    sp_revsc *rev;
    short *nextbuf;
    short *buf0;
    short *buf1;
    int whichbuf;
    unsigned bufsize;
    unsigned nextsize;
    int run;
    sp_synth synth;
} soundpipe;
\
static soundpipe g_sp;

static void create_soundpipe(int sr, int bufsize)
{
    uint32_t seed = time(NULL);
    /*TODO: make this sp variable not sp */
    soundpipe *sound = &g_sp;
    sp_create(&sound->sp);
    sound->sp->sr = sr;
    sp_srand(sound->sp, seed);

    sp_synth_create(sound->sp, &sound->synth);
    sp_synth_init(sound->sp, &sound->synth);

    /* buffer stuff */
    sound->bufsize = bufsize;
    sound->nextsize = sound->bufsize * sizeof(short);
    sound->buf0 = malloc(sizeof(short) * sound->bufsize);
    memset(sound->buf0, 0, sizeof(short) * sound->bufsize);
    sound->buf1 = malloc(sizeof(short) * sound->bufsize);
    memset(sound->buf1, 0, sizeof(short) * sound->bufsize);
    sound->nextbuf = sound->buf1;
    sound->whichbuf = 1;
    sound->run = 1;
}


static int android_read(void* cookie, char* buf, int size) {
  return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
  return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
  return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
  AAsset_close((AAsset*)cookie);
  return 0;
}

jboolean Java_com_example_paul_androidsoundpipe_MainActivity_createSoundpipe(JNIEnv* env, jclass clazz,
        jobject assetManager, jint sampleRate, jint bufSize) {
    soundpipe *sp = &g_sp;
    create_soundpipe(sampleRate, bufSize);
    return JNI_TRUE;
}


static void render_buf() {
    soundpipe *sp = &g_sp;
    short *mybuf;
    int i;
    SPFLOAT tmp1, tmp2, tmp3;
    short itmp;

    if(sp->run == 0) return;

    /* buffer 0 is being played */
    if(sp->whichbuf == 0) {
        sp->whichbuf  = 1;
        mybuf = sp->buf0;
    } else {
        sp->whichbuf = 0;
            mybuf = sp->buf1;
    }
    sp_synth_compute(sp->sp, &sp->synth, sp->bufsize, mybuf);
}

static void swap_buf() { 
    soundpipe *sp = &g_sp;
    if(sp->whichbuf == 1) {
        sp->nextbuf = sp->buf0;
    } else {
        sp->nextbuf = sp->buf1;
    }
}

static void destroy_soundpipe()
{
    soundpipe *sp = &g_sp;
    sp_synth_destroy(sp->sp, &sp->synth);
    sp_destroy(&sp->sp);

    free(sp->buf0);
    free(sp->buf1);
}


// synthesize a mono sawtooth wave and place it into a buffer (called automatically on load)
__attribute__((constructor)) static void onDlOpen(void)
{
    unsigned i;
    for (i = 0; i < SAWTOOTH_FRAMES; ++i) {
        sawtoothBuffer[i] = 32768 - ((i % 100) * 660);
    }
}

void releaseResampleBuf(void) {
    if( 0 == bqPlayerSampleRate) {
        /*
         * we are not using fast path, so we were not creating buffers, nothing to do
         */
        return;
    }

    free(resampleBuf);
    resampleBuf = NULL;
}

/*
 * Only support up-sampling
 */
short* createResampledBuf(uint32_t idx, uint32_t srcRate, unsigned *size) {
    short  *src = NULL;
    short  *workBuf;
    int    upSampleRate;
    int32_t srcSampleCount = 0;

    if(0 == bqPlayerSampleRate) {
        return NULL;
    }
    if(bqPlayerSampleRate % srcRate) {
        /*
         * simple up-sampling, must be divisible
         */
        return NULL;
    }
    upSampleRate = bqPlayerSampleRate / srcRate;

    switch (idx) {
        case 0:
            return NULL;
        case 1: // HELLO_CLIP
            break;
        case 2: // ANDROID_CLIP
            break;
        case 3: // SAWTOOTH_CLIP
            srcSampleCount = SAWTOOTH_FRAMES;
            src = sawtoothBuffer;
            break;
        case 4: // captured frames
            srcSampleCount = recorderSize / sizeof(short);
            src =  recorderBuffer;
            break;
        default:
            assert(0);
            return NULL;
    }

    resampleBuf = (short*) malloc((srcSampleCount * upSampleRate) << 1);
    if(resampleBuf == NULL) {
        return resampleBuf;
    }
    workBuf = resampleBuf;
    for(int sample=0; sample < srcSampleCount; sample++) {
        for(int dup = 0; dup  < upSampleRate; dup++) {
            *workBuf++ = src[sample];
        }
    }

    *size = (srcSampleCount * upSampleRate) << 1;     // sample format is 16 bit
    return resampleBuf;
}

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    soundpipe *sp = &g_sp;
    // for streaming playback, replace this test by logic to find and fill the next buffer
    //if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
    //    SLresult result;
    //    // enqueue another buffer
    //    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
    //    // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
    //    // which for this code example would indicate a programming error
    //    if (SL_RESULT_SUCCESS != result) {
    //        pthread_mutex_unlock(&audioEngineLock);
    //    }
    //    (void)result;
    //} else {
    //    releaseResampleBuf();
    //    pthread_mutex_unlock(&audioEngineLock);
    //}
    SLresult result;
    // enqueue another buffer
    render_buf();
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, sp->nextbuf, sp->nextsize);
    swap_buf();
    // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
    // which for this code example would indicate a programming error
    if (SL_RESULT_SUCCESS != result) {
        LOGD("ERROR");
        pthread_mutex_unlock(&audioEngineLock);
    }
    (void)result;
}


// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == recorderBufferQueue);
    assert(NULL == context);
    // for streaming recording, here we would call Enqueue to give recorder the next buffer to fill
    // but instead, this is a one-time buffer so we stop recording
    SLresult result;
    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
    if (SL_RESULT_SUCCESS == result) {
        recorderSize = RECORDER_FRAMES * sizeof(short);
    }
    pthread_mutex_unlock(&audioEngineLock);
}


// create the engine and output mix objects
void 
Java_com_example_paul_androidsoundpipe_MainActivity_createEngine(JNIEnv* env, jclass clazz)
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
            &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example

}


// create buffer queue audio player
void Java_com_example_paul_androidsoundpipe_MainActivity_createBufferQueueAudioPlayer(JNIEnv* env,
        jclass clazz, jint sampleRate, jint bufSize)
{
    SLresult result;
    if (sampleRate >= 0 && bufSize >= 0 ) {
        bqPlayerSampleRate = sampleRate * 1000;
        /*
         * device native buffer size is another factor to minimize audio latency, not used in this
         * sample: we only play one giant buffer here
         */
        bqPlayerBufSize = bufSize;
    }

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    /*
     * Enable Fast Audio when possible:  once we set the same rate to be the native, fast audio path
     * will be triggered
     */
    if(bqPlayerSampleRate) {
        format_pcm.samplesPerSec = bqPlayerSampleRate;       //sample rate in mili second
    }
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    /*
     * create audio player:
     *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
     *     for fast audio case
     */
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_EFFECTSEND,
                                    /*SL_IID_MUTESOLO,*/};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
                                   /*SL_BOOLEAN_TRUE,*/ };

    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
            bqPlayerSampleRate? 2 : 3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
            &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the effect send interface
    bqPlayerEffectSend = NULL;
    if( 0 == bqPlayerSampleRate) {
        result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
                                                 &bqPlayerEffectSend);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }

#if 0   // mute/solo is not supported for sources that are known to be mono, as this is
    // get the mute/solo interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_MUTESOLO, &bqPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
#endif

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}




// expose the mute/solo APIs to Java for one of the 3 players

static SLMuteSoloItf getMuteSolo()
{
    if (uriPlayerMuteSolo != NULL)
        return uriPlayerMuteSolo;
    else if (fdPlayerMuteSolo != NULL)
        return fdPlayerMuteSolo;
    else
        return bqPlayerMuteSolo;
}


// expose the volume APIs to Java for one of the 3 players

static SLVolumeItf getVolume()
{
    if (uriPlayerVolume != NULL)
        return uriPlayerVolume;
    else if (fdPlayerVolume != NULL)
        return fdPlayerVolume;
    else
        return bqPlayerVolume;
}

jboolean Java_com_example_paul_androidsoundpipe_MainActivity_startYourEngines(JNIEnv* env, jclass clazz)
{
    if (pthread_mutex_trylock(&audioEngineLock)) {
        // If we could not acquire audio engine lock, reject this request and client should re-try
        return JNI_FALSE;
    }
    soundpipe *sp = &g_sp;
    render_buf();
    SLresult result;
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, sp->nextbuf, sp->nextsize);
    swap_buf();
    if (SL_RESULT_SUCCESS != result) {
        pthread_mutex_unlock(&audioEngineLock);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


// shut down the native audio system
void Java_com_example_paul_androidsoundpipe_MainActivity_shutdown(JNIEnv* env, jclass clazz)
{

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy file descriptor audio player object, and invalidate all associated interfaces
    if (fdPlayerObject != NULL) {
        (*fdPlayerObject)->Destroy(fdPlayerObject);
        fdPlayerObject = NULL;
        fdPlayerPlay = NULL;
        fdPlayerSeek = NULL;
        fdPlayerMuteSolo = NULL;
        fdPlayerVolume = NULL;
    }

    // destroy URI audio player object, and invalidate all associated interfaces
    if (uriPlayerObject != NULL) {
        (*uriPlayerObject)->Destroy(uriPlayerObject);
        uriPlayerObject = NULL;
        uriPlayerPlay = NULL;
        uriPlayerSeek = NULL;
        uriPlayerMuteSolo = NULL;
        uriPlayerVolume = NULL;
    }

    // destroy audio recorder object, and invalidate all associated interfaces
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    //destroy_soundpipe();
    pthread_mutex_destroy(&audioEngineLock);
}

void Java_com_example_paul_androidsoundpipe_MainActivity_setX(JNIEnv* env, jclass clazz, 
        jfloat x)
{
    sp_synth *synth = &g_sp.synth; 
    synth->pos_x = MIN(x, 1.0); 
}

void Java_com_example_paul_androidsoundpipe_MainActivity_setY(JNIEnv* env, jclass clazz, 
        jfloat y)
{
    sp_synth *synth = &g_sp.synth; 
    synth->pos_y = MIN(y, 1.0); 
}
