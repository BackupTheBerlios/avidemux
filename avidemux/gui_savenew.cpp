/***************************************************************************
                          gui_savenew.cpp  -  description
                             -------------------
    begin                : Fri May 3 2002
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
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>
#include "config.h"
#include "avi_vars.h"


#include "avi_vars.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_codecs/ADM_divxEncode.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "oplug_avi/op_savecopy.h"
#include "ADM_encoder/adm_encoder.h"

#include "oplug_avi/op_saveprocess.h"
#include "oplug_avi/op_savesmart.hxx"
#include "ADM_filter/video_filters.h"

#include "ADM_toolkit/filesel.h"
extern void oplug_mpegff(char *name);
static void  A_SaveAudioNVideo(char *name);


void A_Save( char *name)
{
	// depending on the type we save a avi, a mpeg or a XVCD
	CodecFamilty family;
	family= videoCodecGetFamily();
	// in case of copy mode, we stick to avi file format
	if(!videoProcessMode)
		family=CodecFamilyAVI;
	printf("**saving:**\n");
	switch(family)
	{
		case CodecFamilyAVI:
					printf(" Avi family\n");
					A_SaveAudioNVideo(name);
					break;
		case CodecFamilyMpeg:
					printf(" Mpeg family\n");
					EncoderSaveMpeg(name);
					break;
		case CodecFamilyXVCD:
					printf(" Xvcd family\n");
					oplug_mpegff(name);
					break;
		default:
					assert(0);
	}
}
void  A_SaveAudioDualAudio(void)
{
GenericAviSaveCopyDualAudio *nw;
char *name;

		if(! secondaudiostream)
		{
				 	GUI_Alert("Please select a second track in misc menu!");
				  	return;
		}

		GUI_FileSelWrite("Select dual audio AVI to write", & name);
		if(!name) return;

     		nw=new   GenericAviSaveCopyDualAudio(secondaudiostream);
		if(!nw->saveAvi(name))
	     	{
        		GUI_Alert(" AVI NOT saved");
       		}
       		else
        		GUI_Alert(" Saved successfully");
       		delete nw;

}

void  A_SaveAudioNVideo(char *name)
{
	 uint32_t needSmart=0,fl;
     GenericAviSave	*nw;
     aviInfo info;

     video_body->getVideoInfo(&info);

     printf("\n video process mode : %d",videoProcessMode);
     if (!videoProcessMode)
     {
       			if(video_body->isMultiSeg()) needSmart=1;
          			video_body->getFlags(frameStart,&fl);
             		if(!(fl&AVI_KEY_FRAME)) needSmart=1;

				if(needSmart) printf("\n probably need smart copy mode\n");
    	
    		if( !isMpeg4Compatible(  info.fcc)
#ifdef USE_FFMPEG
			  && !isMSMpeg4Compatible(info.fcc)

#endif      
      			)
             {
                	printf("\n not encodable, cancelling smart mode\n");
                 	needSmart=0;

               }
    			

#ifdef HAVE_ENCODER
			if(needSmart &&
   										GUI_Question("You may need smart copy.\n Enable it ?"))
             		{
                      nw=new   GenericAviSaveSmart;
                 	}
                  else
                  {
                           nw=new   GenericAviSaveCopy;
                    }



#else
     		nw=new   GenericAviSaveCopy;
#endif
       }
       else
       {
#ifdef HAVE_ENCODER
			printf("\n Process mode\n");
     		nw=new   GenericAviSaveProcess;
#else
			GUI_Alert("\n No encoder , cannot save in process mode");
   			return ;
#endif

        }
     if(!nw->saveAvi(name))
     {
        	GUI_Alert(" AVI NOT saved");
       }
       else
        	GUI_Alert(" Saved successfully");

       delete nw;


}
