/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <config.h>


#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "ADM_library/default.h"
#ifdef USE_FREETYPE

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"

#include "ADM_video/ADM_vidAss_Params.h"

#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#define SPIN_GETI(x,y){param->y=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_GET(x,y){param->y=gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)param->y) ;}
#define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)

#define   LOADSUB 17   
     
static GtkWidget *    create_dialog1 (void);
uint8_t DIA_ass(ASSParams *param)
{
  uint8_t r=0;
  GtkWidget *dialog=create_dialog1();
  gtk_register_dialog(dialog);
  
  SPIN_SET(spinbuttonScale,font_scale);
  SPIN_SET(spinbuttonSpacing,line_spacing); 
  SPIN_SET(spinbuttonTop,top_margin);
  SPIN_SET(spinbuttonBottom,bottom_margin);
  if(param->subfile)
  {
    int r;
    char *s=(char *)(param->subfile);
    gtk_editable_delete_text(GTK_EDITABLE(WID(entrySub)), 0,-1);
    gtk_editable_insert_text(GTK_EDITABLE(WID(entrySub)), s, strlen(s), &r);
  }
  ASSOCIATE(button1,LOADSUB);
  /* Upload */
  int x=0,d;
  while(!x)
  {
      d=gtk_dialog_run(GTK_DIALOG(dialog));
      switch(d)
      {
        case LOADSUB:
          {
            char *name=NULL;
            GUI_FileSelRead("Select ASS/SSA file",&name);
            if(name)
            {
                int r;
                gtk_editable_delete_text(GTK_EDITABLE(WID(entrySub)), 0,-1);
                gtk_editable_insert_text(GTK_EDITABLE(WID(entrySub)), name, strlen(name), &r);
                ADM_dealloc(name);
            }
          }
            break;
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_APPLY:
              {
                SPIN_GET(spinbuttonScale,font_scale);
                SPIN_GET(spinbuttonSpacing,line_spacing); 
                SPIN_GETI(spinbuttonTop,top_margin);
                SPIN_GETI(spinbuttonBottom,bottom_margin);
                char *n;
                if(param->subfile)
                {
                  ADM_dealloc(param->subfile);
                  param->subfile=NULL;
                }
                n=gtk_editable_get_chars(GTK_EDITABLE (WID(entrySub)), 0, -1);
                printf("Name :%s\n",n);
                param->subfile=(ADM_filename *)ADM_strdup(n);
                r=1;
                x=1;
                break;
              }
        default:
                r=0;
                x=1;
                break;
      }
  }
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return r; 
}

GtkWidget *    create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *hbox1;
  GtkWidget *entrySub;
  GtkWidget *button1;
  GtkObject *spinbuttonScale_adj;
  GtkWidget *spinbuttonScale;
  GtkObject *spinbuttonSpacing_adj;
  GtkWidget *spinbuttonSpacing;
  GtkObject *spinbuttonTop_adj;
  GtkWidget *spinbuttonTop;
  GtkObject *spinbuttonBottom_adj;
  GtkWidget *spinbuttonBottom;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("ASS/SSA Subtitle"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Subtitle file (ass/ssa) :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Font scale :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Line spacing"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Top margin"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Bottom margin"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_table_attach (GTK_TABLE (table1), hbox1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  entrySub = gtk_entry_new ();
  gtk_widget_show (entrySub);
  gtk_box_pack_start (GTK_BOX (hbox1), entrySub, TRUE, TRUE, 0);

  button1 = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, FALSE, 0);

  spinbuttonScale_adj = gtk_adjustment_new (1, 0.10000000149, 10, 1, 1, 1);
  spinbuttonScale = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonScale_adj), 1, 2);
  gtk_widget_show (spinbuttonScale);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonScale, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonScale), TRUE);

  spinbuttonSpacing_adj = gtk_adjustment_new (1, 0.10000000149, 10, 1, 1, 1);
  spinbuttonSpacing = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonSpacing_adj), 1, 2);
  gtk_widget_show (spinbuttonSpacing);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonSpacing, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonSpacing), TRUE);

  spinbuttonTop_adj = gtk_adjustment_new (0, 0, 200, 1, 10, 10);
  spinbuttonTop = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTop_adj), 1, 0);
  gtk_widget_show (spinbuttonTop);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTop, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTop), TRUE);

  spinbuttonBottom_adj = gtk_adjustment_new (0, 0, 200, 1, 10, 10);
  spinbuttonBottom = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBottom_adj), 1, 0);
  gtk_widget_show (spinbuttonBottom);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonBottom, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBottom), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, entrySub, "entrySub");
  GLADE_HOOKUP_OBJECT (dialog1, button1, "button1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonScale, "spinbuttonScale");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonSpacing, "spinbuttonSpacing");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTop, "spinbuttonTop");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBottom, "spinbuttonBottom");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}



#endif /* USE_FREETYPE */
