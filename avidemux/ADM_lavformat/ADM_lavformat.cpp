//
// C++ Implementation: ADM_lavformat
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

extern "C" 
{
	
	#include "avformat.h"
};


#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/ADM_a52info.h"
#include "ADM_lavformat.h"




static    AVOutputFormat *fmt;
static    AVFormatContext *oc;
static    AVStream *audio_st, *video_st;
static    double audio_pts, video_pts;
//___________________________________________________________________________
lavMuxer::lavMuxer( void )
{
	fmt=NULL;
	oc=NULL;
	audio_st=NULL;
	video_st=NULL;
}
//___________________________________________________________________________
lavMuxer::~lavMuxer()
{
	close();
}
//___________________________________________________________________________
uint8_t lavMuxer::open( char *filename, uint32_t vbitrate, uint32_t fps1000, WAVHeader *audioheader,float need)
{
 AVCodecContext *c;

	fmt = guess_format("dvd", NULL, NULL);
	if (!fmt) 
	{
        	printf("Lav:Cannot guess format\n");
		return 0;
	}
	oc = av_alloc_format_context();
	if (!oc) 
	{
       		printf("Lav:Cannot allocate context\n");
		return 0;
	}
	oc->oformat = fmt;
	sprintf(oc->filename,"file://%s",filename);
	
	video_st = av_new_stream(oc, 0);
	if (!video_st) 
	{
		printf("Lav: new stream failed\n");
		return 0;
	}	
	
	c = &video_st->codec;
	c->codec_id = CODEC_ID_MPEG2VIDEO;
	c->codec_type = CODEC_TYPE_VIDEO;
	c->bit_rate = 9800000;
   
	c->width = 720;  
	c->height = 576;
    
	if(fps1000==23976 || fps1000==29960)
	{
		c->frame_rate = 3000;  
		c->frame_rate_base = 1001;
	}
	else
	{
		c->frame_rate = 25025;  
		c->frame_rate_base = 1001;
	
	}
	c->gop_size = 12; 	// Should not be needed
	c->max_b_frames = 2;	// id
	
	
	audio_st = av_new_stream(oc, 1);
	if (!audio_st) 
	{
		printf("Lav: new stream failed\n");
		return 0;
	}
	

		
	c = &audio_st->codec;
	c->codec_id = CODEC_ID_MP2;
	c->codec_type = CODEC_TYPE_AUDIO;
	
	c->bit_rate = audioheader->byterate*8;
	c->sample_rate = audioheader->frequency;
	c->channels = audioheader->channels;

//----------------------
	oc->packet_size=2048;
	oc->mux_rate=10080000;
	
	if (av_set_parameters(oc, NULL) < 0) 
	{
		printf("Lav: set param failed \n");
		return 0;
	}
	 if (url_fopen(&(oc->pb), filename, URL_WRONLY) < 0) 
	 {
	 	printf("Lav: Failed to open file :%s\n",filename);
		return 0;
        }

	av_write_header(oc);
	dump_format(oc, 0, filename, 1);


	printf("lavformat mpeg muxer initialized\n");
	
	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::writeAudioPacket(uint32_t len, uint8_t *buf)
{

	int ret;
  	AVPacket pkt;
            av_init_packet(&pkt);
            
            pkt.pts= AV_NOPTS_VALUE;
            pkt.stream_index=1;
            pkt.flags |= PKT_FLAG_KEY;

            pkt.data= buf;
            pkt.size= len;
            
            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
	    if(ret) 
			{
				printf("Error writing audio packet\n");
				return 0;
			}

	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::writeVideoPacket(uint32_t len, uint8_t *buf)
{
int ret;
static int i=0;
  	AVPacket pkt;
            av_init_packet(&pkt);
             pkt.flags |= PKT_FLAG_KEY;

            pkt.pts= 0;
            pkt.stream_index=0;
           
            pkt.data= buf;
            pkt.size= len;
            
            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
		if(ret) 
			{
				printf("Error writing video packet\n");
				return 0;
			}
	printf("Lavf :%d frame done\n",i);
	i++;
	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::forceRestamp(void)
{
	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::close( void )
{
	if(audio_st)
		 av_freep(audio_st);
	if(video_st)
		 av_freep(video_st);
	video_st=NULL;
	audio_st=NULL;
	if(oc)
		av_free(oc);
	oc=NULL;
	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::audioEmpty( void)
{
	return 0;
}
//___________________________________________________________________________
//EOF


