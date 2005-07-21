/***************************************************************************
                          ADM_guiRotate.cpp  -  description
                             -------------------
    begin                : Sat Jan 8 2003
    copyright            : (C) 2003 by Tracy Harton
    email                : tracy@amphibious.org
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

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>


#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidRotate.h"

#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include <ADM_assert.h>


extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);


static GtkWidget*  create_dialog1 (void);


static GtkWidget 			*dialog=NULL;
static  GtkObject 			*adj_angle=NULL;

static int 					getRotateParams(	void);
static void 				gui_update(GtkButton * button, gpointer user_data);
static gboolean  			gui_draw(GtkWidget * widget,
	     							GdkEventExpose * event, gpointer user_data);


// Ugly !
static uint8_t  *video_rgb, *video_yuv;
static ADMImage *video_yuv_orig;
static uint32_t orig_w, orig_h;
static ROTATE_PARAM par;

#define WIDG(widget_name) lookup_widget(dialog,#widget_name)

static ColYuvRgb    *rgbConv=NULL;
uint8_t ADMVideoRotate::configure( AVDMGenericVideoStream *instream)

{
  uint32_t w,h,l,f;
  uint8_t ret;

  printf("\n .. configuring rotate...\n");
  video_rgb = NULL;
  video_yuv_orig = NULL;
  video_yuv = NULL;

  // Get info from previous filter
  w= _in->getInfo()->width;
  h= _in->getInfo()->height;
        rgbConv=new ColYuvRgb(w,h);
        rgbConv->reset(w,h);


//  video_yuv_orig=(uint8_t *)malloc(w*h*4);
//  ADM_assert(video_yuv_orig);
	video_yuv_orig=new ADMImage(w,h);
	
  video_rgb=(uint8_t *)ADM_alloc(w*h*4);
  ADM_assert(video_rgb);
  video_yuv=(uint8_t *)ADM_alloc(w*h*4);
  ADM_assert(video_yuv);

  // ask current frame from previous filter
  ADM_assert(instream->getFrameNumberNoAlloc(curframe, &l, video_yuv_orig, &f));

  memcpy(video_yuv, video_yuv_orig->data, (w*h*3)>>1);
  //COL_yv12rgb(w, h,video_yuv, video_rgb);
  rgbConv->scale(video_yuv,video_rgb);

  ADM_assert(_param);
  memcpy(&par,_param,sizeof(par));


  orig_w = par.width = w;
  orig_h = par.height = h;

  switch(ret=getRotateParams())
  {
    case 0:
    printf("cancelled\n");
    break;
    case 1:
    memcpy(_param,&par,sizeof(par));
    break;
    default:
    ADM_assert(0);
  }

  _info.width = _param->width;
  _info.height = _param->height;

  ADM_dealloc(video_rgb);
  delete video_yuv_orig;
  ADM_dealloc(video_yuv);

  video_rgb =  video_yuv = NULL;
  video_yuv_orig =NULL;
  delete rgbConv;
  rgbConv=NULL;
  return ret;
}

//
//
int getRotateParams(	void )
{

  int 		ret;

	ret=0;
	dialog=create_dialog1();
       GTK_ADJUSTMENT(adj_angle)->value= par.angle ;
    	gtk_widget_set_usize(WIDG(drawingarea1), par.width, par.height);

	// add callback for redraw & value changed

 	 gtk_signal_connect(GTK_OBJECT(WIDG(drawingarea1)), "expose_event",
		       GTK_SIGNAL_FUNC(gui_draw),
		       NULL);

	#define DRAG(x)   gtk_signal_connect(GTK_OBJECT(WIDG(hscale1)), #x, \
		       GTK_SIGNAL_FUNC(gui_update), NULL);

	DRAG(drag_data_received);
	DRAG(drag_motion);
	DRAG(drag_data_get);
	DRAG(drag_begin);
	DRAG(value_changed);

	//
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
			  par.angle = GTK_ADJUSTMENT(adj_angle)->value;
			//printf("\n Angle : %3.2f\n", par.angle);
				ret=1;
	}

	gtk_widget_destroy(dialog);

	return ret;

}

//
void gui_update(GtkButton * button, gpointer user_data)
{
  UNUSED_ARG(button);
  UNUSED_ARG(user_data);

  par.angle = GTK_ADJUSTMENT(adj_angle)->value;
  printf("\n Angle : %3.2f\n", par.angle);

  do_rotate(video_yuv_orig->data, orig_w, orig_h, par.angle, video_yuv, &par.width, &par.height);

  printf("w: %ld, h: %ld\n", par.width, par.height);

  //COL_yv12rgb(par.width, par.height,video_yuv, video_rgb);
  rgbConv->reset(par.width,par.height);
  rgbConv->scale(video_yuv,video_rgb);

  gtk_widget_set_usize(WIDG(drawingarea1), par.width, par.height);
  GUI_RGBDisplay(video_rgb, par.width, par.height,( void *)WIDG(drawingarea1));
}

gboolean  gui_draw(GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
  UNUSED_ARG(widget);
  UNUSED_ARG(event);
  UNUSED_ARG(user_data);

  GUI_RGBDisplay(video_rgb, par.width, par.height, (void *)WIDG(drawingarea1));

  return true;
}


GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hscale1;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Rotate"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  adj_angle=gtk_adjustment_new (0.0, 0.0, 360.0, 90.0, 90.0, 90.0);
  hscale1 = gtk_hscale_new (GTK_ADJUSTMENT (adj_angle));
  gtk_widget_show (hscale1);
  gtk_box_pack_start (GTK_BOX (vbox1), hscale1, FALSE, TRUE, 0);
  gtk_scale_set_digits (GTK_SCALE (hscale1), 0);

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

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


#endif
