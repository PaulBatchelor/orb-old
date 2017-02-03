#include <stdlib.h>
#include "nanovg.h"
#include "orb.h"
#include "soundpipe.h"
#include "synth.h"

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

void orb_set_vals(orb_data *orb)
{
    sp_synth *synth = orb->synth;
    synth->pos_x = orb->x_pos;
    synth->pos_y = orb->y_pos;
}
