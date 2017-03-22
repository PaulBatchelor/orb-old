#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_port *port3;
    sp_butlp *noisefilt;
    sp_trand *tr;
    sp_port *port1;
    sp_rspline *lfo2;
    sp_revscm *rev;
    sp_butlp *lpf;
    SPFLOAT gate;
    sp_randh *randh;
    sp_modal *mode;
    SPFLOAT freq3;
    sp_rspline *lfo3;
    sp_rspline *lfo1;
    sp_critter *critter;
    sp_tenv *env;
    sp_fosc *voice1;
    SPFLOAT freq2;
    sp_ftbl *sine;
    SPFLOAT freq1;
    sp_delay *delay;
    sp_fosc *voice3;
    sp_port *port2;
    sp_fosc *voice2;
    sp_thresh *thresh;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
