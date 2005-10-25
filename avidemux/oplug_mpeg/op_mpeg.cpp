/***************************************************************************
                          op_mpeg.cpp  -  description
                             -------------------
    begin                : Fri Aug 7 2009
    copyright            : (C) 2009 by mean
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
#include "config.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>




#include "ADM_lavcodec.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "subchunk.h"

#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encConfig.h"




//#include "aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"


#include "mpeg2enc/ADM_mpeg2enc.h"

#include "oplug_mpeg/op_mpeg_write.h"
#include "oplug_mpeg/op_mpeg.h"
#include "ADM_toolkit/filesel.h"

#include "prefs.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"

#include "ADM_audiofilter/audioeng_buildfilters.h"

#include "ADM_encoder/adm_encConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

#define CONTEXT_SVCD 1
#define CONTEXT_DVD    2

extern Mpeg2encParam SVCDExtra,DVDExtra; 
extern COMPRES_PARAMS SVCDCodec,DVDCodec;


extern uint8_t DIA_SVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,Mpeg2encParam *conf);

extern uint8_t  DIA_mpeg2opt(uint32_t *maxbitrate, uint32_t *qz, char *opt1,char **opts);

uint8_t oplug_mpegInit(void);
static uint8_t oplug_mpegStore(void);
static uint8_t oplug_mpeg_dvd_run(const char *name);
static uint8_t oplug_mpeg_vcd_run(const char *name);
static uint8_t oplug_mpeg_ts_run(const char *name);
//************************************************
uint8_t  oplug_mpeg_vcd(const char *inname)
{
	const char *name;
        uint8_t ret;

	mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	if(!inname)
	{
	 	GUI_FileSelWrite("VCD file to save", (char**)&name);
		if(!name) return 0;
	}	
	else
	{
		name=inname;
	}


	ret=mpg->save_vcd(name);
	delete(mpg);
        return ret;
}
//_______________________________________
uint8_t oplug_mpeg_vcd_ps(const char *inname)
{
	
	const char *name=NULL;
	WAVHeader *info=NULL,tmpinfo;

	// do some check on audio & video
	// audio must be either AC3 or mp2 at 48 khz
	
	// First check audio
	if(!currentaudiostream)
	{
		GUI_Error_HIG("There is no audio track", NULL);
		return 0;
	}
	if(audioProcessMode())
	{
	  AVDMGenericAudioStream *audio=NULL;
		audio = buildFakeAudioFilter (currentaudiostream,0,0x1000);
		info=audio->getInfo();
		memcpy(&tmpinfo,info,sizeof(tmpinfo));
		info=&tmpinfo;
		deleteAudioFilter();
	
	}
	else
	{
		info=currentaudiostream->getInfo();
	}
	if(info->frequency!=44100 )
	{
		GUI_Error_HIG("Incompatible audio frequency", "For VCD PS, samplerate must be 44.1 kHz.\nUse the resample audio filter.");
		return 0;
	}
	if( info->encoding!=WAV_MP2 )
	{
		printf("Encoding : %d\n",info->encoding);
		GUI_Error_HIG("Incompatible audio codec", "For VCD PS, audio must be MP2.");
		return 0;
	}
	if( info->channels!=2 )
	{
		
		GUI_Error_HIG("Audio must be stereo for VCD PS", NULL);
		return 0;
	}
	
	// Second, check video
	if(strcmp(videoCodecGetName(),"VCD"))// && strcmp(videoCodecGetName(),"XSVCD"))
	{
		GUI_Error_HIG("Wrong video codec", "Select VCD as the video codec.");
		return 0;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("VCD file to save", (char**)&name);
		if(!name) return 0;
	}
	else
	{
		name=inname;
	}
	return oplug_mpeg_vcd_run(name);

	
	
}
//_______________________________________
uint8_t oplug_mpeg_svcd(const char *inname)
{
	const char *name;
        uint8_t ret;

	if(!inname)
	{
	 	GUI_FileSelWrite("SVCD file to save", (char**)&name);
		if(!name) return 0;
	}
	else
	{
		name=inname;
	}


   	mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	ret=mpg->save_svcd(name);
	delete(mpg);
        return ret;
}
//_______________________________________
uint8_t oplug_mpeg_svcd_ps(const char *inname)
{
const char *name=NULL;
uint8_t ret;
WAVHeader info;
AVDMGenericAudioStream *stream;	
	
	//*****
	// First check audio
	if(!currentaudiostream)
	{
		GUI_Error_HIG("There is no audio track", NULL);
		return 0;
	}
	stream=mpt_getAudioStream();
	memcpy(&info,stream->getInfo(),sizeof(info));
	
	deleteAudioFilter();
	printf("Incoming audio:\n");
	printf("fq :%d\n",info.frequency);
	printf("co :%x\n",info.encoding);
	printf("ch :%x\n",info.channels);
	if(info.frequency!=44100 )
	{
		GUI_Error_HIG("Incompatible audio frequency", "For SVCD PS, samplerate must be 44.1 kHz.\nUse the resample audio filter.");
		return 0;
	}
	if( (info.encoding!=WAV_MP2 ))
	{
		printf("Encoding : %d\n",info.encoding);
		GUI_Error_HIG("Incompatible audio codec", "For SVCD PS, audio must be MP2.");
		return 0;
	}
	
	// Second, check video
	if(strcmp(videoCodecGetName(),"SVCD"))// && strcmp(videoCodecGetName(),"XSVCD"))
	{
		GUI_Error_HIG("Wrong video codec", "Select SVCD as the video codec.");
		return 0;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("SVCD file to save", (char**)&name);
		if(!name) return 0;
	}
	else
	{
		name=inname;
	}
	
	mpegWritter *mpg = new mpegWritter(MUXER_SVCD);
	ADM_assert(mpg);

	ret= mpg->save_svcd(name);
	delete(mpg);
        return ret;	

}

//_______________________________________
uint8_t oplug_mpeg_dvd(const char *inname)
{
const char *name=NULL;
uint8_t ret=0;

 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", (char**)&name);
		if(!name) return 0;
	}
	else
	{
		name=inname;
	}


   mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	ret= mpg->save_dvd(name);
	delete(mpg);
        return ret;
}
// Save a PS stream : Mpeg2 video + MP2 or AC3 audio
//_______________________________________
uint8_t oplug_mpeg_dvd_ps(const char *inname)
{
const char *name=NULL;
WAVHeader *info=NULL,tmpinfo;
uint8_t ret=0;

	// do some check on audio & video
	// audio must be either AC3 or mp2 at 48 khz
	
	// First check audio
	if(!currentaudiostream)
	{
		GUI_Error_HIG("There is no audio track", NULL);
		return 0;
	}
	if(audioProcessMode())
	{
	  AVDMGenericAudioStream *audio=NULL;
		audio = buildFakeAudioFilter (currentaudiostream,0,0x1000);
		info=audio->getInfo();
		memcpy(&tmpinfo,info,sizeof(tmpinfo));
		info=&tmpinfo;
		deleteAudioFilter();
	
	}
	else
	{
		info=currentaudiostream->getInfo();
	}
	if(info->frequency!=48000 )
	{
		GUI_Error_HIG("Incompatible audio frequency", "For DVD PS, audio samplerate must be 48 kHz.\nUse the resample audio filter.");
		return 0;
	}
	if( (info->encoding!=WAV_MP2 && info->encoding!=WAV_AC3 && info->encoding!=WAV_LPCM))
	{
		printf("Encoding : %d\n",info->encoding);
		GUI_Error_HIG("Incompatible audio codec", "For DVD PS, audio must be MP2,AC3 or LPCM.");
		return 0;
	}
	
	// Second, check video
	if(strcmp(videoCodecGetName(),"DVD"))// && strcmp(videoCodecGetName(),"XSVCD"))
	{
		GUI_Error_HIG("Wrong video codec", "Select DVD as the video codec.");
		return 0;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", (char**)&name);
		if(!name) return 0;
	}
	else
	{
		name=inname;
	}
	ret=oplug_mpeg_dvd_run(name);
        return ret;

}
//_______________________________________
uint8_t oplug_mpeg_dvd_run(const char *name)
{
        uint8_t ret=0;
   	mpegWritter *mpg = new mpegWritter(MUXER_DVD);
	ADM_assert(mpg);

	ret= mpg->save_dvd(name);
	delete(mpg);
        return ret;

}
//
// Save a TS stream : DVD into Ts wrapper
//_______________________________________
uint8_t oplug_mpeg_ts(const char *inname)
{
const char *name=NULL;
WAVHeader *info=NULL,tmpinfo;
uint8_t ret=0;

        // do some check on audio & video
        // audio must be either AC3 or mp2 at 48 khz
        
        // First check audio
        if(!currentaudiostream)
        {
                GUI_Error_HIG("There is no audio track", NULL);
                return 0;
        }
        if(audioProcessMode())
        {
          AVDMGenericAudioStream *audio=NULL;
                audio = buildFakeAudioFilter (currentaudiostream,0,0x1000);
                info=audio->getInfo();
                memcpy(&tmpinfo,info,sizeof(tmpinfo));
                info=&tmpinfo;
                deleteAudioFilter();
        
        }
        else
        {
                info=currentaudiostream->getInfo();
        }
        if(info->frequency!=48000 )
        {
                GUI_Error_HIG("Incompatible audio frequency", "For DVD, samplerate must be 48 kHz.\nUse the resample audio filter.");
                return 0;
        }
        if( (info->encoding!=WAV_MP2 && info->encoding!=WAV_AC3 && info->encoding!=WAV_LPCM))
        {
                printf("Encoding : %d\n",info->encoding);
                GUI_Error_HIG("Incompatible audio codec", "For DVD, audio must be MP2,AC3 or LPCM.");
                return 0;
        }
        
        // Second, check video
        if(strcmp(videoCodecGetName(),"DVD"))// && strcmp(videoCodecGetName(),"XSVCD"))
        {
                GUI_Error_HIG("Wrong video codec", "Select DVD as the video codec.");
                return 0;
        }
        
        if(!inname)
        {
                GUI_FileSelWrite("DVD file to save", (char**)&name);
                if(!name) return 0;
        }
        else
        {
                name=inname;
        }
        return oplug_mpeg_ts_run(name);


}
//_______________________________________
uint8_t  oplug_mpeg_ts_run(const char *name)
{
        mpegWritter *mpg = new mpegWritter(MUXER_TS);
        ADM_assert(mpg);
        uint8_t ret=0;
        ret=mpg->save_dvd(name);
        delete(mpg);
        return ret;

}
//_______________________________________
uint8_t oplug_mpeg_vcd_run(const char *name)
{
   	mpegWritter *mpg = new mpegWritter(MUXER_VCD);
	ADM_assert(mpg);
        uint8_t ret=0;

	ret=mpg->save_vcd(name);
	delete(mpg);
        return ret;

}
uint8_t oplug_mpegInit(void)
{
		// Init from prefs
		// SVCD
		prefs->get( CODECS_SVCD_ENCTYPE,    (unsigned int*)&SVCDCodec.mode);
		prefs->get( CODECS_SVCD_BITRATE,    &SVCDCodec.bitrate);
		prefs->get( CODECS_SVCD_QUANTIZER,  &SVCDCodec.qz);
		prefs->get( CODECS_SVCD_FINALSIZE,  &SVCDCodec.finalsize);
		
		prefs->get( CODECS_SVCD_INTERLACED, &SVCDExtra.interlaced);
		prefs->get( CODECS_SVCD_BFF,        &SVCDExtra.bff);
		prefs->get( CODECS_SVCD_WIDESCREEN, &SVCDExtra.widescreen);
		prefs->get( CODECS_SVCD_MATRIX,     &SVCDExtra.user_matrix);
		prefs->get( CODECS_SVCD_GOPSIZE,    &SVCDExtra.gop_size);
		prefs->get( CODECS_SVCD_MAXBITRATE, &SVCDExtra.maxBitrate);
		SVCDExtra.maxBitrate = (SVCDExtra.maxBitrate*1000) >> 3;
		//DVD
		prefs->get( CODECS_DVD_ENCTYPE,    (unsigned int*)&SVCDCodec.mode);
		prefs->get( CODECS_DVD_BITRATE,    &DVDCodec.bitrate);
		prefs->get( CODECS_DVD_QUANTIZER,  &DVDCodec.qz);
		prefs->get( CODECS_DVD_FINALSIZE,  &DVDCodec.finalsize);
		                                    
		prefs->get( CODECS_DVD_INTERLACED, &DVDExtra.interlaced);
		prefs->get( CODECS_DVD_BFF,        &DVDExtra.bff);
		prefs->get( CODECS_DVD_WIDESCREEN, &DVDExtra.widescreen);
		prefs->get( CODECS_DVD_MATRIX,     &DVDExtra.user_matrix);
		prefs->get( CODECS_DVD_GOPSIZE,    &DVDExtra.gop_size);
		prefs->get( CODECS_DVD_MAXBITRATE, &DVDExtra.maxBitrate);
		DVDExtra.maxBitrate = (DVDExtra.maxBitrate*1000) >> 3;
		return 1;
}
uint8_t oplug_mpegStore(void)
{
	uint32_t br;
		// Init from prefs
		// SVCD
		prefs->set( CODECS_SVCD_ENCTYPE,    (unsigned int)SVCDCodec.mode);
		prefs->set( CODECS_SVCD_BITRATE,     SVCDCodec.bitrate);
		prefs->set( CODECS_SVCD_QUANTIZER,   SVCDCodec.qz);
		prefs->set( CODECS_SVCD_FINALSIZE,   SVCDCodec.finalsize);
		                                   
		prefs->set( CODECS_SVCD_INTERLACED,  SVCDExtra.interlaced);
		prefs->set( CODECS_SVCD_BFF,         SVCDExtra.bff);
		prefs->set( CODECS_SVCD_WIDESCREEN,  SVCDExtra.widescreen);
		prefs->set( CODECS_SVCD_MATRIX,      SVCDExtra.user_matrix);
		prefs->set( CODECS_SVCD_GOPSIZE,     SVCDExtra.gop_size);
		
		br=(SVCDExtra.maxBitrate<< 3)/1000;;
		prefs->set( CODECS_SVCD_MAXBITRATE,  br);
		
		
		//DVD
		prefs->set( CODECS_DVD_ENCTYPE,    (unsigned int)DVDCodec.mode);
		prefs->set( CODECS_DVD_BITRATE,     DVDCodec.bitrate);
		prefs->set( CODECS_DVD_QUANTIZER,   DVDCodec.qz);
		prefs->set( CODECS_DVD_FINALSIZE,   DVDCodec.finalsize);
		                                  
		prefs->set( CODECS_DVD_INTERLACED,  DVDExtra.interlaced);
		prefs->set( CODECS_DVD_BFF,         DVDExtra.bff);
		prefs->set( CODECS_DVD_WIDESCREEN,  DVDExtra.widescreen);
		prefs->set( CODECS_DVD_MATRIX,      DVDExtra.user_matrix);
		prefs->set( CODECS_DVD_GOPSIZE,     DVDExtra.gop_size);
		
		br=(DVDExtra.maxBitrate<< 3)/1000;;
		prefs->set( CODECS_DVD_MAXBITRATE,  br);
		return 1;
}

//EOF
