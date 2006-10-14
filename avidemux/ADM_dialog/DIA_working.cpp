/***************************************************************************
                          DIA_working.cpp  -  description
                             -------------------
    begin                : Thu Apr 21 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr

	This class deals with the working window

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include "config.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_CLOCKnTIMELEFT
#include "ADM_toolkit/ADM_debug.h"

# include <config.h>
#include "DIA_working.h"

static GtkWidget	*create_dialog1 (void);
static void on_work_abort(GtkObject * object, gpointer user_data);
static gint on_destroy_abort(GtkObject * object, gpointer user_data);

void on_work_abort(GtkObject * object, gpointer user_data)
{
DIA_working *dial;
GtkWidget *dialog;

	UNUSED_ARG(object);
	dial=(DIA_working *)user_data;
	dialog=(GtkWidget *)dial->_priv;
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	dial->_priv=NULL;
	UI_purge();

};
gint on_destroy_abort(GtkObject * object, gpointer user_data)
{


	UNUSED_ARG(object);
	UNUSED_ARG(user_data);

/*DIA_working *dial;
GtkWidget *dialog;

	dial=(DIA_working *)user_data;
	dialog=(GtkWidget *)dial->_priv;
	dial->_priv=NULL;
	printf("destroyed\n");
	gtk_purge();
	*/
	return TRUE;

};



DIA_working::DIA_working( void )
{
	GtkWidget *dialog;

	dialog=create_dialog1();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);
	_priv=(void *)_priv;
	postCtor();
}
DIA_working::DIA_working( const char *title )
{
GtkWidget *dialog;

	dialog=create_dialog1();
	//gtk_transient(dialog);
        gtk_register_dialog(dialog);
	_priv=(void *)dialog;;
	gtk_window_set_title (GTK_WINDOW (dialog), title);
	postCtor();
}
void DIA_working :: postCtor( void )
{
GtkWidget 	*dialog;

		dialog=(GtkWidget *)_priv;
		//gtk_window_set_modal(GTK_WINDOW(dialog), 1);
		gtk_signal_connect(GTK_OBJECT(lookup_widget(dialog,"closebutton1")), "clicked",
		       GTK_SIGNAL_FUNC(on_work_abort), (void *) this);

		       // somehow prevent the window from being erased..
		gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		       GTK_SIGNAL_FUNC(on_destroy_abort), (void *) this);




		gtk_widget_show(dialog);
		UI_purge();
		lastper=0;
		_nextUpdate=0;
}
uint8_t DIA_working::update(uint32_t percent)
{
	#define  GUI_UPDATE_RATE 1000

                if(!_priv) return 1;
                if(!percent) return 0;
                if(percent==lastper)
                {
                   UI_purge();
                   return 0;
                }
                aprintf("DIA_working::update(%lu) called\n", percent);
                elapsed=_clock.getElapsedMS();
                if(elapsed<_nextUpdate) 
                {
                  UI_purge();
                  return 0;
                }
                _nextUpdate=elapsed+1000;
                lastper=percent;
  
        GtkWidget *dialog;
        dialog=(GtkWidget *)_priv;

		//
		// 100/totalMS=percent/elapsed
		// totalM=100*elapsed/percent

		double f;
		f=100.;
		f*=elapsed;
		f/=percent;

		f-=elapsed;
		f/=1000;

		uint32_t sectogo=(uint32_t)floor(f);

	char b[300];
   		int  mm,ss;
    			mm=sectogo/60;
      			ss=sectogo%60;
    			sprintf(b, " %d m %d s left", mm,ss);
			aprintf("DIA_working::update(%lu): new time left: %s\n"
				"\tf=%f sectogo=%lu\n", percent,b,f,sectogo);
    			gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label_time")), b);

		double p;
			p=percent;
			p=p/100.;
       			gtk_progress_set_percentage(GTK_PROGRESS(lookup_widget(dialog,"progressbar1")),(gfloat)p);

		UI_purge();
		return 0;


}
uint8_t DIA_working::update(uint32_t cur, uint32_t total)
{
		double d,n;
		uint32_t percent;
		if(!_priv) return 1;

		aprintf("DIA_working::update(uint32_t %lu,uint32_t %lu) called\n", cur, total);
		if(!total) return 0;

		d=total;
		n=cur;
		n=n*100.;

		n=n/d;

		percent=(uint32_t )floor(n);
		return update(percent);

}

uint8_t DIA_working::isAlive (void )
{
	if(!_priv) return 0;
	return 1;
}

DIA_working::~DIA_working()
{
	closeDialog();
        
}

void DIA_working::closeDialog( void )
{
	GtkWidget *dialog;

	dialog=(GtkWidget *)_priv;
	if(dialog)
	{
                gtk_unregister_dialog(dialog);
		gtk_widget_destroy(dialog);
		_priv=NULL;
	}


}

//-------------------------------------------------------------
GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *label_time;
  GtkWidget *progressbar1;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("dialog1"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Time left :"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  label_time = gtk_label_new (_("0:0:0"));
  gtk_widget_show (label_time);
  gtk_box_pack_start (GTK_BOX (hbox1), label_time, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label_time), GTK_JUSTIFY_LEFT);

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), progressbar1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label_time, "label_time");
  GLADE_HOOKUP_OBJECT (dialog1, progressbar1, "progressbar1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  return dialog1;
}

