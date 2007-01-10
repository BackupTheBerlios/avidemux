/***************************************************************************
  FAC_toggle.cpp
  Handle dialog factory element : Menu
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




diaElemMenu::diaElemMenu(uint32_t *intValue,const char *itle, uint32_t nb, 
               const diaMenuEntry *menu,const char *tip)
  : diaElem(ELEM_MENU)
{
  param=(void *)intValue;
  paramTitle=itle;
  this->tip=tip;
  this->menu=menu;
  this->nbMenu=nb;
}

diaElemMenu::~diaElemMenu()
{
  
}
void diaElemMenu::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkWidget *label;
  GtkWidget *item;
  GtkWidget *combo;
  
  
  label = gtk_label_new (paramTitle);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  combo = gtk_combo_box_new_text ();
  gtk_widget_show (combo);
  gtk_table_attach (GTK_TABLE (opaque), combo, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  for(int i=0;i<nbMenu;i++)
  {
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo),this->menu[i].text);
  }
  
  for(int i=0;i<nbMenu;i++)
  {
    if(this->menu[i].val==*(uint32_t *)param) 
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo),i);
    }
  }
  
  
  myWidget=(void *)combo;
}

void diaElemMenu::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t *val=(uint32_t *)param;
  uint32_t rank;
  ADM_assert(widget);
  
  
  rank=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  ADM_assert(rank<this->nbMenu);
  *(uint32_t *)param=this->menu[rank].val;
}

//EOF
