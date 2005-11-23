/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

# include <config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>



#include "default.h"
#include "prefs.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_assert.h" 

#include "DIA_encoding.h"

static GtkWidget *dialog=NULL;
static GtkWidget	*create_dialog1 (void);
static char string[500];
extern uint8_t DIA_Paused( void );
extern void UI_deiconify( void );
extern void UI_iconify( void );
static gint on_destroy_abort(GtkObject * object, gpointer user_data);
static void DIA_stop( void);

static uint8_t stopReq=0;
DIA_encoding::DIA_encoding( uint32_t fps1000 )
{
uint32_t useTray=0;
        if(!prefs->get(FEATURE_USE_SYSTRAY,&useTray)) useTray=0;

	ADM_assert(dialog==NULL);
	stopReq=0;
	
	_totalSize=0;
	_audioSize=0;
	_videoSize=0;
	_current=0;
	setFps(fps1000);
	dialog=create_dialog1();
        
	gtk_register_dialog(dialog);
	//gtk_transient(dialog);
	gtk_signal_connect(GTK_OBJECT(WID(closebutton1)), "clicked",
                      GTK_SIGNAL_FUNC(DIA_stop),                   NULL);
	gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
		       GTK_SIGNAL_FUNC(on_destroy_abort), NULL);
	gtk_widget_show(dialog);
//	gtk_window_set_modal(GTK_WINDOW(dialog), 1);
        if(useTray)
	       UI_iconify();
	_lastTime=0;
	_lastFrame=0;
	_fps_average=0;
        tray=NULL;
        
        if(useTray)
                tray=new ADM_tray("Encoding");

}
void DIA_encoding::setFps(uint32_t fps)
{
	_roundup=(uint32_t )floor( (fps+999)/1000);
        ADM_assert(_roundup<MAX_BR_SLOT);
	for(uint32_t i=0;i<_roundup;i++)
		_bitrate[i]=0;
  _bitrate_sum=0;
	
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
        if(tray) delete tray;
        tray=NULL;
	ADM_assert(dialog);
	gtk_unregister_dialog(dialog);
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
void DIA_encoding::setContainer(const char *container)
{
        ADM_assert(dialog);
        gtk_label_set_text(GTK_LABEL(WID(label_container)),container);
}
void DIA_encoding::setFrame(uint32_t nb,uint32_t total)
{
	uint32_t tim;
#define  ETA_SAMPLE_PERIOD 60000 //Use last n millis to calculate ETA
#define  GUI_UPDATE_RATE 1000  
  static uint32_t _lastnb=0;

	   ADM_assert(dialog);
     	   //
           //	nb/total=timestart/totaltime -> total time =timestart*total/nb
           //
           //
    
           if(nb < _lastnb || _lastnb == 0) // restart ?
           {
             _lastnb = nb;
                                       
					clock.reset();
		_lastTime=clock.getElapsedMS();
       					_lastFrame=0;
       					_fps_average=0;

                _nextUpdate = _lastTime + GUI_UPDATE_RATE;
                _nextSampleStartTime=_lastTime + ETA_SAMPLE_PERIOD;
                _nextSampleStartFrame=0;
                                        if(tray)
                                                tray->setPercent(0);
					  UI_purge();
					  return;
	  }
             _lastnb = nb;

	tim=clock.getElapsedMS();
	//   printf("%lu / %lu\n",tim,_lastTime);
	   if(_lastTime > tim) return;
	   if( tim < _nextUpdate) return ; 
     _nextUpdate = tim+GUI_UPDATE_RATE;

	sprintf(string,"%lu/%lu",nb,total);
	gtk_label_set_text(GTK_LABEL(WID(label_frame)),string);

	   	// compute fps
		uint32_t deltaFrame, deltaTime;
		deltaTime=tim-_lastTime;
		deltaFrame=nb-_lastFrame;

			sprintf(string,"%lu",(uint32_t)( deltaFrame*1000.0 / deltaTime ));
   			gtk_label_set_text(GTK_LABEL(WID(label_fps)),string);
                        



		uint32_t   hh,mm,ss;

          double framesLeft=(total-nb);
					ms2time((uint32_t)floor(0.5+deltaTime*framesLeft/deltaFrame),&hh,&mm,&ss);
					sprintf(string,"%02d:%02d:%02d",hh,mm,ss);
					gtk_label_set_text(GTK_LABEL(WID(label_eta)),string);

           // Check if we should move on to the next sample period
          if (tim >= _nextSampleStartTime + ETA_SAMPLE_PERIOD ) {
            _lastTime=_nextSampleStartTime;
            _lastFrame=_nextSampleStartFrame;
            _nextSampleStartTime=tim;
            _nextSampleStartFrame=0;
          } else if (tim >= _nextSampleStartTime && _nextSampleStartFrame == 0 ) {
            // Store current point for use later as the next sample period.
            //
            _nextSampleStartTime=tim;
            _nextSampleStartFrame=nb;
          }
		// update progress bar
		 float f=nb;
		 f=f/total;
                if(tray)
                        tray->setPercent((int)(f*100.));
		gtk_progress_set_percentage(GTK_PROGRESS(WID(progressbar1)),(gfloat)f);

		sprintf(string,"Done : %02d%%",(int)(100*f));
		   gtk_progress_bar_set_text       (GTK_PROGRESS_BAR(WID(progressbar1)), string);
		

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

  _bitrate_sum += (size - _bitrate[_current]); //Subtract old value and add new one to total

	_bitrate[_current++]=size;
	_current%=_roundup;

  br=_bitrate_sum;

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

	if(stopReq)
	{
		if(DIA_Paused(  ))		//=GUI_Question("Continue encoding, no will stop it ?"))
		{
			stopReq=0;
		}		
	}

	if(!stopReq) return 1;
	return 0;
}

/*-----------------------------------------------------------*/

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox2;
  GtkWidget *label24;
  GtkWidget *table4;
  GtkWidget *label25;
  GtkWidget *label26;
  GtkWidget *label_phasis;
  GtkWidget *label_frame;
  GtkWidget *label10;
  GtkWidget *label12;
  GtkWidget *label21;
  GtkWidget *label23;
  GtkWidget *Quantizer;
  GtkWidget *label15;
  GtkWidget *label17;
  GtkWidget *label19;
  GtkWidget *label_vcodec;
  GtkWidget *label_acodec;
  GtkWidget *label_fps;
  GtkWidget *label_bitrate;
  GtkWidget *label_quant;
  GtkWidget *label_vsize;
  GtkWidget *label_asize;
  GtkWidget *label_size;
  GtkWidget *label34;
  GtkWidget *label_eta;
  GtkWidget *progressbar1;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label33;
  
   GtkWidget *label_c1,*label_container,*label90;

  dialog1 = gtk_dialog_new ();
  gtk_widget_set_size_request (dialog1, 500, -1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 13);
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Encoding in Progress"));
  gtk_dialog_set_has_separator (GTK_DIALOG (dialog1), FALSE);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox2 = gtk_vbox_new (FALSE, 12);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox2, TRUE, TRUE, 0);

  label24 = gtk_label_new (_("<b><big>Encoding files</big></b>"));
  gtk_widget_show (label24);
  gtk_box_pack_start (GTK_BOX (vbox2), label24, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label24), TRUE);
  gtk_label_set_justify (GTK_LABEL (label24), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  table4 = gtk_table_new (11, 2, FALSE);
  gtk_widget_show (table4);
  gtk_box_pack_start (GTK_BOX (vbox2), table4, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table4), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table4), 5);

  label25 = gtk_label_new (_("<b>Phasis:</b>"));
  gtk_widget_show (label25);
  gtk_table_attach (GTK_TABLE (table4), label25, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label25), TRUE);
  gtk_label_set_justify (GTK_LABEL (label25), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (label25), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label25), 12, 0);

  label26 = gtk_label_new (_("<b>Frame:</b>"));
  gtk_widget_show (label26);
  gtk_table_attach (GTK_TABLE (table4), label26, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label26), TRUE);
  gtk_label_set_justify (GTK_LABEL (label26), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment (GTK_MISC (label26), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label26), 12, 0);

  label_phasis = gtk_label_new (_("None"));
  gtk_widget_show (label_phasis);
  gtk_table_attach (GTK_TABLE (table4), label_phasis, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_phasis), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_phasis), 0, 0.5);

  label_frame = gtk_label_new (_("00000/00000"));
  gtk_widget_show (label_frame);
  gtk_table_attach (GTK_TABLE (table4), label_frame, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_frame), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_frame), 0, 0.5);

  label10 = gtk_label_new (_("<b>Vid codec:</b>"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table4), label10, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label10, 75, -1);
  gtk_label_set_use_markup (GTK_LABEL (label10), TRUE);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label10), 12, 0);

  label12 = gtk_label_new (_("<b>Aud Codec:</b>"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table4), label12, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label12), TRUE);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label12), 12, 0);

  label21 = gtk_label_new (_("<b>FPS:</b>"));
  gtk_widget_show (label21);
  gtk_table_attach (GTK_TABLE (table4), label21, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label21), TRUE);
  gtk_label_set_justify (GTK_LABEL (label21), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label21), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label21), 12, 0);

  label23 = gtk_label_new (_("<b>Vid Bitrate:</b>"));
  gtk_widget_show (label23);
  gtk_table_attach (GTK_TABLE (table4), label23, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label23), TRUE);
  gtk_label_set_justify (GTK_LABEL (label23), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label23), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label23), 12, 0);

  Quantizer = gtk_label_new (_("<b>Quantizer:</b>"));
  gtk_widget_show (Quantizer);
  gtk_table_attach (GTK_TABLE (table4), Quantizer, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (Quantizer), TRUE);
  gtk_label_set_justify (GTK_LABEL (Quantizer), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (Quantizer), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (Quantizer), 12, 0);

  label15 = gtk_label_new (_("<b>Video Size:</b>"));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table4), label15, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label15, 75, -1);
  gtk_label_set_use_markup (GTK_LABEL (label15), TRUE);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label15), 12, 0);

  label17 = gtk_label_new (_("<b>Audio Size:</b>"));
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table4), label17, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label17), TRUE);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label17), 12, 0);

  label19 = gtk_label_new (_("<b>Total Size:</b>"));
  gtk_widget_show (label19);
  gtk_table_attach (GTK_TABLE (table4), label19, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label19), TRUE);
  gtk_label_set_justify (GTK_LABEL (label19), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label19), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label19), 12, 0);

  label_vcodec = gtk_label_new (_("None"));
  gtk_widget_show (label_vcodec);
  gtk_table_attach (GTK_TABLE (table4), label_vcodec, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_vcodec), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_vcodec), 0, 0.5);

  label_acodec = gtk_label_new (_("None"));
  gtk_widget_show (label_acodec);
  gtk_table_attach (GTK_TABLE (table4), label_acodec, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_acodec), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_acodec), 0, 0.5);

  label_fps = gtk_label_new (_("0"));
  gtk_widget_show (label_fps);
  gtk_table_attach (GTK_TABLE (table4), label_fps, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_fps), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_fps), 0, 0.5);

  label_bitrate = gtk_label_new (_("0"));
  gtk_widget_show (label_bitrate);
  gtk_table_attach (GTK_TABLE (table4), label_bitrate, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_bitrate), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_bitrate), 0, 0.5);

  label_quant = gtk_label_new (_("31"));
  gtk_widget_show (label_quant);
  gtk_table_attach (GTK_TABLE (table4), label_quant, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_quant), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_quant), 0, 0.5);

  label_vsize = gtk_label_new (_("0"));
  gtk_widget_show (label_vsize);
  gtk_table_attach (GTK_TABLE (table4), label_vsize, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_vsize), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_vsize), 0, 0.5);

  label_asize = gtk_label_new (_("0"));
  gtk_widget_show (label_asize);
  gtk_table_attach (GTK_TABLE (table4), label_asize, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_asize), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_asize), 0, 0.5);

  label_size = gtk_label_new (_("0"));
  gtk_widget_show (label_size);
  gtk_table_attach (GTK_TABLE (table4), label_size, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_size), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_size), 0, 0.5);

  label34 = gtk_label_new (_("<b>ETA:</b>"));
  gtk_widget_show (label34);
  gtk_table_attach (GTK_TABLE (table4), label34, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label34), TRUE);
  gtk_label_set_justify (GTK_LABEL (label34), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label34), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label34), 12, 0);

  label_eta = gtk_label_new (_("0 h 0 mn 0 s"));
  gtk_widget_show (label_eta);
  gtk_table_attach (GTK_TABLE (table4), label_eta, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_eta), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_eta), 0, 0.5);
/*

*/
  label90 = gtk_label_new (_("<b>Container:</b>"));
  gtk_widget_show (label90);
  gtk_table_attach (GTK_TABLE (table4), label90, 0, 1, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label90), TRUE);
  gtk_label_set_justify (GTK_LABEL (label90), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label90), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label90), 12, 0);

  label_container = gtk_label_new (_("None"));
  gtk_widget_show (label_container);
  gtk_table_attach (GTK_TABLE (table4), label_container, 1, 2, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label_container), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_container), 0, 0.5);
/*

*/  

  progressbar1 = gtk_progress_bar_new ();
  gtk_widget_show (progressbar1);
  gtk_box_pack_start (GTK_BOX (vbox2), progressbar1, FALSE, FALSE, 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar1), _("00:00:00 Time Left"));

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new ();
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (closebutton1), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label33 = gtk_label_new_with_mnemonic (_("Pause / Abort"));
  gtk_widget_show (label33);
  gtk_box_pack_start (GTK_BOX (hbox1), label33, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label33), GTK_JUSTIFY_LEFT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
  GLADE_HOOKUP_OBJECT (dialog1, table4, "table4");
  GLADE_HOOKUP_OBJECT (dialog1, label25, "label25");
  GLADE_HOOKUP_OBJECT (dialog1, label26, "label26");
  GLADE_HOOKUP_OBJECT (dialog1, label_phasis, "label_phasis");
  GLADE_HOOKUP_OBJECT (dialog1, label_frame, "label_frame");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, label21, "label21");
  GLADE_HOOKUP_OBJECT (dialog1, label23, "label23");
  GLADE_HOOKUP_OBJECT (dialog1, Quantizer, "Quantizer");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog1, label19, "label19");
  GLADE_HOOKUP_OBJECT (dialog1, label_vcodec, "label_vcodec");
  GLADE_HOOKUP_OBJECT (dialog1, label_acodec, "label_acodec");
  GLADE_HOOKUP_OBJECT (dialog1, label_fps, "label_fps");
  GLADE_HOOKUP_OBJECT (dialog1, label_bitrate, "label_bitrate");
  GLADE_HOOKUP_OBJECT (dialog1, label_quant, "label_quant");
  GLADE_HOOKUP_OBJECT (dialog1, label_vsize, "label_vsize");
  GLADE_HOOKUP_OBJECT (dialog1, label_asize, "label_asize");
  GLADE_HOOKUP_OBJECT (dialog1, label_size, "label_size");
  GLADE_HOOKUP_OBJECT (dialog1, label34, "label34");
  GLADE_HOOKUP_OBJECT (dialog1, label_eta, "label_eta");
  GLADE_HOOKUP_OBJECT (dialog1, progressbar1, "progressbar1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label33, "label33");
  GLADE_HOOKUP_OBJECT (dialog1, label_container, "label_container");

  return dialog1;
}

