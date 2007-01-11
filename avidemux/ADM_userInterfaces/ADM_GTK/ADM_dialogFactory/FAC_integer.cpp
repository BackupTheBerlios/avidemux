/***************************************************************************
  FAC_toggle.cpp
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




diaElemInteger::diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)intValue;
  paramTitle=toggleTitle;
  this->min=min;
  this->max=max;
  this->tip=tip;
}

diaElemInteger::~diaElemInteger()
{
  
}
void diaElemInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkObject *adj;
  GtkWidget *label;
  
  label = gtk_label_new (paramTitle);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  int32_t val=*(int32_t *)param;
  widget = gtk_spin_button_new_with_range(min,max,1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(widget),TRUE);
  gtk_spin_button_set_digits  (GTK_SPIN_BUTTON(widget),0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget),val);
  
  gtk_widget_show (widget);
  
  gtk_table_attach (GTK_TABLE (opaque), widget, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  
  
  myWidget=(void *)widget;
  
}
void diaElemInteger::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  int32_t *val=(int32_t *)param;
  ADM_assert(widget);
  *val=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
  if(*val<min) *val=min;
  if(*val>max) *val=max;
  
}
//**********************************************************************************

diaElemUInteger::diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)intValue;
  paramTitle=toggleTitle;
  this->min=min;
  this->max=max;
  this->tip=tip;
}

diaElemUInteger::~diaElemUInteger()
{
  
}
void diaElemUInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkObject *adj;
  GtkWidget *label;
  
  label = gtk_label_new (paramTitle);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  uint32_t val=*(uint32_t *)param;
  widget = gtk_spin_button_new_with_range(min,max,1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(widget),TRUE);
  gtk_spin_button_set_digits  (GTK_SPIN_BUTTON(widget),0);
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget),val);
  
  gtk_widget_show (widget);
  
  gtk_table_attach (GTK_TABLE (opaque), widget, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  
  
  myWidget=(void *)widget;
  
}
void diaElemUInteger::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t *val=(uint32_t *)param;
  ADM_assert(widget);
  *val=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
  if(*val<min) *val=min;
  if(*val>max) *val=max;

}


//EOF
