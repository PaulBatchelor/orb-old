#include <stdlib.h>
#include "nanovg.h"
#include "orb.h"

#include "soundpipe.h"
#include "synth.h"

void orb_synth_create(void *ud, orb_data *orb)
{
    sp_data *sp = ud; 
    orb->synth = malloc(sizeof(sp_synth));
    sp_synth_create(sp, orb->synth);
    sp_synth_init(sp, orb->synth);
}

void orb_synth_destroy(orb_data *orb)
{
    orb->synth = malloc(sizeof(sp_synth));
}

void orb_synth_set_vals(orb_data *orb)
{
    sp_synth *synth = orb->synth;
    synth->gate = (synth->gate == 0) ? 1 : 0;
}
