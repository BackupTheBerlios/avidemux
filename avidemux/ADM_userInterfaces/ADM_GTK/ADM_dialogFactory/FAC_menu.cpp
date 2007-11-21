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

static void cb_menu(void *w,void *p);
static void cb_menus(void *w,void *p);

diaElemMenu::diaElemMenu(uint32_t *intValue,const char *itle, uint32_t nb, 
               const diaMenuEntry *menu,const char *tip)
  : diaElem(ELEM_MENU)
{
  param=(void *)intValue;
  paramTitle=itle;
  this->tip=tip;
  this->menu=menu;
  this->nbMenu=nb;
  nbLink=0;
  
  menus=new diaMenuEntryDynamic * [nb];
  for(int i=0;i<nb;i++)
  {
    menus[i]=new  diaMenuEntryDynamic(menu[i].val,menu[i].text,menu[i].desc);
  }
  dyna=new diaElemMenuDynamic(intValue,itle,nb,menus,tip);
}

diaElemMenu::~diaElemMenu()
{
  for(int i=0;i<nbMenu;i++)
      delete menus[i];
  delete [] menus;
  delete dyna;
  
}
void diaElemMenu::setMe(void *dialog, void *opaque,uint32_t line)
{
  dyna->setMe(dialog,opaque,line);
}

void diaElemMenu::getMe(void)
{
 dyna->getMe();
}
void   diaElemMenu::updateMe(void)
{
  dyna->updateMe();
}
uint8_t   diaElemMenu::link(diaMenuEntry *entry,uint32_t onoff,diaElem *w)
{
    for(int i=0;i<nbMenu;i++)
    {
        if(entry->val==menus[i]->val)
            return dyna->link(menus[i],onoff,w);
    }
    ADM_assert(0);
}
void   diaElemMenu::enable(uint32_t onoff)
{
  dyna->enable(onoff);
}
void   diaElemMenu::finalize(void)
{
  dyna->finalize();
}
//*******************************
diaElemMenuDynamic::diaElemMenuDynamic(uint32_t *intValue,const char *itle, uint32_t nb, 
                diaMenuEntryDynamic **menu,const char *tip)
  : diaElem(ELEM_MENU)
{
  param=(void *)intValue;
  paramTitle=itle;
  this->tip=tip;
  this->menu=menu;
  this->nbMenu=nb;
  nbLink=0;
}

diaElemMenuDynamic::~diaElemMenuDynamic()
{
  
}
void diaElemMenuDynamic::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *widget;
  GtkWidget *label;
  GtkWidget *item;
  GtkWidget *combo;
  
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  combo = gtk_combo_box_new_text ();
  gtk_widget_show (combo);
  gtk_table_attach (GTK_TABLE (opaque), combo, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), combo);
  
  for(int i=0;i<nbMenu;i++)
  {
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo),menu[i]->text);
  }
  
  for(int i=0;i<nbMenu;i++)
  {
    if(menu[i]->val==*(uint32_t *)param) 
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(combo),i);
    }
  }
  myWidget=(void *)combo;
  gtk_signal_connect(GTK_OBJECT(combo), "changed",
                      GTK_SIGNAL_FUNC(cb_menu),
                      (void *) this);
  
}

void diaElemMenuDynamic::getMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t *val=(uint32_t *)param;
  uint32_t rank;
  if(!nbMenu) return;
  ADM_assert(widget);
  
  
  rank=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  ADM_assert(rank<this->nbMenu);
  *(uint32_t *)param=this->menu[rank]->val;
}

uint8_t   diaElemMenuDynamic::link(diaMenuEntryDynamic *entry,uint32_t onoff,diaElem *w)
{
    ADM_assert(nbLink<MENU_MAX_lINK);
    links[nbLink].value=entry->val;
    links[nbLink].onoff=onoff;
    links[nbLink].widget=w;
    nbLink++;
    return 1;
}

void   diaElemMenuDynamic::updateMe(void)
{
  GtkWidget *widget=(GtkWidget *)myWidget;
  uint32_t val;
  uint32_t rank;
  if(!nbMenu) return;
  ADM_assert(widget);
  
  
  rank=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  ADM_assert(rank<this->nbMenu);
  val=this->menu[rank]->val;
  /* Now search through the linked list to see if something happens ...*/
  
   /* 1 disable everything */
  for(int i=0;i<nbLink;i++)
  {
    dialElemLink *l=&(links[i]);
    if(l->value==val)
    {
      if(!l->onoff)  l->widget->enable(0);
    }else
    {
       if(l->onoff)  l->widget->enable(0);
    }
    
  }
  /* Then enable */
  for(int i=0;i<nbLink;i++)
  {
    dialElemLink *l=&(links[i]);
    if(l->value==val)
    {
      if(l->onoff)  l->widget->enable(1);
    }else
    {
       if(!l->onoff)  l->widget->enable(1);
    }
    
  }
}
void   diaElemMenuDynamic::finalize(void)
{
  updateMe(); 
}
void   diaElemMenuDynamic::enable(uint32_t onoff)
{
  gtk_widget_set_sensitive(GTK_WIDGET(myWidget),onoff);  
}
//** C callback **
void cb_menu(void *w,void *p)
{
  diaElemMenuDynamic *me=(diaElemMenuDynamic *)p;
  me->updateMe();
}
void cb_menus(void *w,void *p)
{
  diaElemMenu *me=(diaElemMenu *)p;
  me->updateMe();
}

//EOF
