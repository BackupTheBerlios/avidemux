
/***************************************************************************
                          audioeng_vorbis.cpp  -  description
                             -------------------
    begin                : Sun Sep 112004
    copyright            : (C) 2002-4 by mean
    email                : fixounet@free.fr
    
    Interface to FAAC
    
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
#ifdef USE_VORBIS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "ADM_lavcodec.h"

#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "vorbis/vorbisenc.h"
#include "ADM_audiofilter/audioeng_vorbis.h"

#define VD (((vorbisStruct *)_handle)->vd)
#define VI (((vorbisStruct *)_handle)->vi)
#define VB (((vorbisStruct *)_handle)->vb)
typedef struct vorbisStruct
{ 
	vorbis_info 	 vi ;
	vorbis_dsp_state vd ;
	vorbis_block     vb ;
}vorbisStruct;
//__________

AVDMProcessAudio_Vorbis::AVDMProcessAudio_Vorbis(AVDMGenericAudioStream * instream)
:AVDMBufferedAudioStream    (instream)
{

    _wavheader = new WAVHeader;    
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
   _wavheader->encoding=WAV_OGG;
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();
    _handle=(void *)new  vorbisStruct; 
    
};


AVDMProcessAudio_Vorbis::~AVDMProcessAudio_Vorbis()
{
    delete(_wavheader);
   

    if(_handle)
    {
	vorbis_block_clear(&VB);
	vorbis_dsp_clear(&VD);
	vorbis_info_clear(&VI);
	delete (vorbisStruct *)_handle;
    }    	
    _handle=NULL;
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
uint8_t AVDMProcessAudio_Vorbis::init( uint32_t bitrate)
{



 	vorbis_info_init(&VI) ;
 
	if(0>vorbis_encode_init(&VI,
			      _wavheader->channels,
			       _wavheader->frequency,
			      -1, // Max bitrate      
			      bitrate*1000, //long nominal_bitrate,
			      -1 //long min_bitrate))
			      ))
	{
		printf("vorbis init error\n");
		return 0;
	
	}
	vorbis_analysis_init(&VD, &VI) ;
	vorbis_block_init(&VD, &VB);
    
	printf("Vorbis encoder initialized\n");
	printf("Bitrate :%lu\n",bitrate);
	printf("Channels:%lu\n",_wavheader->channels);
	printf("Frequenc:%lu\n",_wavheader->frequency);
    return 1;
}

//_____________________________________________
uint32_t AVDMProcessAudio_Vorbis::grab(uint8_t * obuffer)
{
uint32_t len,sam;
	printf("Vorbis: Read\n");
	if(getPacket(obuffer,&len,&sam))
		return len;
	return MINUS_ONE;
}
//______________________________________________
uint8_t	AVDMProcessAudio_Vorbis::getPacket(uint8_t *dest, uint32_t *len, 
					uint32_t *samples)
{
	return 0;
}

#endif		
// EOF
