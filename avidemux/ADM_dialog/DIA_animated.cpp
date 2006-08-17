/***************************************************************************
                          DIA_animated
    copyright            : (C) 2006 by mean
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

#include "ADM_library/default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_library/default.h"

#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidAnimated_param.h"
#include "ADM_assert.h"


uint8_t DIA_animated(ANIMATED_PARAM *param);

static GtkWidget	*create_dialog1 (void);

#define SPIN_GET(x,y) {param->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)param->y) ;}
#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}	
#define ENTRY_SET(x,y) {gtk_write_entry_string(WID(x),(char *)y);}
#define ENTRY_GET(x,y) {y= (ADM_filename *) ADM_strdup(gtk_editable_get_chars(GTK_EDITABLE ((WID(x))), 0, -1));}
#define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)

#define LOAD_BGD 0x101

uint8_t DIA_animated(ANIMATED_PARAM *param)
{
    uint8_t ret=0;
    GtkWidget *dialog;
    const char *entries[MAX_VIGNETTE]={"spinbuttonTC0","spinbuttonTC1","spinbuttonTC2",
                            "spinbuttonTC3","spinbuttonTC4","spinbuttonTC5" };

        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        CHECK_SET(checkbuttonStd,isNTSC);
        SPIN_SET(spinbuttonvignetteW,vignetteW);
        SPIN_SET(spinbuttonvignetteH,vignetteH);
        if(param->backgroundImg)
            ENTRY_SET(entryBgd,param->backgroundImg);
        for(int i=0;i<MAX_VIGNETTE;i++)
        {
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(dialog,entries[i])),
                    (gfloat)param->timecode[i]) ;
        }

        ASSOCIATE(button1,LOAD_BGD);
        //gtk_widget_show(dialog);
        int action;

        while(1)
        {
            action=gtk_dialog_run(GTK_DIALOG(dialog));
            if(action==LOAD_BGD)
            {
                char *nw;
                    GUI_FileSelRead(_("Select background picture"), &nw);
                    if(nw)
                    {
                        ENTRY_SET(entryBgd,nw);
                        ADM_dealloc(nw);
                    }

                continue;
            }
            break;
        }
        if(action==GTK_RESPONSE_OK)
        {
            CHECK_GET(checkbuttonStd,isNTSC);
            SPIN_GET(spinbuttonvignetteW,vignetteW);
            SPIN_GET(spinbuttonvignetteH,vignetteH);

            if(param->backgroundImg) ADM_dealloc(param->backgroundImg);
            param->backgroundImg=NULL;
            ENTRY_GET(entryBgd,param->backgroundImg);

            for(int i=0;i<MAX_VIGNETTE;i++)
            {
                
                param->timecode[i]=(uint32_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(lookup_widget(dialog,entries[i]))) ;
            }
            ret=1;
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
  GtkWidget *alignment1;
  GtkWidget *table2;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *checkbuttonStd;
  GtkWidget *hbox2;
  GtkWidget *entryBgd;
  GtkWidget *button1;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkObject *spinbuttonvignetteH_adj;
  GtkWidget *spinbuttonvignetteH;
  GtkObject *spinbuttonvignetteW_adj;
  GtkWidget *spinbuttonvignetteW;
  GtkWidget *General;
  GtkWidget *frame2;
  GtkWidget *alignment2;
  GtkWidget *table1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkObject *spinbuttonTC0_adj;
  GtkWidget *spinbuttonTC0;
  GtkObject *spinbuttonTC1_adj;
  GtkWidget *spinbuttonTC1;
  GtkObject *spinbuttonTC2_adj;
  GtkWidget *spinbuttonTC2;
  GtkObject *spinbuttonTC3_adj;
  GtkWidget *spinbuttonTC3;
  GtkObject *spinbuttonTC4_adj;
  GtkWidget *spinbuttonTC4;
  GtkObject *spinbuttonTC5_adj;
  GtkWidget *spinbuttonTC5;
  GtkWidget *label2;
  GtkWidget *hseparator1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Animated Menu"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_NONE);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

  table2 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (alignment1), table2);

  label9 = gtk_label_new (_("BackGround Image :"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table2), label9, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  label10 = gtk_label_new (_("Standard"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  checkbuttonStd = gtk_check_button_new_with_mnemonic (_("NTSC (default is PAL)"));
  gtk_widget_show (checkbuttonStd);
  gtk_table_attach (GTK_TABLE (table2), checkbuttonStd, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_table_attach (GTK_TABLE (table2), hbox2, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  entryBgd = gtk_entry_new ();
  gtk_widget_show (entryBgd);
  gtk_box_pack_start (GTK_BOX (hbox2), entryBgd, TRUE, FALSE, 0);

  button1 = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox2), button1, FALSE, FALSE, 0);

  label11 = gtk_label_new (_("Vignette Width"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new (_("Vignette Height"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  spinbuttonvignetteH_adj = gtk_adjustment_new (1, 16, 240, 2, 10, 10);
  spinbuttonvignetteH = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonvignetteH_adj), 1, 0);
  gtk_widget_show (spinbuttonvignetteH);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonvignetteH, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonvignetteH), TRUE);

  spinbuttonvignetteW_adj = gtk_adjustment_new (1, 16, 240, 2, 10, 10);
  spinbuttonvignetteW = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonvignetteW_adj), 1, 0);
  gtk_widget_show (spinbuttonvignetteW);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonvignetteW, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonvignetteW), TRUE);

  General = gtk_label_new (_("<b>General</b>"));
  gtk_widget_show (General);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), General);
  gtk_label_set_use_markup (GTK_LABEL (General), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

  table1 = gtk_table_new (6, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (alignment2), table1);

  label3 = gtk_label_new (_("Chapter 1"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Chapter 2"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Chapter 3"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new (_("Chapter 4"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Chapter 5"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label8 = gtk_label_new (_("Chapter 6"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table1), label8, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  spinbuttonTC0_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC0 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC0_adj), 1, 0);
  gtk_widget_show (spinbuttonTC0);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC0, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC0, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC0), TRUE);

  spinbuttonTC1_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC1_adj), 1, 0);
  gtk_widget_show (spinbuttonTC1);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC1, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC1, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC1), TRUE);

  spinbuttonTC2_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC2 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC2_adj), 1, 0);
  gtk_widget_show (spinbuttonTC2);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC2, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC2, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC2), TRUE);

  spinbuttonTC3_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC3_adj), 1, 0);
  gtk_widget_show (spinbuttonTC3);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC3, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC3, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC3), TRUE);

  spinbuttonTC4_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC4 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC4_adj), 1, 0);
  gtk_widget_show (spinbuttonTC4);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC4, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC4, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC4), TRUE);

  spinbuttonTC5_adj = gtk_adjustment_new (160, 0, 900000, 2, 10, 10);
  spinbuttonTC5 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTC5_adj), 1, 0);
  gtk_widget_show (spinbuttonTC5);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTC5, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTC5, _("Start frame for this vignette."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTC5), TRUE);

  label2 = gtk_label_new (_("<b>TimeCode</b>"));
  gtk_widget_show (label2);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label2);
  gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonStd, "checkbuttonStd");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, entryBgd, "entryBgd");
  GLADE_HOOKUP_OBJECT (dialog1, button1, "button1");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonvignetteH, "spinbuttonvignetteH");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonvignetteW, "spinbuttonvignetteW");
  GLADE_HOOKUP_OBJECT (dialog1, General, "General");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC0, "spinbuttonTC0");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC1, "spinbuttonTC1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC2, "spinbuttonTC2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC3, "spinbuttonTC3");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC4, "spinbuttonTC4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTC5, "spinbuttonTC5");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

  return dialog1;
}

