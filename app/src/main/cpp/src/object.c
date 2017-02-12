#include "nanovg.h"
#include "orb.h"

void orb_object_set(orb_data *orb, orb_object *obj, int x, int y)
{
    obj->type = 0;
    obj->x = orb_grid_x(orb, x);
    obj->y = orb_grid_x(orb, y);
}

void orb_object_draw(NVGcontext *vg, orb_data *orb, orb_object *obj)
{
    nvgBeginPath(vg);
    nvgRect(vg, obj->x, obj->y, orb_grid_size(orb), orb_grid_size(orb)); 
    nvgClosePath(vg);
    nvgFill(vg);
}
