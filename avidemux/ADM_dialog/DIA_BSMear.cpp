/***************************************************************************
                          DIA_BSMear.cpp  -  description
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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

static GtkWidget	*create_dialog1 (void);
static void  read ( GtkWidget *dialog );
static void  update ( uint8_t *buffer ,uint32_t w,uint32_t h);
static void draw (GtkWidget *dialog,uint32_t w,uint32_t h );
static gboolean gui_draw( void );
static void autocrop (void );
static void upload(GtkWidget *dialog);
static uint32_t blackLine( uint8_t *in, uint32_t width);

#define THRESH   50
#define THRESH2 80
 #define PERCENT 95

extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);


static uint8_t *working=NULL;
static uint8_t *original=NULL;
static GtkWidget *dialog=NULL;
static uint32_t left,right,top,bottom,width,height;
/*
	W: Left
	W2: Right
	h: Top
	h2: Bottom
	Th: total height
	TW: total width

	video : Incoming video in RGB colorspace


*/


#define FILL_ENTRY(widget_name,value) gtk_write_entry(WID(widget_name),  value)

int DIA_getBSMearParams(		uint32_t *w,uint32_t 	*w2, uint32_t *h,uint32_t *h2,
					uint32_t tw,uint32_t th,uint8_t *video)
{
	// Allocate space for green-ised video
	working=new uint8_t [tw*(th)*4];
	memcpy(working,video,tw*th*4);


	uint8_t ret=0;

	dialog=create_dialog1();
	gtk_transient(dialog);
	original=video;
	left=*w;
	right=*w2;
	top=*h;
	bottom=*h2;
	width=tw;
	height=th;

	gtk_widget_set_usize(WID(drawingarea1), tw,th);
	gtk_widget_show(dialog);

	upload(dialog);

	update(working, tw,th);
	 gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
		       GTK_SIGNAL_FUNC(gui_draw),
		       NULL);

	gtk_signal_connect(GTK_OBJECT(WID(buttonAutoCrop)), "clicked",
                      GTK_SIGNAL_FUNC(autocrop),                   NULL);

	draw(dialog,tw,th);

	ret=0;
	int response;
	while( (response=gtk_dialog_run(GTK_DIALOG(dialog)))==GTK_RESPONSE_APPLY)
	{

		read(dialog);
		memcpy(working,video,tw*th*4);
		update(working, tw,th);
		draw(dialog,tw,th);
	}
	if(response==GTK_RESPONSE_OK)
        {
		read(dialog);
		*w=left;
		*w2=right;
		*h=top;
		*h2=bottom;
		ret=1;
	}

	gtk_widget_destroy(dialog);
	delete working;
	working=NULL;
	dialog=NULL;
	original=NULL;
	return ret;

}
/*---------------------------------------------------------------------------
	Green-ify the displayed frame on cropped parts
*/
void update( uint8_t *buffer,uint32_t w,uint32_t h)
{
	uint32_t x,y;
	uint8_t  *in;
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
void read ( GtkWidget *dialog )
{


			left=gtk_read_entry(WID(entryLeft));
			right=gtk_read_entry(WID(entryRight));
			top=gtk_read_entry(WID(entryTop));
			bottom=gtk_read_entry(WID(entryBottom));
			
			/*
			left&=0xffffe;
			right&=0xffffe;
			top&=0xffffe;
			bottom&=0xffffe;
			*/
			
			if((top+bottom)>height)
				{
					top=bottom=0;
				}
			if((left+right)>width)
				{
					left=right=0;
				}
			upload(dialog);
}

void upload(GtkWidget *dialog)
{
	FILL_ENTRY(entryLeft,left);
	FILL_ENTRY(entryRight,right);
	FILL_ENTRY(entryTop,top);
	FILL_ENTRY(entryBottom,bottom);

}
/*----------------------------------
  autocrop
-----------------------------------*/
void autocrop( void )
{
	int stop=0;
        uint32_t sum;
	uint x,y;
	uint8_t *in;
	// float aR,aG,aB;
	uint8_t mR,mG,mB;

	memcpy(working,original,width*height*4);
	mR = mG = mB = 0;

	in=working;
	for(y=0;y<height;y++)
	for(x=0;x<width;x++)
	{
		if(*in<mR) *in=mR-*in; else *in=*in-mR;
		in++;

		if(*in<mG) *in=mG-*in; else *in=*in-mG;
		in++;

		if(*in<mB) *in=mB-*in; else *in=*in-mB;
		in++;
		
		*in++=0;
	}


	in=working;
	printf("top\n");
	for(y=0;y<(height>>1) && stop==0;y++)
	{
		sum=blackLine(in,width);
		top=y;
		printf("top %d : %lu / %lu\n",y,sum,width);
		// if black is more than 90 % it is ok
		// 90%*width< sum

		if(PERCENT*width > 100*sum)
		{
			stop=1;

		}
		in+=4*width;
	}
	// Bottom
	in=working+width*4*(height-1);
	stop=0;
	printf("bottom\n");

	for(y=0;y<(height>>1) && stop==0;y++)
	{
		sum=blackLine(in,width);
		bottom=y;
		printf("bot %d : %lu / %lu\n",y,sum,width);
		if(PERCENT*width > 100*sum)
		{
			stop=1;
		}
		in-=4*width;
	}

	/*
	top=top & 0xfffe;
	bottom=bottom & 0xfffe;
	*/
	upload(dialog);
	update(working, width,height);
	draw(dialog,width,height);
}
/*---------------------------------------------
	Returns the # of "black" pixels
*/
uint32_t blackLine( uint8_t *in, uint32_t width)
{
uint32_t x;
uint32_t sum=0;
		for(x=0;x<width;x++)
		{
			if(    (((*in+*(in+1)+*(in+2))/3) < THRESH)
			&& ( *in< THRESH2 && (*(in+1) < THRESH2) && (*(in+2) < THRESH2)  ) )
			{
				sum++;
			}
			in+=4;
		}
	return sum;
}

GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *entryLeft;
  GtkWidget *entryRight;
  GtkWidget *entryTop;
  GtkWidget *entryBottom;
  GtkWidget *buttonAutoCrop;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *applybutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Add black borders"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Border Left"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Border Right"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Border Top"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Border Bottom"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  entryLeft = gtk_entry_new ();
  gtk_widget_show (entryLeft);
  gtk_table_attach (GTK_TABLE (table1), entryLeft, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryRight = gtk_entry_new ();
  gtk_widget_show (entryRight);
  gtk_table_attach (GTK_TABLE (table1), entryRight, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryTop = gtk_entry_new ();
  gtk_widget_show (entryTop);
  gtk_table_attach (GTK_TABLE (table1), entryTop, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryBottom = gtk_entry_new ();
  gtk_widget_show (entryBottom);
  gtk_table_attach (GTK_TABLE (table1), entryBottom, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  buttonAutoCrop = gtk_button_new_with_mnemonic (_("Autodetect Border"));
  gtk_widget_show (buttonAutoCrop);
  gtk_box_pack_start (GTK_BOX (vbox1), buttonAutoCrop, FALSE, FALSE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (vbox1), drawingarea1, TRUE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  applybutton1 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (applybutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), applybutton1, GTK_RESPONSE_APPLY);
  GTK_WIDGET_SET_FLAGS (applybutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, entryLeft, "entryLeft");
  GLADE_HOOKUP_OBJECT (dialog1, entryRight, "entryRight");
  GLADE_HOOKUP_OBJECT (dialog1, entryTop, "entryTop");
  GLADE_HOOKUP_OBJECT (dialog1, entryBottom, "entryBottom");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAutoCrop, "buttonAutoCrop");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, applybutton1, "applybutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

