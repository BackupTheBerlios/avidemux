/***************************************************************************
                          ADM_guiChromaShift.cpp  -  description
                             -------------------
    begin                : Sun Aug 24 2003
    copyright            : (C) 2002-2003 by mean
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

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>


#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#include <ADM_assert.h>


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>


#include "avi_vars.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "default.h"


#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_video/ADM_vidChromaShift.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_osSupport/ADM_debug.h"



#include "prototype.h"

#define SPIN_GET(x,y) {y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(dialog,#x))) ;\
				printf(#x":%d\n", y);}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(dialog,#x)),(gfloat)y) ; \
				printf(#x":%d\n", y);}


static uint8_t *video_working,*video_rgb;
static	ADMImage *video_src;
static int32_t shift_u, shift_v;
static uint32_t ww,hh;

static GtkWidget *dialog;
static GtkWidget *create_ChromaShift( void );
static uint8_t DIA_getChromaShift( void );
static void chromadraw (GtkWidget *dialog,uint32_t w,uint32_t h );
static void read( void );
static void upload ( void );
static gboolean gui_draw( void );
static gboolean gui_update( void );
static void update(void);

static ColYuvRgb    *rgbConv=NULL;

uint8_t DIA_getChromaShift( uint8_t *work, uint8_t *rgb,ADMImage *src,ColYuvRgb    *rgbC,uint32_t *u,uint32_t *v );
uint8_t DIA_getChromaShift( uint8_t *work, uint8_t *rgb,ADMImage *src,ColYuvRgb    *rgbC,
                              uint32_t *u,uint32_t *v )
{
	uint8_t ret=0;
        video_working=work;
        video_rgb=rgb;
        video_src=src;
        rgbConv=rgbC;
        shift_u=*u;
        shift_v=*v;
	dialog=create_ChromaShift();
	gtk_register_dialog(dialog);
	gtk_widget_set_usize(WID(drawingarea1), ww,hh);
	gtk_widget_show(dialog);
	upload();
	 gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
		       GTK_SIGNAL_FUNC(gui_draw),
		       NULL);
 	gtk_signal_connect (GTK_OBJECT(WID( spinbutton_U)), "value_changed",
                    GTK_SIGNAL_FUNC (gui_update),
                    NULL);
	gtk_signal_connect (GTK_OBJECT(WID( spinbutton_V)), "value_changed",
                    GTK_SIGNAL_FUNC (gui_update),
                    NULL);

	update();

	int response;
	while( (response=gtk_dialog_run(GTK_DIALOG(dialog)))==GTK_RESPONSE_APPLY)
	{
		read();
		update();
	}
	if(response==GTK_RESPONSE_OK)
        {
		read();
                *u=shift_u;
                *v=shift_v;
		ret=1;
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	dialog=NULL;
	return ret;

}
void read( void )
{
	SPIN_GET(spinbutton_U,shift_u);
	SPIN_GET(spinbutton_V,shift_v);
}
void upload ( void )
{
	SPIN_SET(spinbutton_U,shift_u);
	SPIN_SET(spinbutton_V,shift_v);
}
/*---------------------------------------------------------------------------
	Green-ify the displayed frame on cropped parts
*/
void update( void)
{

		// First copy Y
		memcpy(video_working,video_src->data,(ww*hh));
		// then shift u

		 ADMVideoChromaShift::shift(video_working+ww*hh,
		 						video_src->data+ww*hh, ww>>1,hh>>1,shift_u);
		ADMVideoChromaShift::shift(video_working+((5*ww*hh)>>2),
		 						video_src->data+((5*ww*hh)>>2), ww>>1,hh>>1,shift_v);
		if(shift_u)
			ADMVideoChromaShift::fixup(video_working,ww,hh,shift_u*2);
		if(shift_v)
			ADMVideoChromaShift::fixup(video_working,ww,hh,shift_v*2);

		//COL_yv12rgb(ww,hh,video_working,video_rgb);
		rgbConv->scale(video_working,video_rgb);
		chromadraw(dialog,ww,hh);
	//	printf("\n Updated..\n");
}
gboolean gui_update( void )
{
	//printf(" GUI !\n");
	read();
	update();
	return true;
}

/*---------------------------------------------------------------------------
	Actually draw the working frame on screen
*/
gboolean gui_draw( void )
{
	chromadraw(dialog,ww,hh);
	return true;
}
void chromadraw (GtkWidget *dialog,uint32_t w,uint32_t h )
{
	GtkWidget *draw;
	draw=WID(drawingarea1);
	GUI_RGBDisplay(video_rgb, w,h, (void *)draw);
}
/*----------------------------------------------------------------*/
GtkWidget		*create_ChromaShift (void)
{
  GtkWidget *ChromaShift;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkObject *spinbutton_U_adj;
  GtkWidget *spinbutton_U;
  GtkObject *spinbutton_V_adj;
  GtkWidget *spinbutton_V;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *applybutton1;
  GtkWidget *okbutton1;

  ChromaShift = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (ChromaShift), _("ChromaShift"));

  dialog_vbox1 = GTK_DIALOG (ChromaShift)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("U Shift :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("V Shift :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  spinbutton_U_adj = gtk_adjustment_new (0, -32, 32, 1, 10, 10);
  spinbutton_U = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_U_adj), 1, 0);
  gtk_widget_show (spinbutton_U);
  gtk_table_attach (GTK_TABLE (table1), spinbutton_U, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_U), TRUE);

  spinbutton_V_adj = gtk_adjustment_new (0, -32, 32, 1, 10, 10);
  spinbutton_V = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_V_adj), 1, 0);
  gtk_widget_show (spinbutton_V);
  gtk_table_attach (GTK_TABLE (table1), spinbutton_V, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_V), TRUE);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox1), drawingarea1, TRUE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea1, 512, 384);

  dialog_action_area1 = GTK_DIALOG (ChromaShift)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (ChromaShift), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  applybutton1 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (applybutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (ChromaShift), applybutton1, GTK_RESPONSE_APPLY);
  GTK_WIDGET_SET_FLAGS (applybutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (ChromaShift), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (ChromaShift, ChromaShift, "ChromaShift");
  GLADE_HOOKUP_OBJECT_NO_REF (ChromaShift, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (ChromaShift, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (ChromaShift, table1, "table1");
  GLADE_HOOKUP_OBJECT (ChromaShift, label1, "label1");
  GLADE_HOOKUP_OBJECT (ChromaShift, label2, "label2");
  GLADE_HOOKUP_OBJECT (ChromaShift, spinbutton_U, "spinbutton_U");
  GLADE_HOOKUP_OBJECT (ChromaShift, spinbutton_V, "spinbutton_V");
  GLADE_HOOKUP_OBJECT (ChromaShift, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (ChromaShift, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (ChromaShift, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (ChromaShift, applybutton1, "applybutton1");
  GLADE_HOOKUP_OBJECT (ChromaShift, okbutton1, "okbutton1");

  return ChromaShift;
}


