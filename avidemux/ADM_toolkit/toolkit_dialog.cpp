/***************************************************************************
                          toolkit_dialog.cpp  -  description
                             -------------------

  Handle simple dialog (alert, yes./no)


    begin                : Fri Dec 14 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "config.h"

#include "avi_vars.h"
#include "toolkit.hxx"
#include "interface.h"
#include "toolkit.hxx"

# include <config.h>
#include "ADM_gui2/support.h"
#include "toolkit_gtk.h"
#include "ADM_assert.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

static GtkWidget	*create_dialogYN (void);
static GtkWidget	*create_dialogOK (void);
static int beQuiet=0;

static GtkWidget *widgetStack[10];
static int	  widgetCount=0;

void gtk_register_dialog(GtkWidget *newdialog)
{

	widgetStack[widgetCount]=newdialog;
	widgetCount++;
}
void gtk_unregister_dialog(GtkWidget *newdialog)
{
	ADM_assert(widgetCount);
	ADM_assert(widgetStack[widgetCount-1]==newdialog);
	widgetCount--;

}
void		gtk_transient(GtkWidget *widget)
{
GtkWidget *top;
	assert(widgetCount);
	top=widgetStack[widgetCount-1];
		
	// The father is no longer modal
	gtk_window_set_modal(GTK_WINDOW(top), 0);
	// But we are
	gtk_window_set_modal(GTK_WINDOW(widget), 1);
	
	gtk_window_set_transient_for (GTK_WINDOW(widget),GTK_WINDOW(top));
	
	
	


}

/**
	GUI_Quiet : Prevents gui from poping alert
	Answers always no to question

*/
uint8_t isQuiet(void )
{
	return beQuiet;
}
void GUI_Quiet( void )
{
	beQuiet=1;

}
/**
	GUI_Verbose : Allow gui to ask question & popup alert

*/
void GUI_Verbose( void )
{
	beQuiet=0;

}

/**
	GUI_Question
		Ask the question passed in alertstring
			Return 1 if yes
			Return 0 if no

		In silent mode, always return 0

*/
int 		GUI_Question(const char *alertstring)
{
int ret=0;

	GtkWidget *dialog;

	if(beQuiet) 
	{
	
		printf("?? %s ?? -> NO\n",alertstring);
		return 0 ;
	}


	dialog=create_dialogYN();
	gtk_transient(dialog);
	gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label1")),alertstring);
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES)
	{
		ret=1;
	}
	gtk_widget_destroy(dialog);

	return ret;
}
/**
	GUI_Alert : Just display an alert string in a dialog box
	The string can contain \n for multi lines display

*/
void 		GUI_Alert(const char *alertstring)
{
	GtkWidget *dialog;

	if(beQuiet) 
	{
		printf("XXX %s  XXX!!!\n",alertstring);
		return ;
	}


	dialog=create_dialogOK();
	gtk_transient(dialog);
	gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label1")),alertstring);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	
	
	
	UI_purge();

}


GtkWidget	*create_dialogYN (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Question"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  image1 = gtk_image_new_from_stock ("gtk-dialog-question", GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("label1"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-yes");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_YES);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-no");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_NO);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  gtk_widget_grab_focus (cancelbutton1);
  gtk_widget_grab_default (cancelbutton1);
  return dialog1;
}




GtkWidget*
create_dialogOK (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Alert"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  image1 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("label1"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


/**
	Return the line number of a selction
	0 if no selection of fails

*/
uint8_t getSelectionNumber(uint32_t nb,GtkWidget *tree  , GtkListStore 	*store,uint32_t *number)
{
		GtkTreeSelection *selection;
		GtkTreeIter ref; //iter,ref;
		int count=-1;

		*number=0;
		selection= gtk_tree_view_get_selection    (GTK_TREE_VIEW(tree));

		if(TRUE!=    gtk_tree_model_get_iter_first   (GTK_TREE_MODEL(store),&ref))
		{
			return 0;
		}

		for(uint32_t l=0;l<nb;l++)
		{
			if(gtk_tree_selection_iter_is_selected  (selection,&ref)) count=l;
			gtk_tree_model_iter_next  (GTK_TREE_MODEL(store),&ref);
		}
//		printf(" Found sel :%d\n",count);
		if(count==-1)
		{
			return 0;

		}
		else
		{
			*number=count;;
			return 1;
		}
}
/**

	Select the row number number in the list given as arg

*/
uint8_t setSelectionNumber(uint32_t nb,GtkWidget *tree  , GtkListStore 	*store,uint32_t number)
{
		GtkTreeSelection *selection;
		GtkTreeIter ref; //iter,ref;

	 	selection= gtk_tree_view_get_selection    (GTK_TREE_VIEW(tree));
		/*
		gtk_tree_selection_select_all (selection);
		return 1;
		*/
		if(TRUE!=    gtk_tree_model_get_iter_first   (GTK_TREE_MODEL(store),&ref))
		{
			printf("Cannot get first iter...\n");
			return 0;
		}

		for(uint32_t l=0;l<nb;l++)
		{
			if(l==number)
			{
				gtk_tree_selection_select_iter (selection,&ref);
				return 1;
			}
			gtk_tree_model_iter_next  (GTK_TREE_MODEL(store),&ref);
		}
		printf(" Could not set selection %d!!\n",number);
		return 0;
}


