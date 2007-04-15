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


diaElemFrame::diaElemFrame(const char *toggleTitle, const char *tip)
  : diaElem(ELEM_FRAME)
{
  param=NULL;
  paramTitle=toggleTitle;
  this->tip=tip;
  nbElems=0;
  frameSize=0;
  setSize(2);
}
void diaElemFrame::swallow(diaElem *widget)
{
  elems[nbElems]=widget;
  frameSize+=widget->getSize();
 // setSize(frameSize);
  nbElems++;
  ADM_assert(nbElems<DIA_MAX_FRAME); 
  
}
diaElemFrame::~diaElemFrame()
{
  
}
void diaElemFrame::setMe(void *dialog, void *opaque,uint32_t line)
{
  
  GtkWidget *label;
  GtkWidget *table;
  char str[200];
  
  
  sprintf(str,"<b>%s</b>",paramTitle);
  label = gtk_label_new (str);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_widget_show(label);
  
   table = gtk_table_new (frameSize, 2, FALSE);
   
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_container_set_border_width (GTK_CONTAINER (table), 6);
  
   
   gtk_widget_show(table);
   
   gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 1);

   gtk_misc_set_alignment (GTK_MISC (label), 0, 1);

   gtk_table_attach (GTK_TABLE (opaque), table, 0, 2, line+1, line+2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
    
    
  uint32_t v=0;
  for(int i=0;i<nbElems;i++)
  {
    elems[i]->setMe(dialog,table,v); 
    v+=elems[i]->getSize();
  }
}
void diaElemFrame::getMe(void)
{
   for(int i=0;i<nbElems;i++)
  {
    elems[i]->getMe(); 
  }
}
void diaElemFrame::enable(uint32_t onoff)
{
//   GtkWidget *widget=(GtkWidget *)myWidget;
//   gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);
}
//EOF
