/***************************************************************************
                         callback.c, gui that interact with user 
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr

	Some interface or handy functions 
	Link between gui_gtk.cpp and low level handlers

Mean.	

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#  include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>




#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdktypes.h>
#include <time.h>
#include <sys/time.h>


#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_assert.h"
#include "ADM_toolkit/filesel.h"
#include "prototype.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audio/audioex.h"
#include "gui_action.hxx"
#include "gtkgui.h"
#include "gui_action.hxx"
//#include "ADM_gui/GUI_vars.h"

#include "ADM_gui2/GUI_render.h"
#include "ADM_gui2/GUI_ui.h"


extern void HandleAction(Action act);

//static GtkWidget *wabout = NULL;
 int ignore_change = 0;

//static int16_t mp3lock;

//_____________________________________________________________
uint8_t stop_req = 0;



///
/// Update all  informations : current frame # and current time, total frame ...
///_____________________________________________________________

void  update_status_bar(ADMImage *frame)
{
    char text[80];
    double len;
//    int val;

    //    if(!guiReady) return ;
    text[0] = 0;
 
	UI_updateFrameCount( curframe);
	UI_updateTimeCount( curframe,avifileinfo->fps1000);
	UI_setFrameType(  frame->flags,frame->_Qp);
    // progress bar
    len = 100;
    len=len / (double) avifileinfo->nb_frames;
    len *= (double) curframe;

   

     UI_setScale(len);
   	
}

///
/// Update some informations : current frame # and current time
///_____________________________________________________________
void rebuild_status_bar(void)
{
    char text[80];
    double len;
//    int val;

    //    if(!guiReady) return ;
    text[0] = 0;
 
	UI_setFrameCount( curframe, avifileinfo->nb_frames);
	UI_setTimeCount( curframe, avifileinfo->nb_frames,avifileinfo->fps1000);
	
    // progress bar
    len = 100;
    len=len / (double) avifileinfo->nb_frames;
    len *= (double) curframe;



     UI_setScale(len);
}



//
//____________________________________________



///
///	Return the frame # corresponding to the position of the scale/slider
///	Rougth estimation in fact
///
uint32_t GUI_GetScale(void)
{

    double  percent;
    float tg;

    percent = UI_readScale();
    tg= avifileinfo->nb_frames * percent / 100.;
    
    return (uint32_t)floor(tg);;


}



#if 0

void GUI_UpdateAudioToggle(AudioSource nwsource)
{
    gchar text[300];
    switch (nwsource)
      {
      case AudioNone:
	  strcpy(text, "None");
	  break;
      case AudioAvi:
	  strcpy(text, "Avi");
	  break;
      case AudioMP3:
	  strcpy(text, "MP3");
	  break;
      case AudioWav:
	  strcpy(text, "Wav");
	  break;
      case AudioAC3:
	  strcpy(text, "AC3");
	  break;

      default:
	  ADM_assert(0);
      }
#warning fixme
//    gtk_entry_set_text((GtkEntry *) entry_faudioname, text);
}
#endif
// EOF
