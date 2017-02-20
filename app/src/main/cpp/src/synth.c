#include <math.h>
#include "soundpipe.h"
#include "modal.h"
#include "revscm.h"
#include "synth.h"
int sp_synth_create(sp_data *sp, sp_synth *synth)
{
    sp_port_create(&synth->port1);
    sp_delay_create(&synth->delay);
    sp_randh_create(&synth->randh);
    sp_tenv_create(&synth->env);
    sp_port_create(&synth->port3);
    sp_revscm_create(&synth->rev);
    sp_modal_create(&synth->mode);
    sp_butlp_create(&synth->lpf);
    sp_thresh_create(&synth->thresh);
    sp_osc_create(&synth->lfo2);
    sp_fosc_create(&synth->voice2);
    sp_trand_create(&synth->tr);
    sp_osc_create(&synth->lfo1);
    sp_fosc_create(&synth->voice3);
    sp_port_create(&synth->port2);
    sp_ftbl_create(sp, &synth->sine, 2048);
    sp_osc_create(&synth->lfo3);
    sp_fosc_create(&synth->voice1);
    return SP_OK;
}

int sp_synth_destroy(sp_data *sp, sp_synth *synth)
{
    sp_port_destroy(&synth->port1);
    sp_delay_destroy(&synth->delay);
    sp_randh_destroy(&synth->randh);
    sp_tenv_destroy(&synth->env);
    sp_port_destroy(&synth->port3);
    sp_revscm_destroy(&synth->rev);
    sp_modal_destroy(&synth->mode);
    sp_butlp_destroy(&synth->lpf);
    sp_thresh_destroy(&synth->thresh);
    sp_osc_destroy(&synth->lfo2);
    sp_fosc_destroy(&synth->voice2);
    sp_trand_destroy(&synth->tr);
    sp_osc_destroy(&synth->lfo1);
    sp_fosc_destroy(&synth->voice3);
    sp_port_destroy(&synth->port2);
    sp_ftbl_destroy(&synth->sine);
    sp_osc_destroy(&synth->lfo3);
    sp_fosc_destroy(&synth->voice1);
    return SP_OK;
}

int sp_synth_init(sp_data *sp, sp_synth *synth)
{
    synth->sp = sp;
    sp_port_init(sp, synth->port1, 0.06);
    sp_delay_init(sp, synth->delay, 1.1);
    synth->delay->feedback = 0.9;
    sp_randh_init(sp, synth->randh);
    synth->randh->min = -0.01;
    synth->randh->max = 0.01;
    synth->randh->freq = 1000;
    synth->freq2 = 880;
    sp_tenv_init(sp, synth->env);
    synth->env->atk = 0.1;
    synth->env->hold = 0.1;
    synth->env->rel = 0.3;
    sp_port_init(sp, synth->port3, 0.08);
    sp_revscm_init(sp, synth->rev);
    synth->rev->feedback = 0.97;
    synth->rev->lpfreq = 10000;
    sp_modal_init(sp, synth->mode);
    sp_butlp_init(sp, synth->lpf);
    synth->lpf->freq = 2000;
    sp_thresh_init(sp, synth->thresh);
    synth->thresh->thresh = 0.5;
    synth->thresh->mode = 2;
    sp_osc_init(sp, synth->lfo2, synth->sine, 0.2);
    synth->lfo2->freq = 0.14;
    synth->lfo2->amp = 1;
    sp_fosc_init(sp, synth->voice2, synth->sine);
    synth->voice2->freq = 440;
    synth->voice2->amp = 0.04;
    synth->voice2->car = 1;
    synth->voice2->mod = 1;
    synth->voice2->indx = 1.5;
    sp_trand_init(sp, synth->tr);
    synth->tr->min = 1000;
    synth->tr->max = 10000;
    sp_osc_init(sp, synth->lfo1, synth->sine, 0);
    synth->lfo1->freq = 0.1;
    synth->lfo1->amp = 1;
    sp_gen_sine(sp, synth->sine);
    synth->gate = 880;
    synth->freq3 = 880;
    sp_fosc_init(sp, synth->voice3, synth->sine);
    synth->voice3->freq = 440;
    synth->voice3->amp = 0.04;
    synth->voice3->car = 1;
    synth->voice3->mod = 1;
    synth->voice3->indx = 1.5;
    sp_port_init(sp, synth->port2, 0.08);
    synth->freq1 = 880;
    sp_osc_init(sp, synth->lfo3, synth->sine, 0.4);
    synth->lfo3->freq = 0.09;
    synth->lfo3->amp = 1;
    sp_fosc_init(sp, synth->voice1, synth->sine);
    synth->voice1->freq = 440;
    synth->voice1->amp = 0.04;
    synth->voice1->car = 1;
    synth->voice1->mod = 1;
    synth->voice1->indx = 1;
    return SP_OK;
}

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf)
{
    SPFLOAT tmp[10];
    int i;
    for(i = 0; i < bufsize ; i++) {
        sp_thresh_compute(sp, synth->thresh, &synth->gate, &tmp[1]);
        sp_trand_compute(sp, synth->tr, &tmp[1], &tmp[5]);
        synth->randh->freq = tmp[5];
        sp_randh_compute(sp, synth->randh, NULL, &tmp[3]);
        sp_tenv_compute(sp, synth->env, &tmp[1], &tmp[0]);
        tmp[5] = tmp[0] * tmp[3];
        sp_delay_compute(sp, synth->delay, &tmp[5], &tmp[2]);
        sp_butlp_compute(sp, synth->lpf, &tmp[2], &tmp[4]);
        tmp[4] *= 0.6;
        tmp[5] = tmp[5] + tmp[4];
        sp_modal_compute(sp, synth->mode, NULL, &tmp[4]);
        tmp[5] = tmp[5] + tmp[4];
        sp_port_compute(sp, synth->port1, &synth->freq1, &tmp[0]);
        synth->voice1->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice1, NULL, &tmp[4]);
        sp_osc_compute(sp, synth->lfo1, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[4] = tmp[4] * tmp[0];
        sp_port_compute(sp, synth->port2, &synth->freq2, &tmp[0]);
        synth->voice2->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice2, NULL, &tmp[1]);
        sp_osc_compute(sp, synth->lfo2, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[1] = tmp[1] * tmp[0];
        tmp[4] = tmp[4] + tmp[1];
        sp_port_compute(sp, synth->port3, &synth->freq3, &tmp[0]);
        synth->voice3->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice3, NULL, &tmp[1]);
        sp_osc_compute(sp, synth->lfo3, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[1] = tmp[1] * tmp[0];
        tmp[4] = tmp[4] + tmp[1];
        tmp[5] = tmp[5] + tmp[4];
        sp_revscm_compute(sp, synth->rev, &tmp[5], &tmp[4]);
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
        synth->randh->freq = tmp[5];
        sp_randh_compute(sp, synth->randh, NULL, &tmp[3]);
        sp_tenv_compute(sp, synth->env, &tmp[1], &tmp[0]);
        tmp[5] = tmp[0] * tmp[3];
        sp_delay_compute(sp, synth->delay, &tmp[5], &tmp[2]);
        sp_butlp_compute(sp, synth->lpf, &tmp[2], &tmp[4]);
        tmp[4] *= 0.6;
        tmp[5] = tmp[5] + tmp[4];
        sp_modal_compute(sp, synth->mode, NULL, &tmp[4]);
        tmp[5] = tmp[5] + tmp[4];
        sp_port_compute(sp, synth->port1, &synth->freq1, &tmp[0]);
        synth->voice1->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice1, NULL, &tmp[4]);
        sp_osc_compute(sp, synth->lfo1, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[4] = tmp[4] * tmp[0];
        sp_port_compute(sp, synth->port2, &synth->freq2, &tmp[0]);
        synth->voice2->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice2, NULL, &tmp[1]);
        sp_osc_compute(sp, synth->lfo2, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[1] = tmp[1] * tmp[0];
        tmp[4] = tmp[4] + tmp[1];
        sp_port_compute(sp, synth->port3, &synth->freq3, &tmp[0]);
        synth->voice3->freq = tmp[0];
        sp_fosc_compute(sp, synth->voice3, NULL, &tmp[1]);
        sp_osc_compute(sp, synth->lfo3, NULL, &tmp[0]);
        tmp[0] = 0+ (0.5 * (tmp[0] + 1)) * 1;
        tmp[1] = tmp[1] * tmp[0];
        tmp[4] = tmp[4] + tmp[1];
        tmp[5] = tmp[5] + tmp[4];
        sp_revscm_compute(sp, synth->rev, &tmp[5], &tmp[4]);
        tmp[4] *= 0.1;
        tmp[5] = tmp[5] + tmp[4];
        buf[i] = tmp[5] * 32767;
    }
    return SP_OK;
}
