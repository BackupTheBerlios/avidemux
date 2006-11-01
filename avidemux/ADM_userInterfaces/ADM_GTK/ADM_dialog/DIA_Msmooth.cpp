//
// C++ Implementation: DIA_Msmooth
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
// C++ Implementation: DIA_decimate
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
// C++ Implementation: DIA_dectel
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

# include <math.h>

#include "default.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"

#include "ADM_video/ADM_vidMSmooth_param.h"
static GtkWidget	*create_dialog1 (void);
#define SPIN_GET(x,y) {param->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)param->y) ;}
#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}	
uint8_t DIA_getMSmooth(MSMOOTH_PARAM *param);
uint8_t DIA_getMSmooth(MSMOOTH_PARAM *param)
{
GtkWidget *dialog;
int ret=0;
	dialog=create_dialog1();
	
	// Update
	
	CHECK_SET(checkbuttonHQ,highq);
	CHECK_SET(checkbuttonShowMask,showmask);
	
	SPIN_SET(spinbuttonThresh,threshold);
	SPIN_SET(spinbuttonStrength,strength);
	gtk_register_dialog(dialog);
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		CHECK_GET(checkbuttonHQ,highq);
		CHECK_GET(checkbuttonShowMask,showmask);
		SPIN_GET(spinbuttonThresh,threshold);
		SPIN_GET(spinbuttonStrength,strength);		
		ret=1;
	
	}
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	return ret;

}

//__________________________________

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *checkbuttonHQ;
  GtkWidget *checkbuttonShowMask;
  GtkObject *spinbuttonThresh_adj;
  GtkWidget *spinbuttonThresh;
  GtkObject *spinbuttonStrength_adj;
  GtkWidget *spinbuttonStrength;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("MSmooth settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("High Quality :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Show Mask :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Threshold :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Filter Strength :"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  checkbuttonHQ = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonHQ);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonHQ, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonShowMask = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonShowMask);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonShowMask, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  spinbuttonThresh_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbuttonThresh = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonThresh_adj), 1, 0);
  gtk_widget_show (spinbuttonThresh);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonThresh, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonThresh), TRUE);

  spinbuttonStrength_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbuttonStrength = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonStrength_adj), 1, 0);
  gtk_widget_show (spinbuttonStrength);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonStrength, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonStrength), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonHQ, "checkbuttonHQ");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonShowMask, "checkbuttonShowMask");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonThresh, "spinbuttonThresh");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonStrength, "spinbuttonStrength");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

