/***************************************************************************
                          DIA_indexer.cpp  -  description
                             -------------------
	Here we do some funky stuff that should be removed in gtk 2.4 as
		i've understood it
	For each track we scan to see if it exists, then we destroy the
		corresponding optionmenu and rebuilt it
	I dd not found a way to change a optionmenuitem after its creation
	
	in gtk2.4 at least, the text will be changeable (properties of menuitem)
			     
			     
			     
    begin                : Thu Dec 09 2003
    copyright            : (C) 2002/2003 by mean
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
#include "ADM_gui2/support.h"
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
//___________________________________
#include "avi_vars.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_mpegindexer/ADM_mpegparser.h"
//___________________________________
#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

static GtkWidget *optionmenuM;
static GtkWidget *optionmenuL;
static GtkWidget *optionmenuA;
 //--
static GtkWidget	*create_dialog1 (uint8_t prefilled);
static void on_callback_index(GtkButton * button, gpointer user_data);
static void on_callback_mpeg(GtkButton * button, gpointer user_data);
static void on_callback_scan(GtkButton * button, gpointer user_data);
static void menuBuild(GtkWidget *table1, mpegAudioTrack *table);


void mpegCB(char *name);
void indexCB(char *name);

static char _indexName[300]; 
static char _mpegName[300];
static mpegAudioTrack track[24];
static GtkWidget *dialog;


/**
	Prefill the index name & mpeg name with given string.
	Used to automatically index a prompted video

*/

void DIA_indexerPrefill(char *name)
{
		char tmp[500];
				strcpy(_mpegName,name);
				strcpy(tmp,name);
				strcat(tmp,".idx");
				strcpy(_indexName,tmp);

}
/**
	DiampegIndexer :
				mpegFile returns a pointer to the mpeg to index 's name
				indexFile rreturns a pointer to index name
				aid : audio id (C0--C7 for mpeg, 0--7 for AC3)
				already : if set to 1, use the prefilled entries

*/
int  DIA_mpegIndexer(char **mpegFile,char **indexFile,int *aid,int already)
{
uint8_t ret=0;

uint8_t	one_more_time=0;

	

	dialog=create_dialog1(one_more_time);
	gtk_transient(dialog);
	if(already)
	{
		gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label_index")),_indexName);
		gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"labelmpeg")),_mpegName);

	}

	#define CONNECT(A,B)  gtk_signal_connect (GTK_OBJECT(lookup_widget(dialog,#A)), "clicked", \
		      GTK_SIGNAL_FUNC (B), (void *) NULL);

	CONNECT(button_mpeg,on_callback_mpeg);
	CONNECT(button_index,on_callback_index);
	CONNECT(button_scan,on_callback_scan);

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
			*indexFile=_indexName;
			*mpegFile=_mpegName;
			// now grab AC3/mpeg ?
			*aid=0;
			if(TRUE==gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
					 (lookup_widget(dialog,"radiobutton_mpeg"))))
			{
				*aid+=0xC0;
				*aid+=getRangeInMenu(optionmenuM);
			}
			if(TRUE==gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
					 (lookup_widget(dialog,"radiobutton1"))))
			{					 
				 *aid+=0xA0;
				 *aid+=getRangeInMenu(optionmenuL);
			}
			if(TRUE==gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
					 (lookup_widget(dialog,"radiobutton_ac3"))))
			{					 
				
				 *aid+=getRangeInMenu(optionmenuA);
			}			
			ret=1;

	}

	gtk_widget_destroy(dialog);
	
	return ret;
}

//--------------------------------------------------------------------------------------------
void on_callback_index(GtkButton * button, gpointer user_data)
{
        UNUSED_ARG(button);
    	UNUSED_ARG(user_data);
        GUI_FileSelWrite("Index to create", indexCB)  ;
}
void indexCB(char *name)
{
	strcpy(_indexName,name);
	 gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label_index")),name);
}
void on_callback_mpeg(GtkButton * button, gpointer user_data)
{
    	UNUSED_ARG(button);
   	UNUSED_ARG(user_data);
    	GUI_FileSelRead("Mpeg to scan", mpegCB)           ;

}

void on_callback_scan(GtkButton * button, gpointer user_data)
{
	
	char    sac3[400],smpeg[200],slpcm[200],tmp[200];
	mpegAudioTrack *trk=NULL;
	
    	UNUSED_ARG(button);
   	UNUSED_ARG(user_data);
	
	
  	
	strcpy(sac3,"Tracks # found:\n\tAC3 :");
	strcpy(smpeg,"\n\tsmpeg :");
	strcpy(slpcm,"\n\tslpcm :");
	
	if(MpegaudoDetectAudio(_mpegName,track))
	{
		int nac3=0;
		int nmpeg=0;
		int nlpcm=0;
/*
	audioTrack[24]
			00..07 -> Mpeg audio
		08+	00..07 -> AC3 audio
		16+	00..07 -> LPCM audio

*/		
		for(int i=0;i<8;i++)
		{
			if(track[i+8].presence) 
				{
					
					nac3++;
				}
			if(track[i].presence) 
				{
					
					nmpeg++;
				}
			if(track[i+16].presence) 
				{
					
					nlpcm++;
				}
			
			
		}
		gtk_widget_destroy(optionmenuM);		
		gtk_widget_destroy(optionmenuL);	
		gtk_widget_destroy(optionmenuA);
		
		if(nac3)         
		{    RADIO_SET(radiobutton_ac3,1);}
			else if(nmpeg)        { RADIO_SET(radiobutton_mpeg,1);}
				else if(nlpcm)  { RADIO_SET(radiobutton1,1);}
						else 
						{
						GUI_Alert("Could no identift audio stream");
						
						}
		// now the tricky part, we will delete all 3 menuOption and
		// rebuild them with the infos we just got
			
		trk=track;

	}
	else
	{
		GUI_Alert("Could no identift audio stream");
		
	}
	menuBuild(WID(table1),trk);
}


void mpegCB(char *name)
{
	strcpy(_mpegName,name);
	 gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"labelmpeg")),name);
}


GtkWidget	*create_dialog1 (uint8_t prefilled)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox2;
  GtkWidget *labelmpeg;
  GtkWidget *button_mpeg;
  GtkWidget *hbox3;
  GtkWidget *label_index;
  GtkWidget *button_index;
  GtkWidget *hbox1;
  GtkWidget *table1;
  GtkWidget *radiobutton_ac3;
  
  GSList *radiobutton_ac3_group = NULL;

 
  
  GtkWidget *radiobutton_mpeg;
  
 
  GtkWidget *radiobutton1;
  GtkWidget *button_scan;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;


  
  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mpeg Indexer"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  labelmpeg = gtk_label_new ("");
  gtk_widget_show (labelmpeg);
  gtk_box_pack_start (GTK_BOX (hbox2), labelmpeg, TRUE, FALSE, 0);

  button_mpeg = gtk_button_new_with_mnemonic (_("Select Mpeg"));
  gtk_widget_show (button_mpeg);
  gtk_box_pack_start (GTK_BOX (hbox2), button_mpeg, FALSE, FALSE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  label_index = gtk_label_new ("");
  gtk_widget_show (label_index);
  gtk_box_pack_start (GTK_BOX (hbox3), label_index, TRUE, FALSE, 0);

  button_index = gtk_button_new_with_mnemonic (_("Select Index"));
  gtk_widget_show (button_index);
  gtk_box_pack_start (GTK_BOX (hbox3), button_index, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (hbox1), table1, TRUE, TRUE, 0);

  radiobutton_ac3 = gtk_radio_button_new_with_mnemonic (NULL, _("AC3 audio"));
  gtk_widget_show (radiobutton_ac3);
  gtk_table_attach (GTK_TABLE (table1), radiobutton_ac3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_ac3), radiobutton_ac3_group);
  radiobutton_ac3_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_ac3));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_ac3), TRUE);

 

 

  radiobutton_mpeg = gtk_radio_button_new_with_mnemonic (NULL, _("Mpeg audio"));
  gtk_widget_show (radiobutton_mpeg);
  gtk_table_attach (GTK_TABLE (table1), radiobutton_mpeg, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_mpeg), radiobutton_ac3_group);
  radiobutton_ac3_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_mpeg));

  
  /*------------------*/
  menuBuild(table1,NULL);
//--------------------------------------------------
 
  /*---------------*/
  
  radiobutton1 = gtk_radio_button_new_with_mnemonic (NULL, _("LPCM"));
  gtk_widget_show (radiobutton1);
  gtk_table_attach (GTK_TABLE (table1), radiobutton1, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton1), radiobutton_ac3_group);
  radiobutton_ac3_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton1));

  button_scan = gtk_button_new_with_mnemonic (_("Scan file for audio stream"));
  gtk_widget_show (button_scan);
  gtk_box_pack_start (GTK_BOX (vbox1), button_scan, FALSE, FALSE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, labelmpeg, "labelmpeg");
  GLADE_HOOKUP_OBJECT (dialog1, button_mpeg, "button_mpeg");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, label_index, "label_index");
  GLADE_HOOKUP_OBJECT (dialog1, button_index, "button_index");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_ac3, "radiobutton_ac3");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuL, "optionmenuL");
//  GLADE_HOOKUP_OBJECT (dialog1, menu3, "menu3");
 
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_mpeg, "radiobutton_mpeg");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuM, "optionmenuM");
//  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
 
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuA, "optionmenuA");
//  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton1, "radiobutton1");
  GLADE_HOOKUP_OBJECT (dialog1, button_scan, "button_scan");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


void menuBuild(GtkWidget *table1,mpegAudioTrack *table)
{
  GtkWidget *menu1;
  GtkWidget *_1;
  GtkWidget *image1;
  GtkWidget *menu2;
  GtkWidget *menu3;
  char str[100];
  int yes;
  
  
  /*---------LCPM index+16----------*/
  
  optionmenuL = gtk_option_menu_new ();
  gtk_widget_show (optionmenuL);
  gtk_table_attach (GTK_TABLE (table1), optionmenuL, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

 
 
  menu3 = gtk_menu_new ();

  

  for(uint32_t i=0;i<8;i++) //LPCM
  {
  	yes=0;
	sprintf(str,"%d",i);
  	if(table)
	{
		if (table[i+16].presence)
		{
			yes=1;	
  			sprintf(str,"%d shift %d ms",i,table[i+16].shift);			
		}
	}	
  		_1 = gtk_image_menu_item_new_with_mnemonic (str);
  		gtk_widget_show (_1);
  		gtk_container_add (GTK_CONTAINER (menu3), _1);
		if(yes)
			image1 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_MENU);
		else
			image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_MENU);
			
  		gtk_widget_show (image1);
  		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (_1), image1);
	
  }
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuL), menu3);
   	//---//
  /*---------Mpeg index+0----------*/	
  optionmenuM = gtk_option_menu_new ();
  gtk_widget_show (optionmenuM);
  gtk_table_attach (GTK_TABLE (table1), optionmenuM, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
		    
		  
  menu1 = gtk_menu_new ();  
  for(uint32_t i=0;i<8;i++)
  {
  	yes=0;
	sprintf(str,"%d",i);
  	if(table)
	{
		if (table[i+00].presence)
		{
			yes=1;
  			sprintf(str,"%d: %lu channels, %lu kbps shift %d ms",
			i,table[i].channels,(table[i].bitrate*8)/1000,table[i].shift);
		}
	}
	_1 = gtk_image_menu_item_new_with_mnemonic (str);
  	gtk_widget_show (_1);
  	gtk_container_add (GTK_CONTAINER (menu1), _1);

	if(yes)
			image1 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_MENU);
		else
			image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_MENU);
			
  	gtk_widget_show (image1);
  	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (_1), image1);
  }  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuM), menu1);

  		    
		    //--//
  optionmenuA = gtk_option_menu_new ();
  gtk_widget_show (optionmenuA);
  gtk_table_attach (GTK_TABLE (table1), optionmenuA, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  /*---------AC3 index+8----------*/	
  menu2 = gtk_menu_new ();

  for(uint32_t i=0;i<8;i++)
  {
  	yes=0;  				
	sprintf(str,"%d",i);	
	
	if(table)		
  	if(table[i+8].presence)
	{
		yes=1;
		if(table[i+8].channels && table[i+8].bitrate)
		{
			sprintf(str,"%d: %lu channels, %lu kbps shift %d ms",
			i,table[i+8].channels,(table[i+8].bitrate*8)/1000,table[i+8].shift);
			
		}
	
	}
  	
  	_1 = gtk_image_menu_item_new_with_mnemonic (str);
  	gtk_widget_show (_1);
  	gtk_container_add (GTK_CONTAINER (menu2), _1);

	if(yes)
			image1 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_MENU);
		else
			image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_MENU);
			
  	gtk_widget_show (image1);
  	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (_1), image1);
  }
  
 

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuA), menu2);
	
}
