/***************************************************************************
                          ADM_edLoadSave.cpp  -  description
                             -------------------

	Save / load workbench

    begin                : Thu Feb 28 2002
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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "fourcc.h"
#include "avio.hxx"
#include "ADM_toolkit/ADM_quota.h"
#include <ADM_assert.h>
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_inpics/ADM_pics.h"

#include "ADM_nuv/ADM_nuv.h"
#include "ADM_h263/ADM_h263.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_assert.h"
#include "prefs.h"
#include "avi_vars.h"
// Ugly but sooo usefull
extern uint32_t frameStart,frameEnd;
static uint32_t edFrameStart,edFrameEnd;
uint8_t loadVideoCodecConf( char *name);
uint8_t saveVideoCodecConf( char *name);
const char *getCurrentContainerAsString(void);
//extern int audioMP3bitrate ;
extern const char              *audioSourceFromEnum(AudioSource src);
// Ugly, will have to clean it later

uint8_t ADM_Composer::getMarkers(uint32_t *start, uint32_t *end)
{
	if(_haveMarkers)
		{
			*start=edFrameStart;
			*end=edFrameEnd;		
		}
	else
		{
			*start=0;
			*end=_total_frames-1;
		}
	return 1;		
}
//______________________________________________
// Save the config, including name, segment etc...
//______________________________________________

uint8_t ADM_Composer::saveWorbench (char *name)
{
        GUI_Error_HIG("Unsupported", NULL);
        return 0;
}
/*______________________________________________
        Save the project as a script
______________________________________________*/
uint8_t ADM_Composer::saveAsScript (char *name, char *outputname)
{
const char *truefalse[]={"false","true"};
printf("\n **Saving script project **\n");
  char *    tmp;

  if (!_nb_segment)
    return 1;

  FILE *    fd;

  if( !(fd = qfopen (name, "wt")) ){
    fprintf(stderr,"\ncan't open script file \"%s\" for writing: %u (%s)\n",
                   name, errno, strerror(errno));
    return 0;
  }

// Save source and segment
//______________________________________________
  qfprintf( fd,"//AD  <- Needed to identify");
  qfprintf (fd, "//\n");
  qfprintf (fd, "//--automatically built--\n");
  qfprintf (fd, "//--Project: %s\n\n",name);

  qfprintf (fd, "var app = new Avidemux();\n");
  qfprintf (fd,"\n//** Video **\n");
  qfprintf (fd,"// %02ld videos source \n", _nb_video);
  char *nm;
  for (uint32_t i = 0; i < _nb_video; i++)
    {
        nm=cleanupPath(_videos[i]._aviheader->getMyName() );
        if(!i)
        {
                qfprintf (fd, "app.load(\"%s\");\n", nm);
        }
        else
        {
            qfprintf (fd, "app.append(\"%s\");\n", nm);
        }
        ADM_dealloc(nm);
    }
  
  qfprintf (fd,"//%02ld segments\n", _nb_segment);
  qfprintf (fd,"app.clearSegments();\n");
  
 

for (uint32_t i = 0; i < _nb_segment; i++)
    {
        uint32_t src,start,nb;
                src=_segments[i]._reference;
                start=_segments[i]._start_frame;
                nb=_segments[i]._nb_frames;
                qfprintf (fd, "app.addSegment(%lu,%lu,%lu);\n",src,start,nb);
    }
// Markers
//
        qfprintf(fd,"app.markerA=%d;\n",frameStart);
        qfprintf(fd,"app.markerB=%d;\n",frameEnd);
// postproc
//___________________________

        uint32_t pptype, ppstrength,ppswap;
                video_body->getPostProc( &pptype, &ppstrength, &ppswap);
                qfprintf(fd,"\n//** Postproc **\n");
                qfprintf(fd,"app.video.setPostProc(%d,%d,%d);\n",pptype,ppstrength,ppswap);


// Filter
//___________________________
        qfprintf(fd,"\n//** Filters **\n");
        filterSaveScriptJS(fd);
// Video codec
//___________________________
uint8_t  *extraData ;
uint32_t extraDataSize;
char *pth;
        qfprintf(fd,"\n//** Video Codec conf **\n");
        videoCodecGetConf(&extraDataSize,&extraData);
        
        pth= cleanupPath(name );
        qfprintf(fd,"app.video.codec(\"%s\",\"%s\",\"",videoCodecGetName(),videoCodecGetMode());
        ADM_dealloc(pth);
        // Now deal with extra data
        qfprintf(fd,"%d ",extraDataSize);
        if(extraDataSize)
        {
                for(int i=0;i<extraDataSize;i++)
                {
                        qfprintf(fd,"%02x ",extraData[i]);
                }

        }
        qfprintf(fd,"\");\n");
        
// Audio Source
//______________________________________________

// Audio
//______________________________________________

   uint32_t delay;
   
   qfprintf(fd,"\n//** Audio **\n");
   qfprintf(fd,"app.audio.reset();\n");

   // External audio ?
        char *audioName;
        AudioSource  source;

        source=getCurrentAudioSource(&audioName);
        if(!audioName) audioName="";

        if(source!=AudioAvi)
                fprintf(fd,"app.audio.load(%s,\"%s\");\n", audioSourceFromEnum(source),audioName); 

   qfprintf(fd,"app.audio.codec(\"%s\",%d);\n", audioCodecGetName(),audioGetBitrate()); 
   //qfprintf(fd,"app.audio.process=%s;\n",truefalse[audioProcessMode()]);
   qfprintf(fd,"app.audio.normalize=%s;\n",truefalse[audioGetNormalize()]);
   qfprintf(fd,"app.audio.delay=%d;\n",audioGetDelay());
   // Change mono2stereo ?
   switch(audioGetChannelConv())
        {
                case CHANNELCONV_2to1:  qfprintf(fd,"app.audio.mono2stereo=true;\n");break;
                case CHANNELCONV_1to2:  qfprintf(fd,"app.audio.stereo2mono=true;\n");break;
                case CHANNELCONV_NONE: ;break;
                default:ADM_assert(0);
        }     
   // Change fps ?
        switch(audioGetFpsConv())
        {
                case FILMCONV_NONE:      ;break;
                case FILMCONV_PAL2FILM:  qfprintf(fd,"app.audio.pal2film=true;\n");break;
                case FILMCONV_FILM2PAL:  qfprintf(fd,"app.audio.film2pal=true;\n");break;
                default:ADM_assert(0);
        }
   // Resampling
        switch(audioGetResampling())
        {
                case RESAMPLING_NONE:         ;break;
                case RESAMPLING_DOWNSAMPLING:  qfprintf(fd,"app.audio.downsample=true;\n");break;
                case RESAMPLING_CUSTOM:        qfprintf(fd,"app.audio.resample=%u;\n",audioGetResample());break;
                default:ADM_assert(0);
        }
  // container
        
  qfprintf(fd,"app.setContainer(\"%s\");\n",getCurrentContainerAsString());
  if(outputname)
  {
        qfprintf(fd,"app.save(\"%s\");\n",outputname);
  }
  qfprintf(fd,"//app.Exit();\n");
  qfprintf(fd,"\n//End of script\n");
  // All done
  qfclose (fd);
  
  return 1;


}

//______________________________________________
// Save the config, including name, segment etc...
//______________________________________________

uint8_t ADM_Composer::loadWorbench (char *name)
{
 GUI_Error_HIG("Old format project file", "No more supported.");
 return 0;
}
//EOF
