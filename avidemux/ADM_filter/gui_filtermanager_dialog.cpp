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

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window1 (void)
{
  GtkWidget *window1;
  GtkWidget *hbox3;
  GtkWidget *alignment3;
  GtkWidget *frame1;
  GtkWidget *alignment1;
  GtkWidget *notebook1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *hbox5;
  GtkWidget *image1;
  GtkWidget *label11;
  GtkWidget *scrolledwindow2;
  GtkWidget *treeview2;
  GtkWidget *hbox6;
  GtkWidget *image2;
  GtkWidget *label17;
  GtkWidget *scrolledwindow3;
  GtkWidget *treeview3;
  GtkWidget *hbox7;
  GtkWidget *image3;
  GtkWidget *label18;
  GtkWidget *scrolledwindow4;
  GtkWidget *treeview4;
  GtkWidget *hbox8;
  GtkWidget *image4;
  GtkWidget *label19;
  GtkWidget *scrolledwindow5;
  GtkWidget *treeview5;
  GtkWidget *hbox9;
  GtkWidget *image5;
  GtkWidget *label20;
  GtkWidget *scrolledwindow6;
  GtkWidget *treeview6;
  GtkWidget *hbox10;
  GtkWidget *image6;
  GtkWidget *label21;
  GtkWidget *scrolledwindow7;
  GtkWidget *treeview7;
  GtkWidget *hbox4;
  GtkWidget *image7;
  GtkWidget *label22;
  GtkWidget *label23;
  GtkWidget *alignment4;
  GtkWidget *frame2;
  GtkWidget *alignment2;
  GtkWidget *hbox1;
  GtkWidget *scrolledwindow9;
  GtkWidget *treeview0;
  GtkWidget *toolbar1;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *tmp_image;
  GtkWidget *toolbuttonOpen;
  GtkWidget *toolbuttonSave;
  GtkWidget *toolbuttonScript;
  GtkWidget *separatortoolitem1;
  GtkWidget *toolbutton1;
  GtkWidget *toolbuttonRemove;
  GtkWidget *toolbuttonProperties;
  GtkWidget *toolbuttonPartial;
  GtkWidget *toolbuttonPreview;
  GtkWidget *toolbuttonUp;
  GtkWidget *toolbuttonDown;
  GtkWidget *separatortoolitem3;
  GtkWidget *toolbuttonDVD;
  GtkWidget *toolbuttonHalfD1;
  GtkWidget *toolbuttonSVCD;
  GtkWidget *toolbuttonVCD;
  GtkWidget *label2;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _("Video Filter Manager"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_default_size (GTK_WINDOW (window1), 768, 552);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (window1), TRUE);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (window1), hbox3);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment3);
  gtk_box_pack_start (GTK_BOX (hbox3), alignment3, TRUE, TRUE, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment3), 4, 4, 4, 4);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_container_add (GTK_CONTAINER (alignment3), frame1);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 4, 4, 4, 4);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_container_add (GTK_CONTAINER (alignment1), notebook1);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook1), FALSE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook1), GTK_POS_RIGHT);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow1);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview1 = gtk_tree_view_new ();
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview1);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview1), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview1), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview1), FALSE);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), hbox5);

  image1 = create_pixmap (window1, "1.png");
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox5), image1, FALSE, FALSE, 0);

  label11 = gtk_label_new_with_mnemonic (_("_Transform"));
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox5), label11, FALSE, FALSE, 4);
  gtk_label_set_use_markup (GTK_LABEL (label11), TRUE);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview2 = gtk_tree_view_new ();
  gtk_widget_show (treeview2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), treeview2);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview2), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview2), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview2), FALSE);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox6);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), hbox6);

  image2 = create_pixmap (window1, "2.png");
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox6), image2, FALSE, FALSE, 0);

  label17 = gtk_label_new_with_mnemonic (_("_Interlacing"));
  gtk_widget_show (label17);
  gtk_box_pack_start (GTK_BOX (hbox6), label17, FALSE, FALSE, 4);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow3);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview3 = gtk_tree_view_new ();
  gtk_widget_show (treeview3);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), treeview3);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview3), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview3), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview3), FALSE);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox7);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), hbox7);

  image3 = create_pixmap (window1, "4.png");
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox7), image3, FALSE, FALSE, 0);

  label18 = gtk_label_new_with_mnemonic (_("_Colors"));
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (hbox7), label18, FALSE, FALSE, 4);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow4);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview4 = gtk_tree_view_new ();
  gtk_widget_show (treeview4);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), treeview4);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview4), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview4), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview4), FALSE);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox8);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), hbox8);

  image4 = create_pixmap (window1, "5.png");
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox8), image4, FALSE, FALSE, 0);

  label19 = gtk_label_new_with_mnemonic (_("_Denoise"));
  gtk_widget_show (label19);
  gtk_box_pack_start (GTK_BOX (hbox8), label19, FALSE, FALSE, 4);

  scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow5);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow5);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview5 = gtk_tree_view_new ();
  gtk_widget_show (treeview5);
  gtk_container_add (GTK_CONTAINER (scrolledwindow5), treeview5);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview5), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview5), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview5), FALSE);

  hbox9 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox9);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), hbox9);

  image5 = create_pixmap (window1, "3.png");
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox9), image5, FALSE, FALSE, 0);

  label20 = gtk_label_new_with_mnemonic (_("_Blur/Sharpen"));
  gtk_widget_show (label20);
  gtk_box_pack_start (GTK_BOX (hbox9), label20, FALSE, FALSE, 4);

  scrolledwindow6 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow6);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow6);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow6), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview6 = gtk_tree_view_new ();
  gtk_widget_show (treeview6);
  gtk_container_add (GTK_CONTAINER (scrolledwindow6), treeview6);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview6), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview6), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview6), FALSE);

  hbox10 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox10);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 5), hbox10);

  image6 = create_pixmap (window1, "7.png");
  gtk_widget_show (image6);
  gtk_box_pack_start (GTK_BOX (hbox10), image6, FALSE, FALSE, 0);

  label21 = gtk_label_new_with_mnemonic (_("S_ubtitles"));
  gtk_widget_show (label21);
  gtk_box_pack_start (GTK_BOX (hbox10), label21, FALSE, FALSE, 4);

  scrolledwindow7 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow7);
  gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow7), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  treeview7 = gtk_tree_view_new ();
  gtk_widget_show (treeview7);
  gtk_container_add (GTK_CONTAINER (scrolledwindow7), treeview7);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview7), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview7), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview7), FALSE);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 6), hbox4);

  image7 = create_pixmap (window1, "6.png");
  gtk_widget_show (image7);
  gtk_box_pack_start (GTK_BOX (hbox4), image7, FALSE, FALSE, 0);

  label22 = gtk_label_new_with_mnemonic (_("_Misc"));
  gtk_widget_show (label22);
  gtk_box_pack_start (GTK_BOX (hbox4), label22, FALSE, FALSE, 4);

  label23 = gtk_label_new (_("<span size=\"larger\" >Available Filters</span>"));
  gtk_widget_show (label23);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label23);
  gtk_label_set_use_markup (GTK_LABEL (label23), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label23), 1, 1);

  alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment4);
  gtk_box_pack_start (GTK_BOX (hbox3), alignment4, TRUE, TRUE, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment4), 4, 4, 4, 4);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_container_add (GTK_CONTAINER (alignment4), frame2);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 4, 4, 4, 4);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox1);

  scrolledwindow9 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow9);
  gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow9, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_SHADOW_OUT);

  treeview0 = gtk_tree_view_new ();
  gtk_widget_show (treeview0);
  gtk_container_add (GTK_CONTAINER (scrolledwindow9), treeview0);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview0), FALSE);
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview0), TRUE);
  gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview0), FALSE);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (hbox1), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);
  gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar1), GTK_ORIENTATION_VERTICAL);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar1));

  tmp_image = gtk_image_new_from_stock ("gtk-open", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonOpen = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Open"));
  gtk_widget_show (toolbuttonOpen);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonOpen);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonOpen), tooltips, _("Open filter list [Ctrl+O]"), NULL);
  gtk_widget_add_accelerator (toolbuttonOpen, "clicked", accel_group,
                              GDK_o, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-save", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonSave = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Save"));
  gtk_widget_show (toolbuttonSave);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonSave);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonSave), tooltips, _("Save filter list [Ctrl+S]"), NULL);
  gtk_widget_add_accelerator (toolbuttonSave, "clicked", accel_group,
                              GDK_s, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-save-as", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonScript = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Save Script"));
  gtk_widget_show (toolbuttonScript);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonScript);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonScript), tooltips, _("Save as script [Ctrl+J]"), NULL);
  gtk_widget_add_accelerator (toolbuttonScript, "clicked", accel_group,
                              GDK_j, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
  gtk_widget_show (separatortoolitem1);
  gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem1);

  tmp_image = gtk_image_new_from_stock ("gtk-add", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton1 = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Add"));
  gtk_widget_show (toolbutton1);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton1);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbutton1), tooltips, _("Add filter [Ctrl+A]"), NULL);
  gtk_widget_add_accelerator (toolbutton1, "clicked", accel_group,
                              GDK_a, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-remove", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonRemove = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Remove"));
  gtk_widget_show (toolbuttonRemove);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonRemove);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonRemove), tooltips, _("Remove filter [Del]"), NULL);
  gtk_widget_add_accelerator (toolbuttonRemove, "clicked", accel_group,
                              GDK_Delete, (GdkModifierType)0,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-properties", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonProperties = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Configure"));
  gtk_widget_show (toolbuttonProperties);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonProperties);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonProperties), tooltips, _("Configure filter [Ctrl+F]"), NULL);
  gtk_widget_add_accelerator (toolbuttonProperties, "clicked", accel_group,
                              GDK_f, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-dnd-multiple", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonPartial = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Partial"));
  gtk_widget_show (toolbuttonPartial);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonPartial);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonPartial), tooltips, _("Partial filter [Ctrl+L]"), NULL);
  gtk_widget_add_accelerator (toolbuttonPartial, "clicked", accel_group,
                              GDK_l, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-print-preview", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonPreview = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Preview"));
  gtk_widget_show (toolbuttonPreview);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonPreview);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonPreview), tooltips, _("Preview [Ctrl+P]"), NULL);
  gtk_widget_add_accelerator (toolbuttonPreview, "clicked", accel_group,
                              GDK_p, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-go-up", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonUp = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Up"));
  gtk_widget_show (toolbuttonUp);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonUp);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonUp), tooltips, _("Move filter up [Ctrl+Up]"), NULL);
  gtk_widget_add_accelerator (toolbuttonUp, "clicked", accel_group,
                              GDK_Up, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-go-down", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonDown = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Down"));
  gtk_widget_show (toolbuttonDown);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonDown);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonDown), tooltips, _("Move filter down [Ctrl+Down]"), NULL);
  gtk_widget_add_accelerator (toolbuttonDown, "clicked", accel_group,
                              GDK_Down, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  separatortoolitem3 = (GtkWidget*) gtk_separator_tool_item_new ();
  gtk_widget_show (separatortoolitem3);
  gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem3);

  tmp_image = gtk_image_new_from_stock ("gtk-zoom-fit", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonDVD = (GtkWidget*) gtk_tool_button_new (tmp_image, _("DVD res"));
  gtk_widget_show (toolbuttonDVD);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonDVD);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonDVD), tooltips, _("DVD resolution [Ctrl+1]"), NULL);
  gtk_widget_add_accelerator (toolbuttonDVD, "clicked", accel_group,
                              GDK_1, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-zoom-fit", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonHalfD1 = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Half D1 res"));
  gtk_widget_show (toolbuttonHalfD1);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonHalfD1);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonHalfD1), tooltips, _("Half D1 resolution [Ctrl+2]"), NULL);
  gtk_widget_add_accelerator (toolbuttonHalfD1, "clicked", accel_group,
                              GDK_2, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-zoom-fit", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonSVCD = (GtkWidget*) gtk_tool_button_new (tmp_image, _("SVCD res"));
  gtk_widget_show (toolbuttonSVCD);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonSVCD);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonSVCD), tooltips, _("SVCD resolution [Ctrl+3]"), NULL);
  gtk_widget_add_accelerator (toolbuttonSVCD, "clicked", accel_group,
                              GDK_3, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  tmp_image = gtk_image_new_from_stock ("gtk-zoom-fit", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbuttonVCD = (GtkWidget*) gtk_tool_button_new (tmp_image, _("VCD res"));
  gtk_widget_show (toolbuttonVCD);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbuttonVCD);
  gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (toolbuttonVCD), tooltips, _("VCD resolution [Ctrl+4]"), NULL);
  gtk_widget_add_accelerator (toolbuttonVCD, "clicked", accel_group,
                              GDK_4, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  label2 = gtk_label_new (_("<span size=\"larger\" >Active Filters</span>"));
  gtk_widget_show (label2);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label2);
  gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label2), 1, 1);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window1, window1, "window1");
  GLADE_HOOKUP_OBJECT (window1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (window1, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (window1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (window1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (window1, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (window1, treeview1, "treeview1");
  GLADE_HOOKUP_OBJECT (window1, hbox5, "hbox5");
  GLADE_HOOKUP_OBJECT (window1, image1, "image1");
  GLADE_HOOKUP_OBJECT (window1, label11, "label11");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow2, "scrolledwindow2");
  GLADE_HOOKUP_OBJECT (window1, treeview2, "treeview2");
  GLADE_HOOKUP_OBJECT (window1, hbox6, "hbox6");
  GLADE_HOOKUP_OBJECT (window1, image2, "image2");
  GLADE_HOOKUP_OBJECT (window1, label17, "label17");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow3, "scrolledwindow3");
  GLADE_HOOKUP_OBJECT (window1, treeview3, "treeview3");
  GLADE_HOOKUP_OBJECT (window1, hbox7, "hbox7");
  GLADE_HOOKUP_OBJECT (window1, image3, "image3");
  GLADE_HOOKUP_OBJECT (window1, label18, "label18");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow4, "scrolledwindow4");
  GLADE_HOOKUP_OBJECT (window1, treeview4, "treeview4");
  GLADE_HOOKUP_OBJECT (window1, hbox8, "hbox8");
  GLADE_HOOKUP_OBJECT (window1, image4, "image4");
  GLADE_HOOKUP_OBJECT (window1, label19, "label19");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow5, "scrolledwindow5");
  GLADE_HOOKUP_OBJECT (window1, treeview5, "treeview5");
  GLADE_HOOKUP_OBJECT (window1, hbox9, "hbox9");
  GLADE_HOOKUP_OBJECT (window1, image5, "image5");
  GLADE_HOOKUP_OBJECT (window1, label20, "label20");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow6, "scrolledwindow6");
  GLADE_HOOKUP_OBJECT (window1, treeview6, "treeview6");
  GLADE_HOOKUP_OBJECT (window1, hbox10, "hbox10");
  GLADE_HOOKUP_OBJECT (window1, image6, "image6");
  GLADE_HOOKUP_OBJECT (window1, label21, "label21");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow7, "scrolledwindow7");
  GLADE_HOOKUP_OBJECT (window1, treeview7, "treeview7");
  GLADE_HOOKUP_OBJECT (window1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (window1, image7, "image7");
  GLADE_HOOKUP_OBJECT (window1, label22, "label22");
  GLADE_HOOKUP_OBJECT (window1, label23, "label23");
  GLADE_HOOKUP_OBJECT (window1, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (window1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (window1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (window1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow9, "scrolledwindow9");
  GLADE_HOOKUP_OBJECT (window1, treeview0, "treeview0");
  GLADE_HOOKUP_OBJECT (window1, toolbar1, "toolbar1");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonOpen, "toolbuttonOpen");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonSave, "toolbuttonSave");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonScript, "toolbuttonScript");
  GLADE_HOOKUP_OBJECT (window1, separatortoolitem1, "separatortoolitem1");
  GLADE_HOOKUP_OBJECT (window1, toolbutton1, "toolbutton1");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonRemove, "toolbuttonRemove");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonProperties, "toolbuttonProperties");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonPartial, "toolbuttonPartial");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonPreview, "toolbuttonPreview");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonUp, "toolbuttonUp");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonDown, "toolbuttonDown");
  GLADE_HOOKUP_OBJECT (window1, separatortoolitem3, "separatortoolitem3");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonDVD, "toolbuttonDVD");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonHalfD1, "toolbuttonHalfD1");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonSVCD, "toolbuttonSVCD");
  GLADE_HOOKUP_OBJECT (window1, toolbuttonVCD, "toolbuttonVCD");
  GLADE_HOOKUP_OBJECT (window1, label2, "label2");
  GLADE_HOOKUP_OBJECT_NO_REF (window1, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (window1), accel_group);

  return window1;
}

