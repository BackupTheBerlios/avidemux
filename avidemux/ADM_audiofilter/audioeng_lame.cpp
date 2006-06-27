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

#include "default.h"
#include "audioprocess.hxx"

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
	int ret;
	MPEG_mode_e mmode;

    myflags = lame_init();
    if (myflags == NULL)
	return 0;

	// recompute output length
	// since we compress recompute the length in bytes
    double comp;

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

    ret = lame_set_brate(myflags, bitrate);
    ret = lame_set_mode(myflags, mmode);	// 0 stereo 1 jstero
    ret = lame_init_params(myflags);
    if (ret == -1)
	return 0;
    // update bitrate in header
    _wavheader->byterate = (bitrate >> 3) * 1000;
   
    // configure CBR/ABR/...
    _preset=preset;
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

	_in = new float [_chunk];
	ADM_assert(_in);

    lame_print_config(myflags);
    lame_print_internals(myflags);

    return 1;
}
uint8_t	AVDMProcessAudio_Lame::isVBR(void )
{
	if(_preset==ADM_LAME_PRESET_CBR) return 0;
	return 1;

}
//_____________________________________________
uint32_t AVDMProcessAudio_Lame::grab(uint8_t * obuffer)
{
	int32_t nbout;

	if (readChunk() == 0)
		return MINUS_ONE;	// End of stream

	dither16bit();

	if (_wavheader->channels == 1)
		nbout = lame_encode_buffer(myflags, (int16_t *) _in, (int16_t *)_in, _chunk, obuffer, 16 * 1024);
	else
		nbout = lame_encode_buffer_interleaved(myflags, (int16_t *)_in, _chunk/2, obuffer, 16 * 1024);

	if (nbout < 0) {
		printf("\n Error !!! : %ld\n", nbout);
		return MINUS_ONE;
	}

	return (uint32_t) nbout;
}



#endif
