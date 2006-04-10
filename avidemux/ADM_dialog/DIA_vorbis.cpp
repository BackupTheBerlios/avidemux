
#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "ADM_assert.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_assert.h" 


#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_vorbis.h"


#define NB_BITRATE 11
static int BTR[] = {
		48,
		56,
		64,
		80,
    		96,
    		112,
    		128,
   		 160,
    		192,
   		 224,
		 384
};
  
static GtkWidget	*create_dialog1 (void);
  
void initVBR(GtkWidget *widget, GtkWidget *dialog)
{
	char string[50];
  	GtkWidget *fq;

	gtk_container_foreach(GTK_CONTAINER (WID(menu2)),(GtkCallback)gtk_widget_destroy, NULL);
	for (unsigned int i=0;i<=11;i++) {
		sprintf(string,"Quality %d",i-1);
		fq=gtk_menu_item_new_with_mnemonic (string);
		gtk_widget_show(fq);
		gtk_container_add (GTK_CONTAINER (WID(menu2)), fq);
	}
	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_bitrate)), 4);
}
			 
void initCBR(GtkWidget *widget, GtkWidget *dialog)
{
	char string[50];
  	GtkWidget *fq;

	gtk_container_foreach(GTK_CONTAINER (WID(menu2)),(GtkCallback)gtk_widget_destroy, NULL);
	for (unsigned int i=0;i<NB_BITRATE;i++) {
		sprintf(string,"%d kbits",BTR[i]);
		fq=gtk_menu_item_new_with_mnemonic (string);
		gtk_widget_show(fq);
		gtk_container_add (GTK_CONTAINER (WID(menu2)), fq);
	}
	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_bitrate)), 6);
}
			 
int DIA_getVorbisSettings(int *pbitrate, int *mode)
{
	GtkWidget *dialog;
	int ret=0;
	GtkWidget *Tmode[2];
	char *mode_name[2] = {"VBR", "CBR"};

	dialog=create_dialog1();
	gtk_register_dialog(dialog);

	// set bitrate
	if (*mode==0) {  //VBR
		initVBR(Tmode[0], dialog);
		gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_bitrate)), *pbitrate);
	} else {  //CBR
		initCBR(Tmode[1], dialog);
		for (unsigned int i=0;i<NB_BITRATE;i++)
			if (*pbitrate==BTR[i])
				gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_bitrate)), i);
	}

	for (int j=0;j<2;j++) {
		Tmode[j]=gtk_menu_item_new_with_mnemonic (mode_name[j]);
		gtk_widget_show (Tmode[j]);
		gtk_container_add (GTK_CONTAINER (WID(menu1)), Tmode[j]);
	}

	g_signal_connect_swapped (G_OBJECT (Tmode[0]), "activate",
                                      G_CALLBACK (initVBR), G_OBJECT(dialog));		
	g_signal_connect_swapped (G_OBJECT (Tmode[1]), "activate",
                                      G_CALLBACK (initCBR), G_OBJECT(dialog));		

	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_mode)), *mode);
	
	
	gtk_widget_show(dialog);
	
	if(GTK_RESPONSE_OK==gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		// Grab result
		int index;

		index=getRangeInMenu(WID(optionmenu_mode));
		*mode=index;
		
		index=getRangeInMenu(WID(optionmenu_bitrate));
		if (*mode==0) 
			*pbitrate=index;  //VBR
		else
			*pbitrate=BTR[index];  //CBR
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	return ret;	    
  }
  


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *vbox2;
  GtkWidget *optionmenu_mode;
  GtkWidget *label1;
  GtkWidget *optionmenu_bitrate;
  GtkWidget *menu1;
  GtkWidget *menu2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Vorbis settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);


  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame1), vbox2);


  optionmenu_mode = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_mode);
  gtk_box_pack_start (GTK_BOX (vbox2), optionmenu_mode, FALSE, FALSE, 0);

  menu1 = gtk_menu_new ();

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_mode), menu1);


  optionmenu_bitrate = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_bitrate);
  gtk_box_pack_start (GTK_BOX (vbox2), optionmenu_bitrate, FALSE, FALSE, 0);

  menu2 = gtk_menu_new ();

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_bitrate), menu2);


  
  label1 = gtk_label_new (_("<b>Rate</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu_bitrate, "optionmenu_bitrate");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu_mode, "optionmenu_mode");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

