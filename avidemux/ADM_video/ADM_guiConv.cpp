/***************************************************************************
                          ADM_guiConv.cpp  -  description
                             -------------------
    begin                : Sun Nov 24 2002
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
#include <assert.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include "config.h"

#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#ifdef HAVE_ENCODER


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidConvolution.hxx"
#include "ADM_video/ADM_vidLargeMedian.h"

static GtkWidget	*create_dialog1 (void);

#define STOGGLE(x)     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lookup_widget(dialog,#x)), TRUE)
#define GTOGGLE(x)     gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (lookup_widget(dialog,#x)))

uint8_t ADMVideoLargeMedian::configure(AVDMGenericVideoStream * instream)
{
    UNUSED_ARG(instream);
    UNUSED_ARG(instream);
    GtkWidget *dialog;
    uint8_t ret=0;

	dialog=create_dialog1();

	if(_param->luma) STOGGLE(luma);
	if(_param->chroma) STOGGLE(chroma);

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

		_param->luma	= GTOGGLE(luma);
		_param->chroma= GTOGGLE(chroma);
		ret=1;

	}
	gtk_widget_destroy(dialog);
	return ret;

}




uint8_t AVDMFastVideoConvolution::configure(AVDMGenericVideoStream * instream)
{
    UNUSED_ARG(instream);
    GtkWidget *dialog;
    uint8_t ret=0;

	dialog=create_dialog1();

	if(_param->luma) STOGGLE(luma);
	if(_param->chroma) STOGGLE(chroma);

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

		_param->luma	= GTOGGLE(luma);
		_param->chroma= GTOGGLE(chroma);
		ret=1;

	}
	gtk_widget_destroy(dialog);
	return ret;

}

GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *luma;
  GtkWidget *chroma;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Convolution settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  luma = gtk_check_button_new_with_mnemonic (_("Luma"));
  gtk_widget_show (luma);
  gtk_box_pack_start (GTK_BOX (vbox1), luma, FALSE, FALSE, 0);

  chroma = gtk_check_button_new_with_mnemonic (_("Chroma"));
  gtk_widget_show (chroma);
  gtk_box_pack_start (GTK_BOX (vbox1), chroma, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, luma, "luma");
  GLADE_HOOKUP_OBJECT (dialog1, chroma, "chroma");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


#endif
