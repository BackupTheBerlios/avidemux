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


#include "avi_vars.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_assert.h"



#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"


#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_film2pal.h"


void DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate );
static GtkWidget	*create_Calculator (void);  
static GtkWidget *dialog;  
static void prepare( void );
static void update( void );

static uint32_t videoDuration;
static uint32_t track1, track2;

uint32_t numberOfVideoFrames;

static uint32_t videoSize,videoBitrate;

//************************************
void DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate )
{
	if(!avifileinfo) return ;
	
	dialog=create_Calculator();
	prepare();
	update();
	while(1)
	{
		if(GTK_RESPONSE_APPLY==gtk_dialog_run(GTK_DIALOG(dialog)))
		{
			update();
		}
	else
		{
			break;
		}
	}
	gtk_widget_destroy(dialog);
	*sizeInMeg=videoSize;
	*avgBitrate=videoBitrate;
	
}
//******************************************
// Compute the value that will not change
// (duration)
// and init the entries for current bitrate
//******************************************
void prepare( void )
{

	float duration=0;
	aviInfo info;
	char string[200];
	 uint16_t mm,hh,ss,ms;
	 AVDMProcessAudioStream *stream;
	 
	 if(frameStart<frameEnd) numberOfVideoFrames=frameEnd-frameStart;
	 else			 numberOfVideoFrames=frameStart-frameEnd;
 
 	
	
	duration=(float)video_body->getTime (numberOfVideoFrames);
	duration=duration/1000.;
	
	if(duration<0) duration=-duration;
	
	video_body->getVideoInfo(&info);
	
	videoDuration=(uint32_t)ceil(duration);
	frame2time(numberOfVideoFrames,info.fps1000, &hh, &mm, &ss, &ms);
	// now we can set it
	sprintf(string,"%02d:%02d:%02d",hh,mm,ss);
	gtk_label_set_text(GTK_LABEL(WID(labelDuration)),string);
	
	printf("Video duration :%lu\n",videoDuration);

	// Now get audio info
	track1=0;
	if(audioProcessMode && currentaudiostream)
	{
		stream=buildFakeAudioFilter(currentaudiostream,
				0, 0xffffffff);
	
		if(stream)
		{
			track1=(stream->getInfo()->byterate*8)/1000;
		}
		deleteAudioFilter();
	}else
	{
		if(currentaudiostream) track1=(currentaudiostream->getInfo()->byterate*8)/1000;
	}
	
	track2=0;
	gtk_write_entry(WID(entry3), track1);
	gtk_write_entry(WID(entry4), track2);
	
	printf("Track1 bitrate :%lu\n",track1);
	printf("Track2 bitrate :%lu\n",track2);

	
	
}

//*************************************
void update( void )
{
uint32_t audioSize;
uint32_t totalSize;
char string[200];
	track1=gtk_read_entry(WID(entry3));
	track2=gtk_read_entry(WID(entry4));
	gtk_write_entry(WID(entry3), track1);
	gtk_write_entry(WID(entry4), track2);
	
	// kb->Byte
	audioSize=(track1+track2)*1000;
	audioSize/=8;
	
	audioSize*=videoDuration;	
	audioSize>>=20;
	sprintf(string,"%lu",audioSize);
	gtk_label_set_text(GTK_LABEL(WID(labelAudio)),string);
	
	// Compute total size (for Avi)
	uint32_t s74,s80,dvd;
	
	// For avi/ogm 
	int f = getRangeInMenu(WID(optionmenu2));
	if(f==2)
	{ // Mpeg
		s74=730;
		s80=790;
		dvd=4300;
	
	}
	else
	{//AVI or OGM
		s74=690;
		s80=730;
		dvd=4300;
	}
	
	int j=getRangeInMenu(WID(optionmenu1));
	switch(j)
	{
		case 0: totalSize=1*s74;break;
		case 1: totalSize=2*s74;break;
		case 2: totalSize=1*s80;break;
		case 3: totalSize=2*s80;break;
		case 4: totalSize=dvd;break;
		default:
			ADM_assert(0);
	}
	sprintf(string,"%lu",totalSize);
	gtk_label_set_text(GTK_LABEL(WID(labelTotal)),string);
	
	// Compute muxing overhead size
	uint32_t muxingOverheadSize;
	int numberOfAudioTracks = 0;
	int numberOfChunks;
	switch (f)
	{ 
		case 0:
			// AVI
			/*
				Muxing overhead is 8 + 32 = 40 bytes per chunk.
				More or less: numberOfChunks = (x + 1) * numberOfVideoFrames,
				where x - the number of audio tracks
			*/
			if (track1 != 0)
			{
				numberOfAudioTracks++;
			}
			if (track2 != 0)
			{
				numberOfAudioTracks++;
			}
			numberOfChunks = (numberOfAudioTracks + 1) * numberOfVideoFrames;
			muxingOverheadSize = (uint32_t) ceil((numberOfChunks * 40) / 1048576.0);;
			break;
		case 1:
			// OGM
			// Muxing overhead is 1.1% to 1.2% of (videoSize + audioSize)
			muxingOverheadSize = (uint32_t) ceil(totalSize - totalSize / 1.012);
			break;
		case 2:
			// MPEG
			// Muxing overhead is 1%  to 2% of (videoSize + audioSize)
			muxingOverheadSize = (uint32_t) ceil(totalSize - totalSize / 1.02);
			break;
		default:
			ADM_assert(0);
	}
	sprintf(string,"%lu",muxingOverheadSize);
	gtk_label_set_text(GTK_LABEL(WID(labelMux)),string);
	
	// and compute
	//
	uint32_t videoSize;
	
	if(audioSize>=totalSize) sprintf(string,"** NO ROOM LEFT**");
	else
		{
			videoSize=totalSize-audioSize - muxingOverheadSize;
			// Compute average bps
			float avg;
			avg=videoSize;
			avg*=1024.*1024.;
			avg/=videoDuration;
			// now we have byte /sec
			// convert to kb per sec
			avg=(avg*8)/1000;
			videoBitrate=(uint32_t)avg;
			sprintf(string,"%lu",(uint32_t)videoBitrate);
			
			gtk_label_set_text(GTK_LABEL(WID(labelBitrate)),string);
			
			//
			sprintf(string,"%lu",videoSize);
			
		}
	gtk_label_set_text(GTK_LABEL(WID(labelVideo)),string);
	
	
}

GtkWidget*
create_Calculator (void)
{
  GtkWidget *Calculator;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *label2;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *_1x74_cd1;
  GtkWidget *_2x74_cd1;
  GtkWidget *_1x80_cd1;
  GtkWidget *_2x80_cd1;
  GtkWidget *dvd1;
  GtkWidget *hbox3;
  GtkWidget *label3;
  GtkWidget *optionmenu2;
  GtkWidget *menu2;
  GtkWidget *avi1;
  GtkWidget *ogm1;
  GtkWidget *mpeg1;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *hbox4;
  GtkWidget *label6;
  GtkWidget *entry3;
  GtkWidget *label7;
  GtkWidget *entry4;
  GtkWidget *label4;
  GtkWidget *frame3;
  GtkWidget *table1;
  GtkWidget *labelDOO;
  GtkWidget *labelDuration;
  GtkWidget *label12;
  GtkWidget *labelBitrate;
  GtkWidget *labelVideo;
  GtkWidget *label11;
  GtkWidget *label9;
  GtkWidget *labelAudio;
  GtkWidget *labelTotal;
  GtkWidget *label10;
  GtkWidget *label13;
  GtkWidget *labelMux;
  GtkWidget *label8;
  GtkWidget *dialog_action_area1;
  GtkWidget *doit;
  GtkWidget *button1;

  Calculator = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (Calculator), _("Calculator"));

  dialog_vbox1 = GTK_DIALOG (Calculator)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (frame1), hbox1);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), hbox2, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Target Medium :"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_box_pack_start (GTK_BOX (hbox2), optionmenu1, FALSE, FALSE, 0);

  menu1 = gtk_menu_new ();

  _1x74_cd1 = gtk_menu_item_new_with_mnemonic (_("1x74 Cd"));
  gtk_widget_show (_1x74_cd1);
  gtk_container_add (GTK_CONTAINER (menu1), _1x74_cd1);

  _2x74_cd1 = gtk_menu_item_new_with_mnemonic (_("2x74 Cd"));
  gtk_widget_show (_2x74_cd1);
  gtk_container_add (GTK_CONTAINER (menu1), _2x74_cd1);

  _1x80_cd1 = gtk_menu_item_new_with_mnemonic (_("1x80 Cd"));
  gtk_widget_show (_1x80_cd1);
  gtk_container_add (GTK_CONTAINER (menu1), _1x80_cd1);

  _2x80_cd1 = gtk_menu_item_new_with_mnemonic (_("2x80 Cd"));
  gtk_widget_show (_2x80_cd1);
  gtk_container_add (GTK_CONTAINER (menu1), _2x80_cd1);

  dvd1 = gtk_menu_item_new_with_mnemonic (_("DVD5"));
  gtk_widget_show (dvd1);
  gtk_container_add (GTK_CONTAINER (menu1), dvd1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), hbox3, TRUE, TRUE, 0);

  label3 = gtk_label_new (_("Target Format :"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  optionmenu2 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu2);
  gtk_box_pack_start (GTK_BOX (hbox3), optionmenu2, FALSE, FALSE, 0);

  menu2 = gtk_menu_new ();

  avi1 = gtk_menu_item_new_with_mnemonic (_("Avi"));
  gtk_widget_show (avi1);
  gtk_container_add (GTK_CONTAINER (menu2), avi1);

  ogm1 = gtk_menu_item_new_with_mnemonic (_("OGM"));
  gtk_widget_show (ogm1);
  gtk_container_add (GTK_CONTAINER (menu2), ogm1);

  mpeg1 = gtk_menu_item_new_with_mnemonic (_("Mpeg"));
  gtk_widget_show (mpeg1);
  gtk_container_add (GTK_CONTAINER (menu2), mpeg1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu2), menu2);

  label1 = gtk_label_new (_("<b>Target</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (frame2), hbox4);

  label6 = gtk_label_new (_("Track 1 :"));
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (hbox4), label6, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

  entry3 = gtk_entry_new ();
  gtk_widget_show (entry3);
  gtk_box_pack_start (GTK_BOX (hbox4), entry3, TRUE, TRUE, 0);

  label7 = gtk_label_new (_("Track 2 : "));
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox4), label7, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

  entry4 = gtk_entry_new ();
  gtk_widget_show (entry4);
  gtk_box_pack_start (GTK_BOX (hbox4), entry4, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("<b>Audio</b>"));
  gtk_widget_show (label4);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label4);
  gtk_label_set_use_markup (GTK_LABEL (label4), TRUE);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox1), frame3, TRUE, TRUE, 0);

  table1 = gtk_table_new (6, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame3), table1);

  labelDOO = gtk_label_new (_("Duration "));
  gtk_widget_show (labelDOO);
  gtk_table_attach (GTK_TABLE (table1), labelDOO, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelDOO), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelDOO), 0, 0.5);

  labelDuration = gtk_label_new (_("00:00:00"));
  gtk_widget_show (labelDuration);
  gtk_table_attach (GTK_TABLE (table1), labelDuration, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelDuration), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelDuration), 0, 0.5);

  label12 = gtk_label_new (_("Average Video Bitrate (kbps)"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table1), label12, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  labelBitrate = gtk_label_new (_("0"));
  gtk_widget_show (labelBitrate);
  gtk_table_attach (GTK_TABLE (table1), labelBitrate, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelBitrate), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelBitrate), 0, 0.5);

  labelVideo = gtk_label_new (_("0"));
  gtk_widget_show (labelVideo);
  gtk_table_attach (GTK_TABLE (table1), labelVideo, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelVideo), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelVideo), 0, 0.5);

  label11 = gtk_label_new (_("Video Size (MBytes)"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table1), label11, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label9 = gtk_label_new (_("Audio size (MBytes)"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table1), label9, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  labelAudio = gtk_label_new (_("0"));
  gtk_widget_show (labelAudio);
  gtk_table_attach (GTK_TABLE (table1), labelAudio, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelAudio), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelAudio), 0, 0.5);

  labelTotal = gtk_label_new (_("0"));
  gtk_widget_show (labelTotal);
  gtk_table_attach (GTK_TABLE (table1), labelTotal, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelTotal), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelTotal), 0, 0.5);

  label10 = gtk_label_new (_("Total Size (MBytes)"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table1), label10, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label13 = gtk_label_new (_("Muxing overhead size (MBytes)"));
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table1), label13, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  labelMux = gtk_label_new (_("0"));
  gtk_widget_show (labelMux);
  gtk_table_attach (GTK_TABLE (table1), labelMux, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (labelMux), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (labelMux), 0, 0.5);

  label8 = gtk_label_new (_("<b>Result</b>"));
  gtk_widget_show (label8);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label8);
  gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (Calculator)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  doit = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (doit);
  gtk_dialog_add_action_widget (GTK_DIALOG (Calculator), doit, GTK_RESPONSE_APPLY);
  GTK_WIDGET_SET_FLAGS (doit, GTK_CAN_DEFAULT);

  button1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (button1);
  gtk_dialog_add_action_widget (GTK_DIALOG (Calculator), button1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (Calculator, Calculator, "Calculator");
  GLADE_HOOKUP_OBJECT_NO_REF (Calculator, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (Calculator, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (Calculator, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (Calculator, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (Calculator, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (Calculator, label2, "label2");
  GLADE_HOOKUP_OBJECT (Calculator, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (Calculator, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (Calculator, _1x74_cd1, "_1x74_cd1");
  GLADE_HOOKUP_OBJECT (Calculator, _2x74_cd1, "_2x74_cd1");
  GLADE_HOOKUP_OBJECT (Calculator, _1x80_cd1, "_1x80_cd1");
  GLADE_HOOKUP_OBJECT (Calculator, _2x80_cd1, "_2x80_cd1");
  GLADE_HOOKUP_OBJECT (Calculator, dvd1, "dvd1");
  GLADE_HOOKUP_OBJECT (Calculator, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (Calculator, label3, "label3");
  GLADE_HOOKUP_OBJECT (Calculator, optionmenu2, "optionmenu2");
  GLADE_HOOKUP_OBJECT (Calculator, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (Calculator, avi1, "avi1");
  GLADE_HOOKUP_OBJECT (Calculator, ogm1, "ogm1");
  GLADE_HOOKUP_OBJECT (Calculator, mpeg1, "mpeg1");
  GLADE_HOOKUP_OBJECT (Calculator, label1, "label1");
  GLADE_HOOKUP_OBJECT (Calculator, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (Calculator, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (Calculator, label6, "label6");
  GLADE_HOOKUP_OBJECT (Calculator, entry3, "entry3");
  GLADE_HOOKUP_OBJECT (Calculator, label7, "label7");
  GLADE_HOOKUP_OBJECT (Calculator, entry4, "entry4");
  GLADE_HOOKUP_OBJECT (Calculator, label4, "label4");
  GLADE_HOOKUP_OBJECT (Calculator, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (Calculator, table1, "table1");
  GLADE_HOOKUP_OBJECT (Calculator, labelDOO, "labelDOO");
  GLADE_HOOKUP_OBJECT (Calculator, labelDuration, "labelDuration");
  GLADE_HOOKUP_OBJECT (Calculator, label12, "label12");
  GLADE_HOOKUP_OBJECT (Calculator, labelBitrate, "labelBitrate");
  GLADE_HOOKUP_OBJECT (Calculator, labelVideo, "labelVideo");
  GLADE_HOOKUP_OBJECT (Calculator, label11, "label11");
  GLADE_HOOKUP_OBJECT (Calculator, label9, "label9");
  GLADE_HOOKUP_OBJECT (Calculator, labelAudio, "labelAudio");
  GLADE_HOOKUP_OBJECT (Calculator, labelTotal, "labelTotal");
  GLADE_HOOKUP_OBJECT (Calculator, label10, "label10");
  GLADE_HOOKUP_OBJECT (Calculator, label13, "label13");
  GLADE_HOOKUP_OBJECT (Calculator, labelMux, "labelMux");
  GLADE_HOOKUP_OBJECT (Calculator, label8, "label8");
  GLADE_HOOKUP_OBJECT_NO_REF (Calculator, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (Calculator, doit, "doit");
  GLADE_HOOKUP_OBJECT (Calculator, button1, "button1");

  return Calculator;
}

