/***************************************************************************
                          oplug_vcdff.cpp  -  description
                             -------------------
    begin                : Sun Nov 10 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Ouput using FFmpeg mpeg1 encoder
    Much faster than mjpegtools, albeit quality seems inferior
    
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
#include <math.h>


#ifdef USE_FFMPEG
extern "C" {
#include "ADM_lavcodec.h"
};
#include "avi_vars.h"
#include "prototype.h"


#include "ADM_colorspace/colorspace.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"


#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_encoder/adm_encoder.h"
//#include "ADM_codecs/ADM_divxEncode.h"
//#include "ADM_encoder/adm_encdivx.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"
#include "oplug_mpegFF/oplug_vcdff.h"

#include "ADM_dialog/DIA_encoding.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_lavformat/ADM_lavformat.h"

static uint8_t *_buffer=NULL,*_outbuffer=NULL;
static void  end (void);
extern const char *getStrFromAudioCodec( uint32_t codec);
extern FFMPEGConfig ffmpegMpeg1Config;
extern FFMPEGConfig ffmpegMpeg2Config;
extern FFMPEGConfig ffmpegMpeg2ConfigDVD;
//static FFMPEGConfig ffmpegConfigforXVCD;


uint8_t DIA_XVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf	);

extern SelectCodecType  current_codec;

static char *twoPass=NULL;
static char *twoFake=NULL;

void oplug_mpegff_conf( void )
{

	DIA_XVCDParam("XVCD/XSVCD",&ffmpegMpeg1Config.generic.mode,
			&ffmpegMpeg1Config.generic.qz,
			&ffmpegMpeg1Config.generic.bitrate,
			&ffmpegMpeg1Config.generic.finalsize,
			&ffmpegMpeg1Config.specific

			);
}

void oplug_mpegff(char *name, uint8_t ps_stream)
{
AVDMGenericVideoStream *_incoming;
EncoderFFMPEGMpeg1  *encoder;

lavMuxer	*muxer=NULL;
FILE 		*file=NULL;
uint8_t		audioBuffer[48000];
uint32_t	audioLen=0;
uint32_t _w,_h,_fps1000,_page,total;	
AVDMGenericAudioStream	*audio;
uint32_t len,flags;
uint32_t size;
ADM_MUXER_TYPE mux;
uint32_t  audio_encoding=0;
	
	twoPass=new char[strlen(name)+6];
	twoFake=new char[strlen(name)+6];

	strcpy(twoPass,name);
	strcat(twoPass,".stat");
	strcat(twoFake,".fake");
	
		
	_incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
	_w=_incoming->getInfo()->width;
	_h=_incoming->getInfo()->height;
	_fps1000=_incoming->getInfo()->fps1000;
 	_page=_w*_h;
	_page+=_page>>1;


	
	total=_incoming->getInfo()->nb_frames;
	if(!total) return ;	
// ________alternate config____________
	

// override with mpeg1 specific stuff

	// Check if audio
	if(ps_stream)
	{
	
		if(!currentaudiostream)
		{
			GUI_Alert("There is no audio track.");
			return ;
		}
		audio=mpt_getAudioStream();
	
	// Have to check the type
	// If it is mpeg2 we use DVD-PS
	// If it is mpeg1 we use VCD-PS
	// Later check if it is SVCD
		if(!audio)
		{
			GUI_Alert("Audio track is  not suitable!\n");
			return;
		}
		// Check
		WAVHeader *hdr=audio->getInfo();
	
			audio_encoding=hdr->encoding;
	
	
		if(current_codec==CodecXVCD)
		{
			if(hdr->frequency!=44100 ||  hdr->encoding != WAV_MP2)
			{
				GUI_Alert("This is not compatible with VCD mpeg.\n");
				deleteAudioFilter();
				return ;
			}
			mux=MUXER_VCD;
			printf("X*CD: Using VCD PS\n");
	
		}else
		{    
			aviInfo info;
			video_body->getVideoInfo(&info);
			if(hdr->frequency==44100 && _w==480&&hdr->encoding == WAV_MP2 ) // SVCD ?
			{
				mux=MUXER_SVCD;
				printf("X*VCD: Using SVCD PS\n");
			}
			else
			{
			 // mpeg2, we do only DVD right now
			if(hdr->frequency!=48000 || 
			(hdr->encoding != WAV_MP2 && hdr->encoding!=WAV_AC3))
			{
				deleteAudioFilter();
				GUI_Alert("Audio track is not suitable!\n");
				return ;
			}
			mux=MUXER_DVD;
			printf("X*VCD: Using DVD PS\n");
			}
		}
	
		// Create muxer
		muxer=new lavMuxer();
		if(!muxer->open(name,0,mux,avifileinfo,audio->getInfo()))
		{
			delete muxer;
			muxer=NULL;
			deleteAudioFilter();
			printf("Muxer init failed\n");
			return ;
		}
	}
	else
	{	// Else open file (if possible)
		file=fopen(name,"wb");
		if(!file)
		{
			GUI_Alert("Cannot open output file !");
	 		return ;
		}
	}
	switch(current_codec)
	{
		case CodecXVCD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG1,&ffmpegMpeg1Config);
			printf("\n Using ffmpeg mpeg1 encoder\n");
			break;
		case CodecXSVCD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG2,&ffmpegMpeg2Config);
			printf("\n Using ffmpeg mpeg2 encoder\n");
			break;
		case CodecXDVD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG2,&ffmpegMpeg2ConfigDVD);
			printf("\n Using ffmpeg mpeg2 encoder (DVD)\n");
			break;
		
		default:
		ADM_assert(0);
	}
  	encoder->setLogFile(twoPass,total);
  	if(!encoder->configure(_incoming))
  	{
		delete encoder;
		return;
	}


	_buffer=new uint8_t[_page];
	_outbuffer=new uint8_t[_page];

	ADM_assert(  _buffer);
	ADM_assert(  _outbuffer);
 
  DIA_encoding				*encoding;
  encoding =new DIA_encoding(_fps1000);
  switch(current_codec)
	{
		case CodecXVCD:
  				encoding->setCodec("FFmpeg Mpeg1 VBR");
				break;
		case CodecXSVCD:
  				encoding->setCodec("FFmpeg Mpeg2 SVCD VBR");
				break;
		case CodecXDVD:
  				encoding->setCodec("FFmpeg Mpeg2 DVD VBR");
				break;
	}




  		// pass 1
    	if(encoder->isDualPass())
     	{
			FILE *fd;
			uint8_t reuse=0;
			fd=fopen(twoPass,"rt");
			if(fd)
			{
				if(GUI_Question("Reuse log file ?"))
				{
					reuse=1;
				}
				fclose(fd);
			}
			if(!reuse)
			{
				encoding->setPhasis ("Pass 1/2");
				encoder->startPass1();
				for(uint32_t i=0;i<total;i++)
				{
					encoding->setFrame(i,total);
					if(!encoder->encode( i, &len,(uint8_t *) _buffer,&flags))
					{
						GUI_Alert(" Error in pass 1!");
					}
					encoding->feedFrame(len);
					encoding->setQuant(encoder->getLastQz());
				}
			}
				encoder->startPass2();
				encoding->reset();
		}
		if(encoder->isDualPass())
			encoding->setPhasis ("Pass 2/2");
		else
			encoding->setPhasis ("Encoding");

         // Set info for audio if any
         if(muxer)
                  encoding->setAudioCodec(getStrFromAudioCodec(audio_encoding));
		// 2nd or Uniq Pass
		for(uint32_t i=0;i<total;i++)
			{
       	// get frame
				if(!encoder->encode( i, &len,(uint8_t *) _outbuffer,&flags))
				{
					GUI_Alert("Error in pass 2");
					goto finish;
				}
				if(file)
				{
					fwrite(_outbuffer,len,1,file);
				}
				else
				{
					uint32_t samples; 
					uint32_t dts=encoder->getDTS();
					
					//printf("%lu %lu\n",i,dts);
					
					muxer->writeVideoPacket(len,_outbuffer,
							i,dts);
					// _muxer->writeVideoPacket(len,_buffer_out,
					//i-MPEG_PREFILL,_codec->getCodedPictureNumber());
					while(muxer->needAudio()) 
					{				
						if(!audio->getPacket(audioBuffer, &audioLen, &samples))	
						{ 
							break; 
						}
						if(audioLen) 
						{
							muxer->writeAudioPacket(audioLen,audioBuffer); 
							encoding->feedAudioFrame(audioLen); 
						}

					}
				
				}
				encoding->setFrame(i,total);
				encoding->setQuant(encoder->getLastQz());
				encoding->feedFrame(len);
					if(!encoding->isAlive ())
						{
									 goto finish;
									 return ;
						}
			}
			
finish:
			delete encoding;
			GUI_Alert("Encoding done");
		 	end();
			if(file)
			{
				fclose(file);
				file=NULL;
			}
			else
			{
				muxer->close();
				delete muxer;
				muxer=NULL;
			}
			delete encoder;
			return ;
}
	
void end (void)
{
	
	delete [] _buffer;
	delete [] _outbuffer;

	_buffer		=NULL;
	_outbuffer=NULL;
	
	if(twoPass) delete [] twoPass;
	if(twoFake) delete [] twoFake;
	
	twoPass=twoFake=NULL;
	
}	
	
#endif	
// EOF
