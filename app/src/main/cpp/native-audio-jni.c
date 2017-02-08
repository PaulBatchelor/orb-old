/* 
 *  Based on native audio example provided by Google.
 *  
 */

#define NANOVG_GLES2_IMPLEMENTATION
#include <assert.h>
#include <jni.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* android logging */

#include <android/log.h>

#ifndef MAX
#define MAX(A, B) ((A > B) ? A : B)
#endif

#ifndef MIN
#define MIN(A, B) ((A < B) ? A : B)
#endif

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

#include "soundpipe.h"
#include "orb.h"

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
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
} soundpipe;

static soundpipe g_sp;

orb_data orb;

NVGcontext* vg = NULL;
int width,height;


int setupGraphics(int w, int h);

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static void create_soundpipe(int sr, int bufsize)
{
    uint32_t seed = time(NULL);
    /*TODO: make this sp variable not sp */
    soundpipe *sound = &g_sp;

    LOGI("initialising orb\n");
    orb_init(&orb, sr);

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

jboolean Java_com_paulbatchelor_orb_MainActivity_createSoundpipe(JNIEnv* env, jclass clazz,
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

    orb_audio_computei(&orb, mybuf, sp->bufsize);
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
    orb_audio_destroy(&orb);
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

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    soundpipe *sp = &g_sp;
    SLresult result;
    render_buf();
    result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, sp->nextbuf, sp->nextsize);
    swap_buf();
    if (SL_RESULT_SUCCESS != result) {
        pthread_mutex_unlock(&audioEngineLock);
    }
    (void)result;
}


// create the engine and output mix objects
void 
Java_com_paulbatchelor_orb_MainActivity_createEngine(JNIEnv* env, jclass clazz)
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

}


// create buffer queue audio player
void Java_com_paulbatchelor_orb_MainActivity_createBufferQueueAudioPlayer(JNIEnv* env,
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

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}




// expose the volume APIs to Java for one of the 3 players

jboolean Java_com_paulbatchelor_orb_MainActivity_startYourEngines(JNIEnv* env, jclass clazz)
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
void Java_com_paulbatchelor_orb_MainActivity_shutdown(JNIEnv* env, jclass clazz)
{

    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    destroy_soundpipe();
    pthread_mutex_destroy(&audioEngineLock);
}

JNIEXPORT void JNICALL Java_com_paulbatchelor_orb_MainActivity_pos(JNIEnv * env, jobject obj,
jfloat x, jfloat y)
{
    orb.mouse.y_pos = y;
    orb.mouse.x_pos = x;
    orb_poke(&orb);
}

JNIEXPORT void JNICALL Java_com_paulbatchelor_orb_MainActivity_setDimensions(JNIEnv * env, jobject obj,
    jint width, jint height)
{
    orb.width = width;   
    orb.height = height;   
    orb_grid_calculate(&orb);
}

JNIEXPORT void JNICALL Java_com_paulbatchelor_orb_GL2JNILib_init(JNIEnv * env, 
    jobject obj,  jint width, jint height)
{
    orb.width = width;
    orb.height = height;
    LOGI("the width is %d and the height is %d\n", width, height);
    orb_grid_calculate(&orb);
    setupGraphics(width, height);
}

orb_data * orb_get_data()
{
    return &orb;
}

JNIEXPORT void JNICALL Java_com_paulbatchelor_orb_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    orb_step(vg, &orb);
}

int setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");

    vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    
    return 0;
}

