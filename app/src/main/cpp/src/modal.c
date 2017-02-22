#include <stdlib.h>
#include "soundpipe.h"
#include "modal.h"

int sp_modal_create(sp_modal **m)
{
    int i;
    *m = malloc(sizeof(sp_modal));
    for(i = 0; i < 4; i++) {
        sp_mode_create(&(*m)->mode[i]);
    }
    return SP_OK;
}

int sp_modal_destroy(sp_modal **m)
{
    int i;
    for(i = 0; i < 4; i++) {
        sp_mode_destroy(&(*m)->mode[i]);
    }
    free(*m);
    return SP_OK;
}

int sp_modal_init(sp_data *sp, sp_modal *m)
{
    int i;
    m->type = 1;
    m->mat[0] = m->metal;
    m->mat[1] = m->plastic;

    m->metal[0] = 1000;
    m->metal[1] = 1000;
    m->metal[2] = 1800;
    m->metal[3] = 1800;
    m->metal[4] = 440;
    m->metal[5] = 30;
    m->metal[6] = 882;
    m->metal[7] = 13;
    
    m->plastic[0] = 700;
    m->plastic[1] = 10;
    m->plastic[2] = 1800;
    m->plastic[3] = 2;
    m->plastic[4] = 440;
    m->plastic[5] = 60;
    m->plastic[6] = 630;
    m->plastic[7] = 53;
    

    for(i = 0; i < 4; i++) {
        sp_mode_init(sp, m->mode[i]);
        m->mode[i]->freq = m->plastic[2 * i];
        m->mode[i]->q = m->plastic[2 * i + 1];
    }
    m->gate = 0;
    m->prev = 0;
    m->amp = 0;
    return SP_OK;
}

static void compute_q(sp_data *sp, sp_modal *m)
{
    m->mode[0]->q = m->q[0];
    m->mode[1]->q = m->q[1];
    m->mode[2]->q = m->q[2];
    m->mode[3]->q = m->q[3];
}

int sp_modal_compute(sp_data *sp, sp_modal *m, SPFLOAT *in, SPFLOAT *out)
{
    SPFLOAT trig;
    SPFLOAT tmp;
    SPFLOAT exc;
    SPFLOAT col;

    if(m->gate != m->prev) {
        trig = m->amp;
    } else {
        trig = 0.0;
    }

    m->prev = m->gate;

    //compute_q(sp, m);
    exc = 0;
    sp_mode_compute(sp, m->mode[0], &trig, &tmp);
    exc = tmp;
    sp_mode_compute(sp, m->mode[1], &trig, &tmp);
    exc += tmp;
    exc *= 0.3;

    col = 0;
    sp_mode_compute(sp, m->mode[2], &exc, &tmp);
    col = tmp;
    sp_mode_compute(sp, m->mode[3], &exc, &tmp);
    col += tmp;

    col *= 0.1;
    
    *out = col;
    return SP_OK;
}

void sp_modal_scale(sp_modal *m, SPFLOAT amp)
{
    SPFLOAT *mat = m->mat[m->type];
    m->mode[0]->q = 2 + mat[1] * amp;
    m->mode[1]->q = 8 + mat[3] * amp;
    m->mode[2]->q = 30 + mat[5] * amp;
    m->mode[3]->q = 13 + mat[7] * amp;
}

void sp_modal_type(sp_modal *m, int type)
{
    SPFLOAT *mat;
    int i;
    
    mat = m->mat[type];
    m->type = type;
    for(i = 0; i < 4; i++) {
        m->mode[i]->freq = mat[2 * i];
        m->mode[i]->q = mat[(2 * i) + 1];
    }
}
