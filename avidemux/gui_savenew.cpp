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
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "avi_vars.h"


#include "avi_vars.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h"

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
 int A_SaveUnpackedVop( char *name);

int A_Save( char *name)
{
uint32_t end;
	// depending on the type we save a avi, a mpeg or a XVCD
	CodecFamilty family;
	family= videoCodecGetFamily();
	// in case of copy mode, we stick to avi file format
	if(!videoProcessMode)
		family=CodecFamilyAVI;
	printf("**saving:**\n");
	// Check if we need to do a sanity B frame check
	if(!videoProcessMode)
	{	
		uint32_t pb;
		end=avifileinfo->nb_frames;
		// if the last frame is the last frame (!)
		// we add one to keep it, else we systematically skip
		// the last frame
		if(frameEnd==end-1) end=frameEnd+1;
		else
			end=frameEnd;
		if(!video_body->sanityCheckRef(frameStart,end,&pb))
		{
			if(pb)
			{
				GUI_Alert("The video starts/ends by lonely B frame\nPlease remove them");
				return 0;
			}
			if(!GUI_Question("Warning !\n Bframe has lost its reference frame\nContinue ?"))
				return 0;
		}
	
	}
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
					ADM_assert(0);
					return 0;
	}
	getFirstVideoFilter(0,avifileinfo->nb_frames);
	return 1;
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
//___________________________________
int A_SaveUnpackedVop( char *name)
{
  aviInfo info;
GenericAviSave	*nw;

	video_body->getVideoInfo(&info);
	if( !isMpeg4Compatible(  info.fcc))
	{
		GUI_Alert("This cannot have packed vop!");
		return 0;
        }
	//
	nw=new   GenericAviSaveCopy(1);
	if(!nw->saveAvi(name))
	{
        	GUI_Alert(" AVI NOT saved");
	}
	else
        	GUI_Alert(" Saved successfully");

	delete nw;
	return 1;
}
//___________________________________
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
				int value=4;;
	 			 if( ! GUI_getIntegerValue(&value, 2, 31, "Q Factor (set 4)"))
	  					return ;

                      		nw=new   GenericAviSaveSmart(value);
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
