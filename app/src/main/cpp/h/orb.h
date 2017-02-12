#ifndef ORB_H 
#define ORB_H

#include <sys/time.h>

#ifndef SP_SYNTH_STRUCT
#define SP_SYNTH_STRUCT
typedef struct sp_synth sp_synth;
#endif

#define CSTACK_MAX 8
#define OBJECTS_MAX 8

typedef struct orb_data orb_data;

enum {
ORB_SQUARE
};

typedef struct {
    double x_pos;
    double y_pos;
} orb_mouse;

typedef struct {
    double x_pos;
    double y_pos;
    double radius;
    double phs;
    double env;
} orb_avatar;

typedef struct {
    float vel_x;
    float vel_y;
    float acc;
} orb_motion;

typedef struct {
    double x;
    double y;
    double alpha;
    double radius;
} orb_circle;

typedef struct {
    int ncirc;
    int pos;
    double irad;
    double growth;
    double decay;
    orb_circle circ[CSTACK_MAX];
} orb_cstack;

typedef struct {
    int type;
    double x;
    double y;
    int next;
} orb_object;

typedef void (orb_draw) (NVGcontext *vg, orb_data *, orb_object *);

typedef struct {
    orb_object obj[OBJECTS_MAX];
    orb_draw *draw[3];
    int start;
    int nobjects;
    int nextfree;
} orb_object_list;

struct orb_data {
    double x_pos;
    double y_pos;
    int width; 
    int height;
    sp_synth *synth;
    NVGcolor color1;
    NVGcolor color2;
    orb_mouse mouse;
    orb_avatar av;
    int grid_size;
    int bias;
    orb_motion motion;
    orb_cstack cstack;
    struct timeval tv;
    double dtime;
    orb_object_list list;
};


void orb_step(NVGcontext *vg, orb_data *orb);
void orb_audio(orb_data *orb, float **buf, int nframes);
void orb_audio_computei(orb_data *orb, short *buf, int nframes);
void orb_audio_create(orb_data *orb, int sr);
void orb_audio_destroy(orb_data *orb);
void orb_set_vals(orb_data *orb);
void orb_synth_set_vals(orb_data *orb);
void orb_init(orb_data *orb, int sr);
void orb_destroy(orb_data *orb);
void orb_poke(orb_data *orb);

/* orb avatar */

void orb_avatar_init(orb_data *orb, orb_avatar *av);
void orb_avatar_step(NVGcontext *vg, orb_data *orb, orb_avatar *av);
orb_data * orb_data_create();

void orb_synth_destroy(orb_data *orb);
void orb_synth_create(void *ud, orb_data *orb);

/* grid */

void orb_grid_calculate(orb_data *orb);
double orb_grid_size(orb_data *orb);
double orb_grid_x(orb_data *orb, double n);
double orb_grid_y(orb_data *orb, double n);

/* motion */

void orb_motion_init(orb_data *orb, orb_motion *m);
void orb_motion_step(orb_data *orb, 
    orb_motion *m,
    double *x,
    double *y);
void orb_motion_add_force(orb_data *orb, orb_motion *m, double vx, double vy);
void orb_motion_set_acceleration(orb_data *orb, orb_motion *m, double acc);
void orb_motion_bounce_edges(orb_data *orb, orb_motion *m, 
    double *x, double *y, double r);

double orb_dtime(orb_data *orb);

/* circle stack */

void orb_cstack_init(orb_data *orb, orb_cstack *stack);
void orb_cstack_add(orb_data *orb, orb_cstack *stack, double x, double y);
void orb_cstack_display(NVGcontext *vg, orb_data *orb, orb_cstack *stack);

/* objects */

void orb_object_set(orb_data *orb, orb_object *obj, int x, int y, int type);
void orb_object_draw(NVGcontext *vg, orb_data *orb, orb_object *obj);

void orb_object_list_init(orb_data *orb, orb_object_list *list);
void orb_object_list_draw(NVGcontext *vg, orb_data *orb, orb_object_list *list);

int orb_object_new(orb_data *orb, orb_object_list *list, orb_object **obj);
int orb_object_add_square(orb_data *orb, orb_object_list *list, int x, int y);

#endif
