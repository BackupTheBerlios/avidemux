/***************************************************************************
                          gui_navigate.cpp  -  description
                             -------------------

            GUI Part of get next frame, previous key frame, any frame etc...

    begin                : Fri Apr 12 2002
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


#  include <config.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"
#include "prototype.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
#include "gui_action.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_dialog/DIA_working.h"
//____________________________________

void GUI_NextFrame(void)
{
//    uint8_t *ptr;
uint32_t flags;
    if (playing)
	return;

    if( avifileinfo && curframe + 1 == avifileinfo->nb_frames )
	return;

    if (avifileinfo)
      {
        if( !video_body->getUncompressedFrame(curframe + 1,rdr_decomp_buffer,&flags))
        	{
            	GUI_Alert("Decompressing Error NF");
           }
           else
           {
		curframe++;
               	if(mode_preview)
          			editorUpdatePreview( curframe) ;
						
		renderUpdateImage(rdr_decomp_buffer);
     		 update_status_bar(flags);

		UI_purge();
	  	}
     }
}


//_____________________________________________________________
void GUI_NextKeyFrame(void)
{

    uint32_t f;
//    uint32_t flags;


    if (playing)
		return;

   f=curframe;
    if (avifileinfo)
      {
		  if( !video_body->getUncompressedFrameNKF(&f ,rdr_decomp_buffer))
        	{
            //	GUI_Alert("Decompressing Error");
           }
           else
           {
             		curframe=f;
			renderUpdateImage(rdr_decomp_buffer);
              		if(mode_preview)
         				editorUpdatePreview( curframe)     ;

		       update_status_bar(AVI_KEY_FRAME);
		       UI_purge();

	  }
		
  }

}

//**********************************************************************

uint8_t  countLightPixels(int darkness)
{

    uint32_t width = avifileinfo->width;
    uint32_t height = avifileinfo->height;
    uint32_t sz = width * height ;
    const int maxnonb=(width* height)>>8;

    uint8_t *buff;

    int cnt4=0;

    buff=rdr_decomp_buffer+ sz;

    while(--buff>rdr_decomp_buffer)
    {
      if(*buff > darkness )
	cnt4++;
	if(cnt4>=maxnonb)
	  return(1);
    }

    return(0);
}

//**********************************************************************

void GUI_NextPrevBlackFrame(int dir)
{

    uint32_t f;
    uint32_t flags;

    if (playing)
		return;
    if (! avifileinfo)
       return;

   const int darkness=40;

   DIA_working *work=new DIA_working("Seeking");
   while(1)
   {

   	f=curframe+dir;
   	if(work->update(1)) break;

	if((f==0 && dir==-1)|| (f==avifileinfo->nb_frames-1&&dir==1)) break;

     if( !video_body->getUncompressedFrame(f ,rdr_decomp_buffer,&flags))
       {
       		curframe=0;
		video_body->getUncompressedFrame(0 ,rdr_decomp_buffer);
		break;
       }

     curframe=f;

     if(!countLightPixels(darkness)) break;

       update_status_bar();

   }
   	delete work;
       renderUpdateImage(rdr_decomp_buffer);
       if(mode_preview)
	 editorUpdatePreview( curframe)     ;
       update_status_bar(flags);
}

//**********************************************************************

//
// Go to the neareste frame near frame
// we take the previous one to simplify
//_____________________________________________________________
void GUI_GoToKFrame(uint32_t frame)
{
   uint32_t old=curframe;
//  uint32_t flags;

        if (playing)
				return;

       if (avifileinfo)
      {
       // curframe=frame;
       //
     	curframe=frame;
	      if( !video_body->getUncompressedFramePKF(&curframe ,rdr_decomp_buffer))
        	{
           	curframe=old;
            	GUI_Alert("Decompressing Error KF");
           	}
          	 else
         	  {
                	renderUpdateImage(rdr_decomp_buffer);
                        if(mode_preview)
          				editorUpdatePreview( curframe)   ;

             		update_status_bar(AVI_KEY_FRAME);
			UI_purge();

	  	}
     }
}

//_____________________________________________________________
int GUI_GoToFrame(uint32_t frame)
{
uint32_t flags;

	if (playing)
		return 0;

      if (!avifileinfo)
      		return 0;
      

	if( !video_body->getUncompressedFrame(frame ,rdr_decomp_buffer,&flags))
	{
		GUI_Alert("Decompressing Error GF");
		return 0;
	}

	curframe = frame;
	renderUpdateImage(rdr_decomp_buffer);

	if(mode_preview)
		editorUpdatePreview( curframe);

	update_status_bar(flags);
	UI_purge();
	
    	return 1;

}



void GUI_PreviousKeyFrame(void)
{

    	uint32_t f;
//	uint32_t flags;

    if (playing)
	return;

    f=curframe;
    if (avifileinfo)
      {
	  if( !video_body->getUncompressedFramePKF(&f ,rdr_decomp_buffer))
          {
            	//GUI_Alert("Decompressing Error");
           }
           else
           {
             	curframe=f;
   		renderUpdateImage(rdr_decomp_buffer);

              	if(mode_preview)
          				editorUpdatePreview( curframe);
              	update_status_bar(AVI_KEY_FRAME);
		UI_purge();

	  }
   }
};

