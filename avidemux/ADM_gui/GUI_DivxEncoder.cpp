/***************************************************************************
                          GUI_DivxEncoder.cpp  -  description
                             -------------------
    begin                : Sun Jul 14 2002
    copyright            : (C) 2002 by mean
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
#include <stdio.h>
#include <stdlib.h>


#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "ADM_library/default.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

 int getCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fs);

static void gui_ok(GtkButton * button, gpointer user_data);
static void gui_del(GtkButton * button, gpointer user_data);

static GtkWidget *create_compression(void);

static int compresslock;
//static GtkWidget *compress_button_ok;
static GtkWidget *radiobutton1;
static GtkWidget *radiobutton2;
static GtkWidget *radiobutton3;
static GtkWidget *entry_qz;
static GtkWidget *entry_bitrate;
static GtkWidget *entry_size;



void gui_del(GtkButton * button, gpointer user_data)
{
    UNUSED_ARG(button);
    UNUSED_ARG(user_data);
    gui_ok(NULL, (gpointer) -1);
}


void gui_ok(GtkButton * button, gpointer user_data)
{
    UNUSED_ARG(button); 
    int res;

    res = (int) user_data;
    if (compresslock)
	return;			// second call back->ignore.
    switch (res)
      {
      case 1:
	  compresslock = 1;
	  //gtk_hide_widget(wdg);

	  break;
      case 0:
	  compresslock = -1;
	  break;
      case -1:
     	compresslock=-2;
      	break;
       default:
       		ADM_assert(0);
      }
    return;

};


int getCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize)
{
    static GtkWidget *cprm;
    char *str;
    char string[100];
    uint32_t value;
    int ret = 0;
    do
      {
	  compresslock = 0;
	  cprm = create_compression();
	  //
	  switch (*mode)
	    {
	    case COMPRESS_CBR:
	
		    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						 (radiobutton1), 1);

		sprintf(string, "%d", (int) *br / 1000);
		gtk_editable_insert_text(GTK_EDITABLE(entry_bitrate),
					 string, strlen(string), &ret);
		break;

		   case COMPRESS_2PASS:
		    		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
						 	(radiobutton3), 1);

					sprintf(string, "%d", (int) *fsize );
					gtk_editable_insert_text(GTK_EDITABLE(entry_size),
					 string, strlen(string), &ret);

		break;

	    case COMPRESS_CQ:
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (radiobutton2), 1);

		sprintf(string, "%d", (int) *qz);
		gtk_editable_insert_text(GTK_EDITABLE(entry_qz),
					 string, strlen(string), &ret);
		break;
	    }
	  gtk_widget_show(cprm);
	  //__________________________________________________________
	  while (!compresslock)
	    {
		gtk_main_iteration();
	    }
	  if (1 == compresslock)
	    {
		// Get compression mode
		int r1;
		r1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						  (radiobutton1));
		r1 +=
		    2 *
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						 (radiobutton2));
	r1 +=
		    4 *
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
						 (radiobutton3));
		switch (r1)
		  {
		  case 1:
		      *mode = COMPRESS_CBR;
//ubr
		      str =
			  gtk_editable_get_chars(GTK_EDITABLE
						 (entry_bitrate), 0, -1);
		      value = (uint32_t) atoi(str);
		      if (value < 3000)
			  value *= 1000;
		      if (value > 300000 && value < 6000000)
			{
			    *br = value;
			    ret = 1;
		      } else
			  ret = 0;
		      break;
		  case 2:
		      *mode = COMPRESS_CQ;
		      str =
			  gtk_editable_get_chars(GTK_EDITABLE(entry_qz), 0,
						 -1);
		      value = (uint32_t) atoi(str);
		      if (value >= 2 && value <= 32)
						{
			    			*qz = value;
			    			ret = 1;
		      	}
		      	else
						{
				    ret = 0;
						}
		  	    break;
		  case 4:
		     *mode = COMPRESS_2PASS;
		       str=gtk_editable_get_chars(GTK_EDITABLE(entry_size), 0,
						 -1);
		      value = (uint32_t) atoi(str);
        		if((value>0)&&(value<2048))
          		{
       				*fsize=value;
				      	ret=1;
           	}
            	else
             		ret=0;
         		break;
		  default:
		      ADM_assert(0);
		  }



	    }
	  	if(compresslock==-2)
		{
			ret=1;
    			printf("\n destroyed ...\n");
			
		}
			else
   		gtk_widget_destroy(cprm);

      }
    while (!ret);		// end do
    printf("\n exiting ...\n");
    if(compresslock==1) return 1;
    return 0;

}



///---------------
// NEW ONE
//
GtkWidget*
create_compression (void)
{
  GtkWidget *compression;
  GtkWidget *vbox9;
  GtkWidget *label18;
  GtkWidget *hbox6;
  GtkWidget *label19;
  GtkWidget *vbox10;
  GtkWidget *hbox10;
  GSList *pp_compressmode_group = NULL;
//  GtkWidget *radiobutton2;
  GtkWidget *label20;
//  GtkWidget *entry_qz;
  GtkWidget *hbox11;
//  GtkWidget *radiobutton1;
  GtkWidget *label21;
//  GtkWidget *entry_bitrate;
  GtkWidget *hbox12;
//  GtkWidget *radiobutton3;
  GtkWidget *label22;
//  GtkWidget *entry_size;
  GtkWidget *button9;

  compression = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data_full(GTK_OBJECT(compression),
			     "compression",
			     compression,
			     (GtkDestroyNotify) gui_del);

      gtk_window_set_modal(GTK_WINDOW(compression), TRUE);
//  gtk_object_set_data (GTK_OBJECT (compression), "compression", compression);
    gtk_window_set_title(GTK_WINDOW(compression), "MP4-Compression");

gtk_window_set_modal(GTK_WINDOW(compression), 1);
  vbox9 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox9);
  gtk_object_set_data_full (GTK_OBJECT (compression), "vbox9", vbox9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox9);
  gtk_container_add (GTK_CONTAINER (compression), vbox9);

  label18 = gtk_label_new ("Compression Parameters");
  gtk_widget_ref (label18);
  gtk_object_set_data_full (GTK_OBJECT (compression), "label18", label18,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (vbox9), label18, FALSE, FALSE, 0);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox6);
  gtk_object_set_data_full (GTK_OBJECT (compression), "hbox6", hbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox9), hbox6, TRUE, TRUE, 0);

  label19 = gtk_label_new ("Mode:");
  gtk_widget_ref (label19);
  gtk_object_set_data_full (GTK_OBJECT (compression), "label19", label19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label19);
  gtk_box_pack_start (GTK_BOX (hbox6), label19, FALSE, FALSE, 0);

  vbox10 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox10);
  gtk_object_set_data_full (GTK_OBJECT (compression), "vbox10", vbox10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox10);
  gtk_box_pack_start (GTK_BOX (hbox6), vbox10, TRUE, FALSE, 0);

  hbox10 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox10);
  gtk_object_set_data_full (GTK_OBJECT (compression), "hbox10", hbox10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox10);
  gtk_box_pack_start (GTK_BOX (vbox10), hbox10, FALSE, FALSE, 0);

  radiobutton2 = gtk_radio_button_new_with_label (pp_compressmode_group, "CQ");
  pp_compressmode_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
  gtk_widget_ref (radiobutton2);
  gtk_object_set_data_full (GTK_OBJECT (compression), "radiobutton2", radiobutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox10), radiobutton2, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton2), TRUE);

  label20 = gtk_label_new (", quality ");
  gtk_widget_ref (label20);
  gtk_object_set_data_full (GTK_OBJECT (compression), "label20", label20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label20);
  gtk_box_pack_start (GTK_BOX (hbox10), label20, TRUE, TRUE, 0);
  gtk_widget_set_usize (label20, 100, -2);

  entry_qz = gtk_entry_new ();
  gtk_widget_ref (entry_qz);
  gtk_object_set_data_full (GTK_OBJECT (compression), "entry_qz", entry_qz,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry_qz);
  gtk_box_pack_start (GTK_BOX (hbox10), entry_qz, TRUE, TRUE, 0);

  hbox11 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox11);
  gtk_object_set_data_full (GTK_OBJECT (compression), "hbox11", hbox11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox11);
  gtk_box_pack_start (GTK_BOX (vbox10), hbox11, FALSE, FALSE, 0);

  radiobutton1 = gtk_radio_button_new_with_label (pp_compressmode_group, "CBR");
  pp_compressmode_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (compression), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox11), radiobutton1, TRUE, TRUE, 0);

  label21 = gtk_label_new (", bitrate");
  gtk_widget_ref (label21);
  gtk_object_set_data_full (GTK_OBJECT (compression), "label21", label21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label21);
  gtk_box_pack_start (GTK_BOX (hbox11), label21, TRUE, TRUE, 0);
  gtk_widget_set_usize (label21, 100, -2);

  entry_bitrate = gtk_entry_new ();
  gtk_widget_ref (entry_bitrate);
  gtk_object_set_data_full (GTK_OBJECT (compression), "entry_bitrate", entry_bitrate,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry_bitrate);
  gtk_box_pack_start (GTK_BOX (hbox11), entry_bitrate, TRUE, TRUE, 0);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox12);
  gtk_object_set_data_full (GTK_OBJECT (compression), "hbox12", hbox12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox12);
  gtk_box_pack_start (GTK_BOX (vbox10), hbox12, FALSE, FALSE, 0);

  radiobutton3 = gtk_radio_button_new_with_label (pp_compressmode_group, "2-Pass");
  pp_compressmode_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (compression), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (hbox12), radiobutton3, TRUE, TRUE, 0);

  label22 = gtk_label_new (", size in MB");
  gtk_widget_ref (label22);
  gtk_object_set_data_full (GTK_OBJECT (compression), "label22", label22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label22);
  gtk_box_pack_start (GTK_BOX (hbox12), label22, TRUE, TRUE, 0);

  entry_size = gtk_entry_new ();
  gtk_widget_ref (entry_size);
  gtk_object_set_data_full (GTK_OBJECT (compression), "entry_size", entry_size,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry_size);
  gtk_box_pack_start (GTK_BOX (hbox12), entry_size, TRUE, TRUE, 0);

  button9 = gtk_button_new_with_label ("OK");
  gtk_widget_ref (button9);
  gtk_object_set_data_full (GTK_OBJECT (compression), "button9", button9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button9);
  gtk_box_pack_start (GTK_BOX (vbox9), button9, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (button9), "clicked",
                      GTK_SIGNAL_FUNC(gui_ok),
		       (void *) 1);

  return compression;
}
