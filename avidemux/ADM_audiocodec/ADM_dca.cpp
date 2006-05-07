/***************************************************************************
                          ADM_codecac3.cpp  -  description
                             -------------------
    begin                : Fri May 31 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_toolkit/ADM_cpuCap.h"
#include "prefs.h"

#ifdef USE_LIBDCA
extern "C"
{
#include "dts.h"
#include "dts_internal.h"

}
#include "ADM_audio/ADM_dcainfo.h"

#define DTS_HANDLE ((dts_state_t *)dts_handle)
static const int channel[15]=
{
    1,0,2,2,
    2,3,3,4,
    5,5,0,0,
    0,0,0
};

static const CHANNEL_CONF DTS_CONF[16]=
{
    CHANNEL_MONO,   //0
    CHANNEL_INVALID,
    CHANNEL_STEREO,
    CHANNEL_INVALID,
    CHANNEL_INVALID,
    CHANNEL_3F,      //5
    CHANNEL_2F_1R, 
    CHANNEL_3F_1R, 
    CHANNEL_2F_2R,
    CHANNEL_3F_2R, //9
    CHANNEL_INVALID,
    CHANNEL_INVALID,
    CHANNEL_INVALID,
    CHANNEL_INVALID,
    CHANNEL_INVALID,
    CHANNEL_INVALID // 15
    
};
ADM_AudiocodecDCA::ADM_AudiocodecDCA( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
    int flags=0;
    ADM_assert(fourcc==WAV_DTS);
    dts_handle=NULL;
    
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
#define CHK(x,y) if(CpuCaps::has##x()) flags|=MM_ACCEL_X86_##y;
    CHK(MMX,MMX);
    CHK(3DNOW,3DNOW);
    CHK(MMXEXT,MMXEXT);
#endif
    
    dts_handle=(void *)dts_init(flags);
    if(!dts_handle)
    {
        printf("Cannot init libdca\n");
        ADM_assert(0);   
    }
}
ADM_AudiocodecDCA::~ADM_AudiocodecDCA( )
{
    if(dts_handle)
    {
        dts_free(DTS_HANDLE);
        dts_handle=NULL;
    }

}
uint8_t ADM_AudiocodecDCA::beginDecompress( void )
{
    return 1;
}
uint8_t ADM_AudiocodecDCA::endDecompress( void )
{
    return 1;
}

uint8_t ADM_AudiocodecDCA::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut,ADM_ChannelMatrix *matrix)
{
    uint32_t avail;
    uint32_t length,syncoff;
    int flags = 0, samprate = 0, bitrate = 0, chan = 2,frame_length;
    int16_t *ptrOut = (int16_t *) outptr;
    *nbOut=0;


    //  Ready to decode
    while(nbIn)
    {
        if(nbIn<DTS_HEADER_SIZE)
        {
            if(nbIn)
                printf("[DTS]: no data to decode avail %u\n",nbIn);
            break;
        }
        //length = ADM_DCAGetInfo(ptr,nbIn, &samprate, &bitrate, &chan,&syncoff,&flags);
        length = dts_syncinfo (DTS_HANDLE,  ptr, &flags, &samprate,&bitrate, &frame_length);

        if(!length)
        {
            printf("[DTS] dts_syncinfo failed\n");
            ADM_assert(0); 
        }
        if(length>nbIn)
        {
            // not enough data
            break;
        }
     
        sample_t level = 1, bias = 0;
        
       
        if (dts_frame(DTS_HANDLE, ptr, &flags, &level, bias))
        {
            printf("\n DTS_frame failed!");
            ptr+=length;
            nbIn-=length;
            ptrOut+=256*chan;
            *nbOut+=256*2*chan;
            break;
        };
        flags &= DTS_CHANNEL_MASK;
        flags |= DTS_ADJUST_LEVEL;
        chan= channel[flags & DTS_CHANNEL_MASK];
        
        ADM_assert(chan);
        if(matrix)
        {
            matrix->nbChannel=chan;
            matrix->channelConfiguration=DTS_CONF[flags & DTS_CHANNEL_MASK];
            
        }
        ptr+=length;
        nbIn-=length;
        int16_t *cur;
        // Each block is 256 samples
        for (int i = 0; i < dts_blocks_num (DTS_HANDLE); i++) 
        {
            if (dts_block (DTS_HANDLE))
            {
                printf("\n[DTS] dts_block failed on block %d/%d\n",i,dts_blocks_num (DTS_HANDLE));
                break;
            }
            // Convert to uint16_t
            sample_t *sample_base=(sample_t *)dts_samples(DTS_HANDLE);
            cur=ptrOut;
            ptrOut+=256*chan;
            *nbOut+=256*2*chan;
            int32_t i;
            for(int k=0;k<chan;k++)
                    {
                        sample_t *sample32=(sample_t *)sample_base;
                        sample32+=256*k;
                        cur=ptrOut+k;
                        for (int j = 0; j < 256; j++)
                        {
                            i=(int32_t)((*sample32)*(32767.));
                            if(i > 32767) i=32767 ;
                            if(i < -32768) i=-32768 ;
                            *cur = (int16_t) i;
                            cur+=chan;
                            sample32++;
                        }
                    }
        }
    }
    return 1; 

}

#endif
