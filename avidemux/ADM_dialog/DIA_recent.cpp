/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config.h"
#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_library/default.h"

//___________________________________
#include "avi_vars.h"
#include "prefs.h"
#include "ADM_toolkit/toolkit_gtk.h"
//___________________________________
#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

static void on_action( void);  
  
static GtkWidget	*create_dialog1 (void);
static GtkListStore 	*store;
static GtkWidget *dialog,*tree;
uint8_t DIA_RecentFiles( char **name )
{

uint8_t ret=0;
uint32_t nb_item;
const char **names;
GtkTreeIter   iter;

	dialog=create_dialog1();
	gtk_transient(dialog);
	tree=lookup_widget(dialog,"treeview1");
	gtk_signal_connect (GTK_OBJECT (lookup_widget(dialog,"treeview1")),
				"row-activated", 
				GTK_SIGNAL_FUNC (on_action), 
				(void *)dialog);
	names=prefs->get_lastfiles();
// count
	for( nb_item=0;nb_item<4;nb_item++)
	{
		if(!names[nb_item]) break;
	}
//	printf("\n found %d old files \n",nb_item);
	if(!nb_item) return 0;
	for (uint32_t i = 0; i < nb_item; i++)
    	{
		gtk_list_store_append (store, &iter);
     		gtk_list_store_set (store, &iter,0,names[i],-1);
//		printf("\n appending %s\n",names[i]);
    	}

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
	uint32_t sel;
	
		if( (ret=getSelectionNumber(nb_item,tree  , store,&sel)))
		{
			*name=(char *)names[sel];
		}
	}
	gtk_widget_destroy(dialog);

	return ret;
}
void on_action( void)
{
	gtk_dialog_response           (GTK_DIALOG(dialog),GTK_RESPONSE_OK);


}
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Recent files"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);

  //__________ PATCH BEGIN HERE_________________________
   store = gtk_list_store_new (1,  G_TYPE_STRING);
   treeview1 = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new ();
   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   column = gtk_tree_view_column_new_with_attributes ("File", renderer,
                                                      "text", 0,
                                                      NULL);
   /* Add the column to the view. */
   gtk_tree_view_append_column (GTK_TREE_VIEW (treeview1), column);
   //_______________PATCH END HERE____________________



 // treeview1 = gtk_tree_view_new ();
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview1);
  gtk_container_set_border_width (GTK_CONTAINER (treeview1), 1);
    gtk_widget_set_size_request (treeview1, 400, 150);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview1), FALSE);

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
  GLADE_HOOKUP_OBJECT (dialog1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview1, "treeview1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

