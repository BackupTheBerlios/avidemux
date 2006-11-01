
//
// C++ Implementation: DIA_Fade
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2002-206
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "default.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_video/ADM_vidFade_param.h"


#define SPIN_GET(x,y) {param->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)param->y) ;}
#define SGET(x) gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) 

#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}	

static GtkWidget        *create_dialog1 (void);
uint8_t DIA_fade(VIDFADE_PARAM *param)
{
  
  GtkWidget *dialog;
  int ret=0,done=0;
  dialog=create_dialog1();
        
    // Update
  SPIN_SET(spinbuttonStart,startFade);
  SPIN_SET(spinbuttonEnd,endFade);
  gtk_combo_box_set_active(GTK_COMBO_BOX(WID(comboboxinOut)),param->inOut);
  CHECK_SET(checkbuttonBlack,toBlack);
    // Run
  gtk_register_dialog(dialog);
  while(!done)
  {
    switch(gtk_dialog_run(GTK_DIALOG(dialog)))
    {
      case GTK_RESPONSE_OK:
      {
        uint32_t s,e;
        s=SGET(spinbuttonStart);
        e=SGET(spinbuttonEnd);
        if(e<s)
        {
          GUI_Error_HIG("Bad parameters","The end frame must be after the start frame !"); 
        }
        else
        {
          param->startFade=s;
          param->endFade=e;
          param->inOut=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxinOut)));
          CHECK_GET(checkbuttonBlack,toBlack);
          done=1;
          ret=1;
        }
      }
      break;
      default:
        done=1;
        break;
                
    }
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
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *comboboxinOut;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkObject *spinbuttonStart_adj;
  GtkWidget *spinbuttonStart;
  GtkObject *spinbuttonEnd_adj;
  GtkWidget *spinbuttonEnd;
  GtkWidget *label4;
  GtkWidget *checkbuttonBlack;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Fade Filter"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Fade Type"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  comboboxinOut = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxinOut);
  gtk_table_attach (GTK_TABLE (table1), comboboxinOut, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxinOut), _("Out"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxinOut), _("In"));

  label2 = gtk_label_new (_("Start Frame"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("End Frame"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  spinbuttonStart_adj = gtk_adjustment_new (1, 0, 1000000, 1, 10, 10);
  spinbuttonStart = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonStart_adj), 1, 0);
  gtk_widget_show (spinbuttonStart);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonStart, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonStart), TRUE);

  spinbuttonEnd_adj = gtk_adjustment_new (1, 0, 1000000, 1, 10, 10);
  spinbuttonEnd = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonEnd_adj), 1, 0);
  gtk_widget_show (spinbuttonEnd);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonEnd, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonEnd), TRUE);

  label4 = gtk_label_new (_("Fade to black "));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  checkbuttonBlack = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonBlack);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonBlack, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, comboboxinOut, "comboboxinOut");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonStart, "spinbuttonStart");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonEnd, "spinbuttonEnd");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBlack, "checkbuttonBlack");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

