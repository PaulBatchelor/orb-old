#include <stdlib.h>
#include <math.h>

#include "nanovg.h"
#include "orb.h"

#include "soundpipe.h"
#include "modal.h"
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

void orb_synth_collide(orb_data *orb, orb_object *obj)
{
    sp_synth *synth;
    sp_modal *m;
    orb_motion *motion;
    SPFLOAT amp;

    synth = orb->synth;
    m = synth->mode;
    motion = &orb->motion;

    amp = ((fabs(motion->vel_y) + fabs(motion->vel_x)) * 0.5) * 0.2;
        
    LOGI("amp: %g\n", amp);
    m->amp = amp;


    if(obj->type == ORB_SQUARE) {
        sp_modal_type(m, 0);
    } else {
        sp_modal_type(m, 1);
    }

    sp_modal_scale(m, amp);
    if(m->gate == 0) {
        m->gate = 1;
    } else {
        m->gate = 0;
    }

}
