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

#define CONTEXT_SVCD 1
#define CONTEXT_DVD    2

extern uint8_t DIA_SVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,Mpeg2encParam *conf);

extern uint8_t  DIA_mpeg2opt(uint32_t *maxbitrate, uint32_t *qz, char *opt1,char **opts);
extern MPEG2ENCConfig mpeg2encSVCDConfig;
extern MPEG2ENCConfig mpeg2encDVDConfig;

static void oplug_mpeg_dvd_run(char *name);
static void oplug_mpeg_vcd_run(char *name);
//************************************************
void oplug_mpeg_vcd(char *inname)
{
	char *name;

	mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	if(!inname)
	{
	 	GUI_FileSelWrite("VCD file to save", &name);
		if(!name) return;
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
//_______________________________________
void oplug_mpeg_vcd_ps(char *inname)
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
		audio = buildAudioFilter (currentaudiostream,0,0x1000);
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
		GUI_Alert("audio must be 44.1khz for VCD PS!");
		return;
	}
	if( info->encoding!=WAV_MP2 )
	{
		printf("Encoding : %d\n",info->encoding);
		GUI_Alert("audio must be MP2 VCD PS!");
		return;
	}
	
	// Second, check video
	if(strcmp(videoCodecGetName(),"VCD"))// && strcmp(videoCodecGetName(),"XSVCD"))
	{
		GUI_Alert("You need to select VCD as video codec!");
		return;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("VCD file to save", &name);
		if(!name) return;
	}
	else
	{
		name=inname;
	}
	oplug_mpeg_vcd_run(name);

	
	
}
//_______________________________________
void oplug_mpeg_svcd(char *inname)
{
	char *name;

	if(!inname)
	{
	 	GUI_FileSelWrite("SVCD file to save", &name);
		if(!name) return;
	}
	else
	{
		name=inname;
	}


   	mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	if( mpg->save_svcd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);
}
//_______________________________________
void oplug_mpeg_dvd(char *inname)
{
char *name=NULL;

 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", &name);
		if(!name) return;
	}
	else
	{
		name=inname;
	}


   mpegWritter *mpg = new mpegWritter();
	ADM_assert(mpg);

	if( mpg->save_dvd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);
}
// Save a PS stream : Mpeg2 video + MP2 or AC3 audio
//_______________________________________
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
		audio = buildAudioFilter (currentaudiostream,0,0x1000);
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
	if(strcmp(videoCodecGetName(),"DVD"))// && strcmp(videoCodecGetName(),"XSVCD"))
	{
		GUI_Alert("You need to select DVD as video codec!");
		return;
	}
	
 	if(!inname)
	{
	 	GUI_FileSelWrite("DVD file to save", &name);
		if(!name) return;
	}
	else
	{
		name=inname;
	}
	oplug_mpeg_dvd_run(name);


}
//_______________________________________
void oplug_mpeg_dvd_run(char *name)
{
   	mpegWritter *mpg = new mpegWritter(1);
	ADM_assert(mpg);

	if( mpg->save_dvd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);

}
//_______________________________________
void oplug_mpeg_vcd_run(char *name)
{
   	mpegWritter *mpg = new mpegWritter(2);
	ADM_assert(mpg);

	if( mpg->save_vcd(name))
		GUI_Alert("Success !");
	else
		GUI_Alert("Failed !");

	delete(mpg);

}
//_______________________________________
void oplug_mpeg_svcdConf( void )
{
	if( mpeg2encSVCDConfig.configured == 0 ){
		prefs->get( CODECS_SVCD_ENCTYPE,    (unsigned int*)&mpeg2encSVCDConfig.generic.mode);
		prefs->get( CODECS_SVCD_BITRATE,    &mpeg2encSVCDConfig.generic.bitrate);
		prefs->get( CODECS_SVCD_QUANTIZER,  &mpeg2encSVCDConfig.generic.qz);
		prefs->get( CODECS_SVCD_FINALSIZE,  &mpeg2encSVCDConfig.generic.finalsize);
		prefs->get( CODECS_SVCD_INTERLACED, &mpeg2encSVCDConfig.specific.interlaced);
		prefs->get( CODECS_SVCD_BFF,        &mpeg2encSVCDConfig.specific.bff);
		prefs->get( CODECS_SVCD_WIDESCREEN, &mpeg2encSVCDConfig.specific.widescreen);
		prefs->get( CODECS_SVCD_MATRIX,     &mpeg2encSVCDConfig.specific.user_matrix);
		prefs->get( CODECS_SVCD_GOPSIZE,    &mpeg2encSVCDConfig.specific.gop_size);
		prefs->get( CODECS_SVCD_MAXBITRATE, &mpeg2encSVCDConfig.specific.maxBitrate);
		mpeg2encSVCDConfig.specific.maxBitrate = (mpeg2encSVCDConfig.specific.maxBitrate*1000) >> 3;
		mpeg2encSVCDConfig.configured = 1;
	}
	DIA_SVCDParam( (char *)"SVCD", &mpeg2encSVCDConfig.generic.mode,
			&mpeg2encSVCDConfig.generic.qz,
			&mpeg2encSVCDConfig.generic.bitrate,
			&mpeg2encSVCDConfig.generic.finalsize,
			&mpeg2encSVCDConfig.specific

			);
	/* set configured values to prefs */
	prefs->set( CODECS_SVCD_ENCTYPE,    (unsigned int)mpeg2encSVCDConfig.generic.mode);
	prefs->set( CODECS_SVCD_BITRATE,    mpeg2encSVCDConfig.generic.bitrate);
	prefs->set( CODECS_SVCD_QUANTIZER,  mpeg2encSVCDConfig.generic.qz);
	prefs->set( CODECS_SVCD_FINALSIZE,  mpeg2encSVCDConfig.generic.finalsize);
	prefs->set( CODECS_SVCD_INTERLACED, mpeg2encSVCDConfig.specific.interlaced);
	prefs->set( CODECS_SVCD_BFF,        mpeg2encSVCDConfig.specific.bff);
	prefs->set( CODECS_SVCD_WIDESCREEN, mpeg2encSVCDConfig.specific.widescreen);
	prefs->set( CODECS_SVCD_MATRIX,     mpeg2encSVCDConfig.specific.user_matrix);
	prefs->set( CODECS_SVCD_GOPSIZE,    mpeg2encSVCDConfig.specific.gop_size);
	mpeg2encSVCDConfig.specific.maxBitrate = (mpeg2encSVCDConfig.specific.maxBitrate << 3) / 1000;
	prefs->set( CODECS_SVCD_MAXBITRATE, mpeg2encSVCDConfig.specific.maxBitrate);
	/* read "fixed"-"in range" values for further processing */
	prefs->get( CODECS_SVCD_ENCTYPE,    (unsigned int*)&mpeg2encSVCDConfig.generic.mode);
	prefs->get( CODECS_SVCD_BITRATE,    &mpeg2encSVCDConfig.generic.bitrate);
	prefs->get( CODECS_SVCD_QUANTIZER,  &mpeg2encSVCDConfig.generic.qz);
	prefs->get( CODECS_SVCD_FINALSIZE,  &mpeg2encSVCDConfig.generic.finalsize);
	prefs->get( CODECS_SVCD_INTERLACED, &mpeg2encSVCDConfig.specific.interlaced);
	prefs->get( CODECS_SVCD_BFF,        &mpeg2encSVCDConfig.specific.bff);
	prefs->get( CODECS_SVCD_WIDESCREEN, &mpeg2encSVCDConfig.specific.widescreen);
	prefs->get( CODECS_SVCD_MATRIX,     &mpeg2encSVCDConfig.specific.user_matrix);
	prefs->get( CODECS_SVCD_GOPSIZE,    &mpeg2encSVCDConfig.specific.gop_size);
	prefs->get( CODECS_SVCD_MAXBITRATE, &mpeg2encSVCDConfig.specific.maxBitrate);
	mpeg2encSVCDConfig.specific.maxBitrate = (mpeg2encSVCDConfig.specific.maxBitrate*1000) >> 3;
}
//_______________________________________
void oplug_mpeg_dvdConf( void )
{
	if( mpeg2encDVDConfig.configured == 0 ){
		prefs->get( CODECS_DVD_ENCTYPE,    (unsigned int*)&mpeg2encDVDConfig.generic.mode);
		prefs->get( CODECS_DVD_BITRATE,    &mpeg2encDVDConfig.generic.bitrate);
		prefs->get( CODECS_DVD_QUANTIZER,  &mpeg2encDVDConfig.generic.qz);
		prefs->get( CODECS_DVD_FINALSIZE,  &mpeg2encDVDConfig.generic.finalsize);
		prefs->get( CODECS_DVD_INTERLACED, &mpeg2encDVDConfig.specific.interlaced);
		prefs->get( CODECS_DVD_BFF,        &mpeg2encDVDConfig.specific.bff);
		prefs->get( CODECS_DVD_WIDESCREEN, &mpeg2encDVDConfig.specific.widescreen);
		prefs->get( CODECS_DVD_MATRIX,     &mpeg2encDVDConfig.specific.user_matrix);
		prefs->get( CODECS_DVD_GOPSIZE,    &mpeg2encDVDConfig.specific.gop_size);
		prefs->get( CODECS_DVD_MAXBITRATE, &mpeg2encDVDConfig.specific.maxBitrate);
		mpeg2encDVDConfig.specific.maxBitrate = (mpeg2encDVDConfig.specific.maxBitrate*1000) >> 3;
		mpeg2encDVDConfig.configured = 1;
	}
	DIA_SVCDParam( (char *)"DVD", &mpeg2encDVDConfig.generic.mode,
			&mpeg2encDVDConfig.generic.qz,
			&mpeg2encDVDConfig.generic.bitrate,
			&mpeg2encDVDConfig.generic.finalsize,
			&mpeg2encDVDConfig.specific

			);
	/* set configured values to prefs */
	prefs->set( CODECS_DVD_ENCTYPE,    (unsigned int)mpeg2encDVDConfig.generic.mode);
	prefs->set( CODECS_DVD_BITRATE,    mpeg2encDVDConfig.generic.bitrate);
	prefs->set( CODECS_DVD_QUANTIZER,  mpeg2encDVDConfig.generic.qz);
	prefs->set( CODECS_DVD_FINALSIZE,  mpeg2encDVDConfig.generic.finalsize);
	prefs->set( CODECS_DVD_INTERLACED, mpeg2encDVDConfig.specific.interlaced);
	prefs->set( CODECS_DVD_BFF,        mpeg2encDVDConfig.specific.bff);
	prefs->set( CODECS_DVD_WIDESCREEN, mpeg2encDVDConfig.specific.widescreen);
	prefs->set( CODECS_DVD_MATRIX,     mpeg2encDVDConfig.specific.user_matrix);
	prefs->set( CODECS_DVD_GOPSIZE,    mpeg2encDVDConfig.specific.gop_size);
	mpeg2encDVDConfig.specific.maxBitrate = (mpeg2encDVDConfig.specific.maxBitrate << 3) / 1000;
	prefs->set( CODECS_DVD_MAXBITRATE, mpeg2encDVDConfig.specific.maxBitrate);
	/* read "fixed"-"in range" values for further processing */
	prefs->get( CODECS_DVD_ENCTYPE,    (unsigned int*)&mpeg2encDVDConfig.generic.mode);
	prefs->get( CODECS_DVD_BITRATE,    &mpeg2encDVDConfig.generic.bitrate);
	prefs->get( CODECS_DVD_QUANTIZER,  &mpeg2encDVDConfig.generic.qz);
	prefs->get( CODECS_DVD_FINALSIZE,  &mpeg2encDVDConfig.generic.finalsize);
	prefs->get( CODECS_DVD_INTERLACED, &mpeg2encDVDConfig.specific.interlaced);
	prefs->get( CODECS_DVD_BFF,        &mpeg2encDVDConfig.specific.bff);
	prefs->get( CODECS_DVD_WIDESCREEN, &mpeg2encDVDConfig.specific.widescreen);
	prefs->get( CODECS_DVD_MATRIX,     &mpeg2encDVDConfig.specific.user_matrix);
	prefs->get( CODECS_DVD_GOPSIZE,    &mpeg2encDVDConfig.specific.gop_size);
	prefs->get( CODECS_DVD_MAXBITRATE, &mpeg2encDVDConfig.specific.maxBitrate);
	mpeg2encDVDConfig.specific.maxBitrate = (mpeg2encDVDConfig.specific.maxBitrate*1000) >> 3;
}
//EOF
