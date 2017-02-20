#include <math.h>
#include "soundpipe.h"
#include "modal.h"
#include "synth.h"
int sp_synth_create(sp_data *sp, sp_synth *synth)
{
    sp_fosc_create(&synth->voice4);
    sp_fosc_create(&synth->voice3);
    sp_delay_create(&synth->delay);
    sp_trand_create(&synth->tr);
    sp_fosc_create(&synth->voice1);
    sp_ftbl_create(sp, &synth->sine, 4096);
    sp_butlp_create(&synth->lpf);
    sp_revsc_create(&synth->rev);
    sp_fosc_create(&synth->voice2);
    sp_tenvx_create(&synth->env);
    sp_fosc_create(&synth->fm);
    sp_modal_create(&synth->mode);
    sp_thresh_create(&synth->thresh);
    return SP_OK;
}

int sp_synth_destroy(sp_data *sp, sp_synth *synth)
{
    sp_fosc_destroy(&synth->voice4);
    sp_fosc_destroy(&synth->voice3);
    sp_delay_destroy(&synth->delay);
    sp_trand_destroy(&synth->tr);
    sp_fosc_destroy(&synth->voice1);
    sp_ftbl_destroy(&synth->sine);
    sp_butlp_destroy(&synth->lpf);
    sp_revsc_destroy(&synth->rev);
    sp_fosc_destroy(&synth->voice2);
    sp_tenvx_destroy(&synth->env);
    sp_fosc_destroy(&synth->fm);
    sp_modal_destroy(&synth->mode);
    sp_thresh_destroy(&synth->thresh);
    return SP_OK;
}

int sp_synth_init(sp_data *sp, sp_synth *synth)
{
    sp_fosc_init(sp, synth->voice4, synth->sine);
    synth->voice4->freq = 440;
    synth->voice4->amp = 0.2;
    synth->voice4->car = 1;
    synth->voice4->mod = 1;
    synth->voice4->indx = 1;
    sp_fosc_init(sp, synth->voice3, synth->sine);
    synth->voice3->freq = 440;
    synth->voice3->amp = 0.2;
    synth->voice3->car = 1;
    synth->voice3->mod = 1;
    synth->voice3->indx = 1;
    synth->gate = 0;
    sp_delay_init(sp, synth->delay, 0.8);
    synth->delay->feedback = 0.7;
    sp_trand_init(sp, synth->tr);
    synth->tr->min = 400;
    synth->tr->max = 3000;
    sp_fosc_init(sp, synth->voice1, synth->sine);
    synth->voice1->freq = 440;
    synth->voice1->amp = 0.2;
    synth->voice1->car = 1;
    synth->voice1->mod = 1;
    synth->voice1->indx = 1;
    sp_gen_sine(sp, synth->sine);
    sp_butlp_init(sp, synth->lpf);
    synth->lpf->freq = 1000;
    sp_revsc_init(sp, synth->rev);
    synth->rev->feedback = 0.97;
    synth->rev->lpfreq = 10000;
    sp_fosc_init(sp, synth->voice2, synth->sine);
    synth->voice2->freq = 440;
    synth->voice2->amp = 0.2;
    synth->voice2->car = 1;
    synth->voice2->mod = 1;
    synth->voice2->indx = 1;
    sp_tenvx_init(sp, synth->env);
    synth->env->atk = 0.001;
    synth->env->hold = 0.002;
    synth->env->rel = 0.001;
    sp_fosc_init(sp, synth->fm, synth->sine);
    synth->fm->freq = 440;
    synth->fm->amp = 0.2;
    synth->fm->car = 1;
    synth->fm->mod = 1;
    synth->fm->indx = 1;
    sp_modal_init(sp, synth->mode);
    sp_thresh_init(sp, synth->thresh);
    synth->thresh->thresh = 0.5;
    synth->thresh->mode = 2;
    return SP_OK;
}

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf)
{
    SPFLOAT tmp[10];
    int i;
    for(i = 0; i < bufsize ; i++) {
        sp_fosc_compute(sp, synth->voice1, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice2, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice3, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice4, NULL, &tmp[1]);
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
        sp_modal_compute(sp, synth->mode, NULL, &tmp[4]);
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
        sp_fosc_compute(sp, synth->voice1, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice2, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice3, NULL, &tmp[1]);
        sp_fosc_compute(sp, synth->voice4, NULL, &tmp[1]);
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
        sp_modal_compute(sp, synth->mode, NULL, &tmp[4]);
        tmp[5] = tmp[5] + tmp[4];
        sp_revsc_compute(sp, synth->rev, &tmp[5], &tmp[5], &tmp[4], &tmp[2]);
        tmp[4] *= 0.1;
        tmp[5] = tmp[5] + tmp[4];
        buf[i] = tmp[5] * 32767;
    }
    return SP_OK;
}
