/***************************************************************************
                          ADM_vidpreview.cpp  -  description
                             -------------------

       Previewer, we switch to RGB (only one Xv at a time)   and display the incoming
       YV12 buffer in a nice windowd

    begin                : Mon Mar 25 2002
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

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_colorspace/colorspace.h"
#include "prototype.h"
static GtkWidget	*create_dialog1 (void);

static void 			previewRender(void);
static gboolean		preview_exit(GtkButton * button, gpointer user_data);
static gboolean 		preview_exit_short (GtkWidget * widget,GdkEvent * event, gpointer user_data);

static uint8_t *rgb_render=NULL;
static GtkWidget *dialog=NULL;
static uint32_t 	uw, uh;
int 			lock;
uint8_t		needDestroy=0;
extern void UI_setPreviewToggleStatus(uint8_t s);

// Init previewer
//
uint8_t GUI_StillAlive( void )
{
 	if(dialog==NULL) return 0;
  	return 1;

}
void GUI_PreviewInit(uint32_t w , uint32_t h)
{
		if(rgb_render)
		{
			printf("\n Warning rgb render not null...\n");
			free(rgb_render);
		}
      ADM_assert(rgb_render=new uint8_t [w*h*4]);
       uw=w;
       uh=h;

       dialog=create_dialog1();
       gtk_widget_set_usize (lookup_widget(dialog,"drawingarea1"),w,h);

       // add callback for destroy
 	gtk_object_set_data_full(GTK_OBJECT(dialog),
			     "dialog",
			     dialog,
			     (GtkDestroyNotify) preview_exit_short);
	// add callback for button
	gtk_signal_connect(GTK_OBJECT(lookup_widget(dialog,"closebutton1")), "clicked",
                      GTK_SIGNAL_FUNC(preview_exit),                   (void *) NULL);

	// add callback for redraw
	gtk_signal_connect(GTK_OBJECT(lookup_widget(dialog,"drawingarea1")), "expose_event",
		       GTK_SIGNAL_FUNC(previewRender),
		       NULL);
       lock=0;
       needDestroy=1;
       gtk_widget_show(  dialog);
}
// return 1 if preview is still running
uint8_t  GUI_PreviewUpdate(uint8_t *data)
{
      if(  dialog)
       {

		// First convert YV12 to RGB
	       COL_yv12rgb( uw, uh,data, rgb_render);
	       previewRender();
	       if(lock==1)
	    	{
	    		dialog=NULL;
	    		return 0;
	    	}
	    	else
	    		return 1;
	    }
	    return 0;

}
uint8_t  GUI_PreviewRun(uint8_t *data)
{
      if(  dialog)
       {

		// First convert YV12 to RGB
	       	COL_yv12rgb( uw, uh,data, rgb_render);
	       	previewRender();
		// add a new handler
		#define WID(x) lookup_widget(dialog,#x)
	        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(okbutton1), GTK_RESPONSE_OK);
		gtk_dialog_run(GTK_DIALOG(dialog));
		return 1;

	    }
	    return 0;

}

void GUI_PreviewEnd(void)
{
 	if(dialog && needDestroy)
 		{
			//printf("+X\n");
 		 	gtk_widget_destroy(dialog);
			//printf("-X\n");

 		}
 	if(rgb_render)
 		{
 		 	delete [] rgb_render;
 		 	rgb_render=NULL;
 		}
	dialog=NULL;
  	needDestroy=0;
}
void previewRender(void)
{

if(!dialog) return;
GUI_RGBDisplay(rgb_render, uw, uh ,lookup_widget(dialog,"drawingarea1"));


}
// Called when the exit button is pushed
//
gboolean preview_exit(GtkButton * button, gpointer user_data)
{
    UNUSED_ARG(button);
    UNUSED_ARG(user_data);
//	printf("button\n");

 	if(lock==0)
       	{
	   		lock=-1;
			GUI_PreviewEnd();
            		UI_setPreviewToggleStatus( 0);
       	}
	return FALSE;
}
// Called when window is destroyed
gboolean preview_exit_short (GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    UNUSED_ARG(widget);
    UNUSED_ARG(event);
    UNUSED_ARG(user_data);
//	printf("destroyed\n");

    if(lock==0)
    {
    		lock=-1;
		//printf(">\n");
		needDestroy=0;
    		GUI_PreviewEnd();
		//printf("<\n");
       		UI_setPreviewToggleStatus( 0);
    }
	return FALSE;
}
GtkWidget		*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Preview"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), drawingarea1, TRUE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  return dialog1;
}

