#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_fosc *voice2;
    SPFLOAT freq2;
    sp_revscm *rev;
    sp_modal *mode;
    sp_port *port2;
    sp_rspline *lfo2;
    sp_delay *delay;
    sp_ftbl *sine;
    sp_fosc *voice3;
    sp_trand *tr;
    sp_thresh *thresh;
    sp_randh *randh;
    sp_port *port1;
    sp_butlp *lpf;
    sp_fosc *voice1;
    sp_rspline *lfo1;
    SPFLOAT freq3;
    sp_rspline *lfo3;
    SPFLOAT freq1;
    sp_tenv *env;
    sp_port *port3;
    sp_critter *critter;
    SPFLOAT gate;
    sp_butlp *noisefilt;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
