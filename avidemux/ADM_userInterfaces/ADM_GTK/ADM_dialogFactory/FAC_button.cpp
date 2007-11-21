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

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <math.h>

#include "default.h"
#include "../ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "../ADM_toolkit_gtk/toolkit_gtk.h"
#include "../ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "../ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

static void cb_button (GtkWidget *widget,gpointer callback_data);

diaElemButton:: diaElemButton(const char *toggleTitle, ADM_FAC_CALLBACK *cb,void *cookie,const char *tip)
  : diaElem(ELEM_BUTTON)
{
  param=NULL;
  paramTitle=toggleTitle;
  this->tip=tip;
  _cookie=cookie;
  _callBack=cb;
  
}

diaElemButton::~diaElemButton()
{
  
}

void diaElemButton::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *button;
  button = gtk_button_new_from_stock (paramTitle);
  gtk_widget_show (button);
  /**/
  gtk_table_attach (GTK_TABLE (opaque), button, 0, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  /* Add callback ...*/
  
  g_signal_connect(GTK_OBJECT(button), "clicked",
                    GTK_SIGNAL_FUNC(cb_button),  this);
  myWidget=button;
}
void diaElemButton::getMe(void)
{
  
}
void   diaElemButton::enable(uint32_t onoff)
{
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);  
}

void cb_button (GtkWidget *widget,gpointer callback_data)
{
  diaElemButton *button=(diaElemButton *)callback_data;
  button->_callBack(button->_cookie);
}

//EOF
