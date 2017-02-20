#ifndef SP_SYNTH_TYPEDEF
#define SP_SYNTH_TYPEDEF
typedef struct sp_synth sp_synth;
#endif

struct sp_synth {
    sp_data *sp;
    sp_randh *randh;
    SPFLOAT gate;
    sp_thresh *thresh;
    sp_port *port1;
    sp_fosc *voice1;
    sp_osc *lfo2;
    sp_ftbl *sine;
    sp_fosc *voice2;
    sp_osc *lfo1;
    sp_butlp *lpf;
    SPFLOAT freq2;
    sp_tenvx *env;
    sp_trand *tr;
    SPFLOAT freq1;
    sp_delay *delay;
    sp_revsc *rev;
    sp_modal *mode;
    sp_port *port2;
};

int sp_synth_computef(sp_data *sp, sp_synth *synth, int bufsize, SPFLOAT *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
