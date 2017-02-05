#include <math.h>
#include "soundpipe.h"
#include "synth.h"
int sp_synth_create(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_create(&synth->env);
    sp_fosc_create(&synth->fm);
    sp_thresh_create(&synth->thresh);
    sp_trand_create(&synth->tr);
    sp_revsc_create(&synth->rev);
    sp_ftbl_create(sp, &synth->sine, 4096);
    return SP_OK;
}

int sp_synth_destroy(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_destroy(&synth->env);
    sp_fosc_destroy(&synth->fm);
    sp_thresh_destroy(&synth->thresh);
    sp_trand_destroy(&synth->tr);
    sp_revsc_destroy(&synth->rev);
    sp_ftbl_destroy(&synth->sine);
    return SP_OK;
}

int sp_synth_init(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_init(sp, synth->env);
    synth->env->atk = 0.005;
    synth->env->hold = 0.01;
    synth->env->rel = 0.04;
    sp_fosc_init(sp, synth->fm, synth->sine);
    synth->fm->freq = 440;
    synth->fm->amp = 0.1;
    synth->fm->car = 1;
    synth->fm->mod = 1;
    synth->fm->indx = 1;
    sp_gen_sine(sp, synth->sine);
    sp_thresh_init(sp, synth->thresh);
    synth->thresh->thresh = 0.5;
    synth->thresh->mode = 2;
    synth->gate = 0;
    sp_trand_init(sp, synth->tr);
    synth->tr->min = 200;
    synth->tr->max = 2000;
    sp_revsc_init(sp, synth->rev);
    synth->rev->feedback = 0.96;
    synth->rev->lpfreq = 10000;
    return SP_OK;
}

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf)
{
    SPFLOAT tmp[10];
    int i;
    for(i = 0; i < bufsize ; i++) {
        sp_thresh_compute(sp, synth->thresh, &synth->gate, &tmp[1]);
        sp_trand_compute(sp, synth->tr, &tmp[1], &tmp[5]);
        synth->fm->freq = tmp[5];
        sp_fosc_compute(sp, synth->fm, NULL, &tmp[3]);
        sp_tenvx_compute(sp, synth->env, &tmp[1], &tmp[0]);
        tmp[5] = tmp[0] * tmp[3];
        sp_revsc_compute(sp, synth->rev, &tmp[5], &tmp[5], &tmp[4], &tmp[2]);
        tmp[4] *= 0.1;
        tmp[5] = tmp[5] + tmp[4];
        buf[i] = tmp[5];
    }
    return SP_OK;
}

int sp_synth_computei(sp_data *sp, sp_synth *synth, int bufsize, short *buf)
{
    SPFLOAT tmp[10];
    int i;
    for(i = 0; i < bufsize ; i++) {
        sp_thresh_compute(sp, synth->thresh, &synth->gate, &tmp[1]);
        sp_trand_compute(sp, synth->tr, &tmp[1], &tmp[5]);
        synth->fm->freq = tmp[5];
        sp_fosc_compute(sp, synth->fm, NULL, &tmp[3]);
        sp_tenvx_compute(sp, synth->env, &tmp[1], &tmp[0]);
        tmp[5] = tmp[0] * tmp[3];
        sp_revsc_compute(sp, synth->rev, &tmp[5], &tmp[5], &tmp[4], &tmp[2]);
        tmp[4] *= 0.1;
        tmp[5] = tmp[5] + tmp[4];
        buf[i] = tmp[5] * 32767;
    }
    return SP_OK;
}
