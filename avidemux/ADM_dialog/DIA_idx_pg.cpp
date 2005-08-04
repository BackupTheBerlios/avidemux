/***************************************************************************
                          DIA_dmx.cpp  -  description
                             -------------------     
Indexer progress dialog                             
    
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
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config.h"
#include "ADM_gui2/support.h"
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "DIA_idx_pg.h"

static GtkWidget *dialog=NULL;
static GtkWidget       *create_dialog1 (void);

DIA_progressIndexing::DIA_progressIndexing(char *name)
{
        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        gtk_label_set_text(GTK_LABEL(WID(labelName)),name);
        gtk_widget_show(dialog);
        clock.reset();

}
DIA_progressIndexing::~DIA_progressIndexing()
{
        ADM_assert(dialog);
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        dialog=NULL;
}
uint8_t       DIA_progressIndexing::update(uint32_t done,uint32_t total, uint32_t nbImage, uint32_t hh, uint32_t mm, uint32_t ss)
{
        char string[256];
        double f;
        uint32_t tim,tom;
        uint32_t   zhh,zmm,zss;

        UI_purge();

        sprintf(string,"%02d:%02d:%02d",hh,mm,ss);
        gtk_label_set_text(GTK_LABEL(WID(labelTime)),string);

        sprintf(string,"%0lu",nbImage);
        gtk_label_set_text(GTK_LABEL(WID(labelNbImage)),string);

        f=done;
        f/=total;

        gtk_progress_set_percentage(GTK_PROGRESS(WID(progressbar1)),(gfloat)f);

        /* compute ETL */
         tim=clock.getElapsedMS();;
       // Do a simple relation between % and time
        // Elapsed time =total time*percent
        if(f<0.01) return 1;
        f=tim/f;
        // Tom is total time
        tom=(uint32_t)floor(f);
        if(tim>tom) return 1;
        tom=tom-tim;
        ms2time(tom,&zhh,&zmm,&zss);
        sprintf(string,"%02d:%02d:%02d",zhh,zmm,zss);
        gtk_label_set_text(GTK_LABEL(WID(label6)),string);
        return 1;

}


GtkWidget       *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label6;
  GtkWidget *label5;
  GtkWidget *label3;
  GtkWidget *label1;
  GtkWidget *labelTime;
  GtkWidget *labelNbImage;
  GtkWidget *label7;
  GtkWidget *labelName;
  GtkWidget *progressbar1;
  GtkWidget *dialog_action_area1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Indexing..."));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label6 = gtk_label_new (_("0"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label5 = gtk_label_new (_("Time left :"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label3 = gtk_label_new (_("Timecode :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label1 = gtk_label_new (_("Nb pictures seen :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  labelTime = gtk_label_new (_("00:00:00"));
  gtk_widget_show (labelTime);
  gtk_table_attach (GTK_TABLE (table1), labelTime, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelTime), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelTime), 0, 0.5);

  labelNbImage = gtk_label_new (_("0"));
  gtk_widget_show (labelNbImage);
  gtk_table_attach (GTK_TABLE (table1), labelNbImage, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelNbImage), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelNbImage), 0, 0.5);

  label7 = gtk_label_new (_("Indexing :"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  labelName = gtk_label_new (_("/dev/null"));
  gtk_widget_show (labelName);
  gtk_table_attach (GTK_TABLE (table1), labelName, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelName), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelName), 0, 0.5);

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), progressbar1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, labelTime, "labelTime");
  GLADE_HOOKUP_OBJECT (dialog1, labelNbImage, "labelNbImage");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, labelName, "labelName");
  GLADE_HOOKUP_OBJECT (dialog1, progressbar1, "progressbar1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");

  return dialog1;
}
