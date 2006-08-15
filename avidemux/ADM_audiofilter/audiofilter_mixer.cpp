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
#include "ADM_library/default.h"


#include "audioeng_process.h"
#include "audiofilter_mixer.h"
#include "audiofilter_channel_route.h"
#include "audiofilter_dolby.h"


AUDMAudioFilterMixer::AUDMAudioFilterMixer(AUDMAudioFilter *instream,CHANNEL_CONF out):AUDMAudioFilter (instream)
{
    _output=out;
    _previous->rewind();     // rewind
    ADM_assert(_output<CHANNEL_LAST);
    
    
    double d;               // Update duration
    d=_wavHeader.byterate;
    d/=_wavHeader.channels;

	switch (_output) {
		case CHANNEL_MONO:
			_wavHeader.channels = 1;
			ch_route.output_type[0] = CH_MONO;
		break;
		case CHANNEL_STEREO:
			_wavHeader.channels = 2;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
		break;
		case CHANNEL_2F_1R:
			_wavHeader.channels = 3;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_REAR_CENTER;
		break;
		case CHANNEL_3F:
			_wavHeader.channels = 3;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_FRONT_CENTER;
		break;
		case CHANNEL_3F_1R:
			_wavHeader.channels = 4;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_REAR_CENTER;
			ch_route.output_type[3] = CH_FRONT_CENTER;
		break;
		case CHANNEL_2F_2R:
			_wavHeader.channels = 4;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_REAR_LEFT;
			ch_route.output_type[3] = CH_REAR_RIGHT;
		break;
		case CHANNEL_3F_2R:
			_wavHeader.channels = 5;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_REAR_LEFT;
			ch_route.output_type[3] = CH_REAR_RIGHT;
			ch_route.output_type[4] = CH_FRONT_CENTER;
		break;
		case CHANNEL_3F_2R_LFE:
			_wavHeader.channels = 6;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			ch_route.output_type[2] = CH_REAR_LEFT;
			ch_route.output_type[3] = CH_REAR_RIGHT;
			ch_route.output_type[4] = CH_FRONT_CENTER;
			ch_route.output_type[5] = CH_LFE;
		break;
		case CHANNEL_DOLBY_PROLOGIC:
		case CHANNEL_DOLBY_PROLOGIC2:
			_wavHeader.channels = 2;
			ch_route.output_type[0] = CH_FRONT_LEFT;
			ch_route.output_type[1] = CH_FRONT_RIGHT;
			DolbyInit();
		break;
	}

    d*=_wavHeader.channels;
    _wavHeader.byterate = (uint32_t)ceil(d);


//    printf("[mixer]Input channels : %u : %u \n",_previous->getInfo()->channels,input_channels);
//    printf("[mixer]Out   channels : %u : %u \n",_wavHeader.channels,ADM_channel_mixer[_output]);

};

AUDMAudioFilterMixer::~AUDMAudioFilterMixer()
{
};

static int MCOPY(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
    memcpy(out,in,nbSample*chan*sizeof(float));
    return nbSample*chan;
    
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

static int MStereo(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 2);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
				case CH_REAR_CENTER:
				case CH_LFE:
					out[0]  += *in * 0.5;
					out[1]  += *in * 0.5;
				break;
				case CH_FRONT_LEFT:
				case CH_REAR_LEFT:
				case CH_SIDE_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
				case CH_REAR_RIGHT:
				case CH_SIDE_RIGHT:
					out[1]  += *in;
				break;
			}
			in++;
		}
		out += 2;
	}

	return nbSample*2;
}

static int M2F1R(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 3);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
					out[0]  += *in * 0.5;
					out[1]  += *in * 0.5;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_LEFT:
				case CH_REAR_RIGHT:
				case CH_REAR_CENTER:
					out[2]  += *in;
				break;
				case CH_LFE:
					out[0]  += *in * 0.33;
					out[1]  += *in * 0.33;
					out[2]  += *in * 0.33;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
			}
			in++;
		}
		out += 3;
	}

	return nbSample * 3;
}

static int M3F(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 3);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
				case CH_REAR_CENTER:
					out[2]  += *in;
				break;
				case CH_FRONT_LEFT:
				case CH_REAR_LEFT:
				case CH_SIDE_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
				case CH_REAR_RIGHT:
				case CH_SIDE_RIGHT:
					out[1]  += *in;
				break;
				case CH_LFE:
					out[0]  += *in * 0.33;
					out[1]  += *in * 0.33;
					out[2]  += *in * 0.33;
				break;
			}
			in++;
		}
		out += 3;
	}

	return nbSample * 3;
}

static int M3F1R(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 4);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
					out[3]  += *in;
				break;
				case CH_REAR_CENTER:
				case CH_REAR_LEFT:
				case CH_REAR_RIGHT:
					out[2]  += *in;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_LFE:
					out[0]  += *in * 0.25;
					out[1]  += *in * 0.25;
					out[2]  += *in * 0.25;
					out[3]  += *in * 0.25;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
			}
			in++;
		}
		out += 4;
	}

	return nbSample * 4;
}

static int M2F2R(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 4);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
					out[0]  += *in * 0.5;
					out[1]  += *in * 0.5;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_LEFT:
					out[2]  += *in;
				break;
				case CH_REAR_RIGHT:
					out[3]  += *in;
				break;
				case CH_REAR_CENTER:
					out[2]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
				case CH_LFE:
					out[0]  += *in * 0.25;
					out[1]  += *in * 0.25;
					out[2]  += *in * 0.25;
					out[3]  += *in * 0.25;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
			}
			in++;
		}
		out += 4;
	}

	return nbSample * 4;
}

static int M3F2R(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 5);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
					out[4]  += *in;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_LEFT:
					out[2]  += *in;
				break;
				case CH_REAR_RIGHT:
					out[3]  += *in;
				break;
				case CH_REAR_CENTER:
					out[2]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
				case CH_LFE:
					out[0]  += *in * 0.2;
					out[1]  += *in * 0.2;
					out[2]  += *in * 0.2;
					out[3]  += *in * 0.2;
					out[4]  += *in * 0.2;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
			}
			in++;
		}
		out += 5;
	}

	return nbSample * 5;
}

static int M3F2RLFE(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 6);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
					out[4]  += *in;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_LEFT:
					out[2]  += *in;
				break;
				case CH_REAR_RIGHT:
					out[3]  += *in;
				break;
				case CH_REAR_CENTER:
					out[2]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
				case CH_LFE:
					out[5]  += *in;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[2]  += *in * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[3]  += *in * 0.5;
				break;
			}
			in++;
		}
		out += 6;
	}

	return nbSample * 6;
}

static int MDolbyProLogic(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 2);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
				case CH_LFE:
					out[0]  += *in * 0.5;
					out[1]  += *in * 0.5;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_CENTER:
				case CH_REAR_LEFT:
				case CH_REAR_RIGHT:
					out[0]  += DolbyShiftLeft(*in) * 0.707;
					out[1]  += DolbyShiftRight(*in) * 0.707;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[0]  += DolbyShiftLeft(*in) * 0.707 * 0.5;
					out[1]  += DolbyShiftRight(*in) * 0.707 * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[0]  += DolbyShiftLeft(*in) * 0.707 * 0.5;
					out[1]  += DolbyShiftRight(*in) * 0.707 * 0.5;
				break;
			}
			in++;
		}
		out += 2;
	}

	return nbSample*2;
}

static int MDolbyProLogic2(float *in,float *out,uint32_t nbSample,uint32_t chan)
{
	memset(out, 0, sizeof(float) * nbSample * 2);

	for (int i = 0; i < nbSample; i++) {
		for (int c = 0; c < chan; c++) {
			switch (ch_route.input_type[c]) {
				case CH_MONO:
				case CH_FRONT_CENTER:
				case CH_LFE:
					out[0]  += *in * 0.5;
					out[1]  += *in * 0.5;
				break;
				case CH_FRONT_LEFT:
					out[0]  += *in;
				break;
				case CH_FRONT_RIGHT:
					out[1]  += *in;
				break;
				case CH_REAR_CENTER:
					out[0]  += DolbyShiftLeft(*in) * 0.707;
					out[1]  += DolbyShiftRight(*in) * 0.707;
				break;
				case CH_REAR_LEFT:
					out[0]  += DolbyShiftLeft(*in) * 0.8165;
					out[1]  += DolbyShiftRight(*in) * 0.5774;
				break;
				case CH_REAR_RIGHT:
					out[0]  += DolbyShiftLeft(*in) * 0.5774;
					out[1]  += DolbyShiftRight(*in) * 0.8165;
				break;
				case CH_SIDE_LEFT:
					out[0]  += *in * 0.5;
					out[0]  += DolbyShiftLeft(*in) * 0.8165 * 0.5;
					out[1]  += DolbyShiftRight(*in) * 0.5774 * 0.5;
				break;
				case CH_SIDE_RIGHT:
					out[1]  += *in * 0.5;
					out[0]  += DolbyShiftLeft(*in) * 0.5774 * 0.5;
					out[1]  += DolbyShiftRight(*in) * 0.8165 * 0.5;
				break;
			}
			in++;
		}
		out += 2;
	}

	return nbSample*2;
}


typedef int MIXER(float *in,float *out,uint32_t nbSample,uint32_t chan)  ;

static MIXER *matrixCall[CHANNEL_LAST] = {
NULL, MNto1, MStereo, M2F1R, M3F, M3F1R, M2F2R, M3F2R, M3F2RLFE, MDolbyProLogic, MDolbyProLogic2
};
//_____________________________________________
uint32_t AUDMAudioFilterMixer::fill(uint32_t max,float *output,AUD_Status *status)
{

    uint32_t rd = 0;
    uint8_t *in,*out;
    uint32_t window;
    int nbSampleMax=max/_wavHeader.channels;
    uint8_t input_channels = _previous->getInfo()->channels;

// Fill incoming buffer
    shrink();
    fillIncomingBuffer(status);
    // Block not filled ?
    if((_tail-_head)<input_channels)
    {
      if(*status==AUD_END_OF_STREAM && _head)
      {
        memset(&_incomingBuffer[_head],0,sizeof(float) * input_channels);
        _tail=_head+input_channels;
        printf("[Mixer] Warning asked %u symbols, a window is %u symbols\n",max,window);
      }
      else
      {
        return 0;
      }
    }
    // How many ?

    // Let's go
    int processed=0;
    int available=0;
    if(!nbSampleMax)
    {
      printf("[Mixer] Warning max %u, channels %u\n",max,input_channels);
    }
    available=(_tail-_head)/input_channels; // nb Sample
    ADM_assert(available);
    if(available > nbSampleMax) available=nbSampleMax;
    
    ADM_assert(available);
    

    // Now do the downsampling
	if (_output == CHANNEL_INVALID || ch_route.compareChType(&_wavHeader, _previous->getInfo())) {
		ch_route.copy = 1;
		rd= (uint32_t)MCOPY(&_incomingBuffer[_head],output,available,input_channels);
	} else {
		ch_route.copy = 0;
		MIXER *call=matrixCall[_output];
		rd= (uint32_t)call(&_incomingBuffer[_head],output,available,input_channels);
	}

    _head+=available*input_channels;
    return rd;
    
}

