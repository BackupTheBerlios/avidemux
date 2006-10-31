/***************************************************************************
                          ADM_guiBitrate  -  description
                             -------------------

	Simple bitrate analyzer

    begin                : Mon Aug 31 2003
    copyright            : (C) 2003 by mean
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

#include <gtk/gtk.h>
    
#include "ADM_lavcodec.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h"
#include "ADM_toolkit/bitmap.h"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
//#include "ADM_gui/GUI_mux.h"
#include "oplug_mpegFF/oplug_vcdff.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "prefs.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_colorspace/colorspace.h"
#include "ADM_audiodevice/audio_out.h"

#include "ADM_dialog/DIA_busy.h"
#include "ADM_dialog/DIA_working.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"

#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_codecs/ADM_ffmpeg.h"

#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit_gtk.h"

static GtkWidget	*create_dialog1 (uint32_t max, float *value);


void GUI_displayBitrate( void )
{

  int32_t total,max;
 uint32_t len,idx,nb_frame,k,changed;
 uint32_t round,sum,average[60];;

 float f;

 float display[20];

	// 1st compute the total
	total=0;
	max=0;
	if(!(frameEnd>frameStart))
	{
          GUI_Error_HIG(_("No data"), NULL);
		return ;
	}

	round=((avifileinfo->fps1000+990)/1000);
	if(!round) return;

	video_body->getFrameSize (frameStart, &len);
	for(k=0;k<round;k++) average[k]=0;

	sum=0;
	changed=0;

	// 1 st pass, compute max
	for( k=frameStart;k<frameEnd;k++)
	{
 		video_body->getFrameSize (k, &len);
		sum-=average[changed];
		average[changed]=len;
		sum+=average[changed];
		//printf("Frame %lu Br: %lu\n",k,(sum*8)/1000);
		if(sum>max) max=sum;
		changed++;
		changed%=round;
	}
#define ROUNDUP 20000
	max=(max+ROUNDUP -1)/ROUNDUP;
	max=max*ROUNDUP;

	printf("\n Total : %lu bytes, max %d bytes round %d\n",total,max,round);
	if(!max)
	{
          GUI_Error_HIG(_("No data"), NULL);
		return ;
	}
	nb_frame=frameEnd-frameStart;
	// and now build an histogram
	for(k=0;k<20;k++) display[k]=0;

	video_body->getFrameSize (frameStart, &len);
	for(k=0;k<round;k++) average[k]=0;

	sum=0;
	changed=0;


	for(uint32_t k=frameStart;k<frameEnd;k++)
	{
		video_body->getFrameSize (k, &len);
		// which case ?
		sum-=average[changed];
		average[changed]=len;
		sum+=average[changed];
		changed++;
		changed%=round;

		f=sum;
		f=f/max;
		f=f*20.;
		idx=(uint32_t )floor(f+0.5);
	//	printf("idx : %d\n",idx);
		if(idx>19) idx=19;
		display[idx]++;
	}


	for(k=0;k<20;k++) display[k]=(100*display[k])/nb_frame;
	for(k=0;k<20;k++)
	{
		printf(" %d , %04d -- %04d percent -> %02.1f\n",k,(max*k*8)/20000,(8*max*(k+1))/20000,display[k]);

	}
	GtkWidget *dialog=create_dialog1((max*8)/1000,display);
	gtk_register_dialog(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
}


GtkWidget	*create_dialog1 (uint32_t max, float *value)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *progressbar1;
  GtkWidget *progressbar2;
  GtkWidget *dialog_action_area1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "Bitrate Histogram");

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (20, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);


/*----*/
	char string[200];
	float p;

	for(int j=0;j<20;j++)
	{

		sprintf(string,"%05d kbps :",(max*j)/20);
 		label1 = gtk_label_new (string);
  		gtk_widget_show (label1);
 		gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 20-j-1, 20-j,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  		gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  		gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

		gtk_widget_set_usize(label1,100,-1);

		progressbar1 = gtk_progress_bar_new ();
  		gtk_widget_show (progressbar1);
  		gtk_table_attach (GTK_TABLE (table1), progressbar1, 1, 2, 20-j-1, 20-j,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

			p=value[j];
			p=p/100.;	
       			gtk_progress_set_percentage(GTK_PROGRESS(progressbar1),(gfloat)p);

	}

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}



// EOF
