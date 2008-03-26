/***************************************************************************
  FAC_integer.cpp
  Handle dialog factory elements : Integer, Slider, and variations thereupon
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

#include "../ADM_toolkit_gtk/toolkit_gtk.h"
#include "DIA_factory.h"

diaElemInteger::diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip)
  : diaElem(ELEM_INTEGER)
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
  GtkWidget *label;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
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
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), widget);
  
  myWidget=(void *)widget;
  if(readOnly)
    gtk_widget_set_sensitive(widget,0);
   if(tip)
   {
     GtkTooltips *tooltips= gtk_tooltips_new ();
      gtk_tooltips_set_tip (tooltips, widget, tip, NULL);
   }
 
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

void diaElemInteger::enable(uint32_t onoff)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);
}

//**********************************************************************************

diaElemUInteger::diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip)
  : diaElem(ELEM_INTEGER)
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
  GtkWidget *label;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
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
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), widget);
  
  myWidget=(void *)widget;

  if(tip)
  {
      GtkTooltips *tooltips= gtk_tooltips_new ();
      gtk_tooltips_set_tip (tooltips, widget, tip, NULL);
  }
  
  if(readOnly)
      gtk_widget_set_sensitive(widget,0);

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
void diaElemUInteger::enable(uint32_t onoff)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);
}

//****************************************************

template <typename T>
static
uint32_t diaElemSliderDigitsDefault (T incr)
{
    return 0; // used for int types; a specialization overrides this for float
}

template <>  // specialization of above...
uint32_t diaElemSliderDigitsDefault (ELEM_TYPE_FLOAT incr) // ...for float
{
    char tmp [32];
    uint32_t digits = 3;
    sprintf (tmp, "%.*f", digits, incr);
    const char * cptr = tmp + strlen (tmp);
    while (*--cptr == '0')
        --digits;

    if (digits < 1)
        return 1;
    else
        return digits;
}

template <typename T>
diaElemGenericSlider<T>::diaElemGenericSlider(T *value,const char *toggleTitle, T min,T max,T incr,const char *tip)
    : diaElem(ELEM_SLIDER),
      min (min),
      max (max),
      incr (incr),
      digits (diaElemSliderDigitsDefault (incr))
{
    param = (void *)value;
    paramTitle = toggleTitle;
    this->tip = tip;
    size = 2;
}

template <typename T>
diaElemGenericSlider<T>::~diaElemGenericSlider()
{
}

template <typename T>
void diaElemGenericSlider<T>::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 2, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  T val=*(T *)param;

  GtkAdjustment * adj = (GtkAdjustment *) gtk_adjustment_new (val, min, max, incr, incr, 0);

  GtkWidget *spinner = gtk_spin_button_new (adj, 1, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spinner), TRUE);
  gtk_spin_button_set_digits  (GTK_SPIN_BUTTON(spinner), digits);
  
  GtkWidget *slider = gtk_hscale_new (adj);
  gtk_scale_set_draw_value (GTK_SCALE (slider), FALSE);
  gtk_scale_set_digits (GTK_SCALE (slider), digits);
  
  GtkWidget *hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), slider, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), spinner, FALSE, FALSE, 0);

  gtk_table_attach (GTK_TABLE (opaque), hbox, 0, 2, line+1, line+2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), hbox);
  gtk_widget_show (hbox);
  gtk_widget_show (spinner);
  gtk_widget_show (slider);
  
  myWidget=(void *)slider;
  if(readOnly)
  {
    gtk_widget_set_sensitive(spinner,0);
    gtk_widget_set_sensitive(slider,0);
  }
  if(tip)
  {
      GtkTooltips *tooltips= gtk_tooltips_new ();
      gtk_tooltips_set_tip (tooltips, spinner, tip, NULL);
      gtk_tooltips_set_tip (tooltips, slider, tip, NULL);
  }
}

template <typename T>
void diaElemGenericSlider<T>::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  T *val=(T *)param;
  ADM_assert(widget);
  GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE(widget));
  *val = (T)GTK_ADJUSTMENT(adj)->value;
  if(*val<min) *val=min;
  if(*val>max) *val=max;
}

template <typename T>
void diaElemGenericSlider<T>::enable(uint32_t onoff)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);
}

template class diaElemGenericSlider <int32_t>;
template class diaElemGenericSlider <uint32_t>;
template class diaElemGenericSlider <ELEM_TYPE_FLOAT>;

//EOF
