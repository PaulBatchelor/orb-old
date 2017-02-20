typedef struct  {
    SPFLOAT feedback, lpfreq;
    SPFLOAT iSampleRate, iPitchMod, iSkipInit;
    SPFLOAT sampleRate;
    SPFLOAT dampFact;
    SPFLOAT prv_LPFreq;
    int initDone;
    sp_revsc_dl delayLines[8];
    sp_auxdata aux;
} sp_revscm;

int sp_revscm_create(sp_revscm **p);
int sp_revscm_destroy(sp_revscm **p);
int sp_revscm_init(sp_data *sp, sp_revscm *p);
int sp_revscm_compute(sp_data *sp, sp_revscm *p, SPFLOAT *in, SPFLOAT *out);
