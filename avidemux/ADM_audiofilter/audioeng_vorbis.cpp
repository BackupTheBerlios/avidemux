
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
    	free(_extraData);
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
uint8_t AVDMProcessAudio_Vorbis::init( uint32_t bitrate)
{


ogg_packet header1,header2,header3;

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
int16_t  *buf=(int16_t *)dropBuffer;
uint32_t nbSample=0;
uint32_t rdall=0,asked,rd;
int wr;
float s;
ogg_packet op ;
uint64_t total=0;
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

	asked=FRAME*2*_wavheader->channels;
	rd=_instream->read(asked,(uint8_t *)buf);
	nbSample=rd/(2*_wavheader->channels);
	
	if(!nbSample) return 0;
	total+=nbSample;
	//aprintf("Nb Sample:%lu\n",nbSample);
	// Now we got out samples, encode them
	float **float_samples;
	
	float_samples=vorbis_analysis_buffer(&VD, nbSample) ;
	
	// Put our samples in incoming buffer
	switch(_wavheader->channels)
	{
		case 1:
			{
			for(int i=0;i<nbSample;i++)
			{
				s=buf[i];
				s/=32768.;
			 	float_samples[0][i]=s;
			}
			}
			break;
		case 2:
			{
			for(int i=0;i<nbSample;i++)
			{
				s=buf[i*2];
				s/=32768.;
			 	float_samples[0][i]=s;
				s=buf[i*2+1];
				s/=32768.;
			 	float_samples[1][i]=s;

			}
			}
			break;
		default:
			ADM_assert(0);

	
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
