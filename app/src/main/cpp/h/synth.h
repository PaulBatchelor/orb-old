#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    SPFLOAT gate;
    sp_butlp *lpf;
    sp_critter *critter;
    SPFLOAT freq3;
    sp_rspline *lfo3;
    sp_modal *mode;
    sp_randh *randh;
    sp_trand *tr;
    sp_tenv *env;
    sp_thresh *thresh;
    sp_ftbl *sine;
    sp_fosc *voice3;
    sp_rspline *lfo1;
    sp_fosc *voice1;
    SPFLOAT freq2;
    sp_fosc *voice2;
    sp_butlp *noisefilt;
    sp_rspline *lfo2;
    sp_port *port3;
    sp_port *port1;
    SPFLOAT freq1;
    sp_revscm *rev;
    sp_port *port2;
    sp_delay *delay;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
