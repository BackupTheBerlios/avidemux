#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <config.h>
#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"



#include "ADM_gui/GUI_mux.h"

static GtkWidget	*create_dialog1 (void);

#define ADJ_SET(widget_name,value) gtk_adjustment_set_value( GTK_ADJUSTMENT(gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (WID(widget_name)))),value)
#define ADJ_GET(widget_name) (int)gtk_adjustment_get_value( GTK_ADJUSTMENT(gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (WID(widget_name)))))
#define WID(x) lookup_widget(dialog,#x)

uint8_t  DIA_setUserMuxParam( int *mode, int *param, int *muxsize)
{

  GtkWidget *dialog;

	// gchar *str;
	// char string[200];
	int ret=0;

                dialog=create_dialog1();
		gtk_transient(dialog);
                switch(*mode)
                	{
                   	case MUX_REGULAR:
					RADIO_SET(radioEvery,1);

		                    	break;
                   	case MUX_N_FRAMES:
					RADIO_SET(radioN,1);
					ADJ_SET(spinFrame,*param);
		                    	break;
                	case MUX_N_BYTES:
					ADJ_SET(spinSize,*param);
					RADIO_SET(radioBytes,1);
         				break  ;
               		default:
                 				assert(0);
                     }
		ADJ_SET(spinbutton3,*muxsize);

             if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
		{
				int r; // ,value=0;
				ret=1;
				r=RADIO_GET(radioEvery)+(2*RADIO_GET(radioN))+(4*RADIO_GET(radioBytes));
				printf("\n r: %d \n",r);
				switch(r)
				{
					case 1: // every
							*mode=MUX_REGULAR;
							break;
					case 2: // every N
							*mode=MUX_N_FRAMES;
							*param=ADJ_GET(spinFrame);
							break;
					case 4: // byte
							*mode=MUX_N_BYTES;
							*param=ADJ_GET(spinSize);
							break;
					default:
							assert(0);
							break;
				}

	        	        	*muxsize=ADJ_GET(spinbutton3);
            }
	gtk_widget_destroy(dialog);
	 return ret;
}


GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *table1;
  GtkWidget *radioEvery;
  GSList *radioEvery_group = NULL;
  GtkWidget *radioN;
  GtkWidget *radioBytes;
  GtkWidget *hseparator1;
  GtkObject *spinFrame_adj;
  GtkWidget *spinFrame;
  GtkObject *spinSize_adj;
  GtkWidget *spinSize;
  GtkWidget *vbox2;
  GtkWidget *label2;
  GtkObject *spinbutton3_adj;
  GtkWidget *spinbutton3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Avi Muxer Options"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Mux audio every"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  radioEvery = gtk_radio_button_new_with_mnemonic (NULL, _("Every Video frame"));
  gtk_widget_show (radioEvery);
  gtk_table_attach (GTK_TABLE (table1), radioEvery, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioEvery), radioEvery_group);
  radioEvery_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioEvery));

  radioN = gtk_radio_button_new_with_mnemonic (NULL, _("Every N video frames"));
  gtk_widget_show (radioN);
  gtk_table_attach (GTK_TABLE (table1), radioN, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioN), radioEvery_group);
  radioEvery_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioN));

  radioBytes = gtk_radio_button_new_with_mnemonic (NULL, _("Split in packet of size"));
  gtk_widget_show (radioBytes);
  gtk_table_attach (GTK_TABLE (table1), radioBytes, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioBytes), radioEvery_group);
  radioEvery_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioBytes));

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_table_attach (GTK_TABLE (table1), hseparator1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  spinFrame_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinFrame_adj), 1, 0);
  gtk_widget_show (spinFrame);
  gtk_table_attach (GTK_TABLE (table1), spinFrame, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinFrame), TRUE);

  spinSize_adj = gtk_adjustment_new (2048, 0, 100000, 1, 10, 10);
  spinSize = gtk_spin_button_new (GTK_ADJUSTMENT (spinSize_adj), 1, 0);
  gtk_widget_show (spinSize);
  gtk_table_attach (GTK_TABLE (table1), spinSize, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinSize), TRUE);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Autosplit file every N megabytes"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox2), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  spinbutton3_adj = gtk_adjustment_new (10, 10, 4095, 1, 10, 10);
  spinbutton3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton3_adj), 1, 0);
  gtk_widget_show (spinbutton3);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton3, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton3), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, radioEvery, "radioEvery");
  GLADE_HOOKUP_OBJECT (dialog1, radioN, "radioN");
  GLADE_HOOKUP_OBJECT (dialog1, radioBytes, "radioBytes");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (dialog1, spinFrame, "spinFrame");
  GLADE_HOOKUP_OBJECT (dialog1, spinSize, "spinSize");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton3, "spinbutton3");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

