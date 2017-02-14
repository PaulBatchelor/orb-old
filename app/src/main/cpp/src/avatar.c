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
    av->ir = av->radius;
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

void orb_avatar_collisions(orb_data *orb, 
    orb_object_list *list, 
    orb_avatar *av)
{
    int top;
    int bottom;
    int left;
    int right;
    int id;
    int x;
    int y;

    id = orb_avatar_find(orb, av, &x, &y);
    orb_grid_bounds_detection(orb, x, y, &top, &bottom, &left, &right);

    if(orb_avatar_check_collision(orb, list, av, id)) {
        /* if we are right *on* the square, trouble is to be had... */
        return;
    }

    if(!top) {
        orb_avatar_check_collision(orb, list, av, id - GRID_WIDTH);
        if(!left) {
            orb_avatar_check_collision(orb, list, av, id - GRID_WIDTH - 1);
        }
        if(!right) {
            orb_avatar_check_collision(orb, list, av, id - GRID_WIDTH + 1);
        }

    }

    if(!bottom) {
        orb_avatar_check_collision(orb, list, av, id + GRID_WIDTH);
        if(!left) {
            orb_avatar_check_collision(orb, list, av, id + GRID_WIDTH - 1);
        }
        if(!right) {
            orb_avatar_check_collision(orb, list, av, id + GRID_WIDTH + 1);
        }
    }

    if(!left) {
        orb_avatar_check_collision(orb, list, av, id - 1);
    }

    if(!right) {
        orb_avatar_check_collision(orb, list, av, id + 1);
    }
}

static double euclid(double x_a, double y_a, double x_b, double y_b)
{
    return sqrt((x_b - x_a)*(x_b - x_a) + (y_b - y_a) * (y_b - y_a));
}

int orb_avatar_check_collision(orb_data *orb, 
    orb_object_list *list, 
    orb_avatar *av,
    int pos)
{
    double x_a;
    double y_a;

    double x_b;
    double y_b;

    double dist;
    orb_object *obj;
    int id;
    char collision = 0;
    int x, y;
    double grid_size;
    double m;
    double tmp;
    double d;
    orb_motion *motion;

    motion = &orb->motion;
    /* first, see if there is anything. */
    id = orb_object_list_get(orb, list, &obj, pos);
    if(id < 0) {
        return 0;
    }

    /* avatar coordinates */ 
    x_a = av->x_pos;
    y_a = av->y_pos;

    /* block coordinates */

    grid_size = orb_grid_size(orb);
    for(y = 0; y < 2; y++) {
        for(x = 0; x < 2; x++) {
            x_b = orb_grid_x(orb, obj->x + x);
            y_b = orb_grid_y(orb, obj->y + y);
            dist = euclid(x_a, y_a, x_b, y_b);
            if(dist < av->ir) {
                d = 0.09 * grid_size;
                tmp = sqrt(motion->vel_x * motion->vel_x + 
                        motion->vel_y * motion->vel_y);


                if(tmp == 0) {
                    orb_motion_repel(orb, motion, 1.0);
                } else {
                    m = sqrt(2.0 * (d*d)) / tmp;
                    av->x_pos += -m * motion->vel_x;
                    av->y_pos += -m * motion->vel_y;
                }

                collision = 1;
                break;
            }
        }
    }

    /* if within circle radius, it's a collision */
        if(collision) { 
            orb_collide(orb, list, av, obj, id);
            return 1;
        }
    return 0;
}

int orb_avatar_find(orb_data *orb, orb_avatar *av, int *x, int *y)
{
    double xr;
    double yr;
    int id;
    double g;

    xr = av->x_pos;
    yr = av->y_pos - orb->bias;

    g = orb_grid_size(orb);
    *x = floor(xr / g);
    *y = floor(yr / g);

    id = orb_grid_pos(orb, *x, *y);

    return id;
}

