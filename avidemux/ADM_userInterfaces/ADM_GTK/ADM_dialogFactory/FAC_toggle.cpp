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

static void cb_menu(void *w,void *p);



diaElemToggle::diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)toggleValue;
  paramTitle=toggleTitle;
  this->tip=tip;
  nbLink=0;
}

diaElemToggle::~diaElemToggle()
{
  
}
uint8_t   diaElemToggle::link(uint32_t onoff,diaElem *w)
{
    ADM_assert(nbLink<MENU_MAX_lINK);
    links[nbLink].onoff=onoff;
    links[nbLink].widget=w;
    nbLink++;
    return 1;
}
void diaElemToggle::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  
  widget = gtk_check_button_new_with_mnemonic (paramTitle);
  gtk_widget_show (widget);
  myWidget=(void *)widget;
  
  gtk_table_attach (GTK_TABLE (opaque), widget, 0, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), *(uint32_t *)param);
  if(tip)
  {
      GtkTooltips *tooltips= gtk_tooltips_new ();
      gtk_tooltips_set_tip (tooltips, widget, tip, NULL);
  }
  gtk_signal_connect(GTK_OBJECT(widget), "toggled",
                      GTK_SIGNAL_FUNC(cb_menu),
                      (void *) this);
}
void diaElemToggle::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t *val=(uint32_t *)param;
  ADM_assert(widget);
  *(uint32_t *)param=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}
void   diaElemToggle::finalize(void)
{
  updateMe(); 
}
void   diaElemToggle::updateMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t val;
  uint32_t rank;
  if(!nbLink) return;
  ADM_assert(widget);
  
  
  rank=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
  /* Now search through the linked list to see if something happens ...*/
  
   /* 1 disable everything */
  for(int i=0;i<nbLink;i++)
  {
    dialElemLink *l=&(links[i]);
    l->widget->enable(0);
  }
  /* Then enable */
  for(int i=0;i<nbLink;i++)
  {
      dialElemLink *l=&(links[i]);
      if(l->onoff==rank)  l->widget->enable(1);
  }
}
void   diaElemToggle::enable(uint32_t onoff)
{
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);  
}

//** C callback **
void cb_menu(void *w,void *p)
{
  diaElemToggle *me=(diaElemToggle *)p;
  me->updateMe();
}

//EOF
