/***************************************************************************
                          audioeng_piper.cpp  -  description
                             -------------------

			     	Medium levem if to piping

    begin                : Sun Jan 13 2002
    copyright            : (C) 2002 by mean
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
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
//#include "ADM_gui/GUI_mp3.h"
#include "ADM_audiofilter/audioeng_toolame.h"
#include "gui_action.hxx"
/*
  	Ugly should get ride of it. Temporary fix.
*/
#include "avi_vars.h"
#include "audioeng_toolame.h"
#include "ADM_dialog/DIA_working.h"
#include "audioeng_piper.h"
 extern int audioMP3mode, audioMP3bitrate ;

 uint8_t audioPiper2Lame(char *file, AVDMProcessAudioStream *in)
 {
 	AVDMProcessAudio_TooLame *out;
    	uint32_t total=0,pushed=0;
	uint32_t max;


    out=new AVDMProcessAudio_TooLame(in);
    max=in->getLength();
    if(out->init(file,audioMP3mode,audioMP3bitrate))
      {
      		DIA_working *working=new DIA_working("Toolame");
    		while(out->push(&pushed))
		{
			total+=pushed;
			if(working->update(total,max)) break;
			//printf("\n total : %lu",total);
		};
		delete working;
	}
   else
	 GUI_Alert("Error running toolame!");

    delete out;
   return 1;

 }

 uint8_t audioPiperLame(char *file, AVDMProcessAudioStream *in,char *extra)
 {
 	AVDMProcessAudio_ExLame *out;
    	uint32_t total=0,pushed=0;
	uint32_t max;

   if((!extra) || !strlen(extra))
   {
	return 0;
   }

    out=new AVDMProcessAudio_ExLame(in);
    max=in->getLength();
    if(out->init(file,extra))
      {
      		DIA_working *working=new DIA_working("Lame");
    		while(out->push(&pushed))
		{
			total+=pushed;
			if(working->update(total,max)) break;
			//printf("\n total : %lu",total);
		};
		delete working;
	}
   else
	 GUI_Alert("Error running Lame!");

    delete out;
   return 1;

 }

uint8_t audioPipeTo(char *file, AVDMProcessAudioStream *in,char *cmd, char *param)
{

 	UNUSED_ARG(file);
	UNUSED_ARG(in);
	UNUSED_ARG(cmd);
	UNUSED_ARG(param);
	return 0;
}
