#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_ftbl *sine;
    sp_revscm *rev;
    sp_port *port3;
    sp_osc *lfo2;
    sp_port *port1;
    sp_fosc *voice2;
    SPFLOAT freq1;
    sp_randh *randh;
    SPFLOAT gate;
    sp_osc *lfo1;
    sp_trand *tr;
    SPFLOAT freq2;
    sp_butlp *noisefilt;
    sp_thresh *thresh;
    sp_port *port2;
    SPFLOAT freq3;
    sp_fosc *voice1;
    sp_delay *delay;
    sp_fosc *voice3;
    sp_osc *lfo3;
    sp_modal *mode;
    sp_tenv *env;
    sp_butlp *lpf;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
