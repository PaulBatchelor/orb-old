/*
 * RevSC Mono
 *
 * This code has been extracted from the Csound opcode "reverbsc".
 * It has been modified to work as a Soundpipe module.
 *
 * Original Author(s): Sean Costello, Istvan Varga
 * Year: 1999, 2005
 * Location: Opcodes/reverbsc.c
 *
 */

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "soundpipe.h"
#include "revscm.h"

#define DEFAULT_SRATE   44100.0
#define MIN_SRATE       5000.0
#define MAX_SRATE       1000000.0
#define MAX_PITCHMOD    20.0
#define DELAYPOS_SHIFT  28
#define DELAYPOS_SCALE  0x10000000
#define DELAYPOS_MASK   0x0FFFFFFF

#define NDELAYLINES 4

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

/* reverbParams[n][0] = delay time (in seconds)                     */
/* reverbParams[n][1] = random variation in delay time (in seconds) */
/* reverbParams[n][2] = random variation frequency (in 1/sec)       */
/* reverbParams[n][3] = random seed (0 - 32767)                     */

/*
static const SPFLOAT reverbParams[8][4] = {
    { (2473.0 / DEFAULT_SRATE), 0.0010, 3.100,  1966.0 },
    { (2767.0 / DEFAULT_SRATE), 0.0011, 3.500, 29491.0 },

    { (3217.0 / DEFAULT_SRATE), 0.0017, 1.110, 22937.0 },
    { (3557.0 / DEFAULT_SRATE), 0.0006, 3.973,  9830.0 },

    { (3907.0 / DEFAULT_SRATE), 0.0010, 2.341, 20643.0 },
    { (4127.0 / DEFAULT_SRATE), 0.0011, 1.897, 22937.0 },

    { (2143.0 / DEFAULT_SRATE), 0.0017, 0.891, 29491.0 },
    { (1933.0 / DEFAULT_SRATE), 0.0006, 3.221, 14417.0 }
};
*/

static const SPFLOAT reverbParams[8][4] = {
    { (2473.0 / DEFAULT_SRATE), 0.0010, 3.100,  1966.0 },
    { (2767.0 / DEFAULT_SRATE), 0.0011, 3.500, 29491.0 },

    { (3217.0 / DEFAULT_SRATE), 0.0017, 1.110, 22937.0 },
    { (3557.0 / DEFAULT_SRATE), 0.0006, 3.973,  9830.0 },

    { (3907.0 / DEFAULT_SRATE), 0.0010, 2.341, 20643.0 },
    { (4127.0 / DEFAULT_SRATE), 0.0011, 1.897, 22937.0 },

    { (2143.0 / DEFAULT_SRATE), 0.0017, 0.891, 29491.0 },
    { (1933.0 / DEFAULT_SRATE), 0.0006, 3.221, 14417.0 }
};

static int delay_line_max_samples(SPFLOAT sr, SPFLOAT iPitchMod, int n);
static int init_delay_line(sp_revscm *p, sp_revsc_dl *lp, int n);
static int delay_line_bytes_alloc(SPFLOAT sr, SPFLOAT iPitchMod, int n);
static const SPFLOAT outputGain  = 0.35;
static const SPFLOAT jpScale     = 0.50;

int sp_revscm_create(sp_revscm **p){
    *p = malloc(sizeof(sp_revscm));
    return SP_OK;
}

int sp_revscm_init(sp_data *sp, sp_revscm *p)
{
    p->iSampleRate = sp->sr;
    p->sampleRate = sp->sr;
    p->feedback = 0.97;
    p->lpfreq = 10000;
    p->iPitchMod = 1;
    p->iSkipInit = 0;
    p->dampFact = 1.0;
    p->prv_LPFreq = 0.0;
    p->initDone = 1;
    int i, nBytes = 0;
    for(i = 0; i < NDELAYLINES; i++){
        nBytes += delay_line_bytes_alloc(sp->sr, 1, i);
    }
    sp_auxdata_alloc(&p->aux, nBytes);
    nBytes = 0;
    for (i = 0; i < NDELAYLINES; i++) {
        p->delayLines[i].buf = (p->aux.ptr) + nBytes;
        init_delay_line(p, &p->delayLines[i], i);
        nBytes += delay_line_bytes_alloc(sp->sr, 1, i);
    }

    return SP_OK;
}


int sp_revscm_destroy(sp_revscm **p)
{
    sp_revscm *pp = *p;
    sp_auxdata_free(&pp->aux);
    free(*p);
    return SP_OK;
}

static int delay_line_max_samples(SPFLOAT sr, SPFLOAT iPitchMod, int n)
{
    SPFLOAT maxDel;

    maxDel = reverbParams[n][0];
    maxDel += (reverbParams[n][1] * (SPFLOAT) iPitchMod * 1.125);
    return (int) (maxDel * sr + 16.5);
}

static int delay_line_bytes_alloc(SPFLOAT sr, SPFLOAT iPitchMod, int n)
{
    int nBytes = 0;

    nBytes += (delay_line_max_samples(sr, iPitchMod, n) * (int) sizeof(SPFLOAT));
    return nBytes;
}

static void next_random_lineseg(sp_revscm *p, sp_revsc_dl *lp, int n)
{
    SPFLOAT prvDel, nxtDel, phs_incVal;

    /* update random seed */
    if (lp->seedVal < 0)
      lp->seedVal += 0x10000;
    lp->seedVal = (lp->seedVal * 15625 + 1) & 0xFFFF;
    if (lp->seedVal >= 0x8000)
      lp->seedVal -= 0x10000;
    /* length of next segment in samples */
    lp->randLine_cnt = (int) ((p->sampleRate / reverbParams[n][2]) + 0.5);
    prvDel = (SPFLOAT) lp->writePos;
    prvDel -= ((SPFLOAT) lp->readPos
               + ((SPFLOAT) lp->readPosFrac / (SPFLOAT) DELAYPOS_SCALE));
    while (prvDel < 0.0)
      prvDel += lp->bufferSize;
    prvDel = prvDel / p->sampleRate;    /* previous delay time in seconds */
    nxtDel = (SPFLOAT) lp->seedVal * reverbParams[n][1] / 32768.0;
    /* next delay time in seconds */
    nxtDel = reverbParams[n][0] + (nxtDel * (SPFLOAT) p->iPitchMod);
    /* calculate phase increment per sample */
    phs_incVal = (prvDel - nxtDel) / (SPFLOAT) lp->randLine_cnt;
    phs_incVal = phs_incVal * p->sampleRate + 1.0;
    lp->readPosFrac_inc = (int) (phs_incVal * DELAYPOS_SCALE + 0.5);
}

static int init_delay_line(sp_revscm *p, sp_revsc_dl *lp, int n)
{
    SPFLOAT readPos;
    /* int     i; */

    /* calculate length of delay line */
    lp->bufferSize = delay_line_max_samples(p->sampleRate, 1, n);
    lp->dummy = 0;
    lp->writePos = 0;
    /* set random seed */
    lp->seedVal = (int) (reverbParams[n][3] + 0.5);
    /* set initial delay time */
    readPos = (SPFLOAT) lp->seedVal * reverbParams[n][1] / 32768;
    readPos = reverbParams[n][0] + (readPos * (SPFLOAT) p->iPitchMod);
    readPos = (SPFLOAT) lp->bufferSize - (readPos * p->sampleRate);
    lp->readPos = (int) readPos;
    readPos = (readPos - (SPFLOAT) lp->readPos) * (SPFLOAT) DELAYPOS_SCALE;
    lp->readPosFrac = (int) (readPos + 0.5);
    /* initialise first random line segment */
    next_random_lineseg(p, lp, n);
    /* clear delay line to zero */
    lp->filterState = 0.0;
    memset(lp->buf, 0, sizeof(SPFLOAT) * lp->bufferSize);
    return SP_OK;
}


int sp_revscm_compute(sp_data *sp, sp_revscm *p, SPFLOAT *in, SPFLOAT *out)
{
    SPFLOAT ain, aout;
    SPFLOAT vm1, v0, v1, v2, am1, a0, a1, a2, frac;
    sp_revsc_dl *lp;
    int readPos;
    uint32_t n;
    int bufferSize; /* Local copy */
    SPFLOAT dampFact = p->dampFact;

    if (p->initDone <= 0) return SP_NOT_OK;

    /* calculate tone filter coefficient if frequency changed */

    if (p->lpfreq != p->prv_LPFreq) {
        p->prv_LPFreq = p->lpfreq;
        dampFact = 2.0 - cos(p->prv_LPFreq * (2 * M_PI) / p->sampleRate);
        dampFact = p->dampFact = dampFact - sqrt(dampFact * dampFact - 1.0);
    }

    /* calculate "resultant junction pressure" and mix to input signals */

    ain = aout = 0.0;
    for (n = 0; n < NDELAYLINES; n++) {
        ain += p->delayLines[n].filterState;
    }
    ain *= jpScale;
    ain = ain + *in;

    /* loop through all delay lines */

    for (n = 0; n < NDELAYLINES; n++) {
        lp = &p->delayLines[n];
        bufferSize = lp->bufferSize;

        /* send input signal and feedback to delay line */

        lp->buf[lp->writePos] = (SPFLOAT) (ain - lp->filterState);
        if (++lp->writePos >= bufferSize) {
            lp->writePos -= bufferSize;
        }

        /* read from delay line with cubic interpolation */

        if (lp->readPosFrac >= DELAYPOS_SCALE) {
            lp->readPos += (lp->readPosFrac >> DELAYPOS_SHIFT);
            lp->readPosFrac &= DELAYPOS_MASK;
        }
        if (lp->readPos >= bufferSize)
        lp->readPos -= bufferSize;
        readPos = lp->readPos;
        frac = (SPFLOAT) lp->readPosFrac * (1.0 / (SPFLOAT) DELAYPOS_SCALE);

        /* calculate interpolation coefficients */

        a2 = frac * frac; a2 -= 1.0; a2 *= (1.0 / 6.0);
        a1 = frac; a1 += 1.0; a1 *= 0.5; am1 = a1 - 1.0;
        a0 = 3.0 * a2; a1 -= a0; am1 -= a2; a0 -= frac;

        /* read four samples for interpolation */

        if (readPos > 0 && readPos < (bufferSize - 2)) {
            vm1 = (SPFLOAT) (lp->buf[readPos - 1]);
            v0  = (SPFLOAT) (lp->buf[readPos]);
            v1  = (SPFLOAT) (lp->buf[readPos + 1]);
            v2  = (SPFLOAT) (lp->buf[readPos + 2]);
        }
        else {

        /* at buffer wrap-around, need to check index */

        if (--readPos < 0) readPos += bufferSize;
            vm1 = (SPFLOAT) lp->buf[readPos];
        if (++readPos >= bufferSize) readPos -= bufferSize;
            v0 = (SPFLOAT) lp->buf[readPos];
        if (++readPos >= bufferSize) readPos -= bufferSize;
            v1 = (SPFLOAT) lp->buf[readPos];
        if (++readPos >= bufferSize) readPos -= bufferSize;
            v2 = (SPFLOAT) lp->buf[readPos];
        }
        v0 = (am1 * vm1 + a0 * v0 + a1 * v1 + a2 * v2) * frac + v0;

        /* update buffer read position */

        lp->readPosFrac += lp->readPosFrac_inc;

        /* apply feedback gain and lowpass filter */

        v0 *= (SPFLOAT) p->feedback;
        v0 = (lp->filterState - v0) * dampFact + v0;
        lp->filterState = v0;

        /* mix to output */

        aout += v0;

        /* start next random line segment if current one has reached endpoint */

        if (--(lp->randLine_cnt) <= 0) {
            next_random_lineseg(p, lp, n);
        }
    }

    *out  = aout * outputGain;
    return SP_OK;
}
