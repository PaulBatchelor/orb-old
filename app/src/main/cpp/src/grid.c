#include "nanovg.h"
#include "orb.h"

double orb_grid_toreal(orb_data *orb, double n)
{
    return n * orb->grid_size;
}

void orb_grid_calculate(orb_data *orb)
{
    orb->grid_size = orb->width / 16;
}

double orb_grid_size(orb_data *orb)
{
    return orb->grid_size;
}
