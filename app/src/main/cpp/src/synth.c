#include <math.h>
#include "soundpipe.h"
#include "synth.h"
int sp_synth_create(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_create(&synth->env);
    sp_thresh_create(&synth->thresh);
    sp_fosc_create(&synth->fm);
    sp_trand_create(&synth->tr);
    sp_ftbl_create(sp, &synth->sine, 4096);
    sp_revsc_create(&synth->rev);
    sp_butlp_create(&synth->lpf);
    sp_delay_create(&synth->delay);
    return SP_OK;
}

int sp_synth_destroy(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_destroy(&synth->env);
    sp_thresh_destroy(&synth->thresh);
    sp_fosc_destroy(&synth->fm);
    sp_trand_destroy(&synth->tr);
    sp_ftbl_destroy(&synth->sine);
    sp_revsc_destroy(&synth->rev);
    sp_butlp_destroy(&synth->lpf);
    sp_delay_destroy(&synth->delay);
    return SP_OK;
}

int sp_synth_init(sp_data *sp, sp_synth *synth)
{
    sp_tenvx_init(sp, synth->env);
    synth->env->atk = 0.001;
    synth->env->hold = 0.002;
    synth->env->rel = 0.001;
    sp_thresh_init(sp, synth->thresh);
    synth->thresh->thresh = 0.5;
    synth->thresh->mode = 2;
    sp_gen_sine(sp, synth->sine);
    sp_fosc_init(sp, synth->fm, synth->sine);
    synth->fm->freq = 440;
    synth->fm->amp = 0.4;
    synth->fm->car = 1;
    synth->fm->mod = 1;
    synth->fm->indx = 1;
    sp_trand_init(sp, synth->tr);
    synth->tr->min = 200;
    synth->tr->max = 4000;
    sp_revsc_init(sp, synth->rev);
    synth->rev->feedback = 0.97;
    synth->rev->lpfreq = 10000;
    synth->gate = 0;
    sp_butlp_init(sp, synth->lpf);
    synth->lpf->freq = 1000;
    sp_delay_init(sp, synth->delay, 0.8);
    synth->delay->feedback = 0.7;
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
        sp_delay_compute(sp, synth->delay, &tmp[5], &tmp[2]);
        sp_butlp_compute(sp, synth->lpf, &tmp[2], &tmp[4]);
        tmp[4] *= 0.6;
        tmp[5] = tmp[5] + tmp[4];
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
        sp_delay_compute(sp, synth->delay, &tmp[5], &tmp[2]);
        sp_butlp_compute(sp, synth->lpf, &tmp[2], &tmp[4]);
        tmp[4] *= 0.6;
        tmp[5] = tmp[5] + tmp[4];
        sp_revsc_compute(sp, synth->rev, &tmp[5], &tmp[5], &tmp[4], &tmp[2]);
        tmp[4] *= 0.1;
        tmp[5] = tmp[5] + tmp[4];
        buf[i] = tmp[5] * 32767;
    }
    return SP_OK;
}
