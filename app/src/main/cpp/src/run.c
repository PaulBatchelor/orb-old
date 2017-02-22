#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifdef NANOVG_GLEW
#  include <GL/glew.h>
#endif

#ifdef BUILD_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "nanovg.h"

#include "orb.h"

static void orb_draw_bars(NVGcontext *vg, orb_data *orb)
{
    nvgFillColor(vg, nvgRGB(0, 0, 0));
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, orb->width, orb->bias);
    nvgClosePath(vg);
    nvgFill(vg);
    nvgBeginPath(vg);
    nvgRect(vg, 0, orb->height - orb->bias, orb->width, orb->bias);
    nvgClosePath(vg);
    nvgFill(vg);
}

void orb_step(NVGcontext *vg, orb_data *orb)
{    
    int w = orb->width;
    int h = orb->height;


    glClearColor(orb->color2.r, orb->color2.g, orb->color2.b, 1.0f);

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    orb->dtime = orb_dtime(orb);
    
    orb_avatar_collisions(orb, &orb->list, &orb->av);
    nvgBeginFrame(vg, w, h, 1);


    switch(orb->mode) {
        case ORB_MODE_FADE:
            orb->alpha += orb->fade * orb->dtime;
            if(orb->alpha >= 1.0 && orb->fade > 0) {
                orb->mode = ORB_MODE_PLAY;
                orb->alpha = 1.0;
            } else if(orb->alpha <= 0 && orb->fade < 0) {
                orb->mode = ORB_MODE_EMPTY;
                orb->alpha = 0.0;
                orb->wait = 1.0;
                orb_motion_stop(orb, &orb->motion);
            }
            orb->color1.a = orb->alpha;
        case ORB_MODE_PLAY:
            nvgFillColor(vg, orb->color1);
            orb_object_list_draw(vg, orb, &orb->list);
            orb_cstack_display(vg, orb, &orb->cstack);
            orb_avatar_step(vg, orb, &orb->av);
            break;
        case ORB_MODE_EMPTY:
            if(orb->wait <= 0) {
                orb_level_next(orb);
                orb_level_load(orb);
                orb->mode = ORB_MODE_FADE;
                orb->alpha = 0;
                orb->fade = 0.5;
            } else {
                orb->wait -= orb->dtime;
            }
            break;
    }

    if(orb->bias > 0) orb_draw_bars(vg, orb);

    nvgEndFrame(vg);
}

void orb_init(orb_data *orb, int sr)
{
    orb->x_pos = 0.5;
    orb->y_pos = 0.5;
    orb_audio_create(orb, sr);

    orb_avatar_init(orb, &orb->av);
    orb_motion_init(orb, &orb->motion);
    orb_motion_set_acceleration(orb, &orb->motion, 0.8);

    orb->color1 = nvgRGB(211, 186, 169);
    orb->color2 = nvgRGB(39, 27, 20);

    orb_cstack_init(orb, &orb->cstack);
    gettimeofday(&orb->tv, NULL);
    fsm_create(&orb->fs, 5);

    orb_level_init(orb);

    orb_level_set(orb, 0);

    orb_level_load(orb);

    /* set mode to empty mode */
    orb->mode = ORB_MODE_FADE;

    /* set wait time to 0 seconds */
    orb->wait = 1.0;
    orb->alpha = 0.0;
    orb->fade = 0.5;
}

void orb_destroy(orb_data *orb)
{
    orb_audio_destroy(orb);
    fsm_destroy(&orb->fs);
}

void orb_set_vals(orb_data *orb)
{
}


void orb_poke(orb_data *orb)
{
    if(orb->mode == ORB_MODE_PLAY) { 
        orb_avatar *av = &orb->av;
        double mx = orb->mouse.x_pos * orb->width;
        double my = orb->mouse.y_pos * orb->height;
        double ax = av->x_pos;
        double ay = av->y_pos;

        double distance = sqrt( 
            (ax - mx) * (ax - mx) +
            (ay - my) * (ay - my)
        );

        if(distance <= 3 * orb_grid_size(orb)) {
            orb_cstack_add(orb, &orb->cstack, ax, ay);
            orb_avatar_poke(orb, av, &orb->motion, mx, my, ax, ay);
            orb_synth_set_vals(orb);
        }
    }
}

static int count = 0;
static int chord = 0;
void orb_collide(orb_data *orb, 
    orb_object_list *list, orb_avatar *av, 
    orb_object *obj,
    int pos)
{
    if(obj->type == ORB_SQUARE) {
        orb_motion_repel(orb, &orb->motion, 1.01);
        count++;
        if(count == 4) {
            if(chord == 0) {
                orb_synth_set_notes(orb, 62, 69, 71, 76);
                chord = 1;
            } else {
                orb_synth_set_notes(orb, 60, 67, 62, 74);
                chord = 0;
            }
            count = 0;
        }
    } else {
        orb_motion_repel(orb, &orb->motion, .3);
    }

    orb_synth_collide(orb, obj);
    fsm_compute_state(&orb->fs, obj->fsm_pos);
    orb_fsm_update(orb);

    if(fsm_get_state(&orb->fs) == 0) {
        orb->mode = ORB_MODE_FADE;
        orb->fade = -0.5;
    }
}

void orb_resize(orb_data *orb)
{
    orb->av.ir = orb_grid_size(orb) * 0.7;
    orb->av.radius = orb->av.ir;
    orb->cstack.irad = (double)orb_grid_size(orb);
}
