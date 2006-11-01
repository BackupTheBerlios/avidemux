/***************************************************************************
                          Simple bitrate calculator
                             -------------------
    begin                : Wed Dec 18 2002
    copyright            : (C) 2004 by mean
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <gtk/gtk.h>

#include "fourcc.h"
#include "avio.hxx"


#include "default.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_assert.h"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#define NB_TRACK_MAX (8+4)
static GtkWidget *create_dialog1 (void);
extern const char *getStrFromAudioCodec( uint32_t codec);
uint8_t DIA_audioTrack(AudioSource *source, uint32_t *track,uint32_t nbTrack, audioInfo *infos);
//************************************
uint8_t DIA_audioTrack(AudioSource *source, uint32_t *track,uint32_t nbTrack, audioInfo *infos)
{
uint8_t r=0;
GtkWidget *menu,*dialog;
char string[100];
GtkWidget **fq=(GtkWidget **)ADM_alloc(sizeof(GtkWidget *)*NB_TRACK_MAX);
int nbWidget=0;
int old;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        menu = gtk_menu_new ();
#define ADD_STRING() \
                fq[nbWidget]=gtk_menu_item_new_with_mnemonic (string); \
                gtk_widget_show (fq[nbWidget]); \
                gtk_container_add (GTK_CONTAINER (menu), fq[nbWidget]); \
                nbWidget++; 
        
        for(unsigned int i=0;i<nbTrack;i++)
        {
                sprintf(string,"Audio track %d (%s/%d channels/%d kbit per s/%d ms shift)",i,getStrFromAudioCodec(infos[i].encoding),
                        infos[i].channels,infos[i].bitrate,infos[i].av_sync);
                ADD_STRING();
        }
        // Add none
        sprintf(string,"No audio");
        ADD_STRING();

        sprintf(string,"External PCM (Wav)");
        ADD_STRING();

        sprintf(string,"External MP2/3");
        ADD_STRING();

        sprintf(string,"External AC3");
        ADD_STRING();

        switch(*source)
        {
                case  AudioAvi: old=*track;break;
                case  AudioMP3: old=nbWidget-2;break;
                case  AudioWav: old=nbWidget-3;break;
                case  AudioAC3: old=nbWidget-1;break;
                case  AudioNone: old=nbWidget-4;break;
                default:
                        ADM_assert(0);
        }
        

        gtk_option_menu_set_menu (GTK_OPTION_MENU (WID(optionmenu1)), menu);
        gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu1)), old);
        if(GTK_RESPONSE_OK==gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                        r=1;
                        old=r=getRangeInMenu(WID(optionmenu1));
                        if(old<nbTrack)
                        {
                                *source=AudioAvi;
                                *track=old;
                        }
                        else
                        {
                                switch(old-nbTrack)
                                {
                                        case 0: *source=AudioNone;break;
                                        case 1: *source=AudioWav;break;
                                        case 2: *source=AudioMP3;break;
                                        case 3: *source=AudioAC3;break;
                                }
                        }
                }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return r;
}

GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *optionmenu1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Audio selector"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Select audio track"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_box_pack_start (GTK_BOX (vbox1), optionmenu1, FALSE, FALSE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

