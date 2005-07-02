/*
 * The simplest AC3 encoder
 * Copyright (c) 2000 Fabrice Bellard.
 *
 * changed a lot: uses doubles, rematixing, input level = output level, blockswitching, dcfilter
 * 2005 Simone Karin Lehmann < simone at lisanet dot de >
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file ac3enc.c
 * The simplest AC3 encoder.
 */
//#define DEBUG
//#define DEBUG_BITALLOC
#include "avcodec.h"
#include "bitstream.h"
#include "ac3.h"

typedef struct AC3EncodeContext {
    PutBitContext pb;
    int nb_channels;
    int nb_all_channels;
    int lfe_channel;
    int bit_rate;
    unsigned int sample_rate;
    unsigned int bsid;
    unsigned int frame_size_min; /* minimum frame size in case rounding is necessary */
    unsigned int frame_size; /* current frame size in words */
    int halfratecod;
    unsigned int frmsizecod;
    unsigned int fscod; /* frequency */
    unsigned int acmod;
    int lfe;
    unsigned int bsmod;
    short last_samples[AC3_MAX_CHANNELS][256];
    unsigned int chbwcod[NB_BLOCKS][AC3_MAX_CHANNELS];
    int nb_coefs[NB_BLOCKS][AC3_MAX_CHANNELS];

    /* bitrate allocation control */
    int sgaincod, sdecaycod, fdecaycod, dbkneecod, floorcod;
    AC3BitAllocParameters bit_alloc;
    int csnroffst;
    int fgaincod[AC3_MAX_CHANNELS];
    int fsnroffst[AC3_MAX_CHANNELS];
    /* mantissa encoding */
    int mant1_cnt, mant2_cnt, mant4_cnt;

    uint8_t dialnorm; /* dialog normalization */
    uint8_t blksw[NB_BLOCKS][AC3_MAX_CHANNELS]; /* switch 512 - 256 MDCT */
    uint8_t rematstr[NB_BLOCKS];
    uint8_t rematflg[NB_BLOCKS][4]; /* rematrixing flags for each block*/
    int max_coef; /* maximum coef as calcuated by sampling rate */
} AC3EncodeContext;

static uint8_t fftorder[] = {
      0,128, 64,192, 32,160,224, 96, 16,144, 80,208,240,112, 48,176,
      8,136, 72,200, 40,168,232,104,248,120, 56,184, 24,152,216, 88,
      4,132, 68,196, 36,164,228,100, 20,148, 84,212,244,116, 52,180,
    252,124, 60,188, 28,156,220, 92, 12,140, 76,204,236,108, 44,172,
      2,130, 66,194, 34,162,226, 98, 18,146, 82,210,242,114, 50,178,
     10,138, 74,202, 42,170,234,106,250,122, 58,186, 26,154,218, 90,
    254,126, 62,190, 30,158,222, 94, 14,142, 78,206,238,110, 46,174,
      6,134, 70,198, 38,166,230,102,246,118, 54,182, 22,150,214, 86
};

#include "ac3tab.h"

#define MDCT_NBITS 9
#define N         (1 << MDCT_NBITS)

/* new exponents are sent if their Norm 1 exceed this number */
#define EXP_DIFF_THRESHOLD 1000

typedef struct IComplex {
    sample_t re,im;
} IComplex;


/* Twiddle factors for IMDCT */
static IComplex pre1[128];
static IComplex post1[64];
static IComplex pre2[64];
static IComplex post2[32];

/* filter vars */
static sample_t fx1, fy1;

static void fft_init(int ln);
static void ac3_crc_init(void);

#ifdef AC3ENC_DOUBLE
static inline double fix15(double a)
{
    double v;
    v = a * 32768; // sin, cos are symetric functions and are used as factors to the pcm signal

    if (v < -32768.0)
        v = -32768.0;
    else if (v > 32768.0)
        v = 32768.0;
    return v;
}
#else
static inline int16_t fix15(double a)
{
    int v;
    v = (int)(0.5 + a * (float)(1 << 15));

    if (v < -32767)
        v = -32767;
    else if (v > 32767)
        v = 32767;
    return v;
}
#endif

static inline int calc_lowcomp1(int a, int b0, int b1)
{
    if ((b0 + 256) == b1) {
        a = 384 ;
    } else if (b0 > b1) {
        a = a - 64;
        if (a < 0) a=0;
    }
    return a;
}

static inline int calc_lowcomp(int a, int b0, int b1, int bin)
{
    if (bin < 7) {
        if ((b0 + 256) == b1) {
            a = 384 ;
        } else if (b0 > b1) {
            a = a - 64;
            if (a < 0) a=0;
        }
    } else if (bin < 20) {
        if ((b0 + 256) == b1) {
            a = 320 ;
        } else if (b0 > b1) {
            a= a - 64;
            if (a < 0) a=0;
        }
    } else {
        a = a - 128;
        if (a < 0) a=0;
    }
    return a;
}

/* AC3 bit allocation. The algorithm is the one described in the AC3
   spec. */
void ac3_parametric_bit_allocation(AC3BitAllocParameters *s, uint8_t *bap,
                                   int8_t *exp, int start, int end,
                                   int snroffset, int fgain, int is_lfe,
                                   int deltbae,int deltnseg,
                                   uint8_t *deltoffst, uint8_t *deltlen, uint8_t *deltba)
{
    int bin,i,j,k,end1,v,v1,bndstrt,bndend,lowcomp,begin;
    int fastleak,slowleak,address,tmp;
    int16_t psd[256]; /* scaled exponents */
    int16_t bndpsd[50]; /* interpolated exponents */
    int16_t excite[50]; /* excitation */
    int16_t mask[50];   /* masking value */

    /* exponent mapping to PSD */
    for(bin=start;bin<end;bin++) {
        psd[bin]=(3072 - (exp[bin] << 7));
    }

    /* PSD integration */
    j=start;
    k=masktab[start];
    do {
        v=psd[j];
        j++;
/*        end1=bndtab[k+1];    bndtab[51] was dropped, so we now need to calculate the value as in the spec */
        end1=bndtab[k] + bndsz[k];
        if (end1 > end) end1=end;
        for(i=j;i<end1;i++) {
            int c,adr;
            /* logadd */
            v1=psd[j];
            c=v-v1;
            if (c >= 0) {
                adr=c >> 1;
                if (adr > 255) adr=255;
                v=v + latab[adr];
            } else {
                adr=(-c) >> 1;
                if (adr > 255) adr=255;
                v=v1 + latab[adr];
            }
            j++;
        }
        bndpsd[k]=v;
        k++;
    } while (end > end1);
//    } while (end > bndtab[k]);

    /* excitation function */
    bndstrt = masktab[start];
    bndend = masktab[end-1] + 1;

    if (bndstrt == 0) {
        lowcomp = 0;
        lowcomp = calc_lowcomp1(lowcomp, bndpsd[0], bndpsd[1]) ;
        excite[0] = bndpsd[0] - fgain - lowcomp ;
        lowcomp = calc_lowcomp1(lowcomp, bndpsd[1], bndpsd[2]) ;
        excite[1] = bndpsd[1] - fgain - lowcomp ;
        begin = 7 ;
        for (bin = 2; bin < 7; bin++) {
            if (!(is_lfe && bin == 6))
                lowcomp = calc_lowcomp1(lowcomp, bndpsd[bin], bndpsd[bin+1]) ;
            fastleak = bndpsd[bin] - fgain ;
            slowleak = bndpsd[bin] - s->sgain ;
            excite[bin] = fastleak - lowcomp ;
            if (!(is_lfe && bin == 6)) {
                if (bndpsd[bin] <= bndpsd[bin+1]) {
                    begin = bin + 1 ;
                    break ;
                }
            }
        }

        end1=bndend;
        if (end1 > 22) end1=22;

        for (bin = begin; bin < end1; bin++) {
            if (!(is_lfe && bin == 6))
                lowcomp = calc_lowcomp(lowcomp, bndpsd[bin], bndpsd[bin+1], bin) ;

            fastleak -= s->fdecay ;
            v = bndpsd[bin] - fgain;
            if (fastleak < v) fastleak = v;

            slowleak -= s->sdecay ;
            v = bndpsd[bin] - s->sgain;
            if (slowleak < v) slowleak = v;

            v=fastleak - lowcomp;
            if (slowleak > v) v=slowleak;

            excite[bin] = v;
        }
        begin = 22;
    } else {
        /* coupling channel */
        begin = bndstrt;

        fastleak = (s->cplfleak << 8) + 768;
        slowleak = (s->cplsleak << 8) + 768;
    }

    for (bin = begin; bin < bndend; bin++) {
        fastleak -= s->fdecay ;
        v = bndpsd[bin] - fgain;
        if (fastleak < v) fastleak = v;
        slowleak -= s->sdecay ;
        v = bndpsd[bin] - s->sgain;
        if (slowleak < v) slowleak = v;

        v=fastleak;
        if (slowleak > v) v = slowleak;
        excite[bin] = v;
    }

    /* compute masking curve */

    for (bin = bndstrt; bin < bndend; bin++) {
        v1 = excite[bin];
        tmp = s->dbknee - bndpsd[bin];
        if (tmp > 0) {
            v1 += tmp >> 2;
        }
        v=hth[bin >> s->halfratecod][s->fscod];
        if (v1 > v) v=v1;
        mask[bin] = v;
    }

    /* delta bit allocation */

    if (deltbae == 0 || deltbae == 1) {
        int band, seg, delta;
        band = 0 ;
        for (seg = 0; seg < deltnseg; seg++) {
            band += deltoffst[seg] ;
            if (deltba[seg] >= 4) {
                delta = (deltba[seg] - 3) << 7;
            } else {
                delta = (deltba[seg] - 4) << 7;
            }
            for (k = 0; k < deltlen[seg]; k++) {
                mask[band] += delta ;
                band++ ;
            }
        }
    }

    /* compute bit allocation */

    i = start ;
    j = masktab[start] ;
    do {
        v=mask[j];
        v -= snroffset ;
        v -= s->floor ;
        if (v < 0) v = 0;
        v &= 0x1fe0 ;
        v += s->floor ;

        end1=bndtab[j] + bndsz[j];
        if (end1 > end) end1=end;

        for (k = i; k < end1; k++) {
            address = (psd[i] - v) >> 5 ;
            if (address < 0) address=0;
            else if (address > 63) address=63;
            bap[i] = baptab[address];
            i++;
        }
/*    } while (end > bndtab[j++]) ;    bandtab[50]=0, so last 24 sub_bands cannot be used!!*/
        j++;
      } while (end > end1) ;  // this is exactly the same as the a52 spec, 'cause end1 = min(bandtab[j] + bndsz[j], end)
}



static void fft_init(int ln)
{
    int i, j, m, n;
    double alpha;

    n = 1 << ln;

    for(i=0;i<(n/2);i++) {
        alpha = 2 * M_PI * (double)i / (double)n;
        costab[i] = fix15(cos(alpha));
        sintab[i] = fix15(sin(alpha));
    }

    for(i=0;i<n;i++) {
        m=0;
        for(j=0;j<ln;j++) {
            m |= ((i >> j) & 1) << (ln-j-1);
        }
        fft_rev[i]=m;
    }
}

static void fft_init256(int ln)
{
    int i, j, m, n;
    double alpha, alpha2;

    n = 1 << ln;

    for(i=0;i<(n/2);i++) {
        alpha = 2 * M_PI * (double)i / (double)n;
        costab256[i] = fix15(cos(alpha));
        sintab256[i] = fix15(sin(alpha));
    }

    for(i=0;i<n;i++) {
        m=0;
        for(j=0;j<ln;j++) {
            m |= ((i >> j) & 1) << (ln-j-1);
        }
        fft_rev256[i]=m;
    }
}


#ifdef AC3ENC_DOUBLE
#define DIV1 / 2
#define DIV15 / 32768
#else
#define DIV1 >> 1
#define DIV15 >> 15
#endif

/* butter fly op */
#define BF(pre, pim, qre, qim, pre1, pim1, qre1, qim1) \
{\
  int ax, ay, bx, by;\
  bx=pre1;\
  by=pim1;\
  ax=qre1;\
  ay=qim1;\
  pre = (bx + ax) DIV1;\
  pim = (by + ay) DIV1;\
  qre = (bx - ax) DIV1;\
  qim = (by - ay) DIV1;\
}

#define MUL16(a,b) ((a) * (b))

#define CMUL(pre, pim, are, aim, bre, bim) \
{\
   pre = (MUL16(are, bre) - MUL16(aim, bim)) DIV15;\
   pim = (MUL16(are, bim) + MUL16(bre, aim)) DIV15;\
}


/* do a 2^n point complex fft on 2^ln points. */
static void fft(IComplex *z, int ln)
{
    int	j, l, np, np2;
    int	nblocks, nloops;
    register IComplex *p,*q;
    sample_t tmp_re, tmp_im;

    np = 1 << ln;

    /* reverse */
    for(j=0;j<np;j++) {
        int k;
        IComplex tmp;
        k = fft_rev[j];
        if (k < j) {
            tmp = z[k];
            z[k] = z[j];
            z[j] = tmp;
        }
    }

    /* pass 0 */

    p=&z[0];
    j=(np >> 1);
    do {

        BF(p[0].re, p[0].im, p[1].re, p[1].im,
           p[0].re, p[0].im, p[1].re, p[1].im);
        p+=2;
    } while (--j != 0);

    /* pass 1 */

    p=&z[0];
    j=np >> 2;
    do {
        BF(p[0].re, p[0].im, p[2].re, p[2].im,
           p[0].re, p[0].im, p[2].re, p[2].im);
        BF(p[1].re, p[1].im, p[3].re, p[3].im,
           p[1].re, p[1].im, p[3].im, -p[3].re);
        p+=4;
    } while (--j != 0);

    /* pass 2 .. ln-1 */

    nblocks = np >> 3;
    nloops = 1 << 2;
    np2 = np >> 1;
    do {
        p = z;
        q = z + nloops;
        for (j = 0; j < nblocks; ++j) {
            BF(p->re, p->im, q->re, q->im,
               p->re, p->im, q->re, q->im);
            p++;
            q++;
            for(l = nblocks; l < np2; l += nblocks) {
                CMUL(tmp_re, tmp_im, costab[l], -sintab[l], q->re, q->im);
                BF(p->re, p->im, q->re, q->im,
                   p->re, p->im, tmp_re, tmp_im);
                p++;
                q++;
            }
            p += nloops;
            q += nloops;
        }
        nblocks = nblocks >> 1;
        nloops = nloops << 1;
    } while (nblocks != 0);
}

static void fft256(IComplex *z, int ln, int num)
{
    int	j, l, np, np2;
    int	nblocks, nloops;
    register IComplex *p,*q;
    sample_t tmp_re, tmp_im;

    np = 1 << ln;

    /* reverse */
    for(j=0;j<np;j++) {
        int k;
        IComplex tmp;
        k = fft_rev256[j];
        if (k < j) {
            tmp = z[k];
            z[k] = z[j];
            z[j] = tmp;
        }
    }

    /* pass 0 */

    p=&z[0];
    j=(np >> 1);
    do {

        BF(p[0].re, p[0].im, p[1].re, p[1].im,
           p[0].re, p[0].im, p[1].re, p[1].im);
        p+=2;
    } while (--j != 0);

    /* pass 1 */

    p=&z[0];
    j=np >> 2;
    do {
        BF(p[0].re, p[0].im, p[2].re, p[2].im,
           p[0].re, p[0].im, p[2].re, p[2].im);
        BF(p[1].re, p[1].im, p[3].re, p[3].im,
           p[1].re, p[1].im, p[3].im, -p[3].re);
        p+=4;
    } while (--j != 0);

    /* pass 2 .. ln-1 */

    nblocks = np >> 3;
    nloops = 1 << 2;
    np2 = np >> 1;
    do {
        p = z;
        q = z + nloops;
        for (j = 0; j < nblocks; ++j) {
            BF(p->re, p->im, q->re, q->im,
               p->re, p->im, q->re, q->im);
            p++;
            q++;
            for(l = nblocks; l < np2; l += nblocks) {
                CMUL(tmp_re, tmp_im, costab256[l], -sintab256[l], q->re, q->im);
                BF(p->re, p->im, q->re, q->im,
                   p->re, p->im, tmp_re, tmp_im);
                p++;
                q++;
            }
            p += nloops;
            q += nloops;
        }
        nblocks = nblocks >> 1;
        nloops = nloops << 1;
    } while (nblocks != 0);
}

/* do a 512 point mdct */
static void mdct512(sample_t *out, int16_t *in)
{
    int i; // re, im, re1, im1; */
    sample_t re, im, re1, im1;
    int32_t rot[N];
    IComplex x[N/4];

    /* shift to simplify computations */
    /* << 8: only done, because we how enough bits left, should give better precision */
    for(i=0;i<N/4;i++)
        rot[i] = -in[i + 3*N/4] << 8;
    for(i=N/4;i<N;i++)
        rot[i] = in[i - N/4] << 8;

    /* pre rotation */
    for(i=0;i<N/4;i++) {
        re = (sample_t) rot[2*i] - (sample_t) rot[N-1-2*i];
        im = -((sample_t) rot[N/2+2*i] - (sample_t) rot[N/2-1-2*i]);
        CMUL(x[i].re, x[i].im, re, im, -xcos1[i], xsin1[i]);
    }

    fft(x, MDCT_NBITS - 2);

    /* post rotation */
    for(i=0;i<N/4;i++) {
        CMUL(re1, im1, x[i].re, x[i].im, xsin1[i], xcos1[i]);
        out[2*i] = im1 / 256.0;  /* scale back the 'extended bit usage', see above */
        out[N/2-1-2*i] = re1 / 256.0;
    }
}

/* do a 256 point mdct */
static void mdct256(sample_t *out, int16_t *in)
{

    int i, n, nbits;
    sample_t re, im, re1, im1;
    int32_t rot[N/2];
    IComplex x[N/4];
    sample_t out1[N/4], out2[N/4];

    nbits = 8;

/* this works with high freq artefacts */
    /* << 8: only done, because we how enough bits left, should give better precision */
    for(i=0;i<256;i++)
        rot[i] = in[i] << 8;

    for(i=0; i<64 ;i++) {
        re = (sample_t) rot[2*i] - (sample_t) rot[256-1-2*i];
        im = -((sample_t) rot[128+2*i] - (sample_t) rot[128-1-2*i]);
        CMUL(x[i].re, x[i].im, re, im, -xcos1_256[i], xsin1_256[i]);
    }

    fft256(x, nbits - 2, 1);

    for(i=0;i<64;i++) {
        CMUL(re1, im1, x[i].re, x[i].im, xsin1_256[i], xcos1_256[i]);
        out1[2*i] = im1;
        out1[128-1-2*i] = re1;
    }


    for(i=0; i < 128; i++)
        rot[i] = -in[i + 128 +256] << 8;
    for(i=128; i < 256; i++)
        rot[i] = in[i - 128 + 256] << 8;

    for(i=0; i < 64; i++) {
        re = (sample_t) rot[2*i] - (sample_t) rot[256-1-2*i];
        im = -((sample_t) rot[128+2*i] - (sample_t) rot[128-1-2*i]);
        CMUL(x[i].re, x[i].im, re, im, -xcos1_256[i], xsin1_256[i]);
    }

    fft256(x, nbits - 2, 2);

    for(i=0;i<64;i++) {
        CMUL(re1, im1, x[i].re, x[i].im, xsin1_256[i], xcos1_256[i]);
        out2[2*i] = im1;
        out2[128-1-2*i] = re1;
    }

    /* interleave both 256-point-mdct */
    for(i = 0; i < 128; i++) {
        out[2*i]   = out1[i] / 256.0; /* scale back the 'extended bit usage', see above */
        out[2*i+1] = out2[i] / 256.0 ;
    }

}


/* XXX: use another norm ? */
static int calc_exp_diff(uint8_t *exp1, uint8_t *exp2, int n)
{
    int sum, i;
    sum = 0;
    for(i=0;i<n;i++) {
        sum += abs(exp1[i] - exp2[i]);
    }
    return sum;
}

static void compute_exp_strategy(uint8_t exp_strategy[NB_BLOCKS][AC3_MAX_CHANNELS],
                                 uint8_t exp[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                                 int ch, int is_lfe, int nb_coefs)
//                                 int ch, int is_lfe, int nb_coefs[NB_BLOCKS][AC3_MAX_CHANNELS])
{
    int i, j;
    int exp_diff;
    int coefs;


    /* estimate if the exponent variation & decide if they should be
       reused in the next frame */
    exp_strategy[0][ch] = EXP_NEW;
    for(i=1;i<NB_BLOCKS;i++) {
        exp_diff = calc_exp_diff(exp[i][ch], exp[i-1][ch], N/2);
#ifdef DEBUG
        av_log(NULL, AV_LOG_DEBUG, "exp_diff=%d\n", exp_diff);
#endif
        if (exp_diff > EXP_DIFF_THRESHOLD)
            exp_strategy[i][ch] = EXP_NEW;
        else
            exp_strategy[i][ch] = EXP_REUSE;
    }
    if (is_lfe)
	return;

    /* now select the encoding strategy type : if exponents are often
       recoded, we use a coarse encoding */
    i = 0;
    while (i < NB_BLOCKS) {
        j = i + 1;
        while (j < NB_BLOCKS && exp_strategy[j][ch] == EXP_REUSE)
            j++;
        switch(j - i) {
        case 1:
            exp_strategy[i][ch] = EXP_D45;
            break;
        case 2:
        case 3:
            exp_strategy[i][ch] = EXP_D25;
            break;
        default:
            exp_strategy[i][ch] = EXP_D15;
            break;
        }
	i = j;
    }
}

/* set exp[i] to min(exp[i], exp1[i]) */
static void exponent_min(uint8_t exp[N/2], uint8_t exp1[N/2], int n)
{
    int i;
    for(i=0;i<n;i++) {
        if (exp1[i] < exp[i])
            exp[i] = exp1[i];
    }
}

/* update the exponents so that they are the ones the decoder will
   decode. Return the number of bits used to code the exponents */
static int encode_exp(uint8_t encoded_exp[N/2],
                      uint8_t exp[N/2],
                      int nb_exps,
                      int exp_strategy)
{
    int group_size, nb_groups, i, j, k, exp_min;
    uint8_t exp1[N/2];

    switch(exp_strategy) {
    case EXP_D15:
        group_size = 1;
        break;
    case EXP_D25:
        group_size = 2;
        break;
    default:
    case EXP_D45:
        group_size = 4;
        break;
    }
    nb_groups = ((nb_exps + (group_size * 3) - 4) / (3 * group_size)) * 3;

    /* for each group, compute the minimum exponent */
    exp1[0] = exp[0]; /* DC exponent is handled separately */
    k = 1;
    for(i=1;i<=nb_groups;i++) {
        exp_min = exp[k];
        assert(exp_min >= 0 && exp_min <= 24);
        for(j=1;j<group_size;j++) {
            if (exp[k+j] < exp_min)
                exp_min = exp[k+j];
        }
        exp1[i] = exp_min;
        k += group_size;
    }

    /* constraint for DC exponent */
    if (exp1[0] > 15)
        exp1[0] = 15;

    /* Decrease the delta between each groups to within 2
     * so that they can be differentially encoded */
    for (i=1;i<=nb_groups;i++)
	exp1[i] = FFMIN(exp1[i], exp1[i-1] + 2);
    for (i=nb_groups-1;i>=0;i--)
	exp1[i] = FFMIN(exp1[i], exp1[i+1] + 2);

    /* now we have the exponent values the decoder will see */
    encoded_exp[0] = exp1[0];
    k = 1;
    for(i=1;i<=nb_groups;i++) {
        for(j=0;j<group_size;j++) {
            encoded_exp[k+j] = exp1[i];
        }
        k += group_size;
    }

#if defined(DEBUG)
    av_log(NULL, AV_LOG_DEBUG, "exponents: strategy=%d\n", exp_strategy);
    for(i=0;i<=nb_groups * group_size;i++) {
        av_log(NULL, AV_LOG_DEBUG, "%d ", encoded_exp[i]);
    }
    av_log(NULL, AV_LOG_DEBUG, "\n");
#endif

    return 4 + (nb_groups / 3) * 7;
}

/* return the size in bits taken by the mantissa */
static int compute_mantissa_size(AC3EncodeContext *s, uint8_t *m, int nb_coefs)
{
    int bits, mant, i;

    bits = 0;
    for(i=0;i<nb_coefs;i++) {
        mant = m[i];
        switch(mant) {
        case 0:
            /* nothing */
            break;
        case 1:
            /* 3 mantissa in 5 bits */
            if (s->mant1_cnt == 0)
                bits += 5;
            if (++s->mant1_cnt == 3)
                s->mant1_cnt = 0;
            break;
        case 2:
            /* 3 mantissa in 7 bits */
            if (s->mant2_cnt == 0)
                bits += 7;
            if (++s->mant2_cnt == 3)
                s->mant2_cnt = 0;
            break;
        case 3:
            bits += 3;
            break;
        case 4:
            /* 2 mantissa in 7 bits */
            if (s->mant4_cnt == 0)
                bits += 7;
            if (++s->mant4_cnt == 2)
                s->mant4_cnt = 0;
            break;
        case 14:
            bits += 14;
            break;
        case 15:
            bits += 16;
            break;
        default:
            bits += mant - 1;
            break;
        }
    }
    return bits;
}


static int bit_alloc(AC3EncodeContext *s,
                     uint8_t bap[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                     uint8_t encoded_exp[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                     uint8_t exp_strategy[NB_BLOCKS][AC3_MAX_CHANNELS],
                     int frame_bits, int csnroffst, int fsnroffst)
{
    int i, ch;

    /* compute size */
    for(i=0;i<NB_BLOCKS;i++) {
        s->mant1_cnt = 0;
        s->mant2_cnt = 0;
        s->mant4_cnt = 0;
        for(ch=0;ch<s->nb_all_channels;ch++) {
            ac3_parametric_bit_allocation(&s->bit_alloc,
                                          bap[i][ch], (int8_t *)encoded_exp[i][ch],
                                          0, s->nb_coefs[i][ch],
                                          (((csnroffst-15) << 4) +
                                           fsnroffst) << 2,
                                          fgaintab[s->fgaincod[ch]],
                                          ch == s->lfe_channel,
                                          2, 0, NULL, NULL, NULL);
            frame_bits += compute_mantissa_size(s, bap[i][ch],
                                                 s->nb_coefs[i][ch]);
        }
    }
#if 0
    printf("csnr=%d fsnr=%d frame_bits=%d diff=%d\n",
           csnroffst, fsnroffst, frame_bits,
           16 * s->frame_size - ((frame_bits + 7) & ~7));
#endif
    return 16 * s->frame_size - frame_bits;
}

#define SNR_INC1 4

static int compute_bit_allocation(AC3EncodeContext *s,
                                  uint8_t bap[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                                  uint8_t encoded_exp[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                                  uint8_t exp_strategy[NB_BLOCKS][AC3_MAX_CHANNELS],
                                  int frame_bits)
{
    int i, j, ch;
    int csnroffst, fsnroffst;
    uint8_t bap1[NB_BLOCKS][AC3_MAX_CHANNELS][N/2];
    static int frame_bits_inc[8] = { 0, 0, 2, 2, 2, 4, 2, 4 };

    /* init default parameters */
    s->sdecaycod = 2;
    s->fdecaycod = 1;
    s->sgaincod = 1;
    s->dbkneecod = 2;
    s->floorcod = 4;
    for(ch=0;ch<s->nb_all_channels;ch++)
        s->fgaincod[ch] = 4;

    /* compute real values */
    s->bit_alloc.fscod = s->fscod;
    s->bit_alloc.halfratecod = s->halfratecod;
    s->bit_alloc.sdecay = sdecaytab[s->sdecaycod] >> s->halfratecod;
    s->bit_alloc.fdecay = fdecaytab[s->fdecaycod] >> s->halfratecod;
    s->bit_alloc.sgain = sgaintab[s->sgaincod];
    s->bit_alloc.dbknee = dbkneetab[s->dbkneecod];
    s->bit_alloc.floor = floortab[s->floorcod];

    /* header size */
    frame_bits += 65;
    // if (s->acmod == 2)
    //    frame_bits += 2;
    frame_bits += frame_bits_inc[s->acmod];

    /* audio blocks */
    for(i=0;i<NB_BLOCKS;i++) {
        frame_bits += s->nb_channels * 2 + 2; /* blksw * c, dithflag * c, dynrnge, cplstre */
        if (s->acmod == 2) {
            /* since we do not use couling, if rematstr == 1, we always habe 4 bands */
            for (j = 0; j < 4; j++) {
                frame_bits++; /* rematstr */
                if (s->rematstr[j])
                    frame_bits += 4;
            }
        }
        frame_bits += 2 * s->nb_channels; /* chexpstr[2] * c */
	if (s->lfe)
	    frame_bits++; /* lfeexpstr */
        for(ch=0;ch<s->nb_channels;ch++) {
            if (exp_strategy[i][ch] != EXP_REUSE)
                frame_bits += 6 + 2; /* chbwcod[6], gainrng[2] */
        }
        frame_bits++; /* baie */
        frame_bits++; /* snr */
        frame_bits += 2; /* delta / skip */
    }
    frame_bits++; /* cplinu for block 0 */
    /* bit alloc info */
    /* sdcycod[2], fdcycod[2], sgaincod[2], dbpbcod[2], floorcod[3] */
    /* csnroffset[6] */
    /* (fsnoffset[4] + fgaincod[4]) * c */
    frame_bits += 2*4 + 3 + 6 + s->nb_all_channels * (4 + 3);

    /* auxdatae, crcrsv */
    frame_bits += 2;

    /* CRC */
    frame_bits += 16;

    /* now the big work begins : do the bit allocation. Modify the snr
       offset until we can pack everything in the requested frame size */

    csnroffst = s->csnroffst;
    while (csnroffst >= 0 &&
	   bit_alloc(s, bap, encoded_exp, exp_strategy, frame_bits, csnroffst, 0) < 0)
	csnroffst -= SNR_INC1;
    if (csnroffst < 0) {
	av_log(NULL, AV_LOG_ERROR, "Yack, Error !!!\n");
	return -1;
    }
    while ((csnroffst + SNR_INC1) <= 63 &&
           bit_alloc(s, bap1, encoded_exp, exp_strategy, frame_bits,
                     csnroffst + SNR_INC1, 0) >= 0) {
        csnroffst += SNR_INC1;
        memcpy(bap, bap1, sizeof(bap1));
    }
    while ((csnroffst + 1) <= 63 &&
           bit_alloc(s, bap1, encoded_exp, exp_strategy, frame_bits, csnroffst + 1, 0) >= 0) {
        csnroffst++;
        memcpy(bap, bap1, sizeof(bap1));
    }

    fsnroffst = 0;
    while ((fsnroffst + SNR_INC1) <= 15 &&
           bit_alloc(s, bap1, encoded_exp, exp_strategy, frame_bits,
                     csnroffst, fsnroffst + SNR_INC1) >= 0) {
        fsnroffst += SNR_INC1;
        memcpy(bap, bap1, sizeof(bap1));
    }
    while ((fsnroffst + 1) <= 15 &&
           bit_alloc(s, bap1, encoded_exp, exp_strategy, frame_bits,
                     csnroffst, fsnroffst + 1) >= 0) {
        fsnroffst++;
        memcpy(bap, bap1, sizeof(bap1));
    }

    s->csnroffst = csnroffst;
    for(ch=0;ch<s->nb_all_channels;ch++)
        s->fsnroffst[ch] = fsnroffst;
#if defined(DEBUG_BITALLOC)
    {
        int j;

        for(i=0;i<6;i++) {
            for(ch=0;ch<s->nb_all_channels;ch++) {
                printf("Block #%d Ch%d:\n", i, ch);
                printf("bap=");
                for(j=0;j<s->nb_coefs[i][ch];j++) {
                    printf("%d ",bap[i][ch][j]);
                }
                printf("\n");
            }
        }
    }
#endif
    return 0;
}

static double besselI0 (double x)
{
    double bessel = 1;
    int i = 100;

    do
	bessel = bessel * x / (i * i) + 1;
    while (--i);
    return bessel;
}

void ac3_common_init(void)
{
    int i, j, k, l, v;
    double sum;
    sample_t local_mdct_window[256];
    /* compute bndtab and masktab from bandsz */
    k = 0;
    l = 0;
    for(i=0;i<50;i++) {
        bndtab[i] = l;
        v = bndsz[i];
        for(j=0;j<v;j++) masktab[k++]=i;
        l += v;
    }

    /* use the same window function as liba52, gives better precision */
    /* compute mdct window - kaiser-bessel derived window, alpha = 5.0 */
    sum = 0;
    for (i = 0; i < 256; i++) {
	    sum += besselI0 (i * (256 - i) * (5 * M_PI / 256) * (5 * M_PI / 256));
	    local_mdct_window[i] = sum;
    }
    sum++;
    for (i = 0; i < 256; i++)
	    ac3_window[i] = (sample_t) sqrt (local_mdct_window[i] / sum);

}

static int AC3_encode_init(AVCodecContext *avctx)
{
    int freq = avctx->sample_rate;
    int bitrate = avctx->bit_rate;
    int channels = avctx->channels;
    AC3EncodeContext *s = avctx->priv_data;
    int i, j, ch;
    //float alpha;
    double alpha;
    static const uint8_t acmod_defs[6] = {
	0x01, /* C */
	0x02, /* L R */
	0x03, /* L C R */
	0x06, /* L R SL SR */
	0x07, /* L C R SL SR */
	0x07, /* L C R SL SR (+LFE) */
    };
    int hifreqtab[19] = {
        17200, 17200, 17200, 17200, 17200, 17200, 17200, 17200,
        17200, 18000, 18700, 19300, 19800, 20100, 20300, 20300,
        20300, 20300, 20300
    };
    int hifreq, srate;

    avctx->frame_size = AC3_FRAME_SIZE;

    /* number of channels */
    if (channels < 1 || channels > 6)
	return -1;
    s->acmod = acmod_defs[channels - 1];
    s->lfe = (channels == 6) ? 1 : 0;
    s->nb_all_channels = channels;
    s->nb_channels = channels > 5 ? 5 : channels;
    s->lfe_channel = s->lfe ? 5 : -1;
    if (avctx->extradata_size)
        s->dialnorm = ((uint8_t *)avctx->extradata)[0];
    else
        s->dialnorm = 31;

    /* frequency */
    for(i=0;i<3;i++) {
        for(j=0;j<3;j++)
            if ((ac3_freqs[j] >> i) == freq)
                goto found;
    }
    return -1;
 found:

    srate = ac3_freqs[j];
    s->sample_rate = srate;
    s->halfratecod = i;
    s->fscod = j;
    s->bsid = 8 + s->halfratecod;
    s->bsmod = 0; /* complete main audio service */

    /* bitrate & frame size */
    bitrate /= 1000;
    for(i=0;i<19;i++) {
        if ((ac3_bitratetab[i] >> s->halfratecod) == bitrate)
            break;
    }
    if (i == 19)
        return -1;
    hifreq = hifreqtab[i];
    s->bit_rate = bitrate;
    s->frmsizecod = i << 1;
    s->frame_size_min = (bitrate * 1000 * AC3_FRAME_SIZE) / (freq * 16);
    /* for now we do not handle fractional sizes */
    s->frame_size = s->frame_size_min;

    /* bit allocation init */
    for(ch=0;ch<s->nb_channels;ch++) {
        /* bandwidth for each channel */
        /* XXX: should compute the bandwidth according to the frame
           size, so that we avoid anoying high freq artefacts */
        /* bandwith is
          nb_coef = max_encoded_freq * 512 / samplingrate
          grenzfreq is die höchste im siganl forhanden freq die kodiert werden soll
        */
        j = (int) (hifreq * 512.0 / srate);
        if (j > 253)
            j = 253;

        for (i = 0; i < NB_BLOCKS; i++) {
            /* blksw */
            s->blksw[i][ch] = 0;
            /* channel bandwith */
            s->chbwcod[i][ch] = (j - 73) / 3;
            s->nb_coefs[i][ch] = ((s->chbwcod[i][ch] + 12) * 3) + 37;
         }
         s->max_coef = ((s->chbwcod[0][ch] + 12) * 3) + 37;
    }
    if (s->lfe) {
        for (i = 0; i < NB_BLOCKS; i++) {
        	s->nb_coefs[i][s->lfe_channel] = 7; /* fixed */
        }
    }

    for (i = 0; i < NB_BLOCKS; i++) {
        /* init rematrixing flags */
        s->rematstr[i] = 0;
        for (j = 0; j < 4; j++) {
            s->rematflg[i][j] = 0;
        }
        /* block 0 always defines rematstr */
        s->rematstr[0] = 1;
    }

    /* initial snr offset */
    s->csnroffst = 40;

    ac3_common_init();

    /* mdct init */
    fft_init(MDCT_NBITS - 2);
    for(i=0;i<N/4;i++) {
        alpha = 2 * M_PI * (i + 1.0 / 8.0) / (double)N;
        xcos1[i] = fix15(-cos(alpha));
        xsin1[i] = fix15(-sin(alpha));
    }

    fft_init256(8 - 2);
    for(i=0;i<256/4;i++) {
        alpha = 2 * M_PI * (i + 1.0 / 8.0) / (double)256.0;
        xcos1_256[i] = fix15(-cos(alpha));
        xsin1_256[i] = fix15(-sin(alpha));
    }

    ac3_crc_init();

    avctx->coded_frame= avcodec_alloc_frame();
    avctx->coded_frame->key_frame= 1;

    /* filter variables */
    fx1 = fy1 = 0.0;

    return 0;
}

/* output the AC3 frame header */
static void output_frame_header(AC3EncodeContext *s, unsigned char *frame)
{
    init_put_bits(&s->pb, frame, AC3_MAX_CODED_FRAME_SIZE);

    put_bits(&s->pb, 16, 0x0b77); /* frame header */
    put_bits(&s->pb, 16, 0); /* crc1: will be filled later */
    put_bits(&s->pb, 2, s->fscod);
    put_bits(&s->pb, 6, s->frmsizecod + (s->frame_size - s->frame_size_min));
    put_bits(&s->pb, 5, s->bsid);
    put_bits(&s->pb, 3, s->bsmod);
    put_bits(&s->pb, 3, s->acmod);
    if ((s->acmod & 0x01) && s->acmod != 0x01)
	put_bits(&s->pb, 2, 1); /* XXX -4.5 dB */
    if (s->acmod & 0x04)
	put_bits(&s->pb, 2, 1); /* XXX -6 dB */
    if (s->acmod == 0x02)
        put_bits(&s->pb, 2, 0); /* surround not indicated */
    put_bits(&s->pb, 1, s->lfe); /* LFE */
    put_bits(&s->pb, 5, s->dialnorm); /* dialog norm: -31 db */
    put_bits(&s->pb, 1, 0); /* no compression control word */
    put_bits(&s->pb, 1, 0); /* no lang code */
    put_bits(&s->pb, 1, 0); /* no audio production info */
    put_bits(&s->pb, 1, 0); /* no copyright */
    put_bits(&s->pb, 1, 1); /* original bitstream */
    put_bits(&s->pb, 1, 0); /* no time code 1 */
    put_bits(&s->pb, 1, 0); /* no time code 2 */
    put_bits(&s->pb, 1, 0); /* no addtional bit stream info */
}

/* symetric quantization on 'levels' levels */
static inline int sym_quant(int32_t c, int e, int levels)
{
    int v;

    if (c >= 0) {
        v = (levels * (c << e)) >> 24;
        v = (v + 1) >> 1;
        v = (levels >> 1) + v;
    } else {
        v = (levels * ((-c) << e)) >> 24;
        v = (v + 1) >> 1;
        v = (levels >> 1) - v;
    }
    assert (v >= 0 && v < levels);
    return v;
}

/* asymetric quantization on 2^qbits levels */
static inline int asym_quant(int32_t c, int e, int qbits)
{
    int lshift, m, v;

    lshift = e + qbits - 24;
    if (lshift >= 0)
        v = c << lshift;
    else
        v = c >> (-lshift);
    /* rounding */
    v = (v + 1) >> 1;
    m = (1 << (qbits-1));
    if (v >= m)
        v = m - 1;
    assert(v >= -m);
    return v & ((1 << qbits)-1);
}

/* Output one audio block. There are NB_BLOCKS audio blocks in one AC3
   frame */
static void output_audio_block(AC3EncodeContext *s,
                               uint8_t exp_strategy[AC3_MAX_CHANNELS],
                               uint8_t encoded_exp[AC3_MAX_CHANNELS][N/2],
                               uint8_t bap[AC3_MAX_CHANNELS][N/2],
//                               int32_t mdct_coefs[AC3_MAX_CHANNELS][N/2],
                               sample_t mdct_coefs[AC3_MAX_CHANNELS][N/2],
                               int8_t global_exp[AC3_MAX_CHANNELS],
                               int block_num)
{
    int ch, nb_groups, group_size, i, baie, rbnd;
    uint8_t *p;
    uint16_t qmant[AC3_MAX_CHANNELS][N/2];
    int exp0, exp1;
    int mant1_cnt, mant2_cnt, mant4_cnt;
    uint16_t *qmant1_ptr, *qmant2_ptr, *qmant4_ptr;
    int delta0, delta1, delta2;

    for(ch=0;ch<s->nb_channels;ch++)
        put_bits(&s->pb, 1, s->blksw[block_num][ch]); /* 0=512 point MDCT, 1=256 point MDCT */
    for(ch=0;ch<s->nb_channels;ch++)
        put_bits(&s->pb, 1, 0); /* no dither */
    put_bits(&s->pb, 1, 0); /* no dynamic range */
    if (block_num == 0) {
        /* for block 0, even if no coupling, we must say it. This is a
           waste of bit :-) */
        put_bits(&s->pb, 1, 1); /* coupling strategy present */
        put_bits(&s->pb, 1, 0); /* no coupling strategy */
    } else {
        put_bits(&s->pb, 1, 0); /* no new coupling strategy */
    }

    if (s->acmod == 2) {
	    if(block_num==0) {
	    /* first block must define rematrixing (rematstr)  */
	        put_bits(&s->pb, 1, 1);

	        /* since we do not use couling, rbnd is always 4 */
	        for (rbnd=0;rbnd<4;rbnd++)
	            put_bits(&s->pb, 1, s->rematflg[block_num][rbnd]);
	    }
	    else {
            if (s->rematstr[block_num] == 1) {
    	        put_bits(&s->pb, 1, 1);
                /* since we do not use couling, rbnd is always 4 */
                for (rbnd = 0; rbnd < 4; rbnd++) {
                    put_bits(&s->pb, 1, s->rematflg[block_num][rbnd]);
                }
            }
            else
     	        put_bits(&s->pb, 1, 0);
	    }
   }

#if defined(DEBUG)
    {
      static int count = 0;
      av_log(NULL, AV_LOG_DEBUG, "Block #%d (%d)\n", block_num, count++);
    }
#endif
    /* exponent strategy */
    for(ch=0;ch<s->nb_channels;ch++) {
        put_bits(&s->pb, 2, exp_strategy[ch]);
    }

    if (s->lfe) {
	put_bits(&s->pb, 1, exp_strategy[s->lfe_channel]);
    }

    for(ch=0;ch<s->nb_channels;ch++) {
        if (exp_strategy[ch] != EXP_REUSE)
            put_bits(&s->pb, 6, s->chbwcod[block_num][ch]);
    }

    /* exponents */
    for (ch = 0; ch < s->nb_all_channels; ch++) {
        switch(exp_strategy[ch]) {
        case EXP_REUSE:
            continue;
        case EXP_D15:
            group_size = 1;
            break;
        case EXP_D25:
            group_size = 2;
            break;
        default:
        case EXP_D45:
            group_size = 4;
            break;
        }
	nb_groups = (s->nb_coefs[block_num][ch] + (group_size * 3) - 4) / (3 * group_size);
        p = encoded_exp[ch];

        /* first exponent */
        exp1 = *p++;
        put_bits(&s->pb, 4, exp1);

        /* next ones are delta encoded */
        for(i=0;i<nb_groups;i++) {
            /* merge three delta in one code */
            exp0 = exp1;
            exp1 = p[0];
            p += group_size;
            delta0 = exp1 - exp0 + 2;

            exp0 = exp1;
            exp1 = p[0];
            p += group_size;
            delta1 = exp1 - exp0 + 2;

            exp0 = exp1;
            exp1 = p[0];
            p += group_size;
            delta2 = exp1 - exp0 + 2;

            put_bits(&s->pb, 7, ((delta0 * 5 + delta1) * 5) + delta2);
        }

	if (ch != s->lfe_channel)
	    put_bits(&s->pb, 2, 0); /* no gain range info */
    }

    /* bit allocation info */
    baie = (block_num == 0);
    put_bits(&s->pb, 1, baie);
    if (baie) {
        put_bits(&s->pb, 2, s->sdecaycod);
        put_bits(&s->pb, 2, s->fdecaycod);
        put_bits(&s->pb, 2, s->sgaincod);
        put_bits(&s->pb, 2, s->dbkneecod);
        put_bits(&s->pb, 3, s->floorcod);
    }

    /* snr offset */
    put_bits(&s->pb, 1, baie); /* always present with bai */
    if (baie) {
        put_bits(&s->pb, 6, s->csnroffst);
        for(ch=0;ch<s->nb_all_channels;ch++) {
            put_bits(&s->pb, 4, s->fsnroffst[ch]);
            put_bits(&s->pb, 3, s->fgaincod[ch]);
        }
    }

    put_bits(&s->pb, 1, 0); /* no delta bit allocation */
    put_bits(&s->pb, 1, 0); /* no data to skip */

    /* mantissa encoding : we use two passes to handle the grouping. A
       one pass method may be faster, but it would necessitate to
       modify the output stream. */

    /* first pass: quantize */
    mant1_cnt = mant2_cnt = mant4_cnt = 0;
    qmant1_ptr = qmant2_ptr = qmant4_ptr = NULL;

    for (ch = 0; ch < s->nb_all_channels; ch++) {
        int b, e, v;
        int32_t c;

        for(i=0;i<s->nb_coefs[block_num][ch];i++) {
            c = (int32_t) mdct_coefs[ch][i];
            e = encoded_exp[ch][i] - global_exp[ch];
            b = bap[ch][i];
            switch(b) {
            case 0:
                v = 0;
                break;
            case 1:
                v = sym_quant(c, e, 3);
                switch(mant1_cnt) {
                case 0:
                    qmant1_ptr = &qmant[ch][i];
                    v = 9 * v;
                    mant1_cnt = 1;
                    break;
                case 1:
                    *qmant1_ptr += 3 * v;
                    mant1_cnt = 2;
                    v = 128;
                    break;
                default:
                    *qmant1_ptr += v;
                    mant1_cnt = 0;
                    v = 128;
                    break;
                }
                break;
            case 2:
                v = sym_quant(c, e, 5);
                switch(mant2_cnt) {
                case 0:
                    qmant2_ptr = &qmant[ch][i];
                    v = 25 * v;
                    mant2_cnt = 1;
                    break;
                case 1:
                    *qmant2_ptr += 5 * v;
                    mant2_cnt = 2;
                    v = 128;
                    break;
                default:
                    *qmant2_ptr += v;
                    mant2_cnt = 0;
                    v = 128;
                    break;
                }
                break;
            case 3:
                v = sym_quant(c, e, 7);
                break;
            case 4:
                v = sym_quant(c, e, 11);
                switch(mant4_cnt) {
                case 0:
                    qmant4_ptr = &qmant[ch][i];
                    v = 11 * v;
                    mant4_cnt = 1;
                    break;
                default:
                    *qmant4_ptr += v;
                    mant4_cnt = 0;
                    v = 128;
                    break;
                }
                break;
            case 5:
                v = sym_quant(c, e, 15);
                break;
            case 14:
                v = asym_quant(c, e, 14);
                break;
            case 15:
                v = asym_quant(c, e, 16);
                break;
            default:
                v = asym_quant(c, e, b - 1);
                break;
            }
            qmant[ch][i] = v;
        }
    }

    /* second pass : output the values */
    for (ch = 0; ch < s->nb_all_channels; ch++) {
        int b, q;

        for(i=0;i<s->nb_coefs[block_num][ch];i++) {
            q = qmant[ch][i];
            b = bap[ch][i];
            switch(b) {
            case 0:
                break;
            case 1:
                if (q != 128)
                    put_bits(&s->pb, 5, q);
                break;
            case 2:
                if (q != 128)
                    put_bits(&s->pb, 7, q);
                break;
            case 3:
                put_bits(&s->pb, 3, q);
                break;
            case 4:
                if (q != 128)
                    put_bits(&s->pb, 7, q);
                break;
            case 14:
                put_bits(&s->pb, 14, q);
                break;
            case 15:
                put_bits(&s->pb, 16, q);
                break;
            default:
                put_bits(&s->pb, b - 1, q);
                break;
            }
        }
    }
}


static void lshift_tab(int16_t *tab, int n, int lshift)
{
    int i;

    if (lshift > 0) {
        for(i=0;i<n;i++) {
            tab[i] <<= lshift;
        }
    } else if (lshift < 0) {
        lshift = -lshift;
        for(i=0;i<n;i++) {
            tab[i] >>= lshift;
        }
    }
}

/* simple hipass filter */
static void hifilter(int16_t *out, int16_t *in, uint freqCut, uint freqSample, int count, int stage)
{
    sample_t coef0, coef1, coef2, coef3, coef4;
    sample_t x1, x2, y1, y2;
    int i, j;
    sample_t A, Q, omega, sn, cs, alpha, beta, dbGain, S, gaincorrect;
    sample_t a0, a1, a2, b0, b1, b2;


    sample_t input[N], output[N];

    for (i = 0; i < count; i++)
        input[i] = (sample_t) in[i];

    /* setup variables */
    dbGain = -9.0;
    gaincorrect = 0.895; // thsi is pure try-and-error, should be 0.8707173, but rounding errors ??
    Q = pow(10.0, 0.0/20.0); // = 1

    A = pow(10, dbGain /40);
    omega = 2 * M_PI * (sample_t) freqCut / (sample_t) freqSample;
    //omega = 2 * M_PI * 8000.0 / 44100.0;
    sn = sin(omega);
    cs = cos(omega);
    alpha = sn / (2 * Q);
    S = 1.0;
    beta = sqrt((A*A +1)/S - (A-1)*(A-1));

    b0 = (1 + cs) /2;
    b1 = -(1 + cs);
    b2 = (1 + cs) /2;
    a0 = 1 + alpha;
    a1 = -2 * cs;
    a2 = 1 - alpha;

    /* precompute the coefficients */
    coef0 = gaincorrect * b0 /a0;
    coef1 = gaincorrect * b1 /a0;
    coef2 = gaincorrect * b2 /a0;
    coef3 = gaincorrect * a1 /a0;
    coef4 = gaincorrect * a2 /a0;

    for (j = 0; j < stage; j++) {
    /* zero initial samples */
        x1 = x2 = y1 = y2 = 0;

        for (i = 0; i < count; i++) {
            /* compute result */
            output[i] =  (coef0 * input[i] + coef1 * x1 + coef2 * x2 - coef3 * y1 - coef4 * y2);
            /* shift x1 to x2, sample to x1 */
            x2 = x1;
            x1 = input[i];
            /* shift y1 to y2, result to y1 */
            y2 = y1;
            y1 = output[i];
        }

        for (i = 0; i < count; i ++)
            input[i] = output[i];

    }
    for (i = 0; i < count; i ++)
    out[i] = (int16_t) output[i];
}

void dcfilter(int16_t *in, unsigned int srate)
{
    int i;
    double r;
    sample_t y;
    int16_t out[N];

    r = 1.0 - (100.0/srate);
    for (i = 0; i < N; i++) {
        y = in[i] - fx1 + r * fy1;
        fx1 = in[i];
        if (y > 32767.0)
            y = 32767.0;
        if (y < -32768.0)
            y = -32768.0;
        fy1 = y;
        /* conv samples to int again, round to nearest */
        if ( y > 0)
            in[i] = (int16_t) (y + 0.5);
        if ( y < 0)
            in[i] = (int16_t) (y - 0.5);
    }
}


/* transient detection */
static int detect_transient(int16_t *in, unsigned int srate)
{
    int blksw = 0;
    int i, j, block, seg;
    int16_t tmp, p;
    int16_t peak[2][4][5];
    int16_t filter_samples[N];

    hifilter(filter_samples, in, 8000, srate, N, 2);
    /* block segmentation  and peak detection */
    /* two blocks, 256 samples each */
    for (block = 0; block < 2; block++) {
        p = 0;
        j = block * N/2;
        for (i = 0; i < N/2; i++) {
            tmp = filter_samples[j + i];
            if (tmp > p)
                p = tmp;

            if (i == 63) {
                peak[block][3][1] = p; // seg 1 in level 3
                p = 0;
            }
            if (i == 127) {
                peak[block][3][2] = p; // seg 2 in level 3

                if (peak[block][3][1] > p)
                    p = peak[block][3][1];

                peak[block][2][1] = p; // seg 1 in level 2
                p = 0;
            }
            if (i == 192) {
                peak[block][3][3] = p; // seg 3 in level 3
                p = 0;
            }
            if (i == 255) {
                peak[block][3][4] = p; // seg 4 in level 3

                if (peak[block][3][3] > p)
                    p = peak[block][3][3];

                peak[block][2][2] = p; // seg 2 in level 2

                if (peak[block][2][1] > p)
                    p = peak[block][2][1];

                peak[block][1][1] = p; // peak in level 1
                p = 0;
            }
        } // for i
    } // for block
    peak[0][1][0] = 32767;
    peak[0][2][0] = 32767;
    peak[0][3][0] = 32767;
    peak[1][1][0] = peak[0][1][1];
    peak[1][2][0] = peak[0][2][2];
    peak[1][3][0] = peak[0][3][4];
    /* threshold comparison */
    /* does this to be checked in both blocks? IMO only in the second block. ??? */
    if (peak[1][1][1] < 100)
        return 0;           // force 512 block, ????

    /* level 1 */
    seg = 1;
    if (((int16_t)(peak[1][1][seg] * 0.1)) > peak[1][1][seg-1])
         blksw = 1;
    /* level 2 */
    for (seg = 1; seg < 3; seg++) {
        if (((int16_t)(peak[1][2][seg] * 0.075)) > peak[1][2][seg-1])
            blksw = 1;
    }
    /* level 3 */
    for (seg = 1; seg < 5; seg++) {
        if (((int16_t)(peak[1][3][seg] * 0.05)) > peak[1][3][seg-1])
            blksw = 1;
    }

    return blksw;
}

/* rematrixing */
static void rematrix_frame(AC3EncodeContext *s,
                        sample_t mdct_coefs[NB_BLOCKS][AC3_MAX_CHANNELS][N/2],
                        int8_t exp_samples[NB_BLOCKS][AC3_MAX_CHANNELS]
                        )
{
    int i, j, rband, start, end;
    int8_t shift;
    sample_t lch, rch, l, r, lpr, lmr, cl, cr, lmult, rmult;
    int rematband[5] = {13, 25, 37, 61, 253};

    for(i = 0; i < NB_BLOCKS; i++) {
        /* since we do not use coupling, we always have 4 bands */
        for (rband = 0; rband < 4; rband++) {
            start = rematband[rband];
            end = rematband[rband + 1];
            if (end > s->nb_coefs[i][0])
                end = s->nb_coefs[i][0];
            if (end > s->nb_coefs[i][1])
                end = s->nb_coefs[i][1];

            l = r = lpr = lmr = 0;
            for (j = start; j < end; j++) {
                cl = mdct_coefs[i][0][j] / 32768.0;
                cr = mdct_coefs[i][1][j] / 32768.0;
                l += (cl * cl);
                r += (cr * cr);
                lpr += ((cl + cr) * (cl + cr));
                lmr += ((cl - cr) * (cl - cr));
            }
            s->rematflg[i][rband] = 0;

            if ((lpr < l) || (lpr < r))
                s->rematflg[i][rband] = 1;

            if ((lmr < l) || (lmr < r))
                s->rematflg[i][rband] = 1;

            if (s->rematflg[i][rband]) {
                s->rematstr[i] = 1;
                /* take into account the normalizlation we have done before */
                shift = exp_samples[i][0] - exp_samples[i][1];
                rmult = pow(2.0, shift);
                lmult = pow(2.0, -shift);
                /* now do the rematrixing for band0 */
                for (j = start; j < end; j++) {
                    lch = mdct_coefs[i][0][j];
                    rch = mdct_coefs[i][1][j];
                    mdct_coefs[i][0][j] = (lch + rch * rmult) / 2 ;
                    mdct_coefs[i][1][j] = (lch * lmult - rch) / 2 ;
                }
            }
        } /* for rband */
    } /* for i */

}


/* compute the ac3 crc */

#define CRC16_POLY ((1 << 0) | (1 << 2) | (1 << 15) | (1 << 16))

static void ac3_crc_init(void)
{
    unsigned int c, n, k;

    for(n=0;n<256;n++) {
        c = n << 8;
        for (k = 0; k < 8; k++) {
            if (c & (1 << 15))
                c = ((c << 1) & 0xffff) ^ (CRC16_POLY & 0xffff);
            else
                c = c << 1;
        }
        crc_table[n] = c;
    }
}

static unsigned int ac3_crc(uint8_t *data, int n, unsigned int crc)
{
    int i;
    for(i=0;i<n;i++) {
        crc = (crc_table[data[i] ^ (crc >> 8)] ^ (crc << 8)) & 0xffff;
    }
    return crc;
}

static unsigned int mul_poly(unsigned int a, unsigned int b, unsigned int poly)
{
    unsigned int c;

    c = 0;
    while (a) {
        if (a & 1)
            c ^= b;
        a = a >> 1;
        b = b << 1;
        if (b & (1 << 16))
            b ^= poly;
    }
    return c;
}

static unsigned int pow_poly(unsigned int a, unsigned int n, unsigned int poly)
{
    unsigned int r;
    r = 1;
    while (n) {
        if (n & 1)
            r = mul_poly(r, a, poly);
        a = mul_poly(a, a, poly);
        n >>= 1;
    }
    return r;
}


/* compute log2(max(abs(tab[]))) */
static int log2_tab(int16_t *tab, int n)
{
    int i;
    int16_t v;

    v = 0;
    for(i=0;i<n;i++) {
        v |= abs(tab[i]);
    }
    return av_log2(v);
}


/* fill the end of the frame and compute the two crcs */
static int output_frame_end(AC3EncodeContext *s)
{
    int frame_size, frame_size_58, n, crc1, crc2, crc_inv;
    uint8_t *frame;

    frame_size = s->frame_size; /* frame size in words */
    /* align to 8 bits */
    flush_put_bits(&s->pb);
    /* add zero bytes to reach the frame size */
    frame = s->pb.buf;
    n = 2 * s->frame_size - (pbBufPtr(&s->pb) - frame) - 2;
    assert(n >= 0);
    if(n>0)
      memset(pbBufPtr(&s->pb), 0, n);

    /* Now we must compute both crcs : this is not so easy for crc1
       because it is at the beginning of the data... */
    frame_size_58 = (frame_size >> 1) + (frame_size >> 3);
    crc1 = ac3_crc(frame + 4, (2 * frame_size_58) - 4, 0);
    /* XXX: could precompute crc_inv */
    crc_inv = pow_poly((CRC16_POLY >> 1), (16 * frame_size_58) - 16, CRC16_POLY);
    crc1 = mul_poly(crc_inv, crc1, CRC16_POLY);
    frame[2] = crc1 >> 8;
    frame[3] = crc1;

    crc2 = ac3_crc(frame + 2 * frame_size_58, (frame_size - frame_size_58) * 2 - 2, 0);
    frame[2*frame_size - 2] = crc2 >> 8;
    frame[2*frame_size - 1] = crc2;

    //    printf("n=%d frame_size=%d\n", n, frame_size);
    return frame_size * 2;
}

static int AC3_encode_frame(AVCodecContext *avctx,
                            unsigned char *frame, int buf_size, void *data)
{
    AC3EncodeContext *s = avctx->priv_data;
    int16_t *samples = data;
    int i, j, k, v, ch, coefs;
    int16_t input_samples[N];
    sample_t mdct_coef[NB_BLOCKS][AC3_MAX_CHANNELS][N/2];
    uint8_t exp[NB_BLOCKS][AC3_MAX_CHANNELS][N/2];
    uint8_t exp_strategy[NB_BLOCKS][AC3_MAX_CHANNELS];
    uint8_t encoded_exp[NB_BLOCKS][AC3_MAX_CHANNELS][N/2];
    uint8_t bap[NB_BLOCKS][AC3_MAX_CHANNELS][N/2];
    int8_t exp_samples[NB_BLOCKS][AC3_MAX_CHANNELS];
    int frame_bits;

    frame_bits = 0;

    for(ch=0;ch<s->nb_all_channels;ch++) {
        /* fixed mdct to the six sub blocks & exponent computation */
        for(i=0;i<NB_BLOCKS;i++) {
            int16_t *sptr;
            int sinc;

            /* compute input samples */
            memcpy(input_samples, s->last_samples[ch], N/2 * sizeof(int16_t));
            sinc = s->nb_all_channels;
            sptr = samples + (sinc * (N/2) * i) + ch;
            for(j=0;j<N/2;j++) {
                v = *sptr;
                input_samples[j + N/2] = v;
                s->last_samples[ch][j] = v;
                sptr += sinc;
            }

            /* dc highpass filter */
            dcfilter(input_samples, s->sample_rate);

            /* transient detection */
            //s->blksw[i][ch] = detect_transient(input_samples, s->sample_rate);
            s->blksw[i][ch] = 0; // do not use block switching for now.

            /* apply the MDCT window */
            for (j = 0; j < N/2; j++) {
                input_samples[j] = (int16_t) MUL16(input_samples[j], ac3_window[j]);
                input_samples[N-j-1] = (int16_t) MUL16(input_samples[N-j-1], ac3_window[j]);
            }

            /* Normalize the samples to use the maximum available
               precision */
            v = 14 - log2_tab(input_samples, N);
            if (v < 0)
                v = 0;
            exp_samples[i][ch] = v - 8;
            lshift_tab(input_samples, N, v);

            /* do the MDCT */
            if (s->blksw[i][ch]) {
                /* do 256 point mdct */
                mdct256(mdct_coef[i][ch], input_samples);
            }
            else {
                /* do 512 point mdct */
                mdct512(mdct_coef[i][ch], input_samples);
            }

            /* mute unused coeffs above bandwith */
            for (j = s->max_coef; j<N/2; j++) {
                mdct_coef[i][ch][j] = 0;
            }
        }
    }

    /* rematrixing in 2/0 mode, needs all channels to be already done mdct !!! */
    if (s->acmod == 2)
        rematrix_frame(s, mdct_coef, exp_samples);

    for(ch=0;ch<s->nb_all_channels;ch++) {
        for(i=0;i<NB_BLOCKS;i++) {
            /* compute "exponents". We take into account the
               normalization there */
            for(j=0;j<N/2;j++) {
                int e;
                v = (int) (abs(mdct_coef[i][ch][j]));
                if (v == 0)
                    e = 24;
                else {
                    e = 23 - av_log2(v) + exp_samples[i][ch];
                    if (e >= 24) {
                        e = 24;
                        mdct_coef[i][ch][j] = 0;
                    }
                }
                exp[i][ch][j] = e;
            }

        }

        /* compute_exp_strategy(exp_strategy, exp, ch, ch == s->lfe_channel);  orig */
        compute_exp_strategy(exp_strategy, exp, ch, ch == s->lfe_channel, s->nb_coefs[0][ch]);

        /* compute the exponents as the decoder will see them. The
           EXP_REUSE case must be handled carefully : we select the
           min of the exponents */
        i = 0;
        while (i < NB_BLOCKS) {
            j = i + 1;
            while (j < NB_BLOCKS && exp_strategy[j][ch] == EXP_REUSE) {
// orig               exponent_min(exp[i][ch], exp[j][ch], s->nb_coefs[ch]);
                exponent_min(exp[i][ch], exp[j][ch], s->nb_coefs[i][ch]);
                j++;
            }
            frame_bits += encode_exp(encoded_exp[i][ch],
                                     exp[i][ch], s->nb_coefs[i][ch],
// orig                                     exp[i][ch], s->nb_coefs[ch],
                                     exp_strategy[i][ch]);
            /* copy encoded exponents for reuse case */
            for(k=i+1;k<j;k++) {
                memcpy(encoded_exp[k][ch], encoded_exp[i][ch],
                       s->nb_coefs[i][ch] * sizeof(uint8_t));
// orig                    s->nb_coefs[ch] * sizeof(uint8_t));
            }
            i = j;
        }

    }

    compute_bit_allocation(s, bap, encoded_exp, exp_strategy, frame_bits);
    /* everything is known... let's output the frame */
    output_frame_header(s, frame);

    for(i=0;i<NB_BLOCKS;i++) {
        output_audio_block(s, exp_strategy[i], encoded_exp[i],
                           bap[i], mdct_coef[i], exp_samples[i], i);
    }
    return output_frame_end(s);
}

static int AC3_encode_close(AVCodecContext *avctx)
{
    av_freep(&avctx->coded_frame);
    return 0;
}

#if 0
/*************************************************************************/
/* TEST */

#define FN (N/4)

void fft_test(void)
{
    IComplex in[FN], in1[FN];
    int k, n, i;
    float sum_re, sum_im, a;

    /* FFT test */

    for(i=0;i<FN;i++) {
        in[i].re = random() % 65535 - 32767;
        in[i].im = random() % 65535 - 32767;
        in1[i] = in[i];
    }
    fft(in, 7);

    /* do it by hand */
    for(k=0;k<FN;k++) {
        sum_re = 0;
        sum_im = 0;
        for(n=0;n<FN;n++) {
            a = -2 * M_PI * (n * k) / FN;
            sum_re += in1[n].re * cos(a) - in1[n].im * sin(a);
            sum_im += in1[n].re * sin(a) + in1[n].im * cos(a);
        }
        printf("%3d: %6d,%6d %6.0f,%6.0f\n",
               k, in[k].re, in[k].im, sum_re / FN, sum_im / FN);
    }
}

void mdct_test(void)
{
    int16_t input[N];
    int32_t output[N/2];
    float input1[N];
    float output1[N/2];
    float s, a, err, e, emax;
    int i, k, n;

    for(i=0;i<N;i++) {
        input[i] = (random() % 65535 - 32767) * 9 / 10;
        input1[i] = input[i];
    }

    mdct512(output, input);
    
    /* do it by hand */
    for(k=0;k<N/2;k++) {
        s = 0;
        for(n=0;n<N;n++) {
            a = (2*M_PI*(2*n+1+N/2)*(2*k+1) / (4 * N));
            s += input1[n] * cos(a);
        }
        output1[k] = -4 * s / N;
    }
    
    err = 0;
    emax = 0;
    for(i=0;i<N/2;i++) {
        printf("%3d: %7d %7.0f\n", i, output[i], output1[i]);
        e = output[i] - output1[i];
        if (e > emax)
            emax = e;
        err += e * e;
    }
    printf("err2=%f emax=%f\n", err / (N/2), emax);
}

void test_ac3(void)
{
    AC3EncodeContext ctx;
    unsigned char frame[AC3_MAX_CODED_FRAME_SIZE];
    short samples[AC3_FRAME_SIZE];
    int ret, i;
    
    AC3_encode_init(&ctx, 44100, 64000, 1);

    fft_test();
    mdct_test();

    for(i=0;i<AC3_FRAME_SIZE;i++)
        samples[i] = (int)(sin(2*M_PI*i*1000.0/44100) * 10000);
    ret = AC3_encode_frame(&ctx, frame, samples);
    printf("ret=%d\n", ret);
}
#endif

AVCodec ac3_encoder = {
    "ac3",
    CODEC_TYPE_AUDIO,
    CODEC_ID_AC3,
    sizeof(AC3EncodeContext),
    AC3_encode_init,
    AC3_encode_frame,
    AC3_encode_close,
    NULL,
};
