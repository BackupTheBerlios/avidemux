/***************************************************************************
                          audioenf_lame.cpp  -  description
                             -------------------
		Use lame encoder as an output plugin

    begin                : Sat Dec 15 2001
    copyright            : (C) 2001 by mean
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
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"

#ifdef HAVE_LIBMP3LAME
#include "lame/lame.h"

#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
//#include "../toolkit.hxx"

extern uint32_t audioMP3bitrate; // ugly, should be local
lame_global_flags *myflags = NULL;

// Ctor: Duplicate
//__________

AVDMProcessAudio_Lame::AVDMProcessAudio_Lame(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream
    (instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_MP3;


    strcpy(_name, "PROC:LAME");
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
  
    
};

AVDMProcessAudio_Lame::~AVDMProcessAudio_Lame()
{
    delete(_wavheader);
};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AVDMProcessAudio_Lame::initLame(uint32_t frequence,
					uint32_t mode, 
					uint32_t bitrate,
					ADM_LAME_PRESET preset)
{

    int ret, ratio;
    MPEG_mode_e mmode;
    double dratio;

    myflags = lame_init();
    if (myflags == NULL)
	return 0;

	// recompute output length
	// since we compress recompute the length in bytes
    double comp;
    
    comp=_instream->getLength();
    comp/=2; // 16 bits sample
    comp/=_wavheader->channels; // We got sample
    comp/=_wavheader->frequency; // and no we got seconds
    comp*=bitrate*1000;
    comp/=8;			// now we got bytes
    _length=(uint32_t) floor(comp+1);
    printf("Incoming : %lu bytes, fq : %lu, channel : %lu bitrate: %lu outgoing : %lu\n",
    			_instream->getLength(),_wavheader->frequency,_wavheader->channels,bitrate,_length);
    //initLame(frequence,mode,bitrate);	
	
    ret = lame_set_in_samplerate(myflags, _instream->getInfo()->frequency);
    ret = lame_set_num_channels(myflags, _instream->getInfo()->channels);

    if (frequence == 0)		// keep same as instream
	frequence = _instream->getInfo()->frequency;
    printf("\n output frequency : %lu\n", frequence);
    ret = lame_set_out_samplerate(myflags, frequence);

    ret = lame_set_quality(myflags, 2);	//max quality);
    if (_instream->getInfo()->channels == 2)
      {
	  switch (mode)
	    {
	    case ADM_STEREO:
		mmode = STEREO;
		break;
	    case ADM_JSTEREO:
		mmode = JOINT_STEREO;
		break;
	    default:
		printf("\n **** unknown mode ***\n");
		mmode = STEREO;
		break;

	    }
    } else
    {
		mmode = MONO;
     	printf("\n mono audio mp3");
  	}

   _mode=mmode;

   

    ret = lame_set_brate(myflags, bitrate);
    ret = lame_set_mode(myflags, mmode);	// 0 stereo 1 jstero
    ret = lame_init_params(myflags);
    if (ret == -1)
	return 0;
    // update bitrate in header
    _wavheader->byterate = (bitrate >> 3) * 1000;
   
    // configure CBR/ABR/...
    switch(preset)
    {
    	default:
    	case ADM_LAME_PRESET_CBR: break;
	case ADM_LAME_PRESET_ABR:
	  
	  lame_set_preset( myflags, bitrate);
	  _wavheader->blockalign=1152;
	 break;
	case ADM_LAME_PRESET_EXTREME: 
	  _wavheader->blockalign=1152;
	  lame_set_preset( myflags, EXTREME);	
	break;
    
    
    }
    
    lame_print_config(myflags);
    lame_print_internals(myflags);

    return 1;
}

//_____________________________________________
uint32_t AVDMProcessAudio_Lame::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in;
    int32_t nbout;
    uint32_t asked, done;

#define ONE_CHUNK (8192)
//      
// Read n samples
    // Go to the beginning of current block
    in = _bufferin;
    while (rdall < ONE_CHUNK)
      {
	  // don't ask too much front.
	  asked = ONE_CHUNK - rdall;
	  rd = _instream->read(asked, in + rdall);
	  rdall += rd;
	  if (rd == 0)
	      break;
      }
    // Block not filled
    if (rdall != ONE_CHUNK)
      {
	  printf("\n not enough...%lu\n", rdall);
	  if (rdall == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
	  memset(in + rdall, 0, ONE_CHUNK - rdall);
      }
    // input buffer is full , convert it
    if(_mode==MONO)
    {
           nbout = lame_encode_buffer(myflags,	/* global context handlei        */
					   (int16_t *) _bufferin,	/* PCM data for left and right
									   channel, interleaved          */
		             (int16_t *) _bufferin,
					   ONE_CHUNK >> 1,	/* number of samples per channel,
								   _not_ number of samples in
								   pcm[]                         */
					   obuffer,	/* pointer to encoded MP3 stream */
					   16 * 1024);	/* number of valid octets in this
							   stream */
      }
      else
    {
    nbout = lame_encode_buffer_interleaved(myflags,	/* global context handlei        */
					   (int16_t *) _bufferin,	/* PCM data for left and right
									   channel, interleaved          */
					   ONE_CHUNK >> 2,	/* number of samples per channel,
								   _not_ number of samples in
								   pcm[]                         */
					   obuffer,	/* pointer to encoded MP3 stream */
					   16 * 1024);	/* number of valid octets in this
							   stream */
          }
    if (nbout < 0)
      {
	  printf("n Error !!! : %ld\n", nbout);

	  ADM_assert(nbout > 0);
	  return MINUS_ONE;
      }
    done = (uint32_t) nbout;
    return done;
}



#endif
