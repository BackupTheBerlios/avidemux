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
#include <string.h>

extern "C" 
{
	
	#include "avformat.h"
};

#undef malloc
#undef realloc
#undef free
#include <ADM_assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/ADM_a52info.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_editor/ADM_Video.h"

#include "ADM_lavformat.h"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_LAVFORMAT
#include "ADM_toolkit/ADM_debug.h"


static    AVOutputFormat *fmt;
static    AVFormatContext *oc;
static    AVStream *audio_st, *video_st;
static    double audio_pts, video_pts;

static uint32_t one;
uint64_t  _curDTS;

// convert in in us to out in 90Khz tick
int64_t adm_90k( double in)
{
int64_t out;
	// in=in/1000*1000 us->s
	// in/=(1/90000) nb 90 khz tick
	// in*=90*000/000*000
	// in =  in*=90/000
	//in=in*90./1000.;
	out= (int64_t)in;
	return out;
}
//___________________________________________________________________________
lavMuxer::lavMuxer( void )
{
	fmt=NULL;
	oc=NULL;
	audio_st=NULL;
	video_st=NULL;
	_fps1000=0;
	_audioByterate=0;
	_total=0;
	_frameNo=0;
	_running=0;
	_curDTS=0;
}
//___________________________________________________________________________
lavMuxer::~lavMuxer()
{
	close();
}
//___________________________________________________________________________
uint8_t lavMuxer::open(const char *filename, uint32_t inbitrate,ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader) 
{
        return open(filename,inbitrate,type,info,0,NULL,audioheader,0,NULL);
}


uint8_t lavMuxer::open(const char *filename,uint32_t inbitrate, ADM_MUXER_TYPE type, aviInfo *info,uint32_t videoExtraDataSize,
                        uint8_t *videoExtraData, WAVHeader *audioheader,uint32_t audioextraSize,uint8_t *audioextraData)
{
 AVCodecContext *c;
 	_type=type;
	_fps1000=info->fps1000;
	switch(_type)
	{
                case MUXER_TS:
                        fmt=guess_format("mpegts", NULL, NULL);
                        break;
		case MUXER_DVD:
			fmt = guess_format("dvd", NULL, NULL);
			break;
		case MUXER_VCD:
			fmt = guess_format("vcd", NULL, NULL);
			break;
		case MUXER_SVCD:
			fmt = guess_format("svcd", NULL, NULL);
			break;
                case MUXER_MP4:
                        fmt = guess_format("mp4", NULL, NULL);
                        break;
		default:
			fmt=NULL;
	}
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
	snprintf(oc->filename,1000,"file://%s",filename);
	// Video
	//________
	
	video_st = av_new_stream(oc, 0);
	if (!video_st) 
	{
		printf("Lav: new stream failed\n");
		return 0;
	}	
	
	c = video_st->codec;
	switch(_type)
	{
                case MUXER_MP4:
                        if(isMpeg4Compatible(info->fcc))
                        {
                                c->codec_id = CODEC_ID_MPEG4;
                        }else
                        {
                                if(isH264Compatible(info->fcc))
                                {
                                        c->codec_id = CODEC_ID_H264;
                                }
                                else
                                {
                                         c->codec_id = CODEC_ID_MPEG4; // Default value
                                        printf("Ooops, cant mux that...\n");
                                        printf("Ooops, cant mux that...\n");
                                        printf("Ooops, cant mux that...\n");
                                        //return 0;
                                }
                        }
                        if(videoExtraDataSize)
                        {
                                c->extradata=videoExtraData;
                                c->extradata_size= videoExtraDataSize;
                        }
                        c->rc_buffer_size=8*1024*224;
                        c->rc_max_rate=9500*1000;
                        c->rc_min_rate=0;
                        if(!inbitrate)
                                c->bit_rate=9000*1000;
                        else
                                c->bit_rate=inbitrate;
        
                        break;
                case MUXER_TS:
                        c->codec_id = CODEC_ID_MPEG2VIDEO;
                        c->rc_buffer_size=8*1024*224;
                        c->rc_max_rate=9500*1000;
                        c->rc_min_rate=0;
                        if(!inbitrate)
                                c->bit_rate=9000*1000;
                        else
                                c->bit_rate=inbitrate;
        
                        break;
		case MUXER_DVD:
			c->codec_id = CODEC_ID_MPEG2VIDEO;
			c->rc_buffer_size=8*1024*224;
			c->rc_max_rate=9500*1000;
			c->rc_min_rate=0;
			if(!inbitrate)
				c->bit_rate=9000*1000;
			else
				c->bit_rate=inbitrate;
	
			break;
		case MUXER_VCD:
			c->codec_id = CODEC_ID_MPEG1VIDEO;

			c->rc_buffer_size=8*1024*40;
			c->rc_max_rate=1152*1000;
			c->rc_min_rate=1152*1000;
			
			c->bit_rate=1152*1000;
			

			break;
		case MUXER_SVCD:
			c->codec_id = CODEC_ID_MPEG2VIDEO;

			c->rc_buffer_size=8*1024*112;
			c->rc_max_rate=2500*1000;
			c->rc_min_rate=0*1000;
			if(!inbitrate)
				c->bit_rate=2040*1000;
			else
				c->bit_rate=inbitrate;

			break;
		default:
			ADM_assert(0);
	}
	
	c->codec_type = CODEC_TYPE_VIDEO;
	c->flags=CODEC_FLAG_QSCALE;   
	c->width = info->width;  
	c->height = info->height; 
	
    	switch(_fps1000)
	{
		case 25000:
			 c->time_base= (AVRational){1001,25025};
			//c->frame_rate = 25025;  
			//c->frame_rate_base = 1001;	
			break;
		case 23976:
/*
			c->frame_rate = 24000;  
			c->frame_rate_base = 1001;	
			break;
*/
		case  29970:
			 c->time_base= (AVRational){1001,30000};
			//c->frame_rate = 30000;  
			//c->frame_rate_base = 1001;	
			break;
		default:
                        if(_type==MUXER_MP4)
                        {
                                c->time_base= (AVRational){1000,_fps1000};
                                return 1;
                        }
                        else
			     GUI_Error_HIG("Incompatible frame rate", NULL);
			return 0;
	}

			
	c->gop_size=15;
	c->max_b_frames=2;
	c->has_b_frames=1;

	
	// Audio
	//________
        if(audioheader)
        {
	audio_st = av_new_stream(oc, 1);
	if (!audio_st) 
	{
		printf("Lav: new stream failed\n");
		return 0;
	}

		
	c = audio_st->codec;
        switch(audioheader->encoding)
        {
                case WAV_AC3: c->codec_id = CODEC_ID_AC3;break;
                case WAV_MP2: c->codec_id = CODEC_ID_MP2;break;
                case WAV_MP3: c->codec_id = CODEC_ID_MP3;break;
                case WAV_AAC: 
                                c->extradata=audioextraData;
                                c->extradata_size= audioextraSize;
                                c->codec_id = CODEC_ID_AAC;
                                break;
                default:
                        printf("Cant mux that ! audio\n"); 
                        printf("Cant mux that ! audio\n");
                        c->codec_id = CODEC_ID_MP2;
                        return 0;
                        break;
        }
	c->codec_type = CODEC_TYPE_AUDIO;
	
	c->bit_rate = audioheader->byterate*8;
	c->sample_rate = audioheader->frequency;
	c->channels = audioheader->channels;
        _audioByterate=audioheader->byterate;
        }
	
	
//----------------------
	switch(_type)
	{
                case MUXER_MP4:
                        oc->mux_rate=10080*1000; // Needed ?
                        break;

                case MUXER_TS:
                        oc->mux_rate=10080*1000;
                        break;
		case MUXER_DVD:
			oc->packet_size=2048;
			oc->mux_rate=10080*1000;
			break;
		case MUXER_VCD:
			oc->packet_size=2324;
			oc->mux_rate=2352 * 75 * 8;
			
			break;
		case MUXER_SVCD:
			
			oc->packet_size=2324;
			oc->mux_rate=2*2352 * 75 * 8; // ?
			
			break;
		default:
			ADM_assert(0);
	}
	oc->preload=AV_TIME_BASE/10; // 100 ms preloading
	oc->max_delay=200*1000; // 500 ms
	
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
	
	_running=1;

	one=(1000*1000*1000)/_fps1000;
	_curDTS=one;

	return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::writeAudioPacket(uint32_t len, uint8_t *buf,uint32_t sample)
{
        
        int ret;
        AVPacket pkt;
        double f;

           if(!audio_st) return 0;

            av_init_packet(&pkt);


            f=sample*1000.*1000.;
            f=f/(double)audio_st->codec->sample_rate; // in secons

            
            pkt.dts=pkt.pts=(int64_t)floor(f);

            pkt.flags |= PKT_FLAG_KEY; 
            pkt.data= buf;
            pkt.size= len;
            pkt.stream_index=1;

            aprintf("A: frame  pts%d\n",pkt.pts); 

            ret = av_write_frame(oc, &pkt);
            _total=sample;
            if(ret) 
            {
                        printf("Error writing audio packet\n");
                        return 0;
            }
            return 1;
}
//___________________________________________________________________________
uint8_t lavMuxer::needAudio( void )
{
	
        if(!audio_st) return 0;

	double f;
	uint64_t dts;
	 	f=_total;
	    	f*=1000.*1000.;
	    	f/=_audioByterate;
		f+=2000;
		dts=(uint64_t)floor(f);
		aprintf("Need audio  ?: %llu / %llu : %llu\n ",dts,_curDTS,_curDTS+one);
		if((dts>=_curDTS) && (dts<=_curDTS+one)) return 1;
		if(dts<=_curDTS)
		{
			printf("LavMuxer:Audio DTS is too low %llu / %llu!\n",dts,_curDTS);
			return 1;
		}
		return 0;
}
//___________________________________________________________________________
uint8_t lavMuxer::writeVideoPacket(uint32_t len,uint32_t flags, uint8_t *buf,uint32_t frameno,uint32_t displayframe)
{
int ret;

double p,d;
  	AVPacket pkt;
            av_init_packet(&pkt);
	    
	p=displayframe+2;      // Pts
	p=(p*1000*1000*1000);
	p=p/_fps1000;
	
	d=frameno;		// dts
	d=(d*1000*1000*1000);
	d=d/_fps1000;
	
	
	_curDTS=(int64_t)floor(d);
	
	pkt.dts=(int64_t)floor(d);
	pkt.pts=(int64_t)floor(p);
	
	aprintf("Lavformat : Pts :%llu dts:%llu",pkt.pts,pkt.dts);
	pkt.stream_index=0;
           
	pkt.data= buf;
	pkt.size= len;
	// Look if it is a gop start or seq start
        if(_type==MUXER_MP4)
        {
                if(flags & AVI_KEY_FRAME) 
                        pkt.flags |= PKT_FLAG_KEY;
        }else
	if(!buf[0] &&  !buf[1] && buf[2]==1)
	{
		if(buf[3]==0xb3 || buf[3]==0xb8 ) // Seq start or gop start
		pkt.flags |= PKT_FLAG_KEY;
		//printf("Intra\n"); 
	}
           
	ret =av_write_frame(oc, &pkt);
	if(ret) 
	{
		printf("Error writing video packet\n");
		return 0;
	}
	aprintf("V: frame %lu pts%d\n",frameno,pkt.pts);
	
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
	if(_running)
	{
		_running=0;
		// Flush
		// Cause deadlock :
		av_write_trailer(oc);
		url_fclose(&oc->pb);

	}
	if(audio_st)
	{
		 av_free(audio_st);
	}
	if(video_st)
	{
		 av_free(video_st);
	}
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
extern "C"
{
     extern  int        mpegps_init(void );
     extern  int        movenc_init(void );
};
extern URLProtocol file_protocol ;
uint8_t lavformat_init(void)
{
                mpegps_init();
                movenc_init();
                register_protocol(&file_protocol);
}

//___________________________________________________________________________
//EOF


