/*
	gui_filterlist
	Build the dialog box that list all available filters


*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #include "config.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
GtkListStore 	*storeFilterList;
GtkWidget	*create_dialogList (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Select a video filter"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

 //__________ PATCH BEGIN HERE_________________________

 storeFilterList= gtk_list_store_new (1,  G_TYPE_STRING);
   treeview2 = gtk_tree_view_new_with_model (GTK_TREE_MODEL (storeFilterList));
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new ();
   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   column = gtk_tree_view_column_new_with_attributes ("Filter", renderer,
                                                      "text", 0,
                                                      NULL);
   /* Add the column to the view. */
   gtk_tree_view_append_column (GTK_TREE_VIEW (treeview2), column);
   //_______________PATCH END HERE____________________



  /*treeview2 = gtk_tree_view_new ();*/
  gtk_widget_show (treeview2);



  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview2);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview2), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview2), FALSE);

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
  GLADE_HOOKUP_OBJECT (dialog1, treeview2, "treeview2");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

