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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"


#include "audioeng_mixer.h"

typedef int *DOWMIXER(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan);



AVDMProcessAudio_Mixer::AVDMProcessAudio_Mixer(AVDMGenericAudioStream * instream,CHANNEL_CONF out):AVDMBufferedAudioStream (instream)
{
    WAVHeader *src;
     
    _wavheader = new WAVHeader;
    _output=out;
    src=_instream->getInfo();
    memcpy(_wavheader, src, sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    strcpy(_name, "PROC:Mixers");
    _instream->goToTime(0);     // rewind
    ADM_assert(_output<CHANNEL_LAST);
    _wavheader->channels=ADM_channel_mixer[_output];
    
    double d;
    
    d=_instream->getLength();
    d/=src->channels;
    d*=_wavheader->channels;
    _length = (uint32_t)ceil(d);
    
    d=src->byterate;
    d/=src->channels;
    d*=_wavheader->channels;
    _wavheader->byterate=(uint32_t)ceil(d);
    
    int chan;
    chan=src->channels;
    switch(chan)
    {
        case 6: _input=CHANNEL_3F_2R_LFE;break;
        case 5: _input=CHANNEL_3F_2R;break;
        case 1: _input=CHANNEL_MONO;break;
        case 2: _input=CHANNEL_STEREO;break;
        default:
            ADM_assert(0);
    }
    printf("[mixer]Input channels : %u : %u \n",src->channels,ADM_channel_mixer[_input]);
    printf("[mixer]Input Byterate : %u  \n",src->byterate);
    printf("[mixer]Out   channels : %u : %u \n",_wavheader->channels,ADM_channel_mixer[_output]);
    printf("[mixer]Out   Byterate : %u  \n",_wavheader->byterate);
  
    
};

AVDMProcessAudio_Mixer::~AVDMProcessAudio_Mixer()
{
    delete(_wavheader);
    _wavheader=NULL;
};
///******************************************************************
///                Dolby Prologic 2
///******************************************************************
/* -----------------------------------------
    Dolby Prologic 2 downmixing 3-2
 ----------------------------------------- 
*/
#define SHIFTED 12
#define SCALE (2<<SHIFTED)
#define XSCALE (SCALE*0.3225)
const int DB2_coef[5]={
    (int)(XSCALE),
    (int)(0.707*XSCALE),
    (int)(XSCALE),
    (int)(0.8165*XSCALE),
    (int)(0.5774*XSCALE)
};
#define MIX(a,b) (((int32_t)in[a])*DB2_coef[b]) 
/* -----------------------------------------
    Dolby Prologic 2 downmixing 3-2
 ----------------------------------------- 
*/

static int M32_2_DB2(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    for(int i=0;i<nbSample;i++)
    {
        out[0]=(int16_t)((MIX(0,0)+MIX(1,1)-MIX(3,3) -MIX(4,4))>>SHIFTED);
        out[1]=(int16_t)((MIX(2,2)+MIX(1,1)+MIX(3,4) +MIX(4,3))>>SHIFTED);
        in+=chan;
        out+=2;
    }
    return nbSample*2*2;
}
/* -----------------------------------------
    Dolby Prologic 2 downmixing 2-2
 ----------------------------------------- 
*/
static int M22_2_DB2(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    for(int i=0;i<nbSample;i++)
    {
        out[0]=(int16_t)((MIX(0,0)-MIX(2,3) -MIX(3,4))>>SHIFTED);
        out[1]=(int16_t)((MIX(1,2)+MIX(3,3) +MIX(2,4))>>SHIFTED);
        in+=chan;
        out+=2;
    }
    return nbSample*2*2;
}
///******************************************************************
///                Dolby Prologic 1
///******************************************************************
const int DB1_coef[2]={
    (int)(XSCALE),
    (int)(0.707*XSCALE),
    
};
#undef MIX
#define MIX(a,b) (((int32_t)in[a])*DB1_coef[b]) 
/* -----------------------------------------
    Dolby Prologic 1 downmixing 3-2
 ----------------------------------------- 
*/
static int M3_2_DB1(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    int32_t center;
    for(int i=0;i<nbSample;i++)
    {
        center=((int32_t)in[1]*DB1_coef[1]);
        out[0]=(int16_t)((MIX(0,0)+center)>>SHIFTED);
        out[1]=(int16_t)((MIX(2,0)+center)>>SHIFTED);
        in+=5;
        out+=2;
    }
    return nbSample*2*2;
}
/* -----------------------------------------
    Dolby Prologic 1 downmixing 3-1
 ----------------------------------------- 
*/
static int M31_2_DB1(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    int32_t rear,center;
    for(int i=0;i<nbSample;i++)
    {
        center=((int32_t)in[1]*DB1_coef[1]);
        rear=((int32_t)in[3]*DB1_coef[1]);
        out[0]=(int16_t)((MIX(0,0)+center -rear)>>SHIFTED);
        out[1]=(int16_t)((MIX(2,0)+center+rear)>>SHIFTED);
        in+=5;
        out+=2;
    }
    return nbSample*2*2;
}
static int M32_2_DB1(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    int32_t surround;
    for(int i=0;i<nbSample;i++)
    {
        surround=((int32_t)in[3]+(int32_t)in[4])*DB1_coef[1];
        out[0]=(int16_t)((MIX(0,0)+MIX(1,1)-surround)>>SHIFTED);
        out[1]=(int16_t)((MIX(2,0)+MIX(1,1)+surround)>>SHIFTED);
        in+=5;
        out+=2;
    }
    return nbSample*2*2;
}
/* -----------------------------------------
    Dolby Prologic 1 downmixing 2-2
 ----------------------------------------- 
*/
static int M22_2_DB1(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{       
    int32_t surround;
    for(int i=0;i<nbSample;i++)
    {
        surround=(in[2]+in[3])*DB1_coef[1];
        out[0]=(int16_t)((MIX(0,0)-surround)>>SHIFTED);
        out[1]=(int16_t)((MIX(1,0)+surround)>>SHIFTED);
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
static int MCOPY(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{
    memcpy(out,in,nbSample*2*chan);
    return nbSample*2*chan;
    
}
static int M2to1(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{
    for(int i=0;i<nbSample;i++)
    {
        *out++=*in;
        in+=2;   
    }
    return nbSample*2*nbSample;
    
}
static int M1to2(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)
{
    for(int i=0;i<nbSample;i++)
    {
        out[0]=out[1]=*in++;
        out+=2;   
    }
    return nbSample*2*nbSample*2;
    
}


typedef int MIXER(int16_t *in,int16_t *out,uint32_t nbSample,uint32_t chan)  ;

static MIXER *matrixCall[CHANNEL_LAST][CHANNEL_LAST] // output / input
=
{
// INVALID=0,    MONO,STEREO, 2F_1R,            3F,   3F_1R,  2F_2R,  3F_2R,          3F_2R_LFE,  SURROUND, PROLOGIC,  PROLOGIC2,
    {NULL,NULL,NULL,NULL,                           NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    //MONO    
    {NULL,NULL,M2to1,NULL,                          NULL,NULL,NULL,NULL,                  NULL,NULL,NULL,NULL},
    // STEREO
    {NULL,M1to2,NULL,NULL,                          NULL,NULL,NULL,NULL,                  NULL,M2to1,M2to1,M2to1},
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
uint32_t AVDMProcessAudio_Mixer::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in,*out;
    int32_t nbout;
    uint32_t asked, done,window;
#undef NB_SAMPLE
#define NB_SAMPLE (256)

// Read NB_SAMPLE samples
    in = mixBuffer;

    window=NB_SAMPLE*2*ADM_channel_mixer[_input];
    ADM_assert(window);
    ADM_assert(MIXER_BUFFER_SIZE>window);
    while (rdall < window)
    {
          // don't ask too much front.
        asked = window - rdall;
        rd = _instream->read(asked, in + rdall);
        rdall += rd;
        if (rd == 0)
            break;
    }
    // Block not filled
    if (rdall != window)
    {
        printf("[Mixer] not enough...%lu\n", rdall);
        if (rdall == 0)
            return MINUS_ONE; // we could not get a single byte ! End of stream
          // Else fillout with 0
        memset(in + rdall, 0, window - rdall);
    }
    // Now do the downsampling
    MIXER *call=matrixCall[_output][_input];
    if(!call)
        return (uint32_t)MCOPY((int16_t *)mixBuffer,(int16_t *)obuffer,NB_SAMPLE,ADM_channel_mixer[_input]);
    return (uint32_t)call((int16_t *)mixBuffer,(int16_t *)obuffer,NB_SAMPLE,ADM_channel_mixer[_input]);
    
}
