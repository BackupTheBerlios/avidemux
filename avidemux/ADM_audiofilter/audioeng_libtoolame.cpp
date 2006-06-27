//
// C++ Implementation: audioeng_libtoolame
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//


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
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiofilter/audioeng_libtoolame.h"

extern "C"
{
#include "libtoolame/twolame.h"
}
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"



static twolame_options_struct *options=NULL;
// Ctor: Duplicate
//__________

AVDMProcessAudio_LibToolame::AVDMProcessAudio_LibToolame(AVDMGenericAudioStream * instream)
:AVDMBufferedAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
   
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
    options=NULL;
    _wavheader->encoding=WAV_MP2;	
};


AVDMProcessAudio_LibToolame::~AVDMProcessAudio_LibToolame()
{
    if(_wavheader)
    	delete(_wavheader);

    if(options)
    	twolame_close( &options);

	options=NULL;
	_wavheader=NULL;
};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AVDMProcessAudio_LibToolame::init( uint32_t mode, uint32_t bitrate)
{

    int ret;
    double comp;
    TWOLAME_MPEG_mode mmode;    	
    
	_wavheader->byterate=(bitrate*1000)>>3;         
      
 /*   if(_wavheader->frequency<32000)
    	{
		GUI_Error_HIG("Incompatible audio frequency", "Samplerate should be at least 32 kHz.");
		return 0;
	}*/
	_chunk = 4096;

    comp=_instream->getLength();
    comp/=2; // 16 bits sample
    comp/=_wavheader->channels; // We got sample
    comp/=_wavheader->frequency; // and no we got seconds
    comp*=bitrate*1000;
    comp/=8;			// now we got bytes
    _length=(uint32_t) floor(comp+1);
    printf("Incoming : %lu bytes, fq : %lu, channel : %lu bitrate: %lu outgoing : %lu\n",
    	_instream->getLength(),_wavheader->frequency,_wavheader->channels,bitrate,_length);
		
	options=twolame_init();
	if(!options)
	{
		printf("Libtoolame init failed\n");
		return 0;
	}
        twolame_set_in_samplerate(options, _wavheader->frequency);
  	twolame_set_out_samplerate (options, _wavheader->frequency);
        twolame_set_num_channels(options, _wavheader->channels);
	if(_wavheader->channels==1) mmode=TWOLAME_MONO;
	else
	switch (mode)
	{
	    case ADM_STEREO:
		mmode = TWOLAME_STEREO;
		break;
	    case ADM_JSTEREO:
		mmode = TWOLAME_JOINT_STEREO;
		break;
	    case ADM_MONO:
	    	mmode=TWOLAME_MONO;
		break;
				
	   default:
		printf("\n **** unknown mode, going stereo ***\n");
		mmode = TWOLAME_STEREO;
		break;

	}
	twolame_set_mode(options,mmode);
   	twolame_set_error_protection(options,TRUE);	
    	//toolame_setPadding (options,TRUE);
	twolame_set_bitrate (options,bitrate);
	twolame_set_verbosity(options, 2);
        if(twolame_init_params(options))
        {
            printf("Twolame init failed\n");
            return 0;
        }
	
	_in = new float [_chunk];
	ADM_assert(_in);

	printf("Libtoolame successfully initialized\n");
    return 1;       
}

uint32_t AVDMProcessAudio_LibToolame::grab(uint8_t * obuffer)
{
	int32_t nbout;

	if (readChunk() == 0)
		return MINUS_ONE;	// End of stream

	dither16bit();

	if (_wavheader->channels == 1)
		nbout =twolame_encode_buffer(options, (int16_t *) _in, (int16_t *) _in, _chunk, obuffer, _chunk);
	else
		nbout= twolame_encode_buffer_interleaved(options, (int16_t *) _in, _chunk/2, obuffer, _chunk*2);

	if (nbout < 0) {
		printf("\n Error !!! : %ld\n", nbout);
		return MINUS_ONE;
	}

	return (uint32_t) nbout;
}	
// EOF
