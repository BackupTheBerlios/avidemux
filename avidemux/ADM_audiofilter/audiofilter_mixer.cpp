/***************************************************************************
         Downmixer
 ***************************************************************************/
 
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ADM_assert.h>



#include "avifmt.h"
#include "avifmt2.h"



#include "audioeng_process.h"
#include "audiofilter_mixer.h"

typedef int *DOWMIXER_f(float *in,float *out,uint32_t nbSample,uint32_t chan);

static float CLIP(float in)
{
  if(in>1.0) in=1.0;
  if(in<-1.0) in=-1.0;
  return in;
}

AUDMAudioFilterMixer::AUDMAudioFilterMixer(AUDMAudioFilter *instream,CHANNEL_CONF out):AUDMAudioFilter (instream)
{
    _output=out;
    _previous->rewind();     // rewind
    ADM_assert(_output<CHANNEL_LAST);
    
    
    double d;               // Update duration
    d=_wavHeader.byterate;
    d/=_wavHeader.channels;
    _wavHeader.channels=ADM_channel_mixer[_output];
    d*=_wavHeader.channels;
    _wavHeader.byterate = (uint32_t)ceil(d);
    
    int chan;
    chan=_previous->getInfo()->channels;
    switch(chan)
    {
        case 6: _input=CHANNEL_3F_2R_LFE;break;
        case 5: _input=CHANNEL_3F_2R;break;
        case 1: _input=CHANNEL_MONO;break;
        case 2: _input=CHANNEL_STEREO;break;
        default:
            printf("Invalid channel configuration %u\n",chan);
            ADM_assert(0);
    }
    printf("[mixer]Input channels : %u : %u \n",_previous->getInfo()->channels,ADM_channel_mixer[_input]);
    printf("[mixer]Out   channels : %u : %u \n",_wavHeader.channels,ADM_channel_mixer[_output]);

};

AUDMAudioFilterMixer::~AUDMAudioFilterMixer()
{
};
///******************************************************************
///                Dolby Prologic 2
///******************************************************************
/* -----------------------------------------
    Dolby Prologic 2 downmixing 3-2
 ----------------------------------------- 
*/
#define SCALER2 0.325
const float DB2_coef[5]={
    SCALER2*1.0,      // LEFT
    SCALER2*0.707,  // CENTER
    SCALER2*1.0,  // RIGHT
    SCALER2*0.8165,  // SL
    SCALER2*0.5774  // SR
};
#define MIX(a,b) ((in[a])*DB2_coef[b]) 
/* -----------------------------------------
    Dolby Prologic 2 downmixing 3-2
 ----------------------------------------- 
*/

static int M32_2_DB2(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    for(int i=0;i<nbSample;i++)
    {
        out[0]=((MIX(0,0)+MIX(1,1)-MIX(3,3) -MIX(4,4)));
        out[1]=((MIX(2,2)+MIX(1,1)+MIX(3,4) +MIX(4,3)));
        CLIP(out[0]);
        CLIP(out[1]);
        in+=chan;
        out+=2;
    }
    return nbSample*2;
}
/* -----------------------------------------
    Dolby Prologic 2 downmixing 2-2
 ----------------------------------------- 
*/
static int M22_2_DB2(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    for(int i=0;i<nbSample;i++)
    {
        out[0]=((MIX(0,0)-MIX(2,3) -MIX(3,4)));
        out[1]=((MIX(1,2)+MIX(3,3) +MIX(2,4)));
        CLIP(out[0]);
        CLIP(out[1]);
        in+=chan;
        out+=2;
    }
    return nbSample*2;
}
///******************************************************************
///                Dolby Prologic 1
///******************************************************************
#define SCALER1 0.325  // Too low
const float DB1_coef[2]={
    SCALER1*1,
    SCALER1*0.707,
    
};
#undef MIX
#define MIX(a,b) (((int32_t)in[a])*DB1_coef[b]) 
/* -----------------------------------------
    Dolby Prologic 1 downmixing 3-2
 ----------------------------------------- 
*/
static int M3_2_DB1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    float center;
    for(int i=0;i<nbSample;i++)
    {
        center=in[1]*DB1_coef[1];
        out[0]=MIX(0,0)+center ;
        out[1]=MIX(2,0)+center ;
        CLIP(out[0]);
        CLIP(out[1]);
        in+=5;
        out+=2;
    }
    return nbSample*2;
}
/* -----------------------------------------
    Dolby Prologic 1 downmixing 3-1
 ----------------------------------------- 
*/
static int M31_2_DB1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    float rear,center;
    for(int i=0;i<nbSample;i++)
    {
        center=in[1]*DB1_coef[1];
        rear=in[3]*DB1_coef[1];
        out[0]=MIX(0,0)+center -rear;
        out[1]=MIX(2,0)+center+rear;
        CLIP(out[0]);
        CLIP(out[1]);
        in+=5;
        out+=2;
    }
    return nbSample*2*2;
}
static int M32_2_DB1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    float surround;
    for(int i=0;i<nbSample;i++)
    {
        surround= in[3]+in[4]*DB1_coef[1];
        out[0]=MIX(0,0)+MIX(1,1)-surround;
        out[1]=MIX(2,0)+MIX(1,1)+surround;
        CLIP(out[0]);
        CLIP(out[1]);
        in+=chan;
        out+=2;
    }
    return nbSample*2*2;
}
/* -----------------------------------------
    Dolby Prologic 1 downmixing 2-2
 ----------------------------------------- 
*/
static int M22_2_DB1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{       
    float surround;
    for(int i=0;i<nbSample;i++)
    {
        surround=(in[2]+in[3])*DB1_coef[1];
        out[0]=MIX(0,0)-surround;
        out[1]=MIX(1,0)+surround;
        CLIP(out[0]);
        CLIP(out[1]);
        in+=4;
        out+=2;
    }
    return nbSample*2*2;
}
///******************************************************************
///                Misc
///******************************************************************
/* -----------------------------------------
        COPY
 ----------------------------------------- 
*/
static int MCOPY(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
    memcpy(out,in,nbSample*chan);
    return nbSample*chan;
    
}
static int M2to1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
    for(int i=0;i<nbSample;i++)
    {
        *out++=*in;
        in+=2;   
    }
    return nbSample;
    
}
static int M1to2(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
    for(int i=0;i<nbSample;i++)
    {
        out[0]=out[1]=*in++;
        out+=2;   
    }
    return nbSample*2;
    
}
static int MNto1(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
float sum;
int den=(chan+1)&0xfe;
    for(int i=0;i<nbSample;i++)
    {
        sum=0;
        for(int j=0;j<chan;j++)
          sum+=in[j];
        out[0]=sum/(float)den;
        out++;
        in+=chan;
    }
    return nbSample;
    
}


typedef int MIXER(float *in,float *out,uint32_t nbSample,uint32_t chan)  ;

static MIXER *matrixCall[CHANNEL_LAST][CHANNEL_LAST] // output / input
=
{
// INVALID=0,    MONO,STEREO, 2F_1R,            3F,   3F_1R,  2F_2R,  3F_2R,          3F_2R_LFE,  SURROUND, PROLOGIC,  PROLOGIC2,
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    //MONO    
    {NULL,NULL,M2to1,MNto1,                          MNto1,MNto1,MNto1,MNto1,                  MNto1,M2to1,M2to1,M2to1},
    // STEREO
    {NULL,M1to2,NULL,NULL,                          M3_2_DB1,M31_2_DB1,M22_2_DB2,M32_2_DB2,    M32_2_DB2,NULL,NULL,NULL},
    // 2F1R
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // 3F
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // 3F1R
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // 2F2R
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // 3F_2R
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // 3F_2R_LFE
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // SURROUND
    {NULL,M1to2,NULL,NULL,                          M3_2_DB1,M31_2_DB1,NULL,NULL,              NULL,NULL,NULL,NULL},
    // PROLOGIC
    {NULL,M1to2,NULL,NULL,                          M3_2_DB1,M31_2_DB1,M22_2_DB1,M32_2_DB1,    M32_2_DB1,NULL,NULL,NULL},
    // PROLOGIC 2
    {NULL,M1to2,NULL,NULL,                          M3_2_DB1,M31_2_DB1,M22_2_DB2,M32_2_DB2,    M32_2_DB2,NULL,NULL,NULL},
};
//_____________________________________________
uint32_t AUDMAudioFilterMixer::fill(uint32_t max,float *output,AUD_Status *status)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in,*out;
    int32_t nbout;
    uint32_t asked, done,window;
#undef NB_SAMPLE
#define NB_SAMPLE (256)


// Fill incoming buffer
    shrink();
    window=NB_SAMPLE*ADM_channel_mixer[_input];
    ADM_assert(window);
    ADM_assert((AUD_PROCESS_BUFFER_SIZE/2)>window);
    
    fillIncomingBuffer(status);
    // Block not filled ?
    if((_tail-_head)%window)
    {
      if(*status==AUD_END_OF_STREAM)
      {
        int leftover=(_tail-_head)%window;
        leftover=1+window-leftover;
        memset(&_incomingBuffer[_tail],0,sizeof(float)*leftover);
        printf("[Mixer] not enough, filling with zero...%lu\n", leftover);
        _tail+=leftover;
        
      }
    }
    int left=(_tail-_head)%window;
    int workingSet=(_tail-_head)-left;
    
    if(!workingSet)
    {
      *status=AUD_END_OF_STREAM;
      return 0;
    }
    // Let's go
    int processed=0;
    int outWindow=ADM_channel_mixer[_output];
    ADM_assert(max>2*outWindow);
    max-=2*outWindow;
    
    // Now do the downsampling
    MIXER *call=matrixCall[_output][_input];
    if(!call)
        return (uint32_t)MCOPY(&_incomingBuffer[_head],output,NB_SAMPLE,ADM_channel_mixer[_input]);
    return (uint32_t)call(&_incomingBuffer[_head],output,NB_SAMPLE,ADM_channel_mixer[_input]);
    
}
