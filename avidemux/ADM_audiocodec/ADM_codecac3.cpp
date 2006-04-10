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

#ifdef USE_AC3
extern "C" {
#include "a52dec/a52.h"
#include "a52dec/mm_accel.h"
};

#define AC3_HANDLE ((a52_state_t *)ac3_handle)
#define AC3_SAMPLE ((sample_t *)ac3_sample)


ADM_AudiocodecAC3::ADM_AudiocodecAC3( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
    int flags=0;
    ADM_assert(fourcc==WAV_AC3);
    ac3_handle=NULL;
    ac3_sample=NULL;
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
#define CHK(x,y) if(CpuCaps::has##x()) flags|=MM_ACCEL_X86_##y;
    CHK(MMX,MMX);
    CHK(3DNOW,3DNOW);
    CHK(MMXEXT,MMXEXT);
#endif
    
    ac3_handle=(void *)a52_init(flags);
    if(!ac3_handle)
    {
        printf("Cannot init a52\n");
        ADM_assert(0);   
    }
    ac3_sample=(sample_t *)a52_samples(AC3_HANDLE);
    if(!ac3_sample)
    {
        printf("Cannot init a52 sample\n");
        ADM_assert(0);   
    }
}
ADM_AudiocodecAC3::~ADM_AudiocodecAC3( )
{
    if(ac3_handle)
    {
        a52_free(AC3_HANDLE);
        ac3_handle=NULL;
        ac3_sample=NULL;
    }
    
}
uint8_t ADM_AudiocodecAC3::beginDecompress( void )
{
		return 1;
}
uint8_t ADM_AudiocodecAC3::endDecompress( void )
{
    
    return 1;
}
/*
    The memcpy to internal buffer is normally not needed, but just to be on the safe side....
    

*/
uint8_t ADM_AudiocodecAC3::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
    uint32_t avail;
    uint32_t length;
    int flags = 0, samprate = 0, bitrate = 0, chan = 2;
    int16_t *ptrOut = (int16_t *) outptr;
    *nbOut=0;


    //  Ready to decode
    while(nbIn)
    {
        if(nbIn<7)
        {
            if(nbIn)
                printf("[a52]: no data to decode avail %u\n",nbIn);
            break;
        }
        length = a52_syncinfo(ptr, &flags, &samprate, &bitrate);
        if(length==0)
        {
            printf("[a52] No startcode found\n");
            ADM_assert(0); 
        }
        if(length>nbIn)
        {
            // not enough data
            break;
        }
        // Decode
        if(flags & A52_STEREO)
            flags = A52_ADJUST_LEVEL+A52_STEREO;
        else
            flags = A52_ADJUST_LEVEL+A52_MONO;
        sample_t level = 16384, bias = 0;
       
        if (a52_frame(AC3_HANDLE, ptr, &flags, &level, bias))
        {
            printf("\n A52_frame failed!");
            ptr+=length;
            nbIn-=length;
            *nbOut += 256 * 2 * chan*6;
            break;
        };
        ptr+=length;
        nbIn-=length;
        *nbOut += 256 * 2 * chan*6;
        for (int i = 0; i < 6; i++)
        {
            if (a52_block(AC3_HANDLE))
            {
                printf("\n A52_block failed! on fblock :%lu", i);
                // in that case we silent out the chunk
                memset(ptrOut, 0, 256 * 2 * chan);
            }
            // convert the float in ac3_sample to
            // integer
            else
            {
                int16_t *cur;
                for(int k=0;k<chan;k++)
                {
                    sample_t *sample=(sample_t *)ac3_sample;
                    sample+=256*k;
                    cur=ptrOut+k;
                    for (int j = 0; j < 256; j++)
                    {
                        *cur = (int16_t) ceil(*sample++);
                        cur+=chan;
                    }
                }
                ptrOut+=chan*256;
            }
        } // 0.. 6
    }
    return 1; 

}

#endif
