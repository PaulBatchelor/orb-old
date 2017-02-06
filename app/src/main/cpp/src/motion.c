#include <stdlib.h>
#include <math.h>
#include "nanovg.h"
#include "orb.h"

void orb_motion_init(orb_data *orb, orb_motion *m)
{
    m->vel_x = 0;
    m->vel_y = 0;
    m->acc = 0;
    gettimeofday(&m->ptime, NULL);
}

void orb_motion_step(orb_data *orb, 
    orb_motion *m,
    double *x,
    double *y)
{
    double dtime = orb_motion_dtime(orb, m);
    double dx = m->vel_x * dtime;
    double dy = m->vel_y * dtime;
    double a = pow(m->acc, dtime);
    
    *x += dx * orb_grid_size(orb);
    *y += dy * orb_grid_size(orb);

    m->vel_x *= a;
    m->vel_y *= a;

}

void orb_motion_add_force(orb_data *orb, orb_motion *m, double vx, double vy)
{
    m->vel_x += vx;
    m->vel_y += vy;
}

double orb_motion_dtime(orb_data *orb, orb_motion *m)
{
    struct timeval newtime;
    double dtime = 0;

    gettimeofday(&newtime, NULL);
    
    dtime += (newtime.tv_sec - m->ptime.tv_sec);
    dtime += (double)(newtime.tv_usec - m->ptime.tv_usec) / 1000000.0;

    m->ptime = newtime;
    return dtime;
}

void orb_motion_set_acceleration(orb_data *orb, orb_motion *m, double acc)
{
    m->acc = acc;
}

void orb_motion_bounce_edges(orb_data *orb, orb_motion *m, 
        double x, double y, double r)
{
    if(x + r> orb->width || x - r < 0) {
        m->vel_x *= -1;
    }

    if(y + r > orb->height - orb->bias || y - r < orb->bias) {
        m->vel_y *= -1;
    }
}
