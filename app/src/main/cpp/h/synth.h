#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_ftbl *sine;
    sp_randh *randh;
    SPFLOAT gate;
    sp_modal *mode;
    sp_fosc *voice2;
    SPFLOAT freq2;
    sp_tenv *env;
    sp_thresh *thresh;
    sp_fosc *voice1;
    sp_butlp *lpf;
    sp_port *port3;
    sp_butlp *noisefilt;
    sp_critter *critter;
    sp_delay *delay;
    sp_port *port2;
    sp_rspline *lfo3;
    sp_rspline *lfo2;
    sp_rspline *lfo1;
    SPFLOAT freq3;
    SPFLOAT freq1;
    sp_trand *tr;
    sp_revscm *rev;
    sp_fosc *voice3;
    sp_port *port1;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
