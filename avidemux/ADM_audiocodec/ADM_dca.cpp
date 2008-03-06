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
#include "ADM_assert.h"
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_audiofilter/audiofilter_channel_route.h"

#include "prefs.h"

#ifdef USE_LIBDCA
#include "ADM_audio/ADM_dcainfo.h"
#include "ADM_libraries/ADM_libwrapper/libwrapper_global.h"

#define DTS_HANDLE ((dts_state_t *)dts_handle)

ADM_AudiocodecDCA::ADM_AudiocodecDCA( uint32_t fourcc, WAVHeader *info) :   ADM_Audiocodec(fourcc)
{
    int flags=0;
    ADM_assert(fourcc==WAV_DTS);
    dts_handle=NULL;
    
#ifdef ADM_CPU_X86
#define CHK(x,y) if(CpuCaps::has##x()) flags|=MM_ACCEL_X86_##y;
    CHK(MMX,MMX);
    CHK(3DNOW,3DNOW);
    CHK(MMXEXT,MMXEXT);
#endif
    
    dts_handle=(void *)dca->dts_init_(flags);
    if(!dts_handle)
    {
        printf("Cannot init libdca\n");
        ADM_assert(0);   
    }
	_wavHeader = info;
}

ADM_AudiocodecDCA::~ADM_AudiocodecDCA( )
{
    if(dts_handle)
    {
        dca->dts_free_(DTS_HANDLE);
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

uint8_t ADM_AudiocodecDCA::run(uint8_t *inptr, uint32_t nbIn, float *outptr, uint32_t *nbOut)
{
    uint32_t avail;
    uint32_t length,syncoff;
    int flags = 0, samprate = 0, bitrate = 0, frame_length;
    uint8_t chan = _wavHeader->channels;
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
        length = dca->dts_syncinfo_ (DTS_HANDLE,  inptr, &flags, &samprate,&bitrate, &frame_length);

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

	if (ch_route.mode < 1) {
		CHANNEL_TYPE *p_ch_type = ch_route.input_type;
		switch (flags & DTS_CHANNEL_MASK) {
			case DTS_MONO:
				*(p_ch_type++) = CH_MONO;
			break;
			case DTS_STEREO:
			case DTS_DOLBY:
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
			break;
			case DTS_3F:
				*(p_ch_type++) = CH_FRONT_CENTER;
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
			break;
			case DTS_2F1R:
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
				*(p_ch_type++) = CH_REAR_CENTER;
			break;
			case DTS_3F1R:
				*(p_ch_type++) = CH_FRONT_CENTER;
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
				*(p_ch_type++) = CH_REAR_CENTER;
			break;
			case DTS_2F2R:
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
				*(p_ch_type++) = CH_REAR_LEFT;
				*(p_ch_type++) = CH_REAR_RIGHT;
			break;
			case DTS_3F2R:
				*(p_ch_type++) = CH_FRONT_CENTER;
				*(p_ch_type++) = CH_FRONT_LEFT;
				*(p_ch_type++) = CH_FRONT_RIGHT;
				*(p_ch_type++) = CH_REAR_LEFT;
				*(p_ch_type++) = CH_REAR_RIGHT;
			break;
			default:
				ADM_assert(0);
		}
		if (flags & DTS_LFE) {
			*(p_ch_type++) = CH_LFE;
		}
	}

        sample_t level = 1, bias = 0;
        flags &=DTS_CHANNEL_MASK;
        flags |= DTS_ADJUST_LEVEL;

        if (dca->dts_frame_(DTS_HANDLE, inptr, &flags, &level, bias))
        {
            printf("\n DTS_frame failed!");
            inptr+=length;
            nbIn-=length;
            outptr+=256*chan;
            *nbOut+=256*chan;
            break;
        };

        inptr+=length;
        nbIn-=length;
        // Each block is 256 samples
        *nbOut += 256 * chan * dca->dts_blocks_num_(DTS_HANDLE);
	float *cur;
	for (int i = 0; i < dca->dts_blocks_num_(DTS_HANDLE); i++) {
		if (dca->dts_block_ (DTS_HANDLE)) {
			printf("\n[DTS] dts_block failed on block %d/%d\n",i,dca->dts_blocks_num_ (DTS_HANDLE));
			// in that case we silent out the chunk
			memset(outptr, 0, 256 * chan * sizeof(float));
		} else {
			float *cur;
			for (int k = 0; k < chan; k++) {
				sample_t *sample=(sample_t *)dca->dts_samples_(DTS_HANDLE);
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
