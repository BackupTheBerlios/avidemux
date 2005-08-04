
#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "default.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"




//#include "avi_vars.h"

#include "ADM_assert.h" 

#include "ADM_gui2/support.h"



#include "oplug_avi/GUI_mux.h"

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
					RADIO_SET(radioN,1);
					ADJ_SET(spinFrame,1);
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
                 				ADM_assert(0);
                     }
		ADJ_SET(spinbutton3,*muxsize);

             if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
		{
				int r; // ,value=0;
				ret=1;
				r=(RADIO_GET(radioN))+(2*RADIO_GET(radioBytes));
				printf("\n r: %d \n",r);
				switch(r)
				{
					case 1: // every or N
							*param=ADJ_GET(spinFrame);
							if(*param==1)
								*mode=MUX_REGULAR;
							else
								*mode=MUX_N_FRAMES;
							break;
					case 2: // byte
							*mode=MUX_N_BYTES;
							*param=ADJ_GET(spinSize);
							break;
					default:
							ADM_assert(0);
							break;
				}

	        	        	*muxsize=ADJ_GET(spinbutton3);
            }
	gtk_widget_destroy(dialog);
	 return ret;
}

GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *radioN;
  GSList *radioN_group = NULL;
  GtkWidget *radioBytes;
  GtkObject *spinFrame_adj;
  GtkWidget *spinFrame;
  GtkObject *spinSize_adj;
  GtkWidget *spinSize;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkObject *spinbutton3_adj;
  GtkWidget *spinbutton3;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog), _("AVI muxer options"));
  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 5);

  radioN = gtk_radio_button_new_with_mnemonic (NULL, _("Mux audio every"));
  gtk_widget_show (radioN);
  gtk_table_attach (GTK_TABLE (table1), radioN, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioN), radioN_group);
  radioN_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioN));

  radioBytes = gtk_radio_button_new_with_mnemonic (NULL, _("Split in packets of"));
  gtk_widget_show (radioBytes);
  gtk_table_attach (GTK_TABLE (table1), radioBytes, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioBytes), radioN_group);
  radioN_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioBytes));

  spinFrame_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinFrame_adj), 1, 0);
  gtk_widget_show (spinFrame);
  gtk_table_attach (GTK_TABLE (table1), spinFrame, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  spinSize_adj = gtk_adjustment_new (2048, 0, 100000, 1, 10, 10);
  spinSize = gtk_spin_button_new (GTK_ADJUSTMENT (spinSize_adj), 1, 0);
  gtk_widget_show (spinSize);
  gtk_table_attach (GTK_TABLE (table1), spinSize, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label1 = gtk_label_new (_("video frame(s)"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("bytes"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Autosplit file every"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  spinbutton3_adj = gtk_adjustment_new (4090, 10, 4095, 1, 10, 10);
  spinbutton3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton3_adj), 1, 0);
  gtk_widget_show (spinbutton3);
  gtk_table_attach (GTK_TABLE (table1), spinbutton3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label4 = gtk_label_new (_("megabytes"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG (dialog)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog, "dialog");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog, radioN, "radioN");
  GLADE_HOOKUP_OBJECT (dialog, radioBytes, "radioBytes");
  GLADE_HOOKUP_OBJECT (dialog, spinFrame, "spinFrame");
  GLADE_HOOKUP_OBJECT (dialog, spinSize, "spinSize");
  GLADE_HOOKUP_OBJECT (dialog, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog, spinbutton3, "spinbutton3");
  GLADE_HOOKUP_OBJECT (dialog, label4, "label4");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog, okbutton1, "okbutton1");

  return dialog;
}

