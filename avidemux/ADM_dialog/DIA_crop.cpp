/***************************************************************************
                          DIA_crop.cpp  -  description
                             -------------------

			    GUI for cropping including autocrop
			    +Revisted the Gtk2 way
			     +Autocrop now in RGB space (more accurate)

    begin                : Fri May 3 2002
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


#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"
#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/colorspace.h"
#include "ADM_assert.h"

static GtkWidget	*create_dialog1 (void);
static void  		ui_read ( void);
static void  		ui_update ( void);
static void 		draw (GtkWidget *dialog,uint32_t w,uint32_t h );
static gboolean 	gui_draw( void );
static void 		autocrop (void );
static void 		reset( void );
static void 		ui_upload(void);
static gboolean		ui_changed(void);

static uint8_t 		Metrics( uint8_t *in, uint32_t width,uint32_t *avg, uint32_t *eqt);
static uint8_t 		MetricsV( uint8_t *in, uint32_t width,uint32_t *avg, uint32_t *eqt);

static void		prepare(uint32_t img);
static void 		frame_changed( void );

extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);


static uint8_t *working=NULL;
static uint8_t *original=NULL;
static GtkWidget *dialog=NULL;
static uint32_t left,right,top,bottom,width,height;

static AVDMGenericVideoStream *incoming=NULL;
static ADMImage *imgsrc=NULL;

static int lock=0;
/*
	W: Left
	W2: Right
	h: Top
	h2: Bottom
	Th: total height
	TW: total width

	video : Incoming video in RGB colorspace


*/



//
//	Video is in YV12 Colorspace
//
//
int DIA_getCropParams(	char *name,CROP_PARAMS *param,AVDMGenericVideoStream *in)
{
	// Allocate space for green-ised video
	width=in->getInfo()->width;
	height=in->getInfo()->height;
	
	
	working=new uint8_t [width*height*4];	
	original=NULL;

	uint8_t ret=0;

	dialog=create_dialog1();
	gtk_transient(dialog);
	
	left=param->left;
	right=param->right;
	top=param->top;
	bottom=param->bottom;
	
	imgsrc=new ADMImage(width,height);
	incoming=in;
	
	
	
	gtk_widget_set_usize(WID(drawingarea1), width,height);
	gtk_window_set_title (GTK_WINDOW (dialog), name);
	prepare(0);
	gtk_widget_show(dialog);
	

	ui_upload();

	ui_update();
	
#define CONNECT(x,y,z) 	gtk_signal_connect(GTK_OBJECT(WID(x)), #y,GTK_SIGNAL_FUNC(z),   NULL);

	CONNECT(drawingarea1,expose_event,gui_draw);
	CONNECT(buttonAutocrop,clicked,autocrop);
	CONNECT(buttonReset,clicked,reset);
	CONNECT(scale,value_changed,frame_changed);
			      
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonCancel), GTK_RESPONSE_CANCEL);
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonOk),      GTK_RESPONSE_OK);
	  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonApply),  GTK_RESPONSE_APPLY);
		      
#define CONNECT_SPIN(x) CONNECT(spinbutton##x, value_changed,ui_changed)
      	  
	  CONNECT_SPIN(Top);
	  CONNECT_SPIN(Left);
	  CONNECT_SPIN(Right);
	  CONNECT_SPIN(Bottom);
	  
	  
	draw(dialog,width,height);

	ret=0;
	int response;
	while( (response=gtk_dialog_run(GTK_DIALOG(dialog)))==GTK_RESPONSE_APPLY)
	{
		ui_changed();
		
	}
	if(response==GTK_RESPONSE_OK)
        {
		ui_read( );
		param->left=left;
		param->right=right;
		param->top=top;
		param->bottom=bottom;
		ret=1;
	}

	gtk_widget_destroy(dialog);
	delete working;	
	delete imgsrc;
	working=NULL;
	dialog=NULL;
	original=NULL;
	imgsrc=NULL;
	return ret;
}
void frame_changed( void )
{
uint32_t new_frame,max,l,f;
double   percent;
GtkWidget *wid;	
GtkAdjustment *adj;
	
	max=incoming->getInfo()->nb_frames;
	wid=WID(scale);
	adj=gtk_range_get_adjustment (GTK_RANGE(wid));
	new_frame=0;
	
	percent=(double)GTK_ADJUSTMENT(adj)->value;
	percent*=max;
	percent/=100.;
	new_frame=(uint32_t)floor(percent);
	
	if(new_frame>=max) new_frame=max-1;
	
	prepare(new_frame);
	ui_update();
	gui_draw();
	

}
void prepare(uint32_t img)
{
	uint32_t l,f;
	
	ADM_assert(incoming->getFrameNumberNoAlloc(img,&l,imgsrc,&f));
	original=imgsrc->data;
	

}
gboolean ui_changed(void)
{
	if(!lock)
	{
		ui_read();
		memcpy(working,original,(width*height*3)>>1);
		ui_update();
		draw(dialog,width,height);
	}
		return true;
}


/*---------------------------------------------------------------------------
	Actually draw the working frame on screen
*/
gboolean gui_draw( void )
{
	draw(dialog,width,height);
	return true;
}
void draw (GtkWidget *dialog,uint32_t w,uint32_t h )
{
	GtkWidget *draw;
	draw=WID(drawingarea1);

	GUI_RGBDisplay(working, w,h, (void *)draw);
}
/*---------------------------------------------------------------------------
	Read entried from dialog box
*/

#define SPIN_GET(x,y) {x= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(spinbutton##y))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton##y)),(gfloat)x) ;}


void ui_read (void )
{
	int reject=0;
	
			SPIN_GET(left,Left);
			SPIN_GET(right,Right);
			SPIN_GET(top,Top);
			SPIN_GET(bottom,Bottom);
			
			printf("%d %d %d %d\n",left,right,top,bottom);
			
			left&=0xffffe;
			right&=0xffffe;
			top&=0xffffe;
			bottom&=0xffffe;
			
			if((top+bottom)>height)
				{
					top=bottom=0;
					reject=1;
				}
			if((left+right)>width)
				{
					left=right=0;
					reject=1;
				}
			if(reject)
				ui_upload();
}

void ui_upload(void)
{
	lock++;
	SPIN_SET(left,Left);
	SPIN_SET(right,Right);
	SPIN_SET(top,Top);
	SPIN_SET(bottom,Bottom);
	lock--;
}
//____________________________________
void reset( void )
{
	
	top=bottom=left=right=0;
	ui_upload();
	ui_update();
	gui_draw();
	

}
/*---------------------------------------------------------------------------
	Green-ify the displayed frame on cropped parts
*/
void ui_update( )
{
	uint32_t x,y;
	uint8_t  *in;
	uint32_t w=width,h=height;
	uint8_t *buffer=working;
	
	//
	COL_yv12rgb(  w,   h,original,buffer );
	// do top
	in=buffer;
	for(y=0;y<top;y++)
	{
		for(x=0;x<w;x++)
		{
			*in++=0;
			
			
			*in++=0xff;
			
			*in++=0;
			*in++=0;
		}
	}
	// bottom
	in=buffer+(w*4)*(h-bottom);
	for(y=0;y<bottom;y++)
	{
		for(x=0;x<w;x++)
		{
			*in++=0;
			
			
			*in++=0xff;
			*in++=0;
			*in++=0;
		}
	}
	// left
	in=buffer;
	uint32_t stride=4*w-4;
	for(y=0;y<h;y++)
	{
		for(x=0;x<left;x++)
		{
			*(in+4*x)=0;
			
			
			*(in+4*x+1)=0xff;
			*(in+4*x+2)=0;
			*(in+4*x+3)=0;
		}
		for(x=0;x<right;x++)
		{
			*(in-4*x+stride-4)=0;
			
			
			*(in-4*x+stride-3)=0xff;
			*(in-4*x+stride-2)=0;
			*(in-4*x+stride-1)=0;
			
		}
		in+=4*w;

	}

	//right
}

/*----------------------------------
  autocrop
-----------------------------------*/
void autocrop( void )
{
uint8_t *in;
uint32_t y,avg,eqt;
	// Top
	
#define THRESH_AVG   30
#define THRESH_EQT   50
	
	in=original;
	for(y=0;y<((height>>1)-2);y++)	
	{
		Metrics(in,width,&avg,&eqt);
		in+=width;
		//printf("LineT :%d avg: %d eqt: %d\n",y,avg,eqt);
		if(avg> THRESH_AVG || eqt > THRESH_EQT)
			break;
	}
//gotcha_:	
	if(y)
		top=y-1;
	else 
		top=0;
		
	in=original+width*(height-1);
	for(y=0;y<((height>>1)-2);y++)	
	{
		Metrics(in,width,&avg,&eqt);
		in-=width;
		//printf("Line B :%d avg: %d eqt: %d\n",y,avg,eqt);
		if(avg> THRESH_AVG || eqt > THRESH_EQT)
				break;
	}
//gotcha_:	
	if(y)
		bottom=y-1;
	else
		bottom=0;

		
// Left
	in=original;
	for(y=0;y<((width>>1)-2);y++)	
	{
		MetricsV(in,height,&avg,&eqt);
		in++;
		//printf("Line L :%d avg: %d eqt: %d\n",y,avg,eqt);
		if(avg> THRESH_AVG || eqt > THRESH_EQT)
				break;
	}
//gotcha_:	
	if(y)
		left=y-1;
	else
		left=0;		
// Right
	in=original+width-1;
	for(y=0;y<((width>>1)-2);y++)	
	{
		MetricsV(in,height,&avg,&eqt);
		in--;
		//printf("Line R :%d avg: %d eqt: %d\n",y,avg,eqt);
		if(avg> THRESH_AVG || eqt > THRESH_EQT)
				break;
	}
//gotcha_:	
	if(y)
		right=y-1;
	else
		right=0;

		
	// Update display
	top=top & 0xfffe;
	bottom=bottom & 0xfffe;
	ui_upload();
	ui_update();
	draw(dialog,width,height);
	
	
}
/*---------------------------------------------
	Compute the average value of pixels
	and eqt is the "ecart type"
*/
uint8_t Metrics( uint8_t *in, uint32_t width,uint32_t *avg, uint32_t *eqt)
{

uint32_t x;
uint32_t sum=0,eq=0;
uint8_t v;
		for(x=0;x<width;x++)
		{
			sum+=*(in+x);
		}
		sum=sum/width;
		*avg=sum;
		for(x=0;x<width;x++)
		{
			v=*(in+x)-sum;
			eq+=v*v;
		}
		eq=eq/(width*width);
		*eqt=eq;
		return 1;
}
/*---------------------------------------------
	Compute the average value of pixels
	and eqt is the "ecart type"
*/
uint8_t MetricsV( uint8_t *in, uint32_t height,uint32_t *avg, uint32_t *eqt)
{

uint32_t x;
uint32_t sum=0,eq=0;
uint8_t v;
		for(x=0;x<height;x++)
		{
			sum+=*(in+x*width);
		}
		sum=sum/height;
		*avg=sum;
		for(x=0;x<height;x++)
		{
			v=*(in+x*width)-sum;
			eq+=v*v;
		}
		eq=eq/(height*height);
		*eqt=eq;
		return 1;
}
//--------------------------------------------
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbuttonbox1;
  GtkWidget *buttonCancel;
  GtkWidget *buttonApply;
  GtkWidget *buttonOk;
  GtkWidget *hbuttonbox2;
  GtkWidget *buttonAutocrop;
  GtkWidget *buttonReset;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkObject *spinbuttonLeft_adj;
  GtkWidget *spinbuttonLeft;
  GtkObject *spinbuttonTop_adj;
  GtkWidget *spinbuttonTop;
  GtkObject *spinbuttonBottom_adj;
  GtkWidget *spinbuttonBottom;
  GtkObject *spinbuttonRight_adj;
  GtkWidget *spinbuttonRight;
  GtkWidget *scale;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Crop Settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);

  buttonCancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (buttonCancel);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonCancel);
  GTK_WIDGET_SET_FLAGS (buttonCancel, GTK_CAN_DEFAULT);

  buttonApply = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (buttonApply);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonApply);
  GTK_WIDGET_SET_FLAGS (buttonApply, GTK_CAN_DEFAULT);

  buttonOk = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (buttonOk);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonOk);
  GTK_WIDGET_SET_FLAGS (buttonOk, GTK_CAN_DEFAULT);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox2), GTK_BUTTONBOX_START);

  buttonAutocrop = gtk_button_new_with_mnemonic (_("AutoCrop"));
  gtk_widget_show (buttonAutocrop);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), buttonAutocrop);
  GTK_WIDGET_SET_FLAGS (buttonAutocrop, GTK_CAN_DEFAULT);

  buttonReset = gtk_button_new ();
  gtk_widget_show (buttonReset);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), buttonReset);
  GTK_WIDGET_SET_FLAGS (buttonReset, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttonReset), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-undo", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (_("Reset"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  table1 = gtk_table_new (2, 4, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);

  label2 = gtk_label_new (_("Crop Left :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Crop Top:"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Crop Right:"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Crop Bottom:"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_FILL);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  spinbuttonLeft_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonLeft = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonLeft_adj), 1, 0);
  gtk_widget_show (spinbuttonLeft);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonLeft, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonLeft), TRUE);

  spinbuttonTop_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonTop = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTop_adj), 1, 0);
  gtk_widget_show (spinbuttonTop);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTop, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTop), TRUE);

  spinbuttonBottom_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonBottom = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBottom_adj), 1, 0);
  gtk_widget_show (spinbuttonBottom);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonBottom, 3, 4, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBottom), TRUE);

  spinbuttonRight_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
  spinbuttonRight = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonRight_adj), 1, 0);
  gtk_widget_show (spinbuttonRight);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonRight, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonRight), TRUE);

  scale = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 100, 1, 10, 10)));
  gtk_widget_show (scale);
  gtk_box_pack_start (GTK_BOX (vbox1), scale, FALSE, TRUE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox1), drawingarea1, FALSE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea1, 100, 100);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbuttonbox1, "hbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonCancel, "buttonCancel");
  GLADE_HOOKUP_OBJECT (dialog1, buttonApply, "buttonApply");
  GLADE_HOOKUP_OBJECT (dialog1, buttonOk, "buttonOk");
  GLADE_HOOKUP_OBJECT (dialog1, hbuttonbox2, "hbuttonbox2");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAutocrop, "buttonAutocrop");
  GLADE_HOOKUP_OBJECT (dialog1, buttonReset, "buttonReset");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonLeft, "spinbuttonLeft");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTop, "spinbuttonTop");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBottom, "spinbuttonBottom");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonRight, "spinbuttonRight");
  GLADE_HOOKUP_OBJECT (dialog1, scale, "scale");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");

  return dialog1;
}

