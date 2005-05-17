/**/
/***************************************************************************
                          DIA_hue
                             -------------------

			   Ui for hue & sat

    begin                : 08 Apr 2005
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

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidHue.h"
#include "ADM_assert.h"

uint8_t DIA_getHue(Hue_Param *param, ADMImage *image);

static GtkWidget	*create_dialog1 (void);
static void  		update ( void);
static gboolean 	draw (void );
static void 		upload(void);
static void 		download(void);
static void  		read ( void );
static void		recalc( void );
static void 		frame_changed( void );
static void             hue_changed( void);
extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);

static ADMImage *imgsrc,*imgdst,*imgdisplay;
static GtkWidget *dialog=NULL;
static uint32_t w,h;
static uint32_t *rgbbuffer=NULL;
static AVDMGenericVideoStream *incoming;
static Hue_Param  myHue;
static float      hue,sat;

//
//	Video is in YV12 Colorspace
//
//
uint8_t DIA_getHue(Hue_Param *param, AVDMGenericVideoStream *in)
{
        int ret;
        uint32_t l,f;
        uint32_t max=in->getInfo()->nb_frames;
        
        incoming=in;
        // Allocate space for green-ised video
        w=in->getInfo()->width;
        h=in->getInfo()->height;

        rgbbuffer=new uint32_t[w*h];

        imgdst=new ADMImage(w,h);
        imgsrc=new ADMImage(w,h);
        imgdisplay=new ADMImage(w,h);

        if(curframe<max) max=curframe;
        
        ADM_assert(in->getFrameNumberNoAlloc(max, &l, imgsrc,&f));
        // Luma is not changed by this filter
        memcpy(YPLANE(imgdisplay),YPLANE(imgsrc),w*h);
        memcpy(&myHue,param,sizeof(myHue));
        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        gtk_widget_set_usize(WID(drawingarea1), w,h);



        upload();
        gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
            GTK_SIGNAL_FUNC(draw),
            NULL);

        gtk_signal_connect(GTK_OBJECT(WID(hscale1)), "value_changed",GTK_SIGNAL_FUNC(frame_changed),   NULL);
        gtk_signal_connect(GTK_OBJECT(WID(hscaleHue)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
        gtk_signal_connect(GTK_OBJECT(WID(hscaleSaturation)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
        gtk_widget_show(dialog);

        update();
        draw();
        ret=0;
        int response;
        response=gtk_dialog_run(GTK_DIALOG(dialog));

        if(response==GTK_RESPONSE_OK)
        {
	    memcpy(param,&myHue,sizeof(myHue));
            ret=1;
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
    
        delete imgdst;
        delete imgsrc;
        delete imgdisplay;
        delete [] rgbbuffer;
        

        rgbbuffer=NULL;
        imgdst=NULL;
        imgsrc=NULL;
        dialog=NULL;
        imgdisplay=NULL;
        return ret;
}
void hue_changed( void)
{
    download();
    update();
    draw();
}
void frame_changed( void )
{
uint32_t new_frame,max,l,f;
double   percent;
GtkWidget *wid;	
GtkAdjustment *adj;
	
	max=incoming->getInfo()->nb_frames;
	wid=WID(hscale1);
	adj=gtk_range_get_adjustment (GTK_RANGE(wid));
	new_frame=0;
	
	percent=(double)GTK_ADJUSTMENT(adj)->value;
	percent*=max;
	percent/=100.;
	new_frame=(uint32_t)floor(percent);
	
	if(new_frame>=max) new_frame=max-1;
	
	ADM_assert(incoming->getFrameNumberNoAlloc(new_frame, &l, imgsrc,&f));
        memcpy(YPLANE(imgdisplay),YPLANE(imgsrc),w*h);
        update();
        draw();

}
gboolean draw (void)
{
    GtkWidget *draw;
        draw=WID(drawingarea1);
        GUI_RGBDisplay((uint8_t *)rgbbuffer, w,h, (void *)draw);
        return true;
}
void upload( void)
{
    gtk_range_set_value (GTK_RANGE(WID(hscaleHue)),(gdouble)myHue.hue);
    gtk_range_set_value (GTK_RANGE(WID(hscaleSaturation)),(gdouble)myHue.saturation);
}
void download (void)
{
    GtkAdjustment *adj;	

        adj=gtk_range_get_adjustment (GTK_RANGE(WID(hscaleHue)));
        myHue.hue=(float)GTK_ADJUSTMENT(adj)->value;
        
        adj=gtk_range_get_adjustment (GTK_RANGE(WID(hscaleSaturation)));
        myHue.saturation=(float)GTK_ADJUSTMENT(adj)->value;
}
void update(void )
{
uint8_t *src,*dst;
uint32_t stride;
    hue=myHue.hue*M_PI/180.;
    sat=(100+myHue.saturation)/100;
    // Do it!
    HueProcess_C(VPLANE(imgdisplay), UPLANE(imgdisplay),
        VPLANE(imgsrc), UPLANE(imgsrc),
        w>>1,w>>1,
        w>>1,h>>1, 
        hue, sat);
    // Prepare RGB buffer
    
    // Copy half source to display
    dst=imgdisplay->data+w*h;
    src=imgsrc->data+w*h;
    stride=w>>1;
    for(uint32_t y=0;y<h;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
    //
    COL_yv12rgb(  w,   h,imgdisplay->data,(uint8_t *)rgbbuffer );
}
GtkWidget   *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *hscaleHue;
  GtkWidget *hscaleSaturation;
  GtkWidget *hscale1;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Hue/Saturation"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("<b>Hue:</b>"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("<b>Saturation:</b>"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  hscaleHue = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, -90, 90, 1, 1, 1)));
  gtk_widget_show (hscaleHue);
  gtk_table_attach (GTK_TABLE (table1), hscaleHue, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscaleSaturation = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, -10, 10, 1, 1, 1)));
  gtk_widget_show (hscaleSaturation);
  gtk_table_attach (GTK_TABLE (table1), hscaleSaturation, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hscale1 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 100, 1, 1, 1)));
  gtk_widget_show (hscale1);
  gtk_box_pack_start (GTK_BOX (vbox1), hscale1, TRUE, TRUE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleHue, "hscaleHue");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleSaturation, "hscaleSaturation");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}
