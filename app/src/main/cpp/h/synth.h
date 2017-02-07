#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_tenvx *env;
    sp_thresh *thresh;
    sp_fosc *fm;
    sp_trand *tr;
    sp_ftbl *sine;
    sp_revsc *rev;
    SPFLOAT gate;
    sp_butlp *lpf;
    sp_delay *delay;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
