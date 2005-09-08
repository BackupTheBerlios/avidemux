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

#include "ADM_encoder/adm_encConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


static uint8_t *_buffer=NULL,*_outbuffer=NULL;
static void  end (void);
extern const char *getStrFromAudioCodec( uint32_t codec);

extern COMPRES_PARAMS ffmpeg1Codec,ffmpeg2DVDCodec,ffmpeg2SVCDCodec;	
extern FFcodecSetting ffmpeg1Extra,ffmpeg2DVDExtra,ffmpeg2SVCDExtra;



uint8_t DIA_XVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf	);

extern SelectCodecType  current_codec;

static char *twoPass=NULL;
static char *twoFake=NULL;

void oplug_mpegff_conf( void )
{

	DIA_XVCDParam("XVCD/XSVCD",&ffmpeg1Codec.mode,
			&ffmpeg1Codec.qz,
			&ffmpeg1Codec.bitrate,
			&ffmpeg1Codec.finalsize,
			&ffmpeg1Extra

			);
}

uint8_t oplug_mpegff(const char *name, ADM_OUT_FORMAT type)
{
AVDMGenericVideoStream *_incoming;
EncoderFFMPEGMpeg1  *encoder;

ADMMpegMuxer	*muxer=NULL;
FILE 		*file=NULL;
uint8_t		audioBuffer[48000];
uint32_t	audioLen=0;
uint32_t _w,_h,_fps1000,_page,total;	
AVDMGenericAudioStream	*audio;
uint32_t len,flags;
uint32_t size;
ADM_MUXER_TYPE mux;
uint32_t  audio_encoding=0;
uint32_t  real_framenum=0;
uint8_t   ret=0;
	
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
	if(!total) return 0;	
// ________alternate config____________
	

// override with mpeg1 specific stuff

	// Check if audio
        switch(type)
        {
            default:
                    ADM_assert(0);
            case ADM_ES:
                        // Else open file (if possible)                       
                        mux=MUXER_NONE;
                        break;
            case ADM_TS:
                    if(!currentaudiostream)
                    {
                        GUI_Error_HIG("There is no audio track", NULL);
                        return 0;
                    }
                    audio=mpt_getAudioStream();
                    mux=MUXER_TS;
                    break;
            case ADM_PS:
            
            {
                if(!currentaudiostream)
                {
                        GUI_Error_HIG("There is no audio track", NULL);
                        return 0;
                }
                audio=mpt_getAudioStream();
                // Have to check the type
                // If it is mpeg2 we use DVD-PS
                // If it is mpeg1 we use VCD-PS
                // Later check if it is SVCD
                if(!audio)
                {
                        GUI_Error_HIG("Audio track is not suitable", NULL);
                        return 0;
                }
                // Check
                WAVHeader *hdr=audio->getInfo();	
                audio_encoding=hdr->encoding;
                if(current_codec==CodecXVCD)
                {
                        if(hdr->frequency!=44100 ||  hdr->encoding != WAV_MP2)
                        {
                            GUI_Error_HIG("Incompatible audio", "For VCD, audio must be 44.1 kHz MP2.");
                            deleteAudioFilter();
                            return 0;
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
                                GUI_Error_HIG("Incompatible audio", "For DVD, audio must be 48 kHz MP2 or AC3.");
                                return 0 ;
                            }
                            mux=MUXER_DVD;
                            printf("X*VCD: Using DVD PS\n");
                        }
                }
            }
         }        
        // Create muxer
        switch(type)
        {
            case ADM_PS:
                    muxer=new mplexMuxer;
                    break;
            case ADM_TS:
                    muxer=new tsMuxer;
                    break;
            case ADM_ES:
                    break;
            default:
                    ADM_assert(0);
        
        
        }
        if(muxer)
        {
            if(!muxer->open(name,0,mux,avifileinfo,audio->getInfo()))
            {
                delete muxer;
                muxer=NULL;
                deleteAudioFilter();
                printf("Muxer init failed\n");
                return 0 ;
            }
        }
        else
        {
            file=fopen(name,"wb");
            if(!file)
            {
                    GUI_Error_HIG("File error", "Cannot open \"%s\" for writing.", name);
                    return 0 ;
            }
        }
	switch(current_codec)
	{
		
		case CodecXVCD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG1,&ffmpeg1Codec);
			printf("\n Using ffmpeg mpeg1 encoder\n");
			break;
		case CodecXSVCD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG2,&ffmpeg2SVCDCodec);
			printf("\n Using ffmpeg mpeg2 encoder\n");
			break;
		case CodecXDVD:
			encoder=new EncoderFFMPEGMpeg1(FF_MPEG2,&ffmpeg2DVDCodec);
			printf("\n Using ffmpeg mpeg2 encoder (DVD)\n");
			break;
		
		default:
		ADM_assert(0);
	}
        
  	encoder->setLogFile(twoPass,total);
  	if(!encoder->configure(_incoming))
  	{
		delete encoder;
		return 0;
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
switch(mux)
  {
    case MUXER_NONE:encoding->setContainer("Mpeg ES");break;
    case MUXER_TS:  encoding->setContainer("Mpeg TS");break;
    case MUXER_VCD: encoding->setContainer("Mpeg VCD");break;
    case MUXER_SVCD:encoding->setContainer("Mpeg SVCD");break;
    case MUXER_DVD: encoding->setContainer("Mpeg DVD");break;
    default:
        ADM_assert(0);
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
						GUI_Error_HIG("Error in pass 1", NULL);
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
         {
            if(!audioProcessMode())
                  encoding->setAudioCodec("Copy");
            else
                  encoding->setAudioCodec(getStrFromAudioCodec(audio->getInfo()->encoding));
         }
		// 2nd or Uniq Pass
		for(uint32_t i=0;i<total;i++)
			{
       	// get frame
				if(!encoder->encode( i, &len,(uint8_t *) _outbuffer,&flags))
				{
					GUI_Error_HIG("Error in pass 2", NULL);
					goto finish;
				}
				if(!len) continue;
				
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
							real_framenum,dts);
					real_framenum++;
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
                                                         ret=0;        
							 goto finish;
						}
			}
			ret=1;
finish:
			delete encoding;
			GUI_Info_HIG("Done", "Successfully saved \"%s\".", GetFileName(name));
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
			return ret;
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
