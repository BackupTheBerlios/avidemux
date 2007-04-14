/***************************************************************************
  FAC_float.cpp
  Handle dialog factory element : Toggle
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




diaElemFloat::diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, 
                            ELEM_TYPE_FLOAT min, ELEM_TYPE_FLOAT max,const char *tip)
  : diaElem(ELEM_FLOAT)
{
  param=(void *)intValue;
  paramTitle=toggleTitle;
  this->min=min;
  this->max=max;
  this->tip=tip;
}

diaElemFloat::~diaElemFloat()
{
  
}
void diaElemFloat::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkObject *adj;
  GtkWidget *label;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  ELEM_TYPE_FLOAT val=*(ELEM_TYPE_FLOAT *)param;
  widget = gtk_spin_button_new_with_range(min,max,0.1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(widget),TRUE);
  gtk_spin_button_set_digits  (GTK_SPIN_BUTTON(widget),2);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget),*(ELEM_TYPE_FLOAT *)param);
  
  gtk_widget_show (widget);
  
  gtk_table_attach (GTK_TABLE (opaque), widget, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), widget);
  
  myWidget=(void *)widget;
    if(tip)
  {
      GtkTooltips *tooltips= gtk_tooltips_new ();
      gtk_tooltips_set_tip (tooltips, widget, tip, NULL);
  }

}
void diaElemFloat::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  ELEM_TYPE_FLOAT *val=(ELEM_TYPE_FLOAT *)param;
  ADM_assert(widget);
  *val=gtk_spin_button_get_value(GTK_SPIN_BUTTON (widget));
  if(*val<min) *val=min;
  if(*val>max) *val=max;
}
void diaElemFloat::enable(uint32_t onoff)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);
}

//EOF
