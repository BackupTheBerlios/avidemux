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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "config.h"


#include "ADM_lavcodec.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
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

#define CONTEXT_SVCD 1
#define CONTEXT_DVD    2

extern uint8_t DIA_SVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf);

extern uint8_t  DIA_mpeg2opt(uint32_t *maxbitrate, uint32_t *qz, char *opt1,char **opts);
extern FFMPEGConfig mpeg2encSVCDConfig;
extern FFMPEGConfig mpeg2encDVDConfig;

static void oplug_mpeg_dvd_run(char *name);

void oplug_mpeg_vcd(char *inname)
{
	char *name;

	mpegWritter *mpg = new mpegWritter();
	assert(mpg);

	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", &name);
	}
	else
	{
		name=inname;
	}


	if( mpg->save_vcd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);
}

void oplug_mpeg_svcd(char *inname)
{
	char *name;

	if(!inname)
	{
	 	GUI_FileSelWrite("SVCD file to save", &name);
	}
	else
	{
		name=inname;
	}


   	mpegWritter *mpg = new mpegWritter();
	assert(mpg);

	if( mpg->save_svcd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);
}
void oplug_mpeg_dvd(char *inname)
{
char *name=NULL;

 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", &name);
	}
	else
	{
		name=inname;
	}


   mpegWritter *mpg = new mpegWritter();
	assert(mpg);

	if( mpg->save_dvd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);
}
// Save a PS stream : Mpeg2 video + MP2 or AC3 audio
void oplug_mpeg_dvd_ps(char *inname)
{
char *name=NULL;
WAVHeader *info=NULL,tmpinfo;

	// do some check on audio & video
	// audio must be either AC3 or mp2 at 48 khz
	
	// First check audio
	if(!currentaudiostream)
	{
		GUI_Alert("We need an audio track!");
		return;
	}
	if(audioProcessMode)
	{
	  AVDMGenericAudioStream *audio=NULL;
		audio = buildAudioFilter (currentaudiostream,0,1000);
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
		GUI_Alert("audio must be 48khz for DVD PS!");
		return;
	}
	if( (info->encoding!=WAV_MP2 && info->encoding!=WAV_AC3))
	{
		printf("Encoding : %d\n",info->encoding);
		GUI_Alert("audio must be MP2 or AC3 for DVD PS!");
		return;
	}
	
	// Second, check video
	if(strcmp(videoCodecGetName(),"DVD"))
	{
		GUI_Alert("You need to select DVD as video codec!");
		return;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", &name);
	}
	else
	{
		name=inname;
	}
	oplug_mpeg_dvd_run(name);


}
void oplug_mpeg_dvd_run(char *name)
{
   	mpegWritter *mpg = new mpegWritter(1);
	assert(mpg);

	if( mpg->save_dvd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);

}
void oplug_mpeg_svcdConf( void )
{

	DIA_SVCDParam( (char *)"SVCD", &mpeg2encSVCDConfig.generic.mode,
			&mpeg2encSVCDConfig.generic.qz,
			&mpeg2encSVCDConfig.generic.bitrate,
			&mpeg2encSVCDConfig.generic.finalsize,
			&mpeg2encSVCDConfig.specific

			);
}
void oplug_mpeg_dvdConf( void )
{

	DIA_SVCDParam( (char *)"DVD", &mpeg2encDVDConfig.generic.mode,
			&mpeg2encDVDConfig.generic.qz,
			&mpeg2encDVDConfig.generic.bitrate,
			&mpeg2encDVDConfig.generic.finalsize,
			&mpeg2encDVDConfig.specific

			);
}

