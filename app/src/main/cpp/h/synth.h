#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_fosc *voice4;
    sp_fosc *voice3;
    SPFLOAT gate;
    sp_delay *delay;
    sp_trand *tr;
    sp_fosc *voice1;
    sp_ftbl *sine;
    sp_butlp *lpf;
    sp_revsc *rev;
    sp_fosc *voice2;
    sp_tenvx *env;
    sp_fosc *fm;
    sp_modal *mode;
    sp_thresh *thresh;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
