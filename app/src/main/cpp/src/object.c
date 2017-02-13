#include "nanovg.h"
#include "orb.h"

void orb_object_set(orb_data *orb, orb_object *obj, int x, int y, int type)
{
    obj->type = 0;
    obj->x = x;
    obj->y = y;
    obj->type = type;
}

void orb_object_draw(NVGcontext *vg, orb_data *orb, orb_object *obj)
{
    nvgBeginPath(vg);
    nvgRect(vg, 
        orb_grid_x(orb, obj->x), 
        orb_grid_y(orb, obj->y), 
        orb_grid_size(orb), 
        orb_grid_size(orb)); 
    nvgClosePath(vg);
    nvgFill(vg);
}

void orb_object_list_init(orb_data *orb, orb_object_list *list)
{
    int i;
    list->start = -1;
    list->nobjects = 0;
    list->nextfree = 0;
    list->draw[ORB_SQUARE] = orb_object_draw;
    for(i = 0; i < GRID_SIZE; i++) list->map[i] = 0;
}

void orb_object_list_draw(NVGcontext *vg, orb_data *orb, orb_object_list *list)
{
    int o;
    int id;
    orb_object *obj = list->obj;
    
    id = list->start;
    for(o = 0; o < list->nobjects; o++) {
        list->draw[obj[id].type](vg, orb, &obj[id]); 
        id = obj[id].next;
    }
}

int orb_object_new(orb_data *orb, orb_object_list *list, orb_object **obj)
{
    int id;
    if(list->nobjects > OBJECTS_MAX) return -1;

    id = list->nextfree;
    list->nextfree++;
    *obj = &list->obj[id];

    if(list->start < 0) {
        list->start = id;
    } else {
        (*obj)->next = list->start;
        list->start = id;
    }

    list->nobjects++;

    return id;
}

int orb_object_add_square(orb_data *orb, orb_object_list *list, int x, int y)
{
    orb_object *obj;
    int id;

    id = orb_object_new(orb, list, &obj);
    if(id < 0) return -1;

    orb_object_set(orb, obj, x, y, ORB_SQUARE);
    orb_object_list_map(orb, list, obj);

    return id;
}

void orb_object_list_map(orb_data *orb, orb_object_list *list, orb_object *obj)
{
    int id;

    id = orb_grid_id(orb, obj->x, obj->y);
    list->map[id]++;
}
