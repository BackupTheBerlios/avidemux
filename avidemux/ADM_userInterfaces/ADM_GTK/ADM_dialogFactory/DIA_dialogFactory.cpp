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
  dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
  gtk_widget_show (dialog_vbox1);
  
  table1 = gtk_table_new (nb, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);
  
  for(int i=0;i<nb;i++)
  {
    addLine(elems[i],dialog,table1,i);
    
  }
  // Add ok & cancel button
  GtkWidget *cancelbutton1,*okbutton1;
  
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
        getLine(elems[i],dialog,table1,i);
        
      }
    ret=1;  
  }

  // Cleanup
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return ret;
  
}



//EOF
