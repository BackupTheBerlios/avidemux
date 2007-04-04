/***************************************************************************
  DIA_dialogFactory.cpp
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


static uint8_t  buildOneTab(GtkWidget *nb,int index, diaElemTabs *tab);
static uint8_t  readOneTab(int index, diaElemTabs *tab);


/**
    \fn addLine(diaElem *elem, GtkDialog *dialog, GtkWidget *vbox)
    \brief setup an line with the element given as parameter
    @return nothing

*/
void addLine(diaElem *elem, GtkWidget *dialog, GtkWidget *vbox,uint32_t line)
{
            elem->setMe( (void *)dialog,(void *)vbox,line);
  
}
/**
    \fn getLine(diaElem *elem, GtkDialog *dialog, GtkWidget *vbox)
    \brief read back an line with the element given as parameter
    @return nothing

*/
void getLine(diaElem *elem, GtkWidget *dialog, GtkWidget *vbox,uint32_t line)
{
  
             elem->getMe();
  
}
/**
    \fn diaFactoryRun(const char *title,uint32_t nb,diaElem **elems)
    \brief  Run a dialog made of nb elems, each elem being described in the **elems
    @return 0 on failure, 1 on success
*/

uint8_t diaFactoryRun(const char *title,uint32_t nb,diaElem **elems)
{
  uint8_t ret=0;
  ADM_assert(elems);
  ADM_assert(nb);
  ADM_assert(title);
  
  /* First there was a dialog ...*/
  GtkWidget *dialog=gtk_dialog_new ();
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  
  gtk_window_set_title (GTK_WINDOW (dialog),title );
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_box_set_spacing (GTK_BOX(dialog_vbox1), 12);
  gtk_widget_show (dialog_vbox1);
  
  int vsize=0;
  for(int i=0;i<nb;i++)
      vsize+=elems[i]->getSize();
  
  
  table1 = gtk_table_new (vsize, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 6);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 6);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);
  
  int line=0;
  for(int i=0;i<nb;i++)
  {
    addLine(elems[i],dialog,table1,line);
    line+=elems[i]->getSize();
    
  }
  // Add a Close button
  GtkWidget *okbutton1;
  GtkWidget *cancelbutton1;

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);
  
  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);
  
  /* Finalize them */
  for(int i=0;i<nb;i++)
  {
    elems[i]->finalize();
  }
  
  // Show it & run
  gtk_register_dialog(dialog);
  if(GTK_RESPONSE_OK==gtk_dialog_run(GTK_DIALOG(dialog)))
  {
    for(int i=0;i<nb;i++)
      {
        getLine(elems[i],dialog,table1,i);
        
      }
    ret=1;  
  }

  // Cleanup
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return ret;
  
}
/**
    \fn   diaFactoryRun
    \brief   Same as above but for tabbed dialog
*/
uint8_t diaFactoryRunTabs(const char *title,uint32_t nb,diaElemTabs **tabs)
{
  uint8_t ret=0;
  ADM_assert(tabs);
  ADM_assert(nb);
  ADM_assert(title);
  
  /* First there was a dialog ...*/
  GtkWidget *dialog=gtk_dialog_new ();
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *notebook1;
  
  gtk_window_set_title (GTK_WINDOW (dialog),title );
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_box_set_spacing (GTK_BOX(dialog_vbox1), 12);
  gtk_widget_show (dialog_vbox1);
   // Ok  create our tabs
  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (notebook1), 6);
  
  // And fill them
  for(int i=0;i<nb;i++)
  {
    buildOneTab(notebook1,i,tabs[i]); 
  }
  
 
  // Add a Close button
  GtkWidget *okbutton1;
  GtkWidget *cancelbutton1;

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);
  
  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);
  
  // Show it & run
  gtk_register_dialog(dialog);
  if(GTK_RESPONSE_OK==gtk_dialog_run(GTK_DIALOG(dialog)))
  {
    for(int i=0;i<nb;i++)
    {
      readOneTab(i,tabs[i]);
    }
    ret=1;
  }

  // Cleanup
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return ret;
  
}
/**

*/
uint8_t  buildOneTab(GtkWidget *nb,int index, diaElemTabs *tab)
{
  GtkWidget *table1,*label;
  uint32_t vsize=0;
  
  ADM_assert(tab);
  /* Compute vertical size of our elems */
    for(int i=0;i<tab->nbElems;i++)
  {
    vsize+=tab->dias[i]->getSize();
  }
  
  
  table1 = gtk_table_new (vsize, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 6);
  gtk_container_add (GTK_CONTAINER (nb), table1);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 6);
  gtk_widget_show (table1);
  
  label = gtk_label_new (_(tab->title));
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (nb), gtk_notebook_get_nth_page (GTK_NOTEBOOK (nb), index), label);

  
  int line=0;
  for(int i=0;i<tab->nbElems;i++)
  {
    addLine(tab->dias[i],nb,table1,line);
    line+=tab->dias[i]->getSize();
  }
  return 1; 
}
/**

*/
uint8_t  readOneTab(int index, diaElemTabs *tab)
{
  for(int i=0;i<tab->nbElems;i++)
  {
    tab->dias[i]->getMe();
  }
  return 1; 
}

//EOF
