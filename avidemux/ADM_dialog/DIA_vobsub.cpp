
//
// Author: mean , fixounet@free.fr, GPL (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <config.h>

#include <stdio.h>
#include <stdlib.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <math.h>

#include "callbacks.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_assert.h" 
#define CHECK_GET(x,y) {*y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),*y);}   

#include "ADM_video/ADM_vobsubinfo.h"

static GtkWidget        *create_dialog1 (void);
static GtkWidget        **fq;
static GtkWidget        *dialog;

static void update(char *name,int i);

uint8_t DIA_vobsub(char **name, uint32_t *idx);
uint8_t DIA_vobsub(char **name, uint32_t *idx)
{
      
 
  int ret,ext,r;

        
  
        
  ret=0;
  ext=0;
  
  while(!ext)
  {
    dialog=create_dialog1();
    
    gtk_transient(dialog);
    gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonSelect), GTK_RESPONSE_APPLY);
    fq=new GtkWidget*[20];
  
        // Upload if any
        if(*name)
        {
                update(*name,*idx);
        }
        else
        {
                gtk_label_set_text(GTK_LABEL(WID(labelVobsub)),"none");     
        }
        r=gtk_dialog_run(GTK_DIALOG(dialog));
            switch(r)
            {
              case GTK_RESPONSE_APPLY:
              {
                char *file;
                        GUI_FileSelRead("Select .idx file",&file); 
                        if(file)
                        {
                          if(*name) ADM_dealloc(*name);
                          *name=NULL;
                          *name=ADM_strdup(file); // Leak ?                      
                        }                                
              }
                break;
              case GTK_RESPONSE_OK:
                ret=1;
                ext=1;
                if(*name)
                        ADM_dealloc(*name);
                *name=ADM_strdup(gtk_label_get_text(GTK_LABEL(WID(labelVobsub))));
                *idx=getRangeInMenu(WID(optionmenu1));
                break;
              case GTK_RESPONSE_CANCEL:
                ret=0;
                ext=1;
                break;
              default:
                break;
            }
       delete [] fq;          
       gtk_widget_destroy(dialog);
  
  }
     
  return ret;
}    
//*****************************************************   
void update(char *name,int idx)
{
  GtkWidget *menu1;
   // Print
  if(!name) return;
  menu1 = gtk_menu_new ();  
  gtk_label_set_text(GTK_LABEL(WID(labelVobsub)),name);     
    // Try to load it...
  vobSubLanguage *lang=NULL;
    
  lang=vobSubAllocateLanguage();
    
  if(vobSubGetLanguage(name,lang))
  {
      // add them
    for(int i=0;i<lang->nbLanguage;i++)
    {
      fq[i]=gtk_menu_item_new_with_mnemonic (lang->language[i]);
      gtk_widget_show (fq[i]);
      gtk_container_add (GTK_CONTAINER (menu1), fq[i]);
    }
    gtk_option_menu_set_menu (GTK_OPTION_MENU (WID(optionmenu1)), menu1);
    gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu1)), idx);
  }
    // Destroy
  vobSubDestroyLanguage(lang);
}
//*****************************************************
GtkWidget*
    create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *labelVobsub;
  GtkWidget *buttonSelect;
  GtkWidget *optionmenu1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("VobSub settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  labelVobsub = gtk_label_new (_("label1"));
  gtk_widget_show (labelVobsub);
  gtk_box_pack_start (GTK_BOX (hbox1), labelVobsub, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (labelVobsub), GTK_JUSTIFY_LEFT);

  buttonSelect = gtk_button_new_with_mnemonic (_("Select idx"));
  gtk_widget_show (buttonSelect);
  gtk_box_pack_start (GTK_BOX (hbox1), buttonSelect, FALSE, FALSE, 0);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_box_pack_start (GTK_BOX (vbox1), optionmenu1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, labelVobsub, "labelVobsub");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSelect, "buttonSelect");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


//********
