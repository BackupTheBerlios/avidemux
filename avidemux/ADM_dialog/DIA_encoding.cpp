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

#include "ADM_assert.h" 
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

# include <config.h>

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


#include "DIA_encoding.h"

static GtkWidget *dialog=NULL;
static GtkWidget	*create_dialog1 (void);
static char string[500];

extern void UI_deiconify( void );
extern void UI_iconify( void );
static gint on_destroy_abort(GtkObject * object, gpointer user_data);
static void DIA_stop( void);

static uint8_t stopReq=0;
DIA_encoding::DIA_encoding( uint32_t fps1000 )
{
	ADM_assert(dialog==NULL);
	stopReq=0;
	
	_totalSize=0;
	_audioSize=0;
	_videoSize=0;
	_current=0;
	setFps(fps1000);
	dialog=create_dialog1();
	//gtk_transient(dialog);
	gtk_signal_connect(GTK_OBJECT(WID(closebutton1)), "clicked",
                      GTK_SIGNAL_FUNC(DIA_stop),                   NULL);
	gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		       GTK_SIGNAL_FUNC(on_destroy_abort), NULL);
	gtk_widget_show(dialog);
	gtk_window_set_modal(GTK_WINDOW(dialog), 1);
	UI_iconify();
	_lastTime=0;
	_lastFrame=0;
	_fps_average=0;

}
void DIA_encoding::setFps(uint32_t fps)
{
	_roundup=(uint32_t )floor( (fps+999)/1000);
	for(uint32_t i=0;i<_roundup;i++)
		_bitrate[i]=0;
	
}
gint on_destroy_abort(GtkObject * object, gpointer user_data)
{


	UNUSED_ARG(object);
	UNUSED_ARG(user_data);

	stopReq=1;
	return TRUE;

};

void DIA_stop( void)
{
	printf("Stop request\n");
	stopReq=1;
}
DIA_encoding::~DIA_encoding( )
{
	ADM_assert(dialog);
	gtk_widget_destroy(dialog);
	dialog=NULL;
	UI_deiconify();

}
void DIA_encoding::setPhasis(const char *n)
{
	   ADM_assert(dialog);
  	   gtk_label_set_text(GTK_LABEL(WID(label_phasis)),n);

}
void DIA_encoding::setAudioCodec(const char *n)
{
	   ADM_assert(dialog);
  	   gtk_label_set_text(GTK_LABEL(WID(label_acodec)),n);

}

void DIA_encoding::setCodec(const char *n)
{
	   ADM_assert(dialog);
  	   gtk_label_set_text(GTK_LABEL(WID(label_vcodec)),n);

}
void DIA_encoding::setBitrate(uint32_t br)
{
	   ADM_assert(dialog);
	   sprintf(string,"%lu kbps",br);
  	   gtk_label_set_text(GTK_LABEL(WID(label_bitrate)),string);

}
void DIA_encoding::setQuant(uint32_t quant)
{
	   ADM_assert(dialog);
	   sprintf(string,"%lu",quant);
  	   gtk_label_set_text(GTK_LABEL(WID(label_quant)),string);
}
void DIA_encoding::reset(void)
{
	   ADM_assert(dialog);
	   _totalSize=0;
	   _videoSize=0;
	   _current=0;
}
void DIA_encoding::setFrame(uint32_t nb,uint32_t total)
{
	uint32_t tim;

	   ADM_assert(dialog);
	   sprintf(string,"%lu/%lu",nb,total);
  	   gtk_label_set_text(GTK_LABEL(WID(label_frame)),string);
     	   //
           //	nb/total=timestart/totaltime -> total time =timestart*total/nb
           //
           //

           if(nb==0) // restart ?
           {
					clock.reset();
       					_lastTime=clock.getElapsedMS();;
       					_lastFrame=0;
       					_fps_average=0;
					  UI_purge();
					  return;
	  }

           tim=clock.getElapsedMS();;
	//   printf("%lu / %lu\n",tim,_lastTime);
	   if(_lastTime > tim) return;
	   if( tim-_lastTime < 1000) return ; // refresh every 3  seconds

           if(tim)
           {
	   	double d;
		uint32_t fps;

	   	// compute fps
		uint32_t deltaFrame, deltaTime;
		deltaTime=tim-_lastTime;
		deltaFrame=nb-_lastFrame;
		if(deltaTime>500)
		{
			d=deltaTime;
			d=1/d;
			d*=deltaFrame;
			d*=1000.;
			fps=(uint32_t)floor(d);
			sprintf(string,"%lu",fps);
   			gtk_label_set_text(GTK_LABEL(WID(label_fps)),string);

		}


	   	uint32_t sectogo,secdone;
		uint32_t   hh,mm,ss;

	  				secdone = tim;
			 	  	d = secdone;
	  				d /= nb;
       					d*=total;
	  				d -= secdone;
					_lastTime=tim;
					tim=(uint32_t)floor(d);
					ms2time(tim,&hh,&mm,&ss);
					sprintf(string,"%02d:%02d:%02d",hh,mm,ss);
					gtk_label_set_text(GTK_LABEL(WID(label_eta)),string);

              }

		// update progress bar
		 float f=nb;
		 f=f/total;
		gtk_progress_set_percentage(GTK_PROGRESS(WID(progressbar1)),(gfloat)f);

		_lastFrame=nb;

	   	UI_purge();

}

void DIA_encoding::setSize(int size)
{
	ADM_assert(dialog);
	   sprintf(string,"%lu",size);
  	   gtk_label_set_text(GTK_LABEL(WID(label_size)),string);

}
void DIA_encoding::setAudioSize(int size)
{
	ADM_assert(dialog);
	   sprintf(string,"%lu",size);
  	   gtk_label_set_text(GTK_LABEL(WID(label_asize)),string);

}
void DIA_encoding::setVideoSize(int size)
{
	ADM_assert(dialog);
	   sprintf(string,"%lu",size);
  	   gtk_label_set_text(GTK_LABEL(WID(label_vsize)),string);

}
void DIA_encoding::feedFrame(uint32_t size)
{
	uint32_t br;
	ADM_assert(dialog);
	_totalSize+=size;
	_videoSize+=size;
	_bitrate[_current++]=size;
	_current%=_roundup;
	br=0;
	for(uint32_t i=0;i<_roundup;i++)
		br+=_bitrate[i];
	br=(br*8)/1000;		
	setBitrate(br);
	setSize(_totalSize>>20);
	setVideoSize(_videoSize>>20);
	//printf("Bitrate : %lu \n,Size: %lu\n",br,_totalSize);

}
void DIA_encoding::feedAudioFrame(uint32_t size)
{
	uint32_t br;
	ADM_assert(dialog);
	_totalSize+=size;
	_audioSize+=size;
	setSize(_totalSize>>20);
	setAudioSize(_audioSize>>20);
	

}
uint8_t DIA_encoding::isAlive( void )
{
#ifndef CYG_MANGLING
	if(stopReq)
	{
		if(GUI_Question("Continue encoding, no will stop it ?"))
		{
			stopReq=0;
		}		
	}
#endif	
	if(!stopReq) return 1;
	return 0;
}

/*-----------------------------------------------------------*/

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
  GtkWidget *label5;
  GtkWidget *label_phasis;
  GtkWidget *label_frame;
  GtkWidget *label_eta;
  GtkWidget *progressbar1;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table2;
  GtkWidget *label10;
  GtkWidget *label_vcodec;
  GtkWidget *label12;
  GtkWidget *label_acodec;
  GtkWidget *label21;
  GtkWidget *label_fps;
  GtkWidget *label23;
  GtkWidget *label_bitrate;
  GtkWidget *Quantizer;
  GtkWidget *label_quant;
  GtkWidget *Informations;
  GtkWidget *frame3;
  GtkWidget *table3;
  GtkWidget *label15;
  GtkWidget *label_vsize;
  GtkWidget *label17;
  GtkWidget *label_asize;
  GtkWidget *label19;
  GtkWidget *label_size;
  GtkWidget *label14;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Encoding in Progress"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  label2 = gtk_label_new (_("Phasis"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Frame"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Time Left"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label4, 75, -1);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Progress"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label_phasis = gtk_label_new (_("None"));
  gtk_widget_show (label_phasis);
  gtk_table_attach (GTK_TABLE (table1), label_phasis, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_phasis), 0, 0.5);

  label_frame = gtk_label_new (_("00000/00000"));
  gtk_widget_show (label_frame);
  gtk_table_attach (GTK_TABLE (table1), label_frame, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_frame), 0, 0.5);

  label_eta = gtk_label_new (_("00:00:00"));
  gtk_widget_show (label_eta);
  gtk_table_attach (GTK_TABLE (table1), label_eta, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_eta), 0, 0.5);

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_table_attach (GTK_TABLE (table1), progressbar1, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar1), _("Done"));

  label1 = gtk_label_new (_("Status"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame2), table2);

  label10 = gtk_label_new (_("Vid codec"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label10, 75, -1);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label_vcodec = gtk_label_new (_("None"));
  gtk_widget_show (label_vcodec);
  gtk_table_attach (GTK_TABLE (table2), label_vcodec, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_vcodec), 0, 0.5);

  label12 = gtk_label_new (_("Aud Codec"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  label_acodec = gtk_label_new (_("None"));
  gtk_widget_show (label_acodec);
  gtk_table_attach (GTK_TABLE (table2), label_acodec, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_acodec), 0, 0.5);

  label21 = gtk_label_new (_("FPS"));
  gtk_widget_show (label21);
  gtk_table_attach (GTK_TABLE (table2), label21, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label21), 0, 0.5);

  label_fps = gtk_label_new (_("0"));
  gtk_widget_show (label_fps);
  gtk_table_attach (GTK_TABLE (table2), label_fps, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_fps), 0, 0.5);

  label23 = gtk_label_new (_("Vid Bitrate"));
  gtk_widget_show (label23);
  gtk_table_attach (GTK_TABLE (table2), label23, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label23), 0, 0.5);

  label_bitrate = gtk_label_new (_("0"));
  gtk_widget_show (label_bitrate);
  gtk_table_attach (GTK_TABLE (table2), label_bitrate, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_bitrate), 0, 0.5);

  Quantizer = gtk_label_new (_("Quantizer"));
  gtk_widget_show (Quantizer);
  gtk_table_attach (GTK_TABLE (table2), Quantizer, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (Quantizer), 0, 0.5);

  label_quant = gtk_label_new (_("31"));
  gtk_widget_show (label_quant);
  gtk_table_attach (GTK_TABLE (table2), label_quant, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_quant), 0, 0.5);

  Informations = gtk_label_new (_("Infos"));
  gtk_widget_show (Informations);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), Informations);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox1), frame3, TRUE, TRUE, 0);

  table3 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (frame3), table3);

  label15 = gtk_label_new (_("Video Size"));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table3), label15, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label15, 75, -1);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  label_vsize = gtk_label_new (_("0"));
  gtk_widget_show (label_vsize);
  gtk_table_attach (GTK_TABLE (table3), label_vsize, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_vsize), 0, 0.5);

  label17 = gtk_label_new (_("Audio Size"));
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table3), label17, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

  label_asize = gtk_label_new (_("0"));
  gtk_widget_show (label_asize);
  gtk_table_attach (GTK_TABLE (table3), label_asize, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_asize), 0, 0.5);

  label19 = gtk_label_new (_("Total Size"));
  gtk_widget_show (label19);
  gtk_table_attach (GTK_TABLE (table3), label19, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label19), 0, 0.5);

  label_size = gtk_label_new (_("0"));
  gtk_widget_show (label_size);
  gtk_table_attach (GTK_TABLE (table3), label_size, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label_size), 0, 0.5);

  label14 = gtk_label_new (_("Size"));
  gtk_widget_show (label14);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label14);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label_phasis, "label_phasis");
  GLADE_HOOKUP_OBJECT (dialog1, label_frame, "label_frame");
  GLADE_HOOKUP_OBJECT (dialog1, label_eta, "label_eta");
  GLADE_HOOKUP_OBJECT (dialog1, progressbar1, "progressbar1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label_vcodec, "label_vcodec");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, label_acodec, "label_acodec");
  GLADE_HOOKUP_OBJECT (dialog1, label21, "label21");
  GLADE_HOOKUP_OBJECT (dialog1, label_fps, "label_fps");
  GLADE_HOOKUP_OBJECT (dialog1, label23, "label23");
  GLADE_HOOKUP_OBJECT (dialog1, label_bitrate, "label_bitrate");
  GLADE_HOOKUP_OBJECT (dialog1, Quantizer, "Quantizer");
  GLADE_HOOKUP_OBJECT (dialog1, label_quant, "label_quant");
  GLADE_HOOKUP_OBJECT (dialog1, Informations, "Informations");
  GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, label_vsize, "label_vsize");
  GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog1, label_asize, "label_asize");
  GLADE_HOOKUP_OBJECT (dialog1, label19, "label19");
  GLADE_HOOKUP_OBJECT (dialog1, label_size, "label_size");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  return dialog1;
}

