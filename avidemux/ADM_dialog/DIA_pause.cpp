//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ADM_assert.h" 
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

# include <config.h>

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

static GtkWidget	*create_dialog1 (void);
//	Return 1 if resume, 0 if abort
//
uint8_t DIA_Paused( void )
{
	uint8_t ret=2;
	GtkWidget *dialog;
	
	while(ret==2)
	{
		dialog=create_dialog1();
		  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(button1), GTK_RESPONSE_CANCEL);
		  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(button2), GTK_RESPONSE_OK);
		//gtk_transient(dialog);
		int i=gtk_dialog_run(GTK_DIALOG(dialog));
		switch(i)
		{
			case GTK_RESPONSE_OK:
				ret=0;	// abort
				break;
			case GTK_RESPONSE_CANCEL:
				ret=1;	// resume
				break;
			default:
				ret=2;	// continnue;
				break;
		}
		gtk_widget_destroy(dialog);
	};
	return ret;
}


GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *button1;
  GtkWidget *button2;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Paused"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  image1 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, TRUE, 0);

  label1 = gtk_label_new (_("Encoding is paused.\nYou can either resume \nor Abort it."));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  button1 = gtk_button_new_with_mnemonic (_("Resume"));
  gtk_widget_show (button1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), button1, 0);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  button2 = gtk_button_new_with_mnemonic (_("Abort"));
  gtk_widget_show (button2);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), button2, 0);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, button1, "button1");
  GLADE_HOOKUP_OBJECT (dialog1, button2, "button2");

  gtk_widget_grab_focus (button1);
  gtk_widget_grab_default (button1);
  return dialog1;
}

