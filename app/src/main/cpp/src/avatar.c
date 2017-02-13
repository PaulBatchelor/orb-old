#include <math.h>

#include "nanovg.h"
#include "orb.h"

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

    amt = 4.0;
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
