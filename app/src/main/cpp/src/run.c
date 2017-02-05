#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

void orb_step(NVGcontext *vg, orb_data *orb)
{    
    int w = orb->width;
    int h = orb->height;

    static float grey = 0.5;

    glClearColor(grey, grey, grey, 1.0f);

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    nvgBeginFrame(vg, w, h, 1);

    nvgBeginPath(vg);

    orb_avatar_step(vg, orb, &orb->av);

    nvgEndFrame(vg);
}

void orb_init(orb_data *orb, int sr)
{
    orb->x_pos = 0.5;
    orb->y_pos = 0.5;
    orb->height = 1;
    orb->width = 1;
    orb->color = nvgRGB(0, 0, 255);
    orb_audio_create(orb, sr);

    orb_avatar_init(orb, &orb->av);
}

void orb_destroy(orb_data *orb)
{
    orb_audio_destroy(orb);
}

void orb_set_vals(orb_data *orb)
{
}

void orb_poke(orb_data *orb)
{
    orb->x_pos = orb->mouse.x_pos;
    orb->y_pos = orb->mouse.y_pos;
    orb->color = nvgRGB(rand() % 256, rand() % 256, rand() % 256);
    orb_synth_set_vals(orb);
}

void orb_avatar_init(orb_data *orb, orb_avatar *av)
{
    av->x_pos = 0;
    av->y_pos = 0;
}

void orb_avatar_step(NVGcontext *vg, orb_data *orb, orb_avatar *av)
{
    int w;
    int h;

    w = orb->width;
    h = orb->height;

    av->x_pos = orb->x_pos * w;
    av->y_pos = orb->y_pos * h;

    nvgArc(vg, av->x_pos, av->y_pos, w * 0.06, 0, 2 * M_PI, NVG_CCW);
    nvgClosePath(vg);
    nvgFillColor(vg, orb->color);
    nvgFill(vg);
}
