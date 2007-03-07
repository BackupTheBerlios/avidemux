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




diaElemReadOnlyText::diaElemReadOnlyText(char *readyOnly,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_ROTEXT)
{
  param=(void *)readyOnly;
  paramTitle=toggleTitle;
  this->tip=tip;
}

diaElemReadOnlyText::~diaElemReadOnlyText()
{
  
}
void diaElemReadOnlyText::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkObject *adj;
  GtkWidget *label,*label2;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  label2 = gtk_label_new_with_mnemonic ((char *)param);
  gtk_misc_set_alignment (GTK_MISC (label2), 0.0, 0.5);
  gtk_widget_show(label2);
  
 
  
  gtk_table_attach (GTK_TABLE (opaque), label2, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), label2);
  
  myWidget=(void *)label2;
  
}
void diaElemReadOnlyText::getMe(void)
{
 
  
}
//EOF