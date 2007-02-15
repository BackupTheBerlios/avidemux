/***************************************************************************
  FAC_bitrate.cpp
  Handle dialog factory element : Bitrate (encoding mode)
  (C) 2006 Mean Fixounet@free.fr 
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
#include <math.h>

#include "default.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

static void cb_mod(void *w,void *p);


 diaElemBitrate::diaElemBitrate(COMPRES_PARAMS *p,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_BITRATE)
{
  param=(void *)p;
  memcpy(&copy,p,sizeof(copy));
  paramTitle=toggleTitle;
  this->tip=tip;
  setSize(2);
}

diaElemBitrate::~diaElemBitrate()
{
  GtkWidget *w=(GtkWidget *)myWidget;
  delete [] w;
  myWidget=NULL;
}
/**
 * \fn setMe
 * @param dialog  Pointer to father dialog
 * @param opaque  Internal, Gtk table to attach to
 * @param line Line were the widget should be displayed
 */
void diaElemBitrate::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkObject *adj;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *combo;
  GtkWidget *spin;
  
#define PUT_ARRAY(x,y,widget)  gtk_table_attach (GTK_TABLE (opaque), widget, x, x+1, line+y, line+y+1, \
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), \
                    (GtkAttachOptions) (0), 0, 0);
  
  /* Add text -> encoding mode */
  label1 = gtk_label_new_with_mnemonic ("Encoding mode");
  gtk_misc_set_alignment (GTK_MISC (label1), 0.0, 0.5);
  gtk_widget_show(label1);
  
  /* put entry in hbox */
 
  PUT_ARRAY(0,0,label1);
  
  
  /* Add text -> encoding mode */
  label2 = gtk_label_new_with_mnemonic ("Bitrate (kb/s)");
  gtk_misc_set_alignment (GTK_MISC (label2), 0.0, 0.5);
  gtk_widget_show(label2);
  /* put entry in hbox */
  PUT_ARRAY(0,1,label2);
 
  /* Add encoding menu combo */
  
  
  combo = gtk_combo_box_new_text ();
  gtk_widget_show (combo);
  
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label1), combo);
  
#define add(x) gtk_combo_box_append_text (GTK_COMBO_BOX (combo),_(#x));
  
  add(Constant Bitrate);
  add(Constant Quality);
  add(Two pass-filesize);
  add(Two pass-Avg bitrate);
  
  PUT_ARRAY(1,0,combo);
  
  
  /* Now add value */
  spin = gtk_spin_button_new_with_range(0,1,1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin),TRUE);
  gtk_spin_button_set_digits  (GTK_SPIN_BUTTON(spin),0);
  
  gtk_widget_show (spin);
  
    PUT_ARRAY(1,1,spin);
  /*  add button */
   gtk_label_set_mnemonic_widget (GTK_LABEL(label1), combo);
   gtk_label_set_mnemonic_widget (GTK_LABEL(label2), spin); 
   
  gtk_signal_connect(GTK_OBJECT(combo), "changed",
                      GTK_SIGNAL_FUNC(cb_mod),
                      (void *) this);
  
  GtkWidget **w;
  w=new GtkWidget*[4];
  w[0]=label1;
  w[1]=label2;
  w[2]=combo;
  w[3]=spin;
  myWidget=(void *)w;
  int i=0;
  switch(copy.mode)
  {
    case  COMPRESS_CQ: i=1;break;
    case  COMPRESS_CBR:i=0;break;
    case  COMPRESS_2PASS:i=2;break;
    case  COMPRESS_SAME:i=4;break;
    case  COMPRESS_2PASS_BITRATE:i=3;break;
    default: ADM_assert(0);
  }
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo),i);
}


void diaElemBitrate::getMe(void)
{
  
  
  // Read current value
  GtkWidget **w=(GtkWidget **)myWidget;
  GtkComboBox *combo=(GtkComboBox *)w[2];
  GtkSpinButton *spin=(GtkSpinButton*)w[3];
  GtkLabel *label=(GtkLabel*)w[1];
  int rank=gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
#undef P
#undef M
#undef S
#define P(x) 
#define M(x,y)
#define S(x)   x=gtk_spin_button_get_value  (GTK_SPIN_BUTTON(spin))
  switch(rank)
  {
    case 0: //CBR
          P(Bitrate (kb/s));
          M(0,20000);
          S(copy.bitrate);
          copy.mode=COMPRESS_CBR;
          break; 
    case 1:// CQ
          P(Quantizer);
          M(2,31);
          S(copy.qz);
          copy.mode=COMPRESS_CQ;
          break;
    case 2 : // 2pass Filesize
          P(FileSize (MB));
          M(1,8000);
          S(copy.finalsize);
          copy.mode=COMPRESS_2PASS;
          break;
    case 3 : // 2pass Avg
          P(Average Br (kb/s));
          M(0,20000);
          S(copy.avg_bitrate);
          copy.mode=COMPRESS_2PASS_BITRATE;
          break;
    case 4 : // Same Qz as input
          P(-);
          M(0,0);
          copy.mode=COMPRESS_SAME;
          break;
    default:ADM_assert(0);
  }
  memcpy(param,&copy,sizeof(copy));
}
void diaElemBitrate::updateMe(void)
{
  // Read current value
  GtkWidget **w=(GtkWidget **)myWidget;
  GtkComboBox *combo=(GtkComboBox *)w[2];
  GtkSpinButton *spin=(GtkSpinButton*)w[3];
  GtkLabel *label=(GtkLabel*)w[1];
  int rank=gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
#define P(x) gtk_label_set_text(GTK_LABEL(label),_(#x));
#define M(x,y) gtk_spin_button_set_range  (GTK_SPIN_BUTTON(spin),x,y)
#define S(x)   gtk_spin_button_set_value  (GTK_SPIN_BUTTON(spin),x)
  switch(rank)
  {
    case 0: //CBR
          P(Bitrate (kb/s));
          M(0,20000);
          S(copy.bitrate);
          break; 
    case 1:// CQ
          P(Quantizer);
          M(2,31);
          S(copy.qz);
          break;
    case 2 : // 2pass Filesize
          P(FileSize (MB));
          M(1,8000);
          S(copy.finalsize);
          break;
    case 3 : // 2pass Avg
          P(Average Br (kb/s));
          M(0,20000);
          S(copy.avg_bitrate);
          break;
    case 4 : // Same Qz as input
          P(-);
          M(0,0);
          break;
    default:ADM_assert(0);
  }
}

void cb_mod(void *w,void *p)
{
  diaElemBitrate *me=(diaElemBitrate *)p;
  me->updateMe();
}


//EOF
