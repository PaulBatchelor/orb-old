typedef struct {
    sp_data *sp;
    sp_ftbl *rnd;
    sp_clock *clk;
    sp_maygate *mt1;
    sp_maygate *mt2;
    sp_tseq *tseq;
    sp_ftbl *sine;
    sp_fosc *fm;
    sp_tenvx *env1;
    sp_tenvx *env2;
    sp_blsaw *saw;
    sp_moogladder *moog;
    sp_zitarev *rev;
    sp_trand *tr_mod;
    sp_trand *tr_car;
    SPFLOAT pos_x;
    SPFLOAT pos_y;
    sp_port *port;
} sp_synth;

int sp_synth_compute(sp_data *sp, sp_synth *synth, int bufsize, short *buf);
int sp_synth_create(sp_data *sp, sp_synth *synth);
int sp_synth_destroy(sp_data *sp, sp_synth *synth);
int sp_synth_init(sp_data *sp, sp_synth *synth);
