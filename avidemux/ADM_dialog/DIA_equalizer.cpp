
//
/***************************************************************************
                          DIA_Equalizer
                             -------------------

			   Ui for equalizer, ugly

    begin                : 30 Dec 2004
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

#include "ADM_library/default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"

#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidEqualizer.h"
#include "ADM_assert.h"
static ColYuvRgb    *rgbConv=NULL;
uint8_t DIA_getEqualizer(EqualizerParam *param, ADMImage *image);

static GtkWidget	*create_dialog1 (void);
static void  		update ( void);
static gboolean 	draw (void );
// static void 		reset( void );
static void 		upload(void);
static void  		read ( void );
static void		recalc( void );
static void 		drawCross(uint32_t x,uint32_t y);
static void 		spinner(void);
static void 		compute_histogram(void);
static void 		frame_changed( void );

extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);

static ADMImage *imgsrc,*imgdst,*imgdisplay;
static GtkWidget *dialog=NULL;
static uint32_t scaler[256];
static int32_t points[8];
static uint32_t w,h;
static uint32_t *rgbbuffer=NULL;
static uint32_t *bargraph=NULL;
static uint32_t *histogram=NULL;
static uint32_t *histogramout=NULL;
static AVDMGenericVideoStream *incoming;
static const int cross[8]= {0,36,73,109,
			146,182,219,255};
#define CROSS 0xFFFF0000
#define DRAW  0x0000FF00
#define LINER 0x0000FFFF

#define ZOOM_FACTOR 5
//
//	Video is in YV12 Colorspace
//
//
uint8_t DIA_getEqualizer(EqualizerParam *param, AVDMGenericVideoStream *in)
{
	int ret;
	uint32_t l,f;
	uint32_t max=in->getInfo()->nb_frames;
        
	incoming=in;
	// Allocate space for green-ised video
	w=in->getInfo()->width;
	h=in->getInfo()->height;
	  rgbConv=new ColYuvRgb(w,h);
        rgbConv->reset(w,h);
	rgbbuffer=new uint32_t[w*h];
	bargraph=new uint32_t [256*256];
	histogram=new uint32_t [256*128];
        histogramout=new uint32_t [256*128];
	
	imgdst=new ADMImage(w,h);
	imgsrc=new ADMImage(w,h);
        imgdisplay=new ADMImage(w,h);
	
        if(curframe<max) max=curframe;
        
	ADM_assert(in->getFrameNumberNoAlloc(max, &l, imgsrc,&f));
        memcpy(imgdisplay->data+w*h,imgsrc->data+w*h,(w*h)>>1);
	// init local equalizer
	memcpy(points,param->_points,8*sizeof(uint32_t));	
	equalizerBuildScaler((int32_t *)points,(uint32_t *)scaler);

	dialog=create_dialog1();
	gtk_register_dialog(dialog);

	gtk_widget_set_usize(WID(drawingarea1), w,h);
	gtk_widget_set_usize(WID(histogram), 256,256);
	gtk_widget_set_usize(WID(drawingarea_histin), 256,128);
        gtk_widget_set_usize(WID(drawingarea_histout), 256,128);
	
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonCancel), GTK_RESPONSE_CANCEL);
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(button3),      GTK_RESPONSE_OK);
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonApply),  GTK_RESPONSE_APPLY);
	  upload();
	gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
		       GTK_SIGNAL_FUNC(draw),
		       NULL);
		       
#define CONNECT(x) gtk_signal_connect(GTK_OBJECT(WID(spinbutton##x)), "value_changed",GTK_SIGNAL_FUNC(spinner),   NULL);
	CONNECT(1);
	CONNECT(2);
	CONNECT(3);
	CONNECT(4);
	CONNECT(5);
	CONNECT(6);
	CONNECT(7);
	CONNECT(8);
	
	gtk_signal_connect(GTK_OBJECT(WID(gui_scale)), "value_changed",GTK_SIGNAL_FUNC(frame_changed),   NULL);
	gtk_widget_show(dialog);
	compute_histogram();
	spinner();
	
	

	ret=0;
	int response;
	while( (response=gtk_dialog_run(GTK_DIALOG(dialog)))==GTK_RESPONSE_APPLY)
	{
		spinner();	
	}
	if(response==GTK_RESPONSE_OK)
        {
		printf("Accepting new values\n");
		memcpy(param->_points,points,8*sizeof(uint32_t));
		ret=1;
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	
	delete imgdst;
	delete imgsrc;
        delete imgdisplay;
	delete [] rgbbuffer;
	delete [] bargraph;
	delete [] histogram;
        delete [] histogramout;
    delete rgbConv;
    rgbConv=NULL;    
	histogram=NULL;
        histogramout=NULL;
	bargraph=NULL;
	rgbbuffer=NULL;
	imgdst=NULL;
	imgsrc=NULL;
	dialog=NULL;
        imgdisplay=NULL;
	return ret;

}
void frame_changed( void )
{
uint32_t new_frame,max,l,f;
double   percent;
GtkWidget *wid;	
GtkAdjustment *adj;
	
	max=incoming->getInfo()->nb_frames;
	wid=WID(gui_scale);
	adj=gtk_range_get_adjustment (GTK_RANGE(wid));
	new_frame=0;
	
	percent=(double)GTK_ADJUSTMENT(adj)->value;
	percent*=max;
	percent/=100.;
	new_frame=(uint32_t)floor(percent);
	
	if(new_frame>=max) new_frame=max-1;
	
	ADM_assert(incoming->getFrameNumberNoAlloc(new_frame, &l, imgsrc,&f));
         memcpy(imgdisplay->data+w*h,imgsrc->data+w*h,(w*h)>>1);
	compute_histogram();
	update();

}
void spinner(void)
{
		read();
		recalc();
		upload();
                compute_histogram();
		update();
}
void recalc( void )
{
uint32_t y,tgt;
	// compute the in-between field & display them
	
	equalizerBuildScaler((int32_t *)points,(uint32_t *)scaler);
	// Show bargraph
	memset(bargraph,0,256*256*sizeof(uint32_t));
	for(uint32_t x=0;x<256;x++)
	{
		if(x&1)
		{
			tgt=x+((255-x)*256);
			ADM_assert(tgt<256*256);
			bargraph[tgt]=LINER;	
		}
		else
		{		
			tgt=x+((255-scaler[x])*256);
			ADM_assert(tgt<256*256);
			bargraph[tgt]=DRAW;	
		}
		
	}
	// Pur little cross over our drawing points
	for(uint32_t i=0;i<8;i++)
		drawCross(cross[i],points[i]);
	// and draw
        
        
        
        
	draw();
}
void drawCross(uint32_t x,uint32_t y)
{
uint32_t tgt=(255-y)*256+x;
	if(x>0) bargraph[tgt-1]=CROSS;
	if(x>1) bargraph[tgt-2]=CROSS;
	if(x<255) bargraph[tgt+1]=CROSS;
	if(x<254) bargraph[tgt+2]=CROSS;
	if(y>0) bargraph[tgt-256]=CROSS;
	if(y>1) bargraph[tgt-256*2]=CROSS;
	if(y<255) bargraph[tgt+256]=CROSS;
	if(y<254) bargraph[tgt+256*2]=CROSS;

}
void update( void)
{
	uint8_t *src,*dst,*disp;
	src=imgsrc->data;
	dst=imgdst->data;
	// Only do left side of target
	for(int y=0;y<h;y++)
	{
		for(int x=0;x<w;x++)
		{
			*dst=scaler[*src];
			dst++;
			src++;
		}		
	}
        // Img src = 10
        //           01
        // Img dst= 01
        //          10
        uint32_t half=w>>1;
        
        dst=imgdst->data;
        src=imgsrc->data;
        disp=imgdisplay->data;
        
        for(int y=0;y<h;y++)
        {
                if(y>h/2)
                {
                        memcpy(disp,src,half);
                        memcpy(disp+half,dst+half,half);
                
                
                }
                else
                {
                
                        memcpy(disp,dst,half);
                        memcpy(disp+half,src+half,half);
                }
                src+=w;
                dst+=w;
                disp+=w;
        
        }
	// udate u & v
	// now convert to rgb
	//COL_yv12rgb(  w,   h,imgdisplay->data,(uint8_t *)rgbbuffer );
	rgbConv->scale(imgdisplay->data,(uint8_t *)rgbbuffer );
	draw();
}
// Compute histogram
// Top is histogram in, bottom is histogram out
void compute_histogram(void)
{
	uint32_t value[256];
        uint32_t valueout[256];
        uint8_t v;
        
	memset(value,0,256*sizeof(uint32_t));
        memset(valueout,0,256*sizeof(uint32_t));
        // In
	for(uint32_t t=0;t<w*h;t++)
	{
                v=imgsrc->data[t];
		value[v]++;	
                valueout[scaler[v]]++;
	}
	// normalize
	double d,a;
	a=w*h;
	for(uint32_t i=0;i<256;i++)
	{
		d=value[i];
		d*=256*ZOOM_FACTOR;
		d/=a;
		value[i]=(uint32_t)floor(d+0.49);
		
		if(value[i]>127) value[i]=127;
                
                d=valueout[i];
                d*=256*ZOOM_FACTOR;
                d/=a;
                valueout[i]=(uint32_t)floor(d+0.49);
                
                if(valueout[i]>127) valueout[i]=127;
                
                
	}
	// Draw
	memset(histogram,0,256*128*sizeof(uint32_t));
        memset(histogramout,0,256*128*sizeof(uint32_t));
	uint32_t y,tgt,yout;
	for(uint32_t i=0;i<256;i++)
	{
		y=value[i];
                
		for(uint32_t u=0;u<=y;u++)
		{
			tgt=i+(127-u)*256;
			histogram[tgt]=0xFFFFFFFF;
		}
                
                y=valueout[i];
                
                for(uint32_t u=0;u<=y;u++)
                {
                        tgt=i+(127-u)*256;
                        histogramout[tgt]=0xFFFFFFFF;
                }
	}

        
}
/*---------------------------------------------------------------------------
	Actually draw the working frame on screen
*/
#define SPIN_GET(x,y) {points[x]= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(y))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(y)),(gfloat)points[x]) ;}
gboolean draw (void)
{
	GtkWidget *draw;
	draw=WID(drawingarea1);

	GUI_RGBDisplay((uint8_t *)rgbbuffer, w,h, (void *)draw);
	
	draw=WID(histogram);
	GUI_RGBDisplay((uint8_t *)bargraph, 256,256, (void *)draw);
	
	draw=WID(drawingarea_histin);
	GUI_RGBDisplay((uint8_t *)histogram, 256,128, (void *)draw);
	
        draw=WID(drawingarea_histout);
        GUI_RGBDisplay((uint8_t *)histogramout, 256,128, (void *)draw);
        
	return true;
}

void read ( void)
{
	SPIN_GET(0,spinbutton1);
	SPIN_GET(1,spinbutton2);
	SPIN_GET(2,spinbutton3);
	SPIN_GET(3,spinbutton4);
	SPIN_GET(4,spinbutton5);
	SPIN_GET(5,spinbutton6);
	SPIN_GET(6,spinbutton7);
	SPIN_GET(7,spinbutton8);
	upload();
}

void upload(void)
{
	SPIN_SET(0,spinbutton1);
	SPIN_SET(1,spinbutton2);
	SPIN_SET(2,spinbutton3);
	SPIN_SET(3,spinbutton4);
	SPIN_SET(4,spinbutton5);
	SPIN_SET(5,spinbutton6);
	SPIN_SET(6,spinbutton7);
	SPIN_SET(7,spinbutton8);
	
}


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox2;
  GtkWidget *histogram;
  GtkWidget *frame1;
  GtkWidget *vbox2;
  GtkObject *spinbutton1_adj;
  GtkWidget *spinbutton1;
  GtkObject *spinbutton2_adj;
  GtkWidget *spinbutton2;
  GtkObject *spinbutton3_adj;
  GtkWidget *spinbutton3;
  GtkObject *spinbutton4_adj;
  GtkWidget *spinbutton4;
  GtkObject *spinbutton5_adj;
  GtkWidget *spinbutton5;
  GtkObject *spinbutton6_adj;
  GtkWidget *spinbutton6;
  GtkObject *spinbutton7_adj;
  GtkWidget *spinbutton7;
  GtkObject *spinbutton8_adj;
  GtkWidget *spinbutton8;
  GtkWidget *label1;
  GtkWidget *vbox3;
  GtkWidget *drawingarea_histin;
  GtkWidget *drawingarea_histout;
  GtkWidget *hbox1;
  GtkWidget *buttonCancel;
  GtkWidget *buttonApply;
  GtkWidget *button3;
  GtkWidget *gui_scale;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Equalizer"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  histogram = gtk_drawing_area_new ();
  gtk_widget_show (histogram);
  gtk_box_pack_start (GTK_BOX (hbox2), histogram, FALSE, FALSE, 0);
  gtk_widget_set_size_request (histogram, 100, 100);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (hbox2), frame1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame1), vbox2);

  spinbutton1_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_show (spinbutton1);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton1, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);

  spinbutton2_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton2 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton2_adj), 1, 0);
  gtk_widget_show (spinbutton2);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton2, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton2), TRUE);

  spinbutton3_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton3_adj), 1, 0);
  gtk_widget_show (spinbutton3);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton3, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton3), TRUE);

  spinbutton4_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton4 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton4_adj), 1, 0);
  gtk_widget_show (spinbutton4);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton4, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton4), TRUE);

  spinbutton5_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton5 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton5_adj), 1, 0);
  gtk_widget_show (spinbutton5);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton5, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton5), TRUE);

  spinbutton6_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton6 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton6_adj), 1, 0);
  gtk_widget_show (spinbutton6);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton6, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton6), TRUE);

  spinbutton7_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton7 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton7_adj), 1, 0);
  gtk_widget_show (spinbutton7);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton7, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton7), TRUE);

  spinbutton8_adj = gtk_adjustment_new (1, -255, 255, 1, 10, 10);
  spinbutton8 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton8_adj), 1, 0);
  gtk_widget_show (spinbutton8);
  gtk_box_pack_start (GTK_BOX (vbox2), spinbutton8, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton8), TRUE);

  label1 = gtk_label_new (_("Settings"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 0);

  drawingarea_histin = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea_histin);
  gtk_box_pack_start (GTK_BOX (vbox3), drawingarea_histin, TRUE, TRUE, 0);

  drawingarea_histout = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea_histout);
  gtk_box_pack_start (GTK_BOX (vbox3), drawingarea_histout, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

  buttonCancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (buttonCancel);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonCancel, FALSE, FALSE, 0);

  buttonApply = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (buttonApply);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonApply, FALSE, FALSE, 0);

  button3 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (button3);
  gtk_box_pack_start (GTK_BOX (hbox1), button3, FALSE, FALSE, 0);

  gui_scale = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 100, 1, 10, 10)));
  gtk_widget_show (gui_scale);
  gtk_box_pack_start (GTK_BOX (vbox1), gui_scale, TRUE, TRUE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), drawingarea1, TRUE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea1, 100, 100);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, histogram, "histogram");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton1, "spinbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton2, "spinbutton2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton3, "spinbutton3");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton4, "spinbutton4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton5, "spinbutton5");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton6, "spinbutton6");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton7, "spinbutton7");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton8, "spinbutton8");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea_histin, "drawingarea_histin");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea_histout, "drawingarea_histout");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonCancel, "buttonCancel");
  GLADE_HOOKUP_OBJECT (dialog1, buttonApply, "buttonApply");
  GLADE_HOOKUP_OBJECT (dialog1, button3, "button3");
  GLADE_HOOKUP_OBJECT (dialog1, gui_scale, "gui_scale");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");

  gtk_widget_grab_default (buttonApply);
  return dialog1;
}

