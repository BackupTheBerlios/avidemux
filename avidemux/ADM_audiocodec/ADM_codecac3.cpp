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
#include "ADM_audiofilter/audiofilter_channel_route.h"

#include "ADM_toolkit/ADM_cpuCap.h"
#include "prefs.h"

#ifdef USE_AC3
extern "C" {
#include "ADM_liba52/a52.h"
#include "ADM_liba52/mm_accel.h"
};

#define AC3_HANDLE ((a52_state_t *)ac3_handle)

ADM_AudiocodecAC3::ADM_AudiocodecAC3( uint32_t fourcc, WAVHeader *info) :   ADM_Audiocodec(fourcc)
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
        _downmix=0;
      _wavHeader = info;
      ADM_assert(_wavHeader);
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

uint8_t ADM_AudiocodecAC3::run(uint8_t *inptr, uint32_t nbIn, float *outptr,   uint32_t *nbOut)
{
    uint32_t avail;
    uint32_t length;
    int flags = 0, samprate = 0, bitrate = 0;
    uint8_t chan = _wavHeader->channels;
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
        length = a52_syncinfo(inptr, &flags, &samprate, &bitrate);
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

	if (ch_route.mode < 1) {
		CHANNEL_TYPE *p_ch_type = ch_route.input_type;
		if (flags & A52_LFE) {
			*(p_ch_type++) = CH_LFE;
		}
		switch (flags & A52_CHANNEL_MASK) {
			case A52_MONO:
				*(p_ch_type++) = CH_MONO;
			break;
			case A52_STEREO:
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
			break;
			case A52_3F2R:
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_CENTER;
				*(p_ch_type++) = CH_FRONT_RIGHT;
				*(p_ch_type++) = CH_REAR_LEFT;
				*(p_ch_type++) = CH_REAR_RIGHT;
			break;
			default:
				ADM_assert(0);
		}
	}

        sample_t level = 1, bias = 0;

        if (a52_frame(AC3_HANDLE, inptr, &flags, &level, bias))
        {
            printf("\n A52_frame failed!");
            inptr+=length;
            nbIn-=length;
            *nbOut += 256 * chan * 6;
            break;
        };
        inptr+=length;
        nbIn-=length;
        *nbOut += 256 * chan * 6;

        float *cur;
        for (int i = 0; i < 6; i++) {
                if (a52_block(AC3_HANDLE)) {
                        printf("\n A52_block failed! on fblock :%lu", i);
                        // in that case we silent out the chunk
                        memset(outptr, 0, 256 * chan * sizeof(float));
                } else {
                        for (int k = 0; k < chan; k++) {
                                sample_t *sample=(sample_t *)ac3_sample;
                                sample += 256 * k;
                                cur = outptr + k;
                                for (int j = 0; j < 256; j++) {
                                        *cur = *sample++;
                                        cur+=chan;
                                }
                        }
                }
                outptr += chan * 256;
        }
    }
    return 1; 

}

#endif
