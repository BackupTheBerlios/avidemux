/***************************************************************************
                          ADM_guiRotate.cpp  -  description
                             -------------------
    begin                : Sat Jan 8 2003
    copyright            : (C) 2003 by Tracy Harton / (c) 2006 Mean
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

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidRotate_param.h"

#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include <ADM_assert.h>

void    GUI_RGBDisplay(uint8_t *dis,uint32_t w,uint32_t h,void *widg);

static GtkWidget *create_dialog1 (void);
static GtkWidget *dialog = NULL;

static gboolean gui_draw (GtkWidget * widget,  GdkEventExpose * event, gpointer user_data);
static void gui_update (GtkButton * button, gpointer user_data);
static void calculate(void);
static void draw(void);


// Ugly !
static ColYuvRgb    *rgbConv=NULL;
static ADMImage *video_src,*video_rotated;
static AVDMGenericVideoStream *sstream;
static uint8_t *video_rgb;
static uint32_t ww, hh;
static ROTATE_PARAM par;
static GtkObject *adj_angle;

#define CNX(x,y) gtk_signal_connect(GTK_OBJECT(WID(x)), y, GTK_SIGNAL_FUNC(gui_update), (void *) (1));  
uint8_t DIA_rotate(AVDMGenericVideoStream *astream,ROTATE_PARAM *param)
{
  uint8_t ret=0;
  uint32_t len,flags;
  uint32_t curImage=0;
        sstream=astream;
        memcpy(&par,param,sizeof(par));
        
        ww=astream->getInfo()->width;
        hh=astream->getInfo()->height;
        video_src=new ADMImage(ww,hh);
        video_rotated=new ADMImage(par.width,par.height);
        video_rgb=new uint8_t[ww*hh*4];
        rgbConv=new ColYuvRgb(par.width,par.height);
        rgbConv->reset(par.width,par.height);

        dialog=create_dialog1();
        gtk_register_dialog(dialog);
         if(!astream->getFrameNumberNoAlloc(curImage,&len,video_src,&flags))
        {
          GUI_Error_HIG("Frame error","Cannnot read frame to display");

        }
        else
        {
          calculate ();
          CNX (hscale1, "value_changed");
          gtk_signal_connect (GTK_OBJECT (WID (drawingarea1)), "expose_event",
                            GTK_SIGNAL_FUNC (gui_draw), NULL);
          if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
            {
              memcpy(param,&par,sizeof(par));
              ret = 1;
            }
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        dialog=NULL;
        delete video_src;
        delete video_rotated;
        delete [] video_rgb;
        delete rgbConv;
        return ret;
}
//
//        Check entered values and green-out the selected portion on the screen
//        Each value must be even
//
void calculate( void )
{
  do_rotate(video_src,video_rotated, par.angle);
  par.width=video_rotated->_width;
  par.height=video_rotated->_height;
  rgbConv->reset(par.width,par.height);
  rgbConv->scale(video_rotated->data,video_rgb);
}
void draw(void)
{
  GUI_RGBDisplay (video_rgb, par.width, par.height, (void *) WID (drawingarea1));
  gtk_widget_set_usize (WID (drawingarea1), par.width,par.height);
}
void gui_update (GtkButton * button, gpointer user_data)
{
  UNUSED_ARG (button);
  UNUSED_ARG (user_data);
  uint32_t    sz;

  par.angle = GTK_ADJUSTMENT(adj_angle)->value;
  calculate();
  draw();
 
}

gboolean gui_draw (GtkWidget * widget, GdkEventExpose * event, gpointer user_data)
{
  UNUSED_ARG (widget);
  UNUSED_ARG (event);
  UNUSED_ARG (user_data);
  
  gtk_widget_set_usize (WID (drawingarea1), par.width, par.height);
  GUI_RGBDisplay (video_rgb, par.width, par.height, (void *) WID (drawingarea1));
  return TRUE;
}

//*****************************************
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


