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

#define HEX_NB_LINE   8
#define HEX_NB_COLUMN 16



typedef struct hexStruct
{ 
    GtkWidget *grid;
    GtkWidget *entry[HEX_NB_LINE];
    uint8_t   *data;
    uint32_t  size;
    uint32_t  curOffset;
}hexStruct;

static void updateMe(hexStruct *s);
static void prev(void *z,hexStruct *s);
static void next(void *z,hexStruct *s);

diaElemHex::diaElemHex(const char *toggleTitle, uint32_t dataSize,uint8_t *data)
  : diaElem(ELEM_HEXDUMP)
{
  param=NULL;
  paramTitle=toggleTitle;
  this->tip=NULL;
  this->data=data;
  this->dataSize=dataSize;
   setSize(3);
}

diaElemHex::~diaElemHex()
{
  if(myWidget)
  {
     hexStruct *s=(hexStruct *)myWidget;
     delete s;
     myWidget=NULL;
  }
}
void diaElemHex::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkWidget *hexTable,*buttonP,*buttonN;
  uint8_t *tail=data+dataSize;
  
  hexTable=gtk_table_new(1,HEX_NB_LINE,0);
  gtk_widget_show(hexTable);
  
  
  gtk_table_attach (GTK_TABLE (opaque), hexTable, 0, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  hexStruct *s=new hexStruct;
  s->grid=hexTable;
  s->data=data;
  s->size=dataSize;
  s->curOffset=0;
  for(int i=0;i<HEX_NB_LINE;i++)
  {
    s->entry[i]=gtk_label_new("");
    gtk_widget_show(s->entry[i]);
     gtk_table_attach (GTK_TABLE (hexTable), s->entry[i], 0, 1, i, i+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  }
  myWidget=(void *)s;
  //*************************
  buttonP = gtk_button_new_from_stock ("gtk-previous");
  gtk_widget_show (buttonP);
  gtk_table_attach (GTK_TABLE (opaque), buttonP, 0, 1, line+1, line+2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  g_signal_connect(GTK_OBJECT(buttonP), "clicked",
                    GTK_SIGNAL_FUNC(prev),  s);
  
  buttonN = gtk_button_new_from_stock ("gtk-next");
  gtk_widget_show (buttonN);
  gtk_table_attach (GTK_TABLE (opaque), buttonN, 0, 1, line+2, line+3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  g_signal_connect(GTK_OBJECT(buttonN), "clicked",
                    GTK_SIGNAL_FUNC(next),  s);
  
  
  updateMe(s);
}
void prev(void *z,hexStruct *s)
{
  if(s->curOffset>=HEX_NB_COLUMN*4)
      s->curOffset-=HEX_NB_COLUMN*4;
  updateMe(s);
  
}
void next(void *z,hexStruct *s)
{
      s->curOffset+=HEX_NB_COLUMN*4;
  updateMe(s);
}
void updateMe(hexStruct *s)
{
  uint8_t *tail=s->data+s->size;
  uint8_t *cur;
  char string[3000];
  char *ptr;
  for(int i=0;i<HEX_NB_LINE;i++)
  {
     cur=s->data+HEX_NB_COLUMN*i+s->curOffset;
     
     sprintf(string,"%06x:",HEX_NB_COLUMN*i+s->curOffset);
     ptr=string+strlen(string);
     *ptr=0;
     for(int j=0;j<HEX_NB_COLUMN;j++)
     {
       if(cur<tail)
       {
          sprintf(ptr,"%02X ",*cur++);
          ptr+=3;
       }else 
       {
          *ptr++='X';
          *ptr++='X';
          *ptr++=' ';
       }
     }
     ptr[2999]=0;
     gtk_label_set_text(GTK_LABEL(s->entry[i]),string);
     
  }
}
void diaElemHex::getMe(void)
{
 
}
void diaElemHex::finalize(void) 
{

};

//EOF
