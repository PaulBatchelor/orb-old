#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_revscm *rev;
    sp_port *port1;
    sp_modal *mode;
    sp_ftbl *sine;
    sp_osc *lfo3;
    sp_osc *lfo2;
    SPFLOAT freq1;
    sp_osc *lfo1;
    sp_fosc *voice2;
    sp_fosc *voice3;
    sp_delay *delay;
    SPFLOAT freq2;
    SPFLOAT freq3;
    sp_fosc *voice1;
    sp_port *port3;
    sp_butlp *noisefilt;
    sp_tenv *env;
    SPFLOAT gate;
    sp_butlp *lpf;
    sp_trand *tr;
    sp_randh *randh;
    sp_thresh *thresh;
    sp_port *port2;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
