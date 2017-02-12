#include <stdio.h>
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
    nvgBeginFrame(vg, w, h, 1);


    nvgFillColor(vg, orb->color1);
    orb_object_list_draw(vg, orb, &orb->list);

    
    orb_cstack_display(vg, orb, &orb->cstack);
    orb_avatar_step(vg, orb, &orb->av);

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
    orb_object_list_init(orb, &orb->list);
    orb_object_add_square(orb, &orb->list, 4, 4);
    orb_object_add_square(orb, &orb->list, 9, 8);
}

void orb_destroy(orb_data *orb)
{
    orb_audio_destroy(orb);
}

void orb_set_vals(orb_data *orb)
{
}

void orb_avatar_poke(
    orb_data *orb,
    orb_avatar *av,
    orb_motion *m,
    double mx,
    double my,
    double ax,
    double ay) 
{
    double force_x;
    double force_y;

    double x_d;
    double y_d;

    double q_x;
    double q_y;
    double amt;

    amt = 3.0;
    force_x = 0;
    force_y = 0;

    x_d = mx - ax;
    y_d = my - ay;

    /* first, figure out the quandrant */
    if(x_d > 0) {
        if(y_d < 0) {
            q_x = 1;
            q_y = 1;
        } else {
            q_x = 1;
            q_y = -1;
        }
    } else {
        if(y_d < 0) {
            q_x = -1;
            q_y = 1;
        } else {
            q_x = -1;
            q_y = -1;
        }
    }

    force_y = 2 * atan(fabs(y_d) / fabs(x_d)) / M_PI;
    force_x = force_y - 1;

    orb_motion_add_force(orb, m, amt * q_x * force_x, amt * q_y * force_y);
  
    av->env += 1.0;
}

void orb_poke(orb_data *orb)
{
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

void orb_avatar_init(orb_data *orb, orb_avatar *av)
{
    av->x_pos = 0.5 * orb->width;
    av->y_pos = 0.5 * orb->height;
    av->radius = orb_grid_size(orb);
    av->phs = 0;
    av->env = 0;
}

void orb_avatar_step(NVGcontext *vg, orb_data *orb, orb_avatar *av)
{
    orb_motion_step(orb, &orb->motion, &av->x_pos, &av->y_pos);

    if(av->env > 0.001) {
        av->radius = orb_grid_size(orb) + 
            (0.5 * (1 + cos(av->phs)) * orb_grid_size(orb) * 0.04) * av->env;
        av->phs = fmod(av->phs + (av->env * 25.0 * orb->dtime), 2 * M_PI);
        av->env *= pow(0.3, orb->dtime);
    }

    orb_motion_bounce_edges(orb, &orb->motion, 
        &av->x_pos, 
        &av->y_pos, 
        orb_grid_size(orb));

    nvgBeginPath(vg);
    nvgArc(vg, av->x_pos, av->y_pos, av->radius, 0, 2 * M_PI, NVG_CCW);
    nvgClosePath(vg);
    nvgFillColor(vg, orb->color1);
    nvgFill(vg);
}
