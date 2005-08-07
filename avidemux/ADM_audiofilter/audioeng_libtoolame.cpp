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
#include "libtoolame/toolame.h"
}
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"



static toolame_options_struct *options=NULL;
// Ctor: Duplicate
//__________

AVDMProcessAudio_LibToolame::AVDMProcessAudio_LibToolame(AVDMGenericAudioStream * instream)
:AVDMBufferedAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
   
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
    _bfer=NULL;
    options=NULL;
    _wavheader->encoding=WAV_MP2;	
};


AVDMProcessAudio_LibToolame::~AVDMProcessAudio_LibToolame()
{
    if(_wavheader)
    	delete(_wavheader);

    if(options)
    	toolame_close( &options);

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

    int ret;//, ratio;
    double comp;
    MPEG_mode mmode;    	
    
	_wavheader->byterate=(bitrate*1000)>>3;         
      
 /*   if(_wavheader->frequency<32000)
    	{
		GUI_Error_HIG("Incompatible audio frequency", "Samplerate should be at least 32 kHz.");
		return 0;
	}*/
    comp=_instream->getLength();
    comp/=2; // 16 bits sample
    comp/=_wavheader->channels; // We got sample
    comp/=_wavheader->frequency; // and no we got seconds
    comp*=bitrate*1000;
    comp/=8;			// now we got bytes
    _length=(uint32_t) floor(comp+1);
    printf("Incoming : %lu bytes, fq : %lu, channel : %lu bitrate: %lu outgoing : %lu\n",
    	_instream->getLength(),_wavheader->frequency,_wavheader->channels,bitrate,_length);
		
	options=toolame_init();
	if(!options)
	{
		printf("Libtoolame init failed\n");
		return 0;
	}
	
  	toolame_set_out_samplerate (options, _wavheader->frequency);	
	if(_wavheader->channels==1) mmode=MONO;
	else
	switch (mode)
	{
	    case ADM_STEREO:
		mmode = STEREO;
		break;
	    case ADM_JSTEREO:
		mmode = JOINT_STEREO;
		break;
	    case ADM_MONO:
	    	mmode=MONO;
		break;
				
	   default:
		printf("\n **** unknown mode ***\n");
		mmode = STEREO;
		break;

	}
	toolame_set_mode(options,mmode);
   	toolame_set_error_protection(options,TRUE);	
    	//toolame_setPadding (options,TRUE);
	toolame_set_bitrate (options,bitrate);
	toolame_set_verbosity(options, 2);
	toolame_init_params(options);	
	

	printf("Libtoolame successfully initialized\n");
    return 1;       
}
#define ONE_CHUNK 8192
//_____________________________________________
uint32_t AVDMProcessAudio_LibToolame::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in;
    int nbout,nbframe;
    uint32_t asked, done;
    uint8_t *myBuffer;
    
    myBuffer=(uint8_t *)_buffer;


//      
// Read n samples
    // Go to the beginning of current block
    in = _bfer;
    while (rdall < (uint32_t)ONE_CHUNK)
      {
	  // don't ask too much front.
	  asked = ONE_CHUNK - rdall;
	  rd = _instream->read(asked, myBuffer + rdall);
	  rdall += rd;
	  if (rd == 0)
	      break;
      }
    // Block not filled
    if (rdall != (uint32_t)ONE_CHUNK)
      {
	  printf("\n not enough...%lu\n", rdall);
	  if (rdall == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
	  memset(_buffer + rdall, 0, ONE_CHUNK - rdall);
      }
    
  	// 
	if(_wavheader->channels==1)
	{
		nbout =toolame_encode_buffer(options, _buffer, _buffer,
			  rdall>>1, obuffer,8192  );
		aprintf("in:%d out;%d\n",rdall,nbout);	
	}		
	else
	{
		nbout= toolame_encode_buffer_interleaved(options, _buffer,
			  rdall>>2, obuffer, ONE_CHUNK*2);
		aprintf("2Lame: in:%d out:%d frame:%d\n",rdall,nbout,nbframe);	  
		
	}
    done = (uint32_t) nbout;
    return done;
}	
// EOF
