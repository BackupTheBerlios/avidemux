
/*
	gui_filterlist
	Build the dialog box that list the active filters


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

GtkWidget		*create_filterMain (void);
GtkListStore 	*storeMainFilter;
#if plop
GtkWidget*
create_filterMain (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *vbuttonbox1;
  GtkWidget *buttonAdd;
  GtkWidget *buttonRemove;
  GtkWidget *buttonConfigure;
  GtkWidget *buttonUp;
  GtkWidget *buttonDown;
  GtkWidget *buttonVCD;
  GtkWidget *buttonSVCD;
  GtkWidget *buttonDVD;
  GtkWidget *buttonHaldD1;
  GtkWidget *buttonLoad;
  GtkWidget *buttonSave;
  GtkWidget *buttonPartial;
  GtkWidget *buttonPreview;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Video Filters"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

 // treeview1 = gtk_tree_view_new ();
 
  /*--------------*/
  //__________ PATCH BEGIN HERE_________________________

 storeMainFilter= gtk_list_store_new (1,  G_TYPE_STRING);
   treeview1 = gtk_tree_view_new_with_model (GTK_TREE_MODEL (storeMainFilter));
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new ();
   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   column = gtk_tree_view_column_new_with_attributes ("Filter", renderer,
                                                      "text", (GdkModifierType) 0,
                                                      NULL);
   /* Add the column to the view. */
   gtk_tree_view_append_column (GTK_TREE_VIEW (treeview1), column);
   //_______________PATCH END HERE____________________
 gtk_widget_show (treeview1);
  
  /*-------------*/
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview1), FALSE);

  vbuttonbox1 = gtk_vbutton_box_new ();
  gtk_widget_show (vbuttonbox1);
  gtk_box_pack_start (GTK_BOX (hbox1), vbuttonbox1, FALSE, TRUE, 0);

  buttonAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (buttonAdd);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonAdd);
  GTK_WIDGET_SET_FLAGS (buttonAdd, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (buttonAdd, "clicked", accel_group,
                              GDK_A, (GdkModifierType)0,
                              GTK_ACCEL_VISIBLE);

  buttonRemove = gtk_button_new_from_stock ("gtk-remove");
  gtk_widget_show (buttonRemove);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonRemove);
  GTK_WIDGET_SET_FLAGS (buttonRemove, GTK_CAN_DEFAULT);

  buttonConfigure = gtk_button_new_from_stock ("gtk-properties");
  gtk_widget_show (buttonConfigure);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonConfigure);
  GTK_WIDGET_SET_FLAGS (buttonConfigure, GTK_CAN_DEFAULT);

  buttonUp = gtk_button_new_from_stock ("gtk-go-up");
  gtk_widget_show (buttonUp);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonUp);
  GTK_WIDGET_SET_FLAGS (buttonUp, GTK_CAN_DEFAULT);

  buttonDown = gtk_button_new_from_stock ("gtk-go-down");
  gtk_widget_show (buttonDown);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDown);
  GTK_WIDGET_SET_FLAGS (buttonDown, GTK_CAN_DEFAULT);

  buttonVCD = gtk_button_new_with_mnemonic (_("VCD res"));
  gtk_widget_show (buttonVCD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonVCD);
  GTK_WIDGET_SET_FLAGS (buttonVCD, GTK_CAN_DEFAULT);

  buttonSVCD = gtk_button_new_with_mnemonic (_("SVCD res"));
  gtk_widget_show (buttonSVCD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonSVCD);
  GTK_WIDGET_SET_FLAGS (buttonSVCD, GTK_CAN_DEFAULT);

  buttonDVD = gtk_button_new_with_mnemonic (_("DVD res"));
  gtk_widget_show (buttonDVD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDVD);
  GTK_WIDGET_SET_FLAGS (buttonDVD, GTK_CAN_DEFAULT);

  buttonHaldD1 = gtk_button_new_with_mnemonic (_("HalfD1 Res"));
  gtk_widget_show (buttonHaldD1);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonHaldD1);
  GTK_WIDGET_SET_FLAGS (buttonHaldD1, GTK_CAN_DEFAULT);

  buttonLoad = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonLoad);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonLoad);
  GTK_WIDGET_SET_FLAGS (buttonLoad, GTK_CAN_DEFAULT);

  buttonSave = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (buttonSave);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonSave);
  GTK_WIDGET_SET_FLAGS (buttonSave, GTK_CAN_DEFAULT);

  buttonPartial = gtk_button_new_with_mnemonic (_("Partial"));
  gtk_widget_show (buttonPartial);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonPartial);
  GTK_WIDGET_SET_FLAGS (buttonPartial, GTK_CAN_DEFAULT);

  buttonPreview = gtk_button_new ();
  gtk_widget_show (buttonPreview);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonPreview);
  GTK_WIDGET_SET_FLAGS (buttonPreview, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttonPreview), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-print-preview", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (_("Preview"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox2), label1, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview1, "treeview1");
  GLADE_HOOKUP_OBJECT (dialog1, vbuttonbox1, "vbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAdd, "buttonAdd");
  GLADE_HOOKUP_OBJECT (dialog1, buttonRemove, "buttonRemove");
  GLADE_HOOKUP_OBJECT (dialog1, buttonConfigure, "buttonConfigure");
  GLADE_HOOKUP_OBJECT (dialog1, buttonUp, "buttonUp");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDown, "buttonDown");
  GLADE_HOOKUP_OBJECT (dialog1, buttonVCD, "buttonVCD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSVCD, "buttonSVCD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDVD, "buttonDVD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonHaldD1, "buttonHaldD1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonLoad, "buttonLoad");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSave, "buttonSave");
  GLADE_HOOKUP_OBJECT (dialog1, buttonPartial, "buttonPartial");
  GLADE_HOOKUP_OBJECT (dialog1, buttonPreview, "buttonPreview");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  gtk_window_add_accel_group (GTK_WINDOW (dialog1), accel_group);

  return dialog1;
}
#endif

GtkWidget*
create_filterMain (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *vbuttonbox1;
  GtkWidget *buttonAdd;
  GtkWidget *buttonRemove;
  GtkWidget *buttonConfigure;
  GtkWidget *buttonUp;
  GtkWidget *buttonDown;
  GtkWidget *buttonVCD;
  GtkWidget *buttonSVCD;
  GtkWidget *buttonDVD;
  GtkWidget *buttonHaldD1;
  GtkWidget *buttonLoad;
  GtkWidget *buttonSave;
  GtkWidget *buttonPartial;
  GtkWidget *buttonPreview;
  GtkWidget *alignment1;
  GtkWidget *hbox2;
  GtkWidget *image1;
  GtkWidget *label1;
  GtkWidget *buttonScript;
  GtkWidget *alignment2;
  GtkWidget *hbox3;
  GtkWidget *image2;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Video Filters"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  //treeview1 = gtk_tree_view_new ();
   /*--------------*/
  //__________ PATCH BEGIN HERE_________________________

 storeMainFilter= gtk_list_store_new (1,  G_TYPE_STRING);
   treeview1 = gtk_tree_view_new_with_model (GTK_TREE_MODEL (storeMainFilter));
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new ();
   /* Create a column, associating the "text" attribute of the
    * cell_renderer to the first column of the model */
   column = gtk_tree_view_column_new_with_attributes ("Filter", renderer,
                                                      "text", (GdkModifierType) 0,
                                                      NULL);
   /* Add the column to the view. */
   gtk_tree_view_append_column (GTK_TREE_VIEW (treeview1), column);
   //_______________PATCH END HERE____________________
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), FALSE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview1), FALSE);

  vbuttonbox1 = gtk_vbutton_box_new ();
  gtk_widget_show (vbuttonbox1);
  gtk_box_pack_start (GTK_BOX (hbox1), vbuttonbox1, FALSE, TRUE, 0);

  buttonAdd = gtk_button_new_from_stock ("gtk-add");
  gtk_widget_show (buttonAdd);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonAdd);
  GTK_WIDGET_SET_FLAGS (buttonAdd, GTK_CAN_DEFAULT);
  gtk_widget_add_accelerator (buttonAdd, "clicked", accel_group,
                              GDK_A,(GdkModifierType) 0,
                              GTK_ACCEL_VISIBLE);

  buttonRemove = gtk_button_new_from_stock ("gtk-remove");
  gtk_widget_show (buttonRemove);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonRemove);
  GTK_WIDGET_SET_FLAGS (buttonRemove, GTK_CAN_DEFAULT);

  buttonConfigure = gtk_button_new_from_stock ("gtk-properties");
  gtk_widget_show (buttonConfigure);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonConfigure);
  GTK_WIDGET_SET_FLAGS (buttonConfigure, GTK_CAN_DEFAULT);

  buttonUp = gtk_button_new_from_stock ("gtk-go-up");
  gtk_widget_show (buttonUp);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonUp);
  GTK_WIDGET_SET_FLAGS (buttonUp, GTK_CAN_DEFAULT);

  buttonDown = gtk_button_new_from_stock ("gtk-go-down");
  gtk_widget_show (buttonDown);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDown);
  GTK_WIDGET_SET_FLAGS (buttonDown, GTK_CAN_DEFAULT);

  buttonVCD = gtk_button_new_with_mnemonic (_("VCD res"));
  gtk_widget_show (buttonVCD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonVCD);
  GTK_WIDGET_SET_FLAGS (buttonVCD, GTK_CAN_DEFAULT);

  buttonSVCD = gtk_button_new_with_mnemonic (_("SVCD res"));
  gtk_widget_show (buttonSVCD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonSVCD);
  GTK_WIDGET_SET_FLAGS (buttonSVCD, GTK_CAN_DEFAULT);

  buttonDVD = gtk_button_new_with_mnemonic (_("DVD res"));
  gtk_widget_show (buttonDVD);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDVD);
  GTK_WIDGET_SET_FLAGS (buttonDVD, GTK_CAN_DEFAULT);

  buttonHaldD1 = gtk_button_new_with_mnemonic (_("HalfD1 Res"));
  gtk_widget_show (buttonHaldD1);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonHaldD1);
  GTK_WIDGET_SET_FLAGS (buttonHaldD1, GTK_CAN_DEFAULT);

  buttonLoad = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonLoad);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonLoad);
  GTK_WIDGET_SET_FLAGS (buttonLoad, GTK_CAN_DEFAULT);

  buttonSave = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (buttonSave);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonSave);
  GTK_WIDGET_SET_FLAGS (buttonSave, GTK_CAN_DEFAULT);

  buttonPartial = gtk_button_new_with_mnemonic (_("Partial"));
  gtk_widget_show (buttonPartial);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonPartial);
  GTK_WIDGET_SET_FLAGS (buttonPartial, GTK_CAN_DEFAULT);

  buttonPreview = gtk_button_new ();
  gtk_widget_show (buttonPreview);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonPreview);
  GTK_WIDGET_SET_FLAGS (buttonPreview, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttonPreview), alignment1);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox2);

  image1 = gtk_image_new_from_stock ("gtk-print-preview", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox2), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (_("Preview"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox2), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  buttonScript = gtk_button_new ();
  gtk_widget_show (buttonScript);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonScript);
  GTK_WIDGET_SET_FLAGS (buttonScript, GTK_CAN_DEFAULT);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (buttonScript), alignment2);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox3);

  image2 = gtk_image_new_from_stock ("gtk-justify-fill", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox3), image2, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic (_("Save as script"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox3), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview1, "treeview1");
  GLADE_HOOKUP_OBJECT (dialog1, vbuttonbox1, "vbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAdd, "buttonAdd");
  GLADE_HOOKUP_OBJECT (dialog1, buttonRemove, "buttonRemove");
  GLADE_HOOKUP_OBJECT (dialog1, buttonConfigure, "buttonConfigure");
  GLADE_HOOKUP_OBJECT (dialog1, buttonUp, "buttonUp");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDown, "buttonDown");
  GLADE_HOOKUP_OBJECT (dialog1, buttonVCD, "buttonVCD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSVCD, "buttonSVCD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDVD, "buttonDVD");
  GLADE_HOOKUP_OBJECT (dialog1, buttonHaldD1, "buttonHaldD1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonLoad, "buttonLoad");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSave, "buttonSave");
  GLADE_HOOKUP_OBJECT (dialog1, buttonPartial, "buttonPartial");
  GLADE_HOOKUP_OBJECT (dialog1, buttonPreview, "buttonPreview");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonScript, "buttonScript");
  GLADE_HOOKUP_OBJECT (dialog1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, image2, "image2");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  gtk_window_add_accel_group (GTK_WINDOW (dialog1), accel_group);

  return dialog1;
}



