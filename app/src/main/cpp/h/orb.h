#ifndef SP_SYNTH_STRUCT
#define SP_SYNTH_STRUCT
typedef struct sp_synth sp_synth;
#endif

typedef struct {
    double x_pos;
    double y_pos;
} orb_mouse;

typedef struct {
    int x_pos;
    int y_pos;
} orb_avatar;

typedef struct {
    /* temporary demo data */
    double x_pos;
    double y_pos;
    int width; 
    int height;
    sp_synth *synth;
    NVGcolor color;
    orb_mouse mouse;
    orb_avatar av;
    int grid_size;
} orb_data;

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

double orb_grid_toreal(orb_data *orb, double n);
void orb_grid_calculate(orb_data *orb);
double orb_grid_size(orb_data *orb);


