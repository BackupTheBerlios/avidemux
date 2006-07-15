#ifndef AUDIOENG_SOX_H
#define AUDIOENG_SOX_H

typedef struct resamplestuff 
{
   double Factor;     /* Factor = Fout/Fin sample rates */
   double rolloff;    /* roll-off frequency */
   double beta;       /* passband/stopband tuning magic */
   int quadr;         /* non-zero to use qprodUD quadratic interpolation */
   long Nmult;
   long Nwing;
   long Nq;
   float *Imp;        /* impulse [Nwing+1] Filter coefficients */

   double Time;       /* Current time/pos in input sample */
   long dhb;

   long a,b;          /* gcd-reduced input,output rates   */
   long t;            /* Current time/pos for exact-coeff's method */

   long Xh;           /* number of past/future samples needed by filter  */
   long Xoff;         /* Xh plus some room for creep  */
   long Xread;        /* X[Xread] is start-position to enter new samples */
   long Xp;           /* X[Xp] is position to start filter application   */
   long Xsize,Ysize;  /* size (Floats) of X[],Y[]         */
   float *X, *Y;      /* I/O buffers */
} *resample_t,ResampleStruct;

/*----------------*/
int sox_init	(uint32_t fin, uint32_t fout,ResampleStruct *r) ;
int sox_run	(ResampleStruct *r, float *ibuf, float *obuf,uint32_t *isamp, uint32_t *osamp,uint32_t inc);
int sow_drain	(ResampleStruct *r, float *obuf, uint32_t *osamp);
int sox_stop	(ResampleStruct *r)	;
#endif
