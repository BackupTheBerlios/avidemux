/***************************************************************************
                          audioeng_ffmp2.cpp  -  description
                             -------------------
    begin                : Sun Dec 1 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Interface to FFmpeg mpeg1/2 audio encoder
    
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
#include "ADM_lavcodec.h"

#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiofilter/audioeng_ffmp2.h"

#define CONTEXT ((AVCodecContext  	*)_context)
#define ONE_CHUNK (CONTEXT->frame_size*2*_wavheader->channels)

// Ctor: Duplicate
//__________

AVDMProcessAudio_FFmpeg::AVDMProcessAudio_FFmpeg(int codecid,AVDMGenericAudioStream * instream)
:AVDMBufferedAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    _codec=codecid;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
   
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
    _bfer=NULL;
};


AVDMProcessAudio_FFmpeg::~AVDMProcessAudio_FFmpeg()
{
    delete(_wavheader);
    avcodec_close(CONTEXT);
    free(_context);
    if(_bfer)
    	{
			 	delete [] _bfer;
			  _bfer=NULL;
			}
};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AVDMProcessAudio_FFmpeg::init( uint32_t bitrate)
{

    int ret;//, ratio;
    double comp;
        	
    _context=( void *)avcodec_alloc_context();
         
    _wavheader->byterate=(bitrate*1000)>>3;  
    
    comp=_instream->getLength();
    comp/=2; // 16 bits sample
    comp/=_wavheader->channels; // We got sample
    comp/=_wavheader->frequency; // and no we got seconds
    comp*=bitrate*1000;
    comp/=8;			// now we got bytes
    _length=(uint32_t) floor(comp+1);
    printf("Incoming : %lu bytes, fq : %lu, channel : %lu bitrate: %lu outgoing : %lu\n",
    			_instream->getLength(),_wavheader->frequency,_wavheader->channels,bitrate,_length);
			
    CONTEXT->channels 			=  _wavheader->channels;
    CONTEXT->sample_rate 		=  _wavheader->frequency;
    CONTEXT->bit_rate 			= (bitrate*1000); // bits -> kbits

    printf("\n Conf : Channel : %d, samplerate : %ld, bit rate  : %d\n",
               _wavheader->channels,
               _wavheader->frequency,CONTEXT->bit_rate);
    
    AVCodec *codec;
   
    codec = avcodec_find_encoder((CodecID)_codec);
    if (!codec) {
        fprintf(stderr, "codec MP2 not found\n");
        return 0;
    }
    		 ret=avcodec_open(CONTEXT,codec); 
         if(0> ret )
         {
          	printf("\n FF MP2 init failed err : %d!\n",ret);
         		return 0;
         }
		_bfer=new uint8_t [ CONTEXT->frame_size*2*_wavheader->channels];
		ADM_assert(_bfer);         
    return 1;       
}

//_____________________________________________
uint32_t AVDMProcessAudio_FFmpeg::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in;
    int32_t nbout;
    uint32_t asked, done;


//      
// Read n samples
    // Go to the beginning of current block
    in = _bfer;
    while (rdall < (uint32_t)ONE_CHUNK)
      {
	  // don't ask too much front.
	  asked = ONE_CHUNK - rdall;
	  rd = _instream->read(asked, _bfer + rdall);
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
	  memset(_bfer + rdall, 0, ONE_CHUNK - rdall);
      }
    
  
		nbout= avcodec_encode_audio(CONTEXT, 
																	obuffer, 5000, 
								         							(short *) _bfer);          
          
	    if (nbout < 0)
    		  {
			  printf("n Error !!! : %ld\n", nbout);
			  return MINUS_ONE;
		      }
    done = (uint32_t) nbout;
    return done;
}
/*__________________________________________*/

	AVDMProcessAudio_FFmp2::AVDMProcessAudio_FFmp2(AVDMGenericAudioStream *instream	) :
				AVDMProcessAudio_FFmpeg(CODEC_ID_MP2,instream)
{
		_wavheader->encoding=WAV_MP2;			
					
};		
	AVDMProcessAudio_FFmp2::~AVDMProcessAudio_FFmp2()
	{
		
		};		
/*_____________________________________________*/

AVDMProcessAudio_FFAC3::AVDMProcessAudio_FFAC3(AVDMGenericAudioStream *instream	) :
				AVDMProcessAudio_FFmpeg(CODEC_ID_AC3,instream)
{
		_wavheader->encoding=WAV_AC3;			
					
};		
	AVDMProcessAudio_FFAC3::~AVDMProcessAudio_FFAC3()
	{
		
		};		
// EOF
