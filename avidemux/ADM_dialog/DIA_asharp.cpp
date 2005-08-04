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
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"

#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidASharp_param.h"
#include "ADM_assert.h"

uint8_t DIA_getASharp(ASHARP_PARAM *param, AVDMGenericVideoStream *in);

static GtkWidget        *create_dialog1 (void);
static void             update ( void);
static gboolean         draw (void );
static void             upload(void);
static void             download(void);
static void             read ( void );
static void             recalc( void );
static void             frame_changed( void );
static void             hue_changed( void);
extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);

static ADMImage *imgsrc,*imgdst,*imgdisplay;
static GtkWidget *dialog=NULL;
static uint32_t w,h;
static uint32_t *rgbbuffer=NULL;
static AVDMGenericVideoStream *incoming;
static ASHARP_PARAM  myHue;
static float      hue,sat;
static ColYuvRgb    *rgbConv=NULL;
//
//      Video is in YV12 Colorspace
//
//
uint8_t DIA_getASharp(ASHARP_PARAM *param, AVDMGenericVideoStream *in)
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

        imgdst=new ADMImage(w,h);
        imgsrc=new ADMImage(w,h);
        imgdisplay=new ADMImage(w,h);

        if(curframe<max) max=curframe;
        
        ADM_assert(in->getFrameNumberNoAlloc(max, &l, imgsrc,&f));
        // chroma is not changed by this filter
        
        memcpy(UPLANE(imgdisplay),UPLANE(imgsrc),(w*h)>>2);
        memcpy(VPLANE(imgdisplay),VPLANE(imgsrc),(w*h)>>2);
        memcpy(&myHue,param,sizeof(myHue));
        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        gtk_widget_set_usize(WID(drawingarea1), w,h);

        upload();
        gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
            GTK_SIGNAL_FUNC(draw),
            NULL);
//
        gtk_signal_connect(GTK_OBJECT(WID(hscale1)), "value_changed",GTK_SIGNAL_FUNC(frame_changed),   NULL);
        gtk_signal_connect(GTK_OBJECT(WID(spinbuttonT)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
        gtk_signal_connect(GTK_OBJECT(WID(spinbuttonD)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
        gtk_signal_connect(GTK_OBJECT(WID(spinbuttonB)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
     //   gtk_signal_connect(GTK_OBJECT(WID(checkbuttonBF)), "value_changed",GTK_SIGNAL_FUNC(hue_changed),   NULL);
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
        delete rgbConv;
        rgbConv=NULL;

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
        memcpy(UPLANE(imgdisplay),UPLANE(imgsrc),(w*h)>>2);
        memcpy(VPLANE(imgdisplay),VPLANE(imgsrc),(w*h)>>2);
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
//
#define SPIN_GET(x,y) {myHue.x= gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(y))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(y)),(gfloat)myHue.x) ;}

#define CHECK_GET(x,y) {myHue.x=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(y)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(y)),myHue.x);}

void upload( void)
{
//
        SPIN_SET(t,spinbuttonT);
        SPIN_SET(d,spinbuttonD);
        SPIN_SET(b,spinbuttonB);

        CHECK_SET(bf,checkbuttonBF);
}
void download (void)
{
        SPIN_GET(t,spinbuttonT);
        SPIN_GET(d,spinbuttonD);
        SPIN_GET(b,spinbuttonB);

        CHECK_GET(bf,checkbuttonBF);
}
void update(void )
{
uint8_t *src,*dst;
uint32_t stride;
int32_t T,D,B,B2;   
ASHARP_PARAM *_param=&myHue;
uint32_t ww,hh;

                download();
                ww=incoming->getInfo()->width;
                hh=incoming->getInfo()->height;
                // parameters floating point to fixed point convertion
                T = (int)(_param->t*(4<<7));
                D = (int)(_param->d*(4<<7));
                B = (int)(256-_param->b*64);
                B2= (int)(256-_param->b*48);

                // clipping (recommended for SIMD code)

                if (T<-(4<<7)) T = -(4<<7); // yes, negatives values are accepted
                if (D<0) D = 0;
                if (B<0) B = 0;
                if (B2<0) B2 = 0;

                if (T>(32*(4<<7))) T = (32*(4<<7));
                if (D>(16*(4<<7))) D = (16*(4<<7));
                if (B>256) B = 256;
                if (B2>256) B2 = 256;


                memcpy(YPLANE(imgdisplay),YPLANE(imgsrc),ww*hh);
                asharp_run_c(     imgdisplay->GetWritePtr(PLANAR_Y),
                        imgdisplay->GetPitch(PLANAR_Y), 
                        hh,
                        ww,
                        T,
                        D,
                        B,
                        B2,
                        myHue.bf);
    
    // Copy half source to display
    dst=imgdisplay->data;
    src=imgsrc->data;
    stride=ww;
    for(uint32_t y=0;y<hh;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
    //
    //COL_yv12rgb(  w,   h,imgdisplay->data,(uint8_t *)rgbbuffer );
    rgbConv->scale(imgdisplay->data,(uint8_t *)rgbbuffer);
}

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkObject *spinbuttonT_adj;
  GtkWidget *spinbuttonT;
  GtkObject *spinbuttonD_adj;
  GtkWidget *spinbuttonD;
  GtkObject *spinbuttonB_adj;
  GtkWidget *spinbuttonB;
  GtkWidget *checkbuttonBF;
  GtkWidget *hscale1;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("ASharp by MarcFD"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Threshold :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Strength :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Block Adaptative :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Unknown flag :"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  spinbuttonT_adj = gtk_adjustment_new (1, -1, 100, 0.5, 0.1, 0.1);
  spinbuttonT = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonT_adj), 1, 1);
  gtk_widget_show (spinbuttonT);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonT, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonT), TRUE);

  spinbuttonD_adj = gtk_adjustment_new (1, -1, 100, 0.5, 0.1, 0.1);
  spinbuttonD = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonD_adj), 1, 1);
  gtk_widget_show (spinbuttonD);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonD, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonD), TRUE);

  spinbuttonB_adj = gtk_adjustment_new (1, -1, 100, 0.5, 0.1, 0.1);
  spinbuttonB = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonB_adj), 1, 1);
  gtk_widget_show (spinbuttonB);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonB, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonB), TRUE);

  checkbuttonBF = gtk_check_button_new_with_mnemonic (_(" "));
  gtk_widget_show (checkbuttonBF);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonBF, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonT, "spinbuttonT");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonD, "spinbuttonD");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonB, "spinbuttonB");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBF, "checkbuttonBF");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

