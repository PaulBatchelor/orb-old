#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_trand *tr;
    sp_revscm *rev;
    sp_tenv *env;
    sp_fosc *voice3;
    sp_ftbl *sine;
    sp_modal *mode;
    sp_butlp *lpf;
    sp_delay *delay;
    sp_osc *lfo1;
    sp_butlp *noisefilt;
    sp_thresh *thresh;
    sp_port *port3;
    sp_osc *lfo2;
    SPFLOAT freq2;
    SPFLOAT freq3;
    sp_fosc *voice2;
    sp_osc *lfo3;
    SPFLOAT freq1;
    sp_port *port2;
    sp_port *port1;
    SPFLOAT gate;
    sp_fosc *voice1;
    sp_randh *randh;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
