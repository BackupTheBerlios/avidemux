//
// C++ Implementation: DIA_audiodevice
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <config.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_audiodevice/ADM_audiodevice.h"

static GtkWidget	*create_dialog1 (void);

void DIA_getAudioDevice(AUDIO_DEVICE *id)
{
GtkWidget *dialog;
GtkWidget *wids[10];
GtkWidget *menu;
uint32_t k=sizeof(audioDeviceList)/sizeof(DEVICELIST)-1;
	dialog=create_dialog1();
	// Build dialog
	menu=gtk_menu_new ();
	
	for(uint32_t i=0;i<sizeof(audioDeviceList)/sizeof(DEVICELIST);i++)
	{
		if(audioDeviceList[i].id==*id)
			{
				k=i;
			}
		wids[i] = gtk_menu_item_new_with_mnemonic ( audioDeviceList[i].name);
		gtk_widget_show (wids[i]);
		gtk_container_add (GTK_CONTAINER (menu), wids[i]);
	}		
	 gtk_option_menu_set_menu (GTK_OPTION_MENU (WID(optionmenu1)), menu);
	 gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu1)), k);
	 
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
		{
			uint32_t j;
			j=getRangeInMenu(WID(optionmenu1));
			*id=audioDeviceList[j].id;
		}
	gtk_widget_destroy(dialog);
}



GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *optionmenu1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
   

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Select Audio Device"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), optionmenu1, FALSE, FALSE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

