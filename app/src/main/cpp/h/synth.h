#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_rspline *lfo3;
    sp_trand *tr;
    sp_butlp *lpf;
    sp_port *port1;
    sp_modal *mode;
    sp_fosc *voice2;
    SPFLOAT gate;
    SPFLOAT freq3;
    sp_port *port3;
    SPFLOAT freq1;
    sp_thresh *thresh;
    SPFLOAT freq2;
    sp_critter *critter;
    sp_rspline *lfo1;
    sp_tenv *env;
    sp_randh *randh;
    sp_port *port2;
    sp_delay *delay;
    sp_fosc *voice3;
    sp_rspline *lfo2;
    sp_revscm *rev;
    sp_butlp *noisefilt;
    sp_ftbl *sine;
    sp_fosc *voice1;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
