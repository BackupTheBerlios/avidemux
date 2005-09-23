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
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview10;
  GtkWidget *label1;
  GtkWidget *treeview11;
  GtkWidget *label2;
  GtkWidget *treeview12;
  GtkWidget *label3;
  GtkWidget *treeview13;
  GtkWidget *label4;
  GtkWidget *treeview14;
  GtkWidget *label5;
  GtkWidget *treeview15;
  GtkWidget *label15;
  GtkWidget *treeview16;
  GtkWidget *label7;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("dialog1"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow1);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  treeview10 = gtk_tree_view_new ();
  gtk_widget_show (treeview10);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview10);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview10), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview10), FALSE);

  label1 = gtk_label_new (_("Alteration"));
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

  treeview11 = gtk_tree_view_new ();
  gtk_widget_show (treeview11);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview11);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview11), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview11), FALSE);

  label2 = gtk_label_new (_("Interlacing"));
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

  treeview12 = gtk_tree_view_new ();
  gtk_widget_show (treeview12);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview12);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview12), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview12), FALSE);

  label3 = gtk_label_new (_("Deblender"));
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);

  treeview13 = gtk_tree_view_new ();
  gtk_widget_show (treeview13);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview13);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview13), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview13), FALSE);

  label4 = gtk_label_new (_("Convolution"));
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label4);

  treeview14 = gtk_tree_view_new ();
  gtk_widget_show (treeview14);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview14);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview14), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview14), FALSE);

  label5 = gtk_label_new (_("Luma Chroma"));
  gtk_widget_show (label5);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), label5);

  treeview15 = gtk_tree_view_new ();
  gtk_widget_show (treeview15);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview15);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview15), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview15), FALSE);

  label15 = gtk_label_new (_("Smoother"));
  gtk_widget_show (label15);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 5), label15);

  treeview16 = gtk_tree_view_new ();
  gtk_widget_show (treeview16);
  gtk_container_add (GTK_CONTAINER (notebook1), treeview16);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview16), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview16), FALSE);

  label7 = gtk_label_new (_("Subs"));
  gtk_widget_show (label7);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 6), label7);

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
  GLADE_HOOKUP_OBJECT (dialog1, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (dialog1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview10, "treeview10");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview11, "treeview11");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, treeview12, "treeview12");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, treeview13, "treeview13");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, treeview14, "treeview14");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, treeview15, "treeview15");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, treeview16, "treeview16");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

