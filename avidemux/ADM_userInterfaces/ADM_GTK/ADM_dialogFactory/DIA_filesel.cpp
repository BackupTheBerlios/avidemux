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
#include "ADM_toolkit/filesel.h"
static void fileRead(void *w,void *p);
static void dirSel(void *w,void *p);


diaElemFileRead::diaElemFileRead(char **filename,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_FILE_READ)
{
  param=(void *)filename;
  paramTitle=toggleTitle;
  this->tip=tip;
}

diaElemFileRead::~diaElemFileRead()
{
GtkWidget **wid=(GtkWidget **)myWidget;
        if(wid)
        {
            delete [] wid;
            myWidget=NULL;
        }
  
}
void diaElemFileRead::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkObject *adj;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *hbox1;
  GtkWidget *button;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  /**/
  
  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 6);
  gtk_widget_show (hbox1);
  
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  if(param)
  {
      char **val=(char **)param;
      gtk_entry_set_text (GTK_ENTRY (entry), *val);
  }
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), entry);
  
  /* put entry in hbox */
  gtk_box_pack_start (GTK_BOX (hbox1), entry, TRUE, TRUE, 0); /* expand fill padding */
  
  /*  add button */
  
  button = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox1), button, FALSE, FALSE, 0);

  
  /**/
  gtk_table_attach (GTK_TABLE (opaque), hbox1, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  
  /**/
  
  /* Add callback ...*/
  g_signal_connect(GTK_OBJECT(button), "clicked",
                    GTK_SIGNAL_FUNC(fileRead),  this);

  GtkWidget **w=new GtkWidget *[2];
  w[0]=entry;
  w[1]=button;
  myWidget=(void *)w;
  
}
void diaElemFileRead::getMe(void)
{
  GtkWidget **widget=(GtkWidget **)myWidget;
  char **name=(char **)param;
  if(*name) delete [] *name;
  *name=NULL;
  *name =ADM_strdup(gtk_entry_get_text (GTK_ENTRY (widget[0])));
}

void diaElemFileRead::changeFile(void)
{
#define MAX_SEL 200
  char buffer[MAX_SEL+1];
  GtkWidget **wid=(GtkWidget **)myWidget;
  GtkWidget *widget=(GtkWidget *)wid[0];
  const char *txt;
  txt =gtk_entry_get_text (GTK_ENTRY (widget));
  
  if(FileSel_SelectRead(paramTitle,buffer,MAX_SEL,txt))
  {
    char **name=(char **)param;
    if(*name) delete [] *name;
    *name=NULL;
    *name =ADM_strdup(buffer);
     gtk_entry_set_text (GTK_ENTRY (widget), *name);
  }
  
}

void   diaElemFileRead::enable(uint32_t onoff)
{
GtkWidget **wid=(GtkWidget **)myWidget;
     gtk_widget_set_sensitive(GTK_WIDGET(wid[0]),onoff);  
     gtk_widget_set_sensitive(GTK_WIDGET(wid[1]),onoff);  
}

void fileRead(void *w,void *p)
{
  diaElemFileRead *me=(diaElemFileRead *)p;
  me->changeFile();
}



/*********************************************************************/
diaElemDirSelect::diaElemDirSelect(char **filename,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_DIR_SELECT)
{
  param=(void *)filename;
  paramTitle=toggleTitle;
  this->tip=tip;
}

diaElemDirSelect::~diaElemDirSelect()
{
  GtkWidget **wid=(GtkWidget **)myWidget;
        if(wid)
        {
            delete [] wid;
            myWidget=NULL;
        }
}
void diaElemDirSelect::setMe(void *dialog, void *opaque,uint32_t line)
{
  GtkObject *adj;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *hbox1;
  GtkWidget *button;
  
  label = gtk_label_new_with_mnemonic (paramTitle);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show(label);
  
  gtk_table_attach (GTK_TABLE (opaque), label, 0, 1, line, line+1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  /**/
  
  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (hbox1), 6);
  gtk_widget_show (hbox1);
  
  entry = gtk_entry_new ();
  gtk_widget_show (entry);
  if(param)
  {
      char **val=(char **)param;
      gtk_entry_set_text (GTK_ENTRY (entry), *val);
  }
  gtk_label_set_mnemonic_widget (GTK_LABEL(label), entry);
  
  /* put entry in hbox */
  gtk_box_pack_start (GTK_BOX (hbox1), entry, TRUE, TRUE, 0); /* expand fill padding */
  
  /*  add button */
  
  button = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (button);
  gtk_box_pack_start (GTK_BOX (hbox1), button, FALSE, FALSE, 0);

  
  /**/
  gtk_table_attach (GTK_TABLE (opaque), hbox1, 1, 2, line, line+1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  
  
  /**/
  
  /* Add callback ...*/
  g_signal_connect(GTK_OBJECT(button), "clicked",
                    GTK_SIGNAL_FUNC(dirSel),  this);

  
   GtkWidget **w=new GtkWidget *[2];
  w[0]=entry;
  w[1]=button;
  myWidget=(void *)w;
  
}
void diaElemDirSelect::getMe(void)
{
GtkWidget **wid=(GtkWidget **)myWidget;
  GtkWidget *widget=wid[0];
  char **name=(char **)param;
  if(*name) delete [] *name;
  *name=NULL;
  *name =ADM_strdup(gtk_entry_get_text (GTK_ENTRY (widget)));
}

void diaElemDirSelect::changeFile(void)
{
#define MAX_SEL 200
  char buffer[MAX_SEL+1];
  GtkWidget **wid=(GtkWidget **)myWidget;
  GtkWidget *widget=wid[0];

  
  const char *txt;
  txt =gtk_entry_get_text (GTK_ENTRY (widget));
  
  if(FileSel_SelectDir(paramTitle,buffer,MAX_SEL,txt))
  {
    char **name=(char **)param;
    if(*name) delete [] *name;
    *name=NULL;
    *name =ADM_strdup(buffer);
     gtk_entry_set_text (GTK_ENTRY (widget), *name);
  }
  
}
void   diaElemDirSelect::enable(uint32_t onoff)
{
    GtkWidget **wid=(GtkWidget **)myWidget;
     gtk_widget_set_sensitive(GTK_WIDGET(wid[0]),onoff);  
     gtk_widget_set_sensitive(GTK_WIDGET(wid[1]),onoff);  

}

void dirSel(void *w,void *p)
{
  diaElemDirSelect *me=(diaElemDirSelect *)p;
  me->changeFile();
}

//EOF
