/**/
/***************************************************************************
                          DIA_hue
                             -------------------

                           Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/5 by mean
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



#include <config.h>


#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"
#include "ADM_video/ADM_vidEq2.h"
static GtkWidget*create_dialog1 (void);
static float getAdj(GtkWidget *widget);
static void setAdj(GtkWidget *widget,float val);
uint8_t DIA_getEQ2Param(Eq2_Param *param)
{
uint8_t r=0;
GtkWidget *dialog;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        // upload
#define SET(x,y)        setAdj(WID(hscale##x),param->y)
#define GET(x,y)        param->y=getAdj(WID(hscale##x))
        SET(Contrast,contrast);
        SET(Brightness,brightness);
        SET(Saturation,saturation);

        SET(Gamma,gamma);        
        SET(GammaWeight,gamma_weight);
        SET(R,rgamma);
        SET(G,ggamma);
        SET(B,bgamma);


        // run
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                // download
                GET(Contrast,contrast);
                GET(Brightness,brightness);
                GET(Saturation,saturation);

                GET(Gamma,gamma);        
                GET(GammaWeight,gamma_weight);
                GET(R,rgamma);
                GET(G,ggamma);
                GET(B,bgamma);
                r=1;
        }
        
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return r;

}
//**
float getAdj(GtkWidget *widget)
{
GtkAdjustment *adj;        
        adj=gtk_range_get_adjustment (GTK_RANGE(widget));
        return (float)adj->value;
}
void setAdj(GtkWidget *widget,float val)
{
 gtk_range_set_value (GTK_RANGE(widget),(gdouble)val);
}

//**
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *hscaleContrast;
  GtkWidget *hscaleBrightness;
  GtkWidget *hscaleSaturation;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table2;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *hscaleGamma;
  GtkWidget *hscaleGammaWeight;
  GtkWidget *hscaleR;
  GtkWidget *hscaleG;
  GtkWidget *hscaleB;
  GtkWidget *label5;
  GtkWidget *hseparator1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mplayer EQ2"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  label2 = gtk_label_new (_("Contrast :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Brightness:"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Saturation :"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  hscaleContrast = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleContrast);
  gtk_table_attach (GTK_TABLE (table1), hscaleContrast, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleBrightness = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleBrightness);
  gtk_table_attach (GTK_TABLE (table1), hscaleBrightness, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleSaturation = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleSaturation);
  gtk_table_attach (GTK_TABLE (table1), hscaleSaturation, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label1 = gtk_label_new (_("<b>Contrast...</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame2), table2);

  label6 = gtk_label_new (_("Gamma"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table2), label6, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Gamma Weight"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table2), label7, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label8 = gtk_label_new (_("Gamma R"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table2), label8, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  label9 = gtk_label_new (_("Gamma G"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table2), label9, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  label10 = gtk_label_new (_("Gamma B"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  hscaleGamma = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleGamma);
  gtk_table_attach (GTK_TABLE (table2), hscaleGamma, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleGammaWeight = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleGammaWeight);
  gtk_table_attach (GTK_TABLE (table2), hscaleGammaWeight, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleR = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleR);
  gtk_table_attach (GTK_TABLE (table2), hscaleR, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleG = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleG);
  gtk_table_attach (GTK_TABLE (table2), hscaleG, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleB = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 2, 0.1, 0.1, 0.1)));
  gtk_widget_show (hscaleB);
  gtk_table_attach (GTK_TABLE (table2), hscaleB, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label5 = gtk_label_new (_("<b>Gamma</b>"));
  gtk_widget_show (label5);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label5);
  gtk_label_set_use_markup (GTK_LABEL (label5), TRUE);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleContrast, "hscaleContrast");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleBrightness, "hscaleBrightness");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleSaturation, "hscaleSaturation");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGamma, "hscaleGamma");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGammaWeight, "hscaleGammaWeight");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleR, "hscaleR");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleG, "hscaleG");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleB, "hscaleB");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

