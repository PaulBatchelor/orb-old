#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_port *port2;
    sp_delay *delay;
    sp_osc *lfo3;
    sp_revscm *rev;
    SPFLOAT gate;
    sp_trand *tr;
    sp_port *port3;
    sp_critter *critter;
    sp_modal *mode;
    sp_fosc *voice3;
    sp_randi *lfo2;
    sp_tenv *env;
    SPFLOAT freq1;
    sp_port *port1;
    SPFLOAT freq3;
    sp_ftbl *sine;
    sp_fosc *voice1;
    sp_fosc *voice2;
    sp_osc *lfo1;
    sp_butlp *noisefilt;
    SPFLOAT freq2;
    sp_butlp *lpf;
    sp_randh *randh;
    sp_thresh *thresh;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
