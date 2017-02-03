#include <stdlib.h>
#include "nanovg.h"
#include "orb.h"
#include "soundpipe.h"
#include "synth.h"

#ifndef MAX
#define MAX(A, B) (A > B ? A : B)
#endif

#ifndef MIN
#define MIN(A, B) (A < B ? A : B)
#endif

#ifndef CLAMP
#define CLAMP(N, A, B) (MIN(MAX(N, A), B))
#endif

static sp_data *sp = NULL;

void orb_audio(orb_data *orb, float **buf, int nframes)
{
    int i, c;
    int chan;
    sp_synth_computef(sp, orb->synth, nframes, buf[0]);
    for(i = 0; i < nframes; i++) {
        buf[1][i] = buf[0][i];
    }
}

void orb_audio_computei(orb_data *orb, short *buf, int nframes)
{
    sp_synth_computei(sp, orb->synth, nframes, buf);
}

void orb_audio_create(orb_data *orb, int sr)
{
    orb->synth = malloc(sizeof(sp_synth));
    sp_create(&sp);
    sp->sr = sr;
    sp_synth_create(sp, orb->synth);
    sp_synth_init(sp, orb->synth);
}

void orb_audio_destroy(orb_data *orb)
{
    sp_synth_destroy(sp, orb->synth);
}

void orb_synth_set_vals(orb_data *orb)
{
    sp_synth *synth = orb->synth;
    synth->pos_x = CLAMP(orb->x_pos, 0, 1);
    synth->pos_y = CLAMP(orb->y_pos, 0, 1);
}
