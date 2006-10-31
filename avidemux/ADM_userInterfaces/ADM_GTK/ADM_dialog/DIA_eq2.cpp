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
#include "ADM_library/default.h"

#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_toolkit/ADM_gladeSupport.h"




#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"
#include "ADM_library/ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidEq2.h"
#include "prototype.h"
#include "ADM_assert.h"

#include "ADM_osSupport/ADM_cpuCap.h"



static GtkWidget*create_dialog1 (void);
static float getAdj(GtkWidget *widget);
static void setAdj(GtkWidget *widget,float val);

static ADMImage *imgsrc,*imgdst,*imgdisplay;
static GtkWidget *dialog=NULL;
static uint32_t w,h;
static uint32_t *rgbbuffer=NULL;
static AVDMGenericVideoStream *incoming;
static Eq2_Param myEq2;

static void             update ( void);
static gboolean         draw (void );
static void             eq2_changed( void);

static void frame_changed( void );
static Eq2Settings mySettings;
static ColYuvRgb    *rgbConv=NULL;
/************************************************************************/
uint8_t DIA_getEQ2Param(Eq2_Param *param, AVDMGenericVideoStream *in)
{
uint8_t r=0;

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

        memcpy(YPLANE(imgdisplay),YPLANE(imgsrc),w*h);
        memcpy(UPLANE(imgdisplay),UPLANE(imgsrc),(w*h)>>2);
        memcpy(VPLANE(imgdisplay),VPLANE(imgsrc),(w*h)>>2);
        
        rgbConv=new ColYuvRgb(w,h);
        rgbConv->reset(w,h);

        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        memcpy(&myEq2,param,sizeof(myEq2));

        // upload
#undef SET
#undef GET
#define SET(x,y)        setAdj(WID(hscale##x),myEq2.y)
#define GET(x,y)        myEq2.y=getAdj(WID(hscale##x))

        SET(Contrast,contrast);
        SET(Brightness,brightness);
        SET(Saturation,saturation);

        SET(Gamma,gamma);        
        SET(GammaWeight,gamma_weight);
        SET(GammaR,rgamma);
        SET(GammaG,ggamma);
        SET(GammaB,bgamma);

        gtk_signal_connect(GTK_OBJECT(WID(hscale1)), "value_changed",GTK_SIGNAL_FUNC(frame_changed),   NULL);
#define HCONECT(x)  gtk_signal_connect(GTK_OBJECT(WID(hscale##x)), "value_changed",GTK_SIGNAL_FUNC(eq2_changed),   NULL);

        HCONECT(Brightness);
        HCONECT(Saturation);
        HCONECT(Contrast);

        HCONECT(Gamma);        
        HCONECT(GammaWeight);
        HCONECT(GammaR);
        HCONECT(GammaG);
        HCONECT(GammaB);
        
        gtk_widget_set_usize(WID(drawingarea1), w,h);
        gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event", GTK_SIGNAL_FUNC(draw),   NULL);
        update();
        // run
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                // download
               memcpy(param,&myEq2,sizeof(myEq2));
                r=1;
        }
        
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);

        delete imgdst;
        delete imgsrc;
        delete imgdisplay;
        delete [] rgbbuffer;
        delete rgbConv;

        rgbbuffer=NULL;
        imgdst=NULL;
        imgsrc=NULL;
        dialog=NULL;
        imgdisplay=NULL;
        return r;

}
/**********************/
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
        memcpy(UPLANE(imgdisplay),UPLANE(imgsrc),(w*h)>>2);
        memcpy(VPLANE(imgdisplay),VPLANE(imgsrc),(w*h)>>2);
        update();
        draw();

}
void download (void)
{
                GET(Contrast,contrast);
                GET(Brightness,brightness);
                GET(Saturation,saturation);

                GET(Gamma,gamma);        
                GET(GammaWeight,gamma_weight);
                GET(GammaR,rgamma);
                GET(GammaG,ggamma);
                GET(GammaB,bgamma);
}
void eq2_changed( void)
{
    download();
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
void update(void )
{
uint8_t *src,*dst;
uint32_t stride;

        update_lut(&mySettings,&myEq2);
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
        if(CpuCaps::hasMMX())
        {
                affine_1d_MMX(&(mySettings.param[0]),YPLANE(imgdisplay),YPLANE(imgsrc),w,h);
                affine_1d_MMX(&(mySettings.param[2]),UPLANE(imgdisplay),UPLANE(imgsrc),w>>1,h>>1);
                affine_1d_MMX(&(mySettings.param[1]),VPLANE(imgdisplay),VPLANE(imgsrc),w>>1,h>>1);       
        }
        else
#endif
        {
                apply_lut(&(mySettings.param[0]),YPLANE(imgdisplay),YPLANE(imgsrc),w,h);
                apply_lut(&(mySettings.param[2]),UPLANE(imgdisplay),UPLANE(imgsrc),w>>1,h>>1);
                apply_lut(&(mySettings.param[1]),VPLANE(imgdisplay),VPLANE(imgsrc),w>>1,h>>1);       
        }

        // copy Y half
    dst=YPLANE(imgdisplay);
    src=YPLANE(imgsrc);
    stride=w;
    for(uint32_t y=0;y<h;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
        // U
    dst=UPLANE(imgdisplay);
    src=UPLANE(imgsrc);
    stride=w>>1;
    for(uint32_t y=0;y<h>>1;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
        // V
    dst=VPLANE(imgdisplay);
    src=VPLANE(imgsrc);
    stride=w>>1;
    for(uint32_t y=0;y<h>>1;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
    //
    //COL_yv12rgb(  w,   h,imgdisplay->data,(uint8_t *)rgbbuffer );
     rgbConv->scale(imgdisplay->data,(uint8_t *)rgbbuffer );
}

//**
float getAdj(GtkWidget *widget)
{
GtkAdjustment *adj;        
        adj=gtk_range_get_adjustment (GTK_RANGE(widget));
        return (float)adj->value;
}
void setAdj(GtkWidget *widget,float val)
{
 gtk_range_set_value (GTK_RANGE(widget),(gdouble)val);
}

//**

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *frame1;
  GtkWidget *table2;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *hscaleBrightness;
  GtkWidget *hscaleSaturation;
  GtkWidget *hscaleContrast;
  GtkWidget *label3;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table3;
  GtkWidget *label6;
  GtkWidget *hscaleGamma;
  GtkWidget *hscaleGammaR;
  GtkWidget *label8;
  GtkWidget *hscaleGammaG;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label7;
  GtkWidget *hscaleGammaB;
  GtkWidget *hscaleGammaWeight;
  GtkWidget *label2;
  GtkWidget *hscale1;
  GtkWidget *drawingarea1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 6);
  gtk_window_set_title (GTK_WINDOW (dialog1), _("MPlayer eq2"));
  gtk_window_set_resizable (GTK_WINDOW (dialog1), FALSE);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (hbox1), frame1, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_NONE);

  table2 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame1), table2);
  gtk_container_set_border_width (GTK_CONTAINER (table2), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table2), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table2), 12);

  label4 = gtk_label_new_with_mnemonic (_("Brigh_tness:"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table2), label4, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new_with_mnemonic (_("_Saturation:"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table2), label5, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  hscaleBrightness = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, -1, 1, 0.05, 1, 0)));
  gtk_widget_show (hscaleBrightness);
  gtk_table_attach (GTK_TABLE (table2), hscaleBrightness, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleBrightness, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleBrightness), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleBrightness), 2);

  hscaleSaturation = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 3, 0.05, 1, 0)));
  gtk_widget_show (hscaleSaturation);
  gtk_table_attach (GTK_TABLE (table2), hscaleSaturation, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleSaturation, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleSaturation), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleSaturation), 2);

  hscaleContrast = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, -2, 2, 0.05, 1, 0)));
  gtk_widget_show (hscaleContrast);
  gtk_table_attach (GTK_TABLE (table2), hscaleContrast, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleContrast, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleContrast), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleContrast), 2);

  label3 = gtk_label_new_with_mnemonic (_("_Contrast:"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table2), label3, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label1 = gtk_label_new ("");
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (hbox1), frame2, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

  table3 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (frame2), table3);
  gtk_container_set_border_width (GTK_CONTAINER (table3), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table3), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table3), 12);

  label6 = gtk_label_new_with_mnemonic (_("_Initial:"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table3), label6, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  hscaleGamma = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0.1, 3, 0.05, 1, 0)));
  gtk_widget_show (hscaleGamma);
  gtk_table_attach (GTK_TABLE (table3), hscaleGamma, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleGamma, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleGamma), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleGamma), 2);

  hscaleGammaR = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0.1, 3, 0.05, 1, 0)));
  gtk_widget_show (hscaleGammaR);
  gtk_table_attach (GTK_TABLE (table3), hscaleGammaR, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleGammaR, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleGammaR), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleGammaR), 2);

  label8 = gtk_label_new_with_mnemonic (_("_Red:"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table3), label8, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  hscaleGammaG = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0.1, 3, 0.05, 1, 0)));
  gtk_widget_show (hscaleGammaG);
  gtk_table_attach (GTK_TABLE (table3), hscaleGammaG, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleGammaG, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleGammaG), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleGammaG), 2);

  label9 = gtk_label_new_with_mnemonic (_("_Green:"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table3), label9, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  label10 = gtk_label_new_with_mnemonic (_("_Blue:"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table3), label10, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label7 = gtk_label_new_with_mnemonic (_("_Weight:"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table3), label7, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  hscaleGammaB = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0.1, 3, 0.05, 1, 0)));
  gtk_widget_show (hscaleGammaB);
  gtk_table_attach (GTK_TABLE (table3), hscaleGammaB, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleGammaB, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleGammaB), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleGammaB), 2);

  hscaleGammaWeight = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 1, 0.05, 1, 0)));
  gtk_widget_show (hscaleGammaWeight);
  gtk_table_attach (GTK_TABLE (table3), hscaleGammaWeight, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hscaleGammaWeight, 144, 0);
  gtk_scale_set_value_pos (GTK_SCALE (hscaleGammaWeight), GTK_POS_RIGHT);
  gtk_scale_set_digits (GTK_SCALE (hscaleGammaWeight), 2);

  label2 = gtk_label_new (_("<b>Gamma</b>"));
  gtk_widget_show (label2);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label2);
  gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  hscale1 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 100, 1, 1, 0)));
  gtk_widget_show (hscale1);
  gtk_box_pack_start (GTK_BOX (vbox1), hscale1, FALSE, FALSE, 0);

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

  gtk_label_set_mnemonic_widget (GTK_LABEL (label4), hscaleBrightness);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label5), hscaleSaturation);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label3), hscaleContrast);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label6), hscaleGamma);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label8), hscaleGammaR);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label9), hscaleGammaG);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label10), hscaleGammaB);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label7), hscaleGammaWeight);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleBrightness, "hscaleBrightness");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleSaturation, "hscaleSaturation");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleContrast, "hscaleContrast");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGamma, "hscaleGamma");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGammaR, "hscaleGammaR");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGammaG, "hscaleGammaG");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGammaB, "hscaleGammaB");
  GLADE_HOOKUP_OBJECT (dialog1, hscaleGammaWeight, "hscaleGammaWeight");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

