
/***************************************************************************
                          audioeng_vorbis.cpp  -  description
                             -------------------
    begin                : Sun Sep 112004
    copyright            : (C) 2002-4 by mean
    email                : fixounet@free.fr
    
    Interface to Vorbis lib
    Strongly derived from ffmpeg code
    
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

#include "audioprocess.hxx"
#include "vorbis/vorbisenc.h"
#include "ADM_audiofilter/audioeng_vorbis.h"

#define VD (((vorbisStruct *)_handle)->vd)
#define VI (((vorbisStruct *)_handle)->vi)
#define VB (((vorbisStruct *)_handle)->vb)
#define VC (((vorbisStruct *)_handle)->vc)
typedef struct vorbisStruct
{ 
	vorbis_info 	 vi ;
	vorbis_dsp_state vd ;
	vorbis_block     vb ;
	vorbis_comment   vc ;
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
    _oldpos=0;
    _extraData=NULL;
    _extraLen=0;
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
    if(_extraData)
    {
    	ADM_dealloc(_extraData);
    }

};


//________________________________________________
//   Init Vorbis encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode			: VBR CBR
// bitrate for CBR	: bitrate in kbps (96,192...)
//		  for VBR	: quality (0, 1, 2, ... 11)
//				
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AVDMProcessAudio_Vorbis::init( uint32_t bitrate, uint8_t mode)
{
	ogg_packet header1,header2,header3;
	int err;

 	vorbis_info_init(&VI) ;
 
	if (mode==0) {  //VBR
		err=vorbis_encode_init_vbr(&VI,
			      _wavheader->channels,
			       _wavheader->frequency,
			      ((float)bitrate-1)/10
			      );
	} else {  //CBR
		err=vorbis_encode_init(&VI,
			      _wavheader->channels,
			       _wavheader->frequency,
			      -1, // Max bitrate      
			      bitrate*1000, //long nominal_bitrate,
			      -1 //long min_bitrate))
			      );
	}
	if (err!=0) {
		printf("vorbis init error\n");
		return 0;
	}
	vorbis_analysis_init(&VD, &VI) ;
	vorbis_block_init(&VD, &VB);
      	vorbis_comment_init(&VC);
	vorbis_comment_add_tag(&VC, "encoder", "AVIDEMUX2") ;
	
	vorbis_analysis_headerout(&VD, &VC, &header1,
                                &header2, &header3);

				
	// Store all headers as extra data
	// see ogg vorbis decode for details
	// we need 3 packets
	
	_extraLen=header1.bytes+header2.bytes+header3.bytes+3*sizeof(uint32_t);
	_extraData=new uint8_t[_extraLen];
	
	uint32_t *ex=(uint32_t *)_extraData;
	uint8_t *d;
	d=_extraData+sizeof(uint32_t)*3;
	ex[0]=header1.bytes;
	ex[1]=header2.bytes;
	ex[2]=header3.bytes;
	memcpy(d,header1.packet,ex[0]);
	d+=ex[0];
	memcpy(d,header2.packet,ex[1]);
	d+=ex[1];
	memcpy(d,header3.packet,ex[2]);
	vorbis_comment_clear(&VC);
			
	printf("\nVorbis encoder initialized\n");
	if (mode==0)
		printf("VBR Quality:%i\n",bitrate-1);
	else
		printf("CBR Bitrate:%lu\n",bitrate);
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
uint32_t nbSample=0;
uint32_t asked,rd;
float **float_samples;
ogg_packet op ;
//uint64_t total=0;
#define FRAME 1024
#define ROUNDMAX 3000

int count=ROUNDMAX;
// Check that we have packet from previous pass
while(count--)
{
	//printf("Round %d\n",ROUNDMAX-count);
  	if(vorbis_analysis_blockout(&VD, &VB) == 1) 
	{
	vorbis_analysis(&VB, NULL);
	vorbis_bitrate_addblock(&VB) ;
	//printf("Blockout\n");
	
		if(vorbis_bitrate_flushpacket(&VD, &op)) 
		{
			if(op.bytes<2) continue; // avoid the 1byte sized packet

			memcpy(dest, op.packet,op.bytes);
			*len=op.bytes;
			*samples=op.granulepos-_oldpos;
			_oldpos=op.granulepos;
			//  aprintf("1st packet :sampl:%lu len :%lu sample:%lu abs:%llu\n",*samples,op.bytes,total,op.granulepos);
			return 1;
		}
        }

	asked=FRAME*_wavheader->channels;
	rd=_instream->read(asked,(float *)dropBuffer);

	if(!rd)
	{
		rd=asked;
		memset(dropBuffer,0,rd);
		printf("Vorbis : No audio : padding\n");
	}
	nbSample=rd/_wavheader->channels;
	
	//total+=nbSample;
	//aprintf("Nb Sample:%lu\n",nbSample);
	// Now we got out samples, encode them
	float_samples=vorbis_analysis_buffer(&VD, nbSample) ;
	
	// Put our samples in incoming buffer
	for (int i = 0; i < nbSample; i++)
		for (int j = 0; j < _wavheader->channels; j++) {
			float_samples[j][i] = dropBuffer[i * _wavheader->channels + j];
			if (float_samples[j][i] > 1) float_samples[j][i] = 1;
			if (float_samples[j][i] < -1) float_samples[j][i] = -1;
	}
	// Buffer full, go go go
	 vorbis_analysis_wrote(&VD, nbSample) ; 
	 
	}
       
	return 0;
	
}
uint8_t		AVDMProcessAudio_Vorbis::extraData(uint32_t *l,uint8_t **d)
{
			*l=_extraLen;
			*d=_extraData;
		return 1;
}
#endif		
// EOF
