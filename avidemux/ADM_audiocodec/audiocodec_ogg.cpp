/***************************************************************************
                          audiocodec_ogg.cpp  -  description
                             -------------------
    begin                : Thu May 23 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr

	Strongly inspired by mplayer ogg vorbis decoder
	(ripp off should be more appropriate)
	
	We expect the first packet as extraData. It contains needed info such
	as frequency/channels etc...
	
	How to deal with comment and codebook ?
	Mmmmm
    
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
#include <assert.h>
#include <math.h>



#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"

#ifdef USE_VORBIS

#include <vorbis/codec.h>
#include "ADM_audiocodec/ADM_vorbis.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_OGM_AUDIO
#include "ADM_toolkit/ADM_debug.h"


 #define STRUCT ((oggVorbis *)_contextVoid)
 
 ADM_vorbis::~ADM_vorbis()
 {
 	oggVorbis *o;
	o=(oggVorbis *)_contextVoid;
	if(o) 
	{
		vorbis_block_clear(&o->vblock);
		vorbis_info_clear(&o->vinfo);
		delete o;
	}
	_contextVoid=NULL;
 
 }
 
 ADM_vorbis::ADM_vorbis(uint32_t fcc, WAVHeader *info, uint32_t extra, uint8_t *extraData)
 	: ADM_Audiocodec(fcc)
 {
 ogg_packet packet;
 vorbis_comment comment;
 oggVorbis *vrbis;
 uint8_t *hdr,*cmt,*code;
 uint32_t size_hdr,size_cmt, size_code;
 uint32_t *ptr;
 
 	_init=0;
 	printf("Trying to initialize vorbis codec with %lu bytes of header data\n",extra);
	
	vrbis=new oggVorbis;
 	_contextVoid=(void *)vrbis;
	
	memset(vrbis,0,sizeof(oggVorbis));
	
	// init everything
	vorbis_info_init(&STRUCT->vinfo);
	vorbis_comment_init(&STRUCT->vcomment);
	
	// split extradata as header/comment/code
	ptr=(uint32_t *)extraData;
	size_hdr=*ptr++;
	size_cmt=*ptr++;
	size_code=*ptr++;
	
	hdr=extraData+3*sizeof(uint32_t);
	cmt=hdr+size_hdr;
	code=cmt+size_cmt;
	
 	// Feed header passed as extraData
	packet.bytes=size_hdr;
	packet.packet=hdr;
	packet.b_o_s=1; // yes, it is a new stream	
	if(0>vorbis_synthesis_headerin(&STRUCT->vinfo,&comment,&packet))
	{
		printf("Mmm something bad happened , cannot init 1st packet\n");
		return;
	}
	// update some info in header this is the only place to get them
	// especially frequency.
	info->channels=STRUCT->vinfo.channels;
	info->frequency=STRUCT->vinfo.rate;
	info->byterate=STRUCT->vinfo.bitrate_nominal>>3;
	
	if(!info->byterate) 
	{
		printf("Mmm, no nominal bitrate...\n");
		info->byterate=16000;
	}
	// now unpack comment
	packet.bytes=size_cmt;
	packet.packet=cmt;
	packet.b_o_s=0; // Not new
	if(0>vorbis_synthesis_headerin(&STRUCT->vinfo,&comment,&packet))
	{
		printf("Mmm something bad happened , cannot init 2st packet\n");
		return;
	}
	// and codebook
	packet.bytes=size_code;
	packet.packet=code;
	packet.b_o_s=0; // Not new
	if(0>vorbis_synthesis_headerin(&STRUCT->vinfo,&comment,&packet))
	{
		printf("Mmm something bad happened , cannot init 3st packet\n");
		return;
	}
	vorbis_comment_clear(&comment);
	vorbis_synthesis_init(&STRUCT->vdsp,&STRUCT->vinfo);
	vorbis_block_init(&STRUCT->vdsp,&STRUCT->vblock);
	printf("Vorbis init successfull\n");
	STRUCT->ampscale=1;
	_init=1;

 
 }
 // This codec expects more or less one packet at a time !
 
 uint8_t ADM_vorbis::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
ogg_packet packet;
float **sample_pcm;
int	nb_synth;

	*nbOut=0;
	if(!_init) return 0;
	packet.b_o_s=0;
	packet.e_o_s=0;
	packet.bytes=nbIn;
	packet.packet=ptr;
	if(!vorbis_synthesis(&STRUCT->vblock,&packet))
	{
	      vorbis_synthesis_blockin(&STRUCT->vdsp,&STRUCT->vblock);
	 }
	 nb_synth=vorbis_synthesis_pcmout(&STRUCT->vdsp,&sample_pcm);
	 if(nb_synth<0)
	 {
	 	printf("error decoding vorbis %d\n",nb_synth);
		return 0;
	 }
	 
	 // Now convert the float / per channel samples to interleaved 16 bits pcm audio
 
 	 float scale =   32767.f * STRUCT->ampscale;
	 int16_t *out;
	 int channels,val;	 

	 
	 channels=STRUCT->vinfo.channels;
	 out=(int16_t *)outptr;
	 *nbOut=channels*2*nb_synth;
	 
	 for(uint32_t samp=0;samp<nb_synth;samp++)
	 {
	 for(uint32_t chan=0;chan<channels;chan++)
	 {
	 	     
	        val=(int)floor(sample_pcm[chan][samp]*scale);
		
		if(val>32767)	
		  	val=32767;
		if(val<-32768)
		  	val=-32768;
		*out++=val;      
	  }
	  }
	// Puge them
	 vorbis_synthesis_read(&STRUCT->vdsp,nb_synth); 
	 aprintf("This round : in %d bytes, out %d bytes\n",nbIn,*nbOut);
	return 1;
	
}								
#endif

