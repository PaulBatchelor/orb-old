#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_revscm *rev;
    sp_modal *mode;
    sp_fosc *voice1;
    sp_butlp *lpf;
    sp_port *port2;
    sp_fosc *voice2;
    sp_port *port1;
    sp_osc *lfo2;
    sp_osc *lfo3;
    sp_osc *lfo1;
    SPFLOAT freq3;
    sp_ftbl *sine;
    SPFLOAT gate;
    SPFLOAT freq2;
    sp_randh *randh;
    SPFLOAT freq1;
    sp_fosc *voice3;
    sp_port *port3;
    sp_trand *tr;
    sp_thresh *thresh;
    sp_delay *delay;
    sp_tenv *env;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
