/***************************************************************************
                          DIA_dmx.cpp  -  description
                             -------------------     
        2nd gen indexer
                             
    
    copyright            : (C) 2005 by mean
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config.h"
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_gladeSupport.h"

#include "ADM_toolkit/toolkit.hxx"
//___________________________________


#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"

#include "ADM_toolkit/filesel.h"
#include "ADM_mpegdemuxer/dmx_identify.h"
#include "ADM_mpegdemuxer/dmx_demuxer.h"

static GtkWidget *create_dialog1 (void);
/****************************************************************/
uint8_t DIA_dmx(char *file,DMX_TYPE format,uint32_t nbTracks, MPEG_TRACK *tracks, uint32_t *selectedTracks)
{
GtkWidget *dialog,*optionmenu=NULL,*menu=NULL,*_1;
uint8_t ret=0;
char *fmt;
char str[512];
        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        optionmenu=WID(optionmenuAudio);
        // Fill entry
        gtk_label_set_text(GTK_LABEL(WID(labelFile)),file);
        switch(format)
        {
                case DMX_MPG_PS:
                        gtk_label_set_text(GTK_LABEL(WID(labelType)),"Program Stream");
                        break;
                case DMX_MPG_TS:
                        gtk_label_set_text(GTK_LABEL(WID(labelType)),"Transport Stream");
                        break;

                default: ADM_assert(0);
        }
        sprintf(str,"%x (%x)",tracks[0].pes,tracks[0].pid);
        gtk_label_set_text(GTK_LABEL(WID(labelVideoStream)),str);
        
        // Build option menu
        menu = gtk_menu_new ();
        uint32_t pes;
        for(uint32_t i=1;i<nbTracks;i++)
        {
                sprintf(str,"???");
                pes=tracks[i].pes;
                if(pes<9) sprintf(str,"%d: AC3 :%x channels:%d bitrate:%d kbps",i,pes,tracks[i].channels,tracks[i].bitrate);
                if(pes>=0xC0 && pes<0xC9) sprintf(str,"%d: MP2  :%x channels:%d bitrate:%d kbps",
                                                i,pes,tracks[i].channels,tracks[i].bitrate);
                if(pes>=0xA0 && pes<0xA9) sprintf(str,"%d: LPCM :%x",i,pes);
                
                _1 = gtk_image_menu_item_new_with_mnemonic (str);
                gtk_widget_show (_1);
                gtk_container_add (GTK_CONTAINER (menu), _1);
        }
                        
        
        gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu), menu);
        //

        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                        *selectedTracks=getRangeInMenu(optionmenu);
                        ret=1;

        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        
        return ret;
}
/****************************************************************/
GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *labelFile;
  GtkWidget *labelType;
  GtkWidget *labelVideoStream;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *hbox1;
  GtkWidget *label9;
  GtkWidget *optionmenuAudio;
  GtkWidget *label8;
  GtkWidget *hseparator1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mpeg, Select audio"));

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

  label2 = gtk_label_new (_("File :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Type :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Video stream :"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  labelFile = gtk_label_new (_("label5"));
  gtk_widget_show (labelFile);
  gtk_table_attach (GTK_TABLE (table1), labelFile, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelFile), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelFile), 0, 0.5);

  labelType = gtk_label_new (_("label6"));
  gtk_widget_show (labelType);
  gtk_table_attach (GTK_TABLE (table1), labelType, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelType), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelType), 0, 0.5);

  labelVideoStream = gtk_label_new (_("label7"));
  gtk_widget_show (labelVideoStream);
  gtk_table_attach (GTK_TABLE (table1), labelVideoStream, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelVideoStream), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelVideoStream), 0, 0.5);

  label1 = gtk_label_new (_("<b>Input file</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (frame2), hbox1);

  label9 = gtk_label_new (_("Detected Audio :"));
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox1), label9, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  optionmenuAudio = gtk_option_menu_new ();
  gtk_widget_show (optionmenuAudio);
  gtk_box_pack_start (GTK_BOX (hbox1), optionmenuAudio, FALSE, FALSE, 0);

  label8 = gtk_label_new (_("<b>Audio</b>"));
  gtk_widget_show (label8);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label8);
  gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, labelFile, "labelFile");
  GLADE_HOOKUP_OBJECT (dialog1, labelType, "labelType");
  GLADE_HOOKUP_OBJECT (dialog1, labelVideoStream, "labelVideoStream");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuAudio, "optionmenuAudio");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

