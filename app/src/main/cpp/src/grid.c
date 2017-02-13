#include "nanovg.h"
#include "orb.h"

void orb_grid_calculate(orb_data *orb)
{
    orb->grid_size = orb->width / 16;
    if(orb->height == orb->grid_size * 10) {
        orb->bias = orb->grid_size / 2;
    } else {
        orb->bias = 0;
    }
}

double orb_grid_size(orb_data *orb)
{
    return orb->grid_size;
}

double orb_grid_x(orb_data *orb, double n)
{
    return n * orb->grid_size;
}

double orb_grid_y(orb_data *orb, double n) 
{
    return n * orb->grid_size + orb->bias;
}

int orb_grid_pos(orb_data *orb, int x, int y)
{
    return y * GRID_WIDTH + x;
}

void orb_grid_bounds_detection(orb_data *orb, 
        int x, int y, 
        int *t, int *b, int *l, int *r)
{
    *t = (y == 0);
    *b = (y == GRID_WIDTH - 1);
    *l = (x % GRID_WIDTH == 0);
    *r = (x == GRID_WIDTH -1);
}
