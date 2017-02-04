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
