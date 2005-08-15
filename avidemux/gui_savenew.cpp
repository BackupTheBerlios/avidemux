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
#include "ADM_gui2/GUI_ui.h"
#include "oplug_mpegFF/oplug_vcdff.h"

static uint8_t  A_SaveAudioNVideo(char *name);
 extern int A_SaveUnpackedVop( char *name);
 extern uint8_t ogmSave(char *name);
 extern uint8_t ADM_saveRaw(char *name);
 uint8_t A_SaveAudioDualAudio(char *name);
 extern uint8_t mpeg_passthrough(char *name,ADM_OUT_FORMAT format);

int A_Save( char *name)
{
uint32_t end;
int ret=0;
	// depending on the type we save a avi, a mpeg or a XVCD
	CodecFamilty family;
	family= videoCodecGetFamily();
	// in case of copy mode, we stick to avi file format
	if(!videoProcessMode())
	{
		family=CodecFamilyAVI;
		if( UI_GetCurrentFormat()==ADM_PS ||UI_GetCurrentFormat()==ADM_TS )  // exception
		{
			family=CodecFamilyMpeg;
		}
	}
        else
        {
                if(UI_GetCurrentFormat()==ADM_AVI_DUAL)
                {
                        GUI_Error_HIG("Dual audio can only be used in copy mode", "Select Copy as the video codec.");
                        return 0;
                }
        }
	printf("**saving:**\n");
	// Check if we need to do a sanity B frame check
	if(!videoProcessMode())
	{	
		uint32_t pb;
		end=avifileinfo->nb_frames;
		// if the last frame is the last frame (!)
		// we add one to keep it, else we systematically skip
		// the last frame
#if 0					
		if(frameEnd==end-1) end=frameEnd+1;
		else
			end=frameEnd;

		if(!video_body->sanityCheckRef(frameStart,end,&pb))
		{
			if(pb)
			{
				GUI_Error_HIG("Cannot save the file", "The video starts/ends with a lonely B-frame. Please remove it.");
				return 0;
			}
			if(!GUI_Question("Warning !\n Bframe has lost its reference frame\nContinue ?"))
				return 0;
		}
#endif
		// Alter frameEnd so that it is not a B frame	
		// as frameEnd -1 position	
		uint32_t tgt=frameEnd;;
		uint32_t flag=0,found=0;
		
		// need to do something ?
		if(frameEnd>frameStart)
		{
			tgt=frameEnd;
			if(tgt==end-1) tgt++;
			video_body->getFlags(tgt-1,&flag);
			if((frameEnd&AVI_B_FRAME))
			{
				printf("Last frame is a B frame, choosing better candidate\n");
				 // The last real one is not a I/P Frame
				 // Go forward or rewind
				 if(tgt<end-1)
				{	// Try next if possible
					video_body->getFlags(tgt,&flag);
					if(!(flag&AVI_B_FRAME))
					{
						printf("Taking next frame as last frame %lu\n",tgt+1);
				 		frameEnd=tgt+1;
				 		found=1;
					}
				}
				if(!found) // next frame not possible, rewind
				{
					if(tgt>=end-2) tgt=end-2;
					while(tgt>frameStart)
					{
						printf("Trying :%lu\n",tgt);
						video_body->getFlags(tgt,&flag);
						if(!(flag&AVI_B_FRAME))
						{
							printf("Taking previous frame as last frame %lu\n",tgt+1);
				 			frameEnd=tgt+1;
				 			found=1;
							break;
						}
						else tgt--;
					}
				}
				ADM_assert(found);
			}
		}
		
		
		
	}
        printf("Output format:%d\n",UI_GetCurrentFormat());
	switch(family)
	{
		case CodecFamilyAVI:
					printf(" AVI family\n");
					switch(UI_GetCurrentFormat())
					{
						case ADM_AVI:
								ret=A_SaveAudioNVideo(name);
								break;
						case ADM_OGM:
								ret=ogmSave(name);
								break;
						case ADM_ES:
								ret=ADM_saveRaw(name);
								break;
						case ADM_AVI_DUAL:
								ret=A_SaveAudioDualAudio(name);
								break;
						case ADM_AVI_UNP:
								ret=A_SaveUnpackedVop(name);
								break;
						default:
								GUI_Error_HIG("Incompatible output format", NULL);
					}
					break;
		case CodecFamilyMpeg:
					printf(" MPEG family\n");
					if(!videoProcessMode())
					{
						
						printf("Using pass through\n");
                                                switch(UI_GetCurrentFormat())
                                                {
                                                  case ADM_PS:
                                                  case ADM_TS:
						          ret=mpeg_passthrough(name,UI_GetCurrentFormat());
                                                          break;
                                                  default:
                                                        GUI_Error_HIG("Incompatible output format", NULL);
                                                }
					}
					else
					{
						switch(UI_GetCurrentFormat())
						{
							case ADM_PS:
							case ADM_ES:
                                                        case ADM_TS:
								ret=EncoderSaveMpeg(name);
								break;
							default:
								GUI_Error_HIG("Incompatible output format", NULL);
						}
					
					}
					break;
		case CodecFamilyXVCD:
                    switch(UI_GetCurrentFormat())
                    {
                        case ADM_TS:
                        case ADM_PS:
                        case ADM_ES:
                                ret=oplug_mpegff(name,UI_GetCurrentFormat());;
                                break;
                        default:
                            GUI_Error_HIG("Incompatible output format", NULL);
                    }
                    break;
                default:
                            ADM_assert(0);
                            return 0;
        }
        getFirstVideoFilter(0,avifileinfo->nb_frames);
        return ret;
}
uint8_t  A_SaveAudioDualAudio(char *inname)
{
GenericAviSaveCopyDualAudio *nw;
char *name;
uint8_t ret=0;

		if(! secondaudiostream)
		{
				 	GUI_Error_HIG("There is no second track", "Select a second audio track in the Audio menu.");
				  	return 0;
		}
		if(!inname)
			GUI_FileSelWrite("Select dual audio AVI to write", & name);
		else
			name=inname;
			
		if(!name) return 0;

     		nw=new   GenericAviSaveCopyDualAudio(secondaudiostream);
		ret=nw->saveAvi(name);
       		delete nw;
                return ret;

}
//___________________________________
int A_SaveUnpackedVop( char *name)
{
  aviInfo info;
GenericAviSave	*nw;

	video_body->getVideoInfo(&info);
	if( !isMpeg4Compatible(  info.fcc))
	{
		GUI_Error_HIG("This cannot have packed VOP", "It is not MPEG-4 video. File will not be saved.");
		return 0;
        }
	//
	nw=new   GenericAviSaveCopy(1);
	if(!nw->saveAvi(name))
	{
        	GUI_Error_HIG("AVI not saved", NULL);
	}
	else
        	GUI_Info_HIG("Done", "Successfully saved \"%s\".", GetFileName(name));

	delete nw;
	return 1;
}
//___________________________________
uint8_t  A_SaveAudioNVideo(char *name)
{
	 uint32_t needSmart=0,fl;
     GenericAviSave	*nw;
     aviInfo info;
     uint8_t ret=0;

     video_body->getVideoInfo(&info);

     printf("\n video process mode : %d",videoProcessMode());
     if (!videoProcessMode())
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
	  					return 0;

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
			GUI_Error_HIG("No encoder", "Cannot save in process mode.");
   			return ;
#endif

        }
     ret=nw->saveAvi(name);
     delete nw;

return ret;
}
