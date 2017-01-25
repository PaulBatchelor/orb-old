#include <math.h>
#include "soundpipe.h"
#include "synth.h"
int sp_synth_create(sp_data *sp, sp_synth *synth)
{
    sp_ftbl_create(sp, &synth->rnd, 32);
    sp_clock_create(&synth->clk);
    sp_maygate_create(&synth->mt1);
    sp_maygate_create(&synth->mt2);
    sp_tseq_create(&synth->tseq);
    sp_ftbl_create(sp, &synth->sine, 4096);
    sp_fosc_create(&synth->fm);
    sp_tenvx_create(&synth->env1);
    sp_tenvx_create(&synth->env2);
    sp_blsaw_create(&synth->saw);
    sp_moogladder_create(&synth->moog);
    sp_zitarev_create(&synth->rev);
    sp_trand_create(&synth->tr_mod);
    sp_trand_create(&synth->tr_car);
    sp_port_create(&synth->port);
    return SP_OK;
}

int sp_synth_destroy(sp_data *sp, sp_synth *synth)
{
    sp_ftbl_destroy(&synth->rnd);
    sp_clock_destroy(&synth->clk);
    sp_maygate_destroy(&synth->mt1);
    sp_maygate_destroy(&synth->mt2);
    sp_tseq_destroy(&synth->tseq);
    sp_ftbl_destroy(&synth->sine);
    sp_fosc_destroy(&synth->fm);
    sp_tenvx_destroy(&synth->env1);
    sp_tenvx_destroy(&synth->env2);
    sp_blsaw_destroy(&synth->saw);
    sp_moogladder_destroy(&synth->moog);
    sp_zitarev_destroy(&synth->rev);
    sp_trand_destroy(&synth->tr_mod);
    sp_trand_destroy(&synth->tr_car);
    sp_port_destroy(&synth->port);
    return SP_OK;
}

int sp_synth_init(sp_data *sp, sp_synth *synth)
{
    sp_gen_rand(sp, synth->rnd, "0 0.4 7 0.4 12 0.1 5 0.1");
    sp_clock_init(sp, synth->clk);
    synth->clk->bpm = 110;
    synth->clk->subdiv = 4;
    sp_maygate_init(sp, synth->mt1);
    synth->mt1->prob = 0.6;
    synth->mt1->mode = 1;
    sp_maygate_init(sp, synth->mt2);
    synth->mt2->prob = 0.3;
    synth->mt2->mode = 1;
    sp_tseq_init(sp, synth->tseq, synth->rnd);
    synth->tseq->shuf = 1;
    sp_gen_sine(sp, synth->sine);
    sp_fosc_init(sp, synth->fm, synth->sine);
    synth->fm->freq = 440;
    synth->fm->amp = 0.1;
    synth->fm->car = 1;
    synth->fm->mod = 1;
    synth->fm->indx = 1;
    sp_tenvx_init(sp, synth->env1);
    synth->env1->atk = 0.005;
    synth->env1->hold = 0.01;
    synth->env1->rel = 0.04;
    sp_tenvx_init(sp, synth->env2);
    synth->env2->atk = 0.01;
    synth->env2->hold = 0.01;
    synth->env2->rel = 0.1;
    sp_blsaw_init(sp, synth->saw);
    *synth->saw->amp = 0.1;
    sp_moogladder_init(sp, synth->moog);
    synth->moog->freq = 1000;
    synth->moog->res = 0.1;
    sp_zitarev_init(sp, synth->rev);
    *synth->rev->rt60_low = 10;
    *synth->rev->rt60_mid = 10;
    *synth->rev->hf_damping = 10000;
    sp_trand_init(sp, synth->tr_mod);
    synth->tr_mod->min = 0;
    synth->tr_mod->max = 7;
    sp_trand_init(sp, synth->tr_car);
    synth->tr_car->min = 0;
    synth->tr_car->max = 3;
    synth->pos_x = 0.5;
    synth->pos_y = 0.5;
    sp_port_init(sp, synth->port, 0.01);
    return SP_OK;
}

int sp_synth_compute(sp_data *sp, sp_synth *synth, int bufsize, short *buf)
{
    SPFLOAT tmp[9];
    int i;
    for(i = 0; i < bufsize ; i++) {
        tmp[4] = 0;
        synth->clk->bpm = 60 + (synth->pos_x * 160);
        sp_clock_compute(sp, synth->clk, &tmp[4], &tmp[0]);
        synth->mt1->prob = 0.3 + (synth->pos_x * 0.5);
        sp_maygate_compute(sp, synth->mt1, &tmp[0], &tmp[1]);
        sp_tseq_compute(sp, synth->tseq, &tmp[1], &tmp[4]);
        tmp[4] += 48;
        synth->port->htime = 0.03 + (synth->pos_x * -0.0295);
        sp_port_compute(sp, synth->port, &tmp[4], &tmp[2]);
        tmp[4] = sp_midi2cps(tmp[2]);
        synth->fm->freq = tmp[4];
        sp_trand_compute(sp, synth->tr_car, &tmp[1], &tmp[4]);
        tmp[4] = floor(tmp[4]);
        synth->fm->car = tmp[4];
        sp_trand_compute(sp, synth->tr_mod, &tmp[1], &tmp[4]);
        tmp[4] = floor(tmp[4]);
        synth->fm->mod = tmp[4];
        synth->fm->indx = 4 + (synth->pos_y * -4);
        synth->fm->amp = 0.7 + (synth->pos_y * -0.69);
        sp_fosc_compute(sp, synth->fm, NULL, &tmp[4]);
        sp_tenvx_compute(sp, synth->env1, &tmp[1], &tmp[5]);
        tmp[4] = tmp[5] * tmp[4];
        tmp[5] = tmp[2];
        tmp[5] += -12;
        tmp[5] = sp_midi2cps(tmp[5]);
        *synth->saw->freq = tmp[5];
        sp_blsaw_compute(sp, synth->saw, NULL, &tmp[5]);
        sp_moogladder_compute(sp, synth->moog, &tmp[5], &tmp[6]);
        sp_maygate_compute(sp, synth->mt2, &tmp[1], &tmp[5]);
        sp_tenvx_compute(sp, synth->env2, &tmp[5], &tmp[7]);
        tmp[5] = tmp[6] * tmp[7];
        tmp[4] = tmp[4] + tmp[5];
        buf[i] = tmp[4] * 32767;
    }
    return SP_OK;
}
