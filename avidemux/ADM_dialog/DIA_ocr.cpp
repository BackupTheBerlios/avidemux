//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <config.h>

#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <gtk/gtk.h>

#include "callbacks.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_assert.h" 
 GtkWidget	*DIA_ocr(void);
GtkWidget   *DIA_ocr (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox7;
  GtkWidget *frame1;
  GtkWidget *alignment1;
  GtkWidget *vbox2;
  GtkWidget *buttonLoad;
  GtkWidget *buttonSave;
  GtkWidget *label1;
  GtkWidget *frame8;
  GtkWidget *alignment8;
  GtkWidget *table1;
  GtkWidget *label23;
  GtkWidget *label24;
  GtkWidget *labelNbGLyph;
  GtkWidget *labelNbLines;
  GtkWidget *label22;
  GtkWidget *frame2;
  GtkWidget *alignment2;
  GtkWidget *vbox3;
  GtkWidget *frame3;
  GtkWidget *alignment3;
  GtkWidget *vbox4;
  GtkWidget *labelVobSub;
  GtkWidget *labelLanguage;
  GtkWidget *buttonSelectVob;
  GtkWidget *label14;
  GtkWidget *frame4;
  GtkWidget *alignment4;
  GtkWidget *vbox5;
  GtkWidget *labelSRT;
  GtkWidget *buttonSRT;
  GtkWidget *label17;
  GtkWidget *label4;
  GtkWidget *buttonStart;
  GtkWidget *frame5;
  GtkWidget *alignment5;
  GtkWidget *drawingareaVobSub;
  GtkWidget *label19;
  GtkWidget *hbox2;
  GtkWidget *Current_Glyph;
  GtkWidget *alignment6;
  GtkWidget *vbox6;
  GtkWidget *drawingareaMini;
  GtkWidget *entryEntry;
  GtkWidget *hbox3;
  GtkWidget *buttonIgnore;
  GtkWidget *buttonSkip;
  GtkWidget *buttonAccept;
  GtkWidget *label20;
  GtkWidget *frame7;
  GtkWidget *alignment7;
  GtkWidget *labelText;
  GtkWidget *label21;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mini OCR"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox7, FALSE, FALSE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox7), frame1, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_NONE);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (alignment1), vbox2);

  buttonLoad = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonLoad);
  gtk_box_pack_start (GTK_BOX (vbox2), buttonLoad, FALSE, FALSE, 0);

  buttonSave = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (buttonSave);
  gtk_box_pack_start (GTK_BOX (vbox2), buttonSave, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("<b>Glyphs</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);

  frame8 = gtk_frame_new (NULL);
  gtk_widget_show (frame8);
  gtk_box_pack_start (GTK_BOX (vbox7), frame8, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame8), GTK_SHADOW_NONE);

  alignment8 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment8);
  gtk_container_add (GTK_CONTAINER (frame8), alignment8);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment8), 0, 0, 12, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (alignment8), table1);

  label23 = gtk_label_new (_("# Glyphs:"));
  gtk_widget_show (label23);
  gtk_table_attach (GTK_TABLE (table1), label23, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label23), 0, 0.5);

  label24 = gtk_label_new (_("# Lines :"));
  gtk_widget_show (label24);
  gtk_table_attach (GTK_TABLE (table1), label24, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  labelNbGLyph = gtk_label_new (_("000"));
  gtk_widget_show (labelNbGLyph);
  gtk_table_attach (GTK_TABLE (table1), labelNbGLyph, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelNbGLyph), 0, 0.5);

  labelNbLines = gtk_label_new (_("000"));
  gtk_widget_show (labelNbLines);
  gtk_table_attach (GTK_TABLE (table1), labelNbLines, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelNbLines), 0, 0.5);

  label22 = gtk_label_new (_("<b>Stats</b>"));
  gtk_widget_show (label22);
  gtk_frame_set_label_widget (GTK_FRAME (frame8), label22);
  gtk_label_set_use_markup (GTK_LABEL (label22), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (hbox1), frame2, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (alignment2), vbox3);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox3), frame3, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame3), GTK_SHADOW_NONE);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (frame3), alignment3);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment3), 0, 0, 12, 0);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (alignment3), vbox4);

  labelVobSub = gtk_label_new ("");
  gtk_widget_show (labelVobSub);
  gtk_box_pack_start (GTK_BOX (vbox4), labelVobSub, FALSE, FALSE, 0);

  labelLanguage = gtk_label_new ("");
  gtk_widget_show (labelLanguage);
  gtk_box_pack_start (GTK_BOX (vbox4), labelLanguage, FALSE, FALSE, 0);

  buttonSelectVob = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonSelectVob);
  gtk_box_pack_start (GTK_BOX (vbox4), buttonSelectVob, FALSE, FALSE, 0);

  label14 = gtk_label_new (_("<b>Vobsub</b>"));
  gtk_widget_show (label14);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label14);
  gtk_label_set_use_markup (GTK_LABEL (label14), TRUE);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (vbox3), frame4, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame4), GTK_SHADOW_NONE);

  alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (frame4), alignment4);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment4), 0, 0, 12, 0);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (alignment4), vbox5);

  labelSRT = gtk_label_new ("");
  gtk_widget_show (labelSRT);
  gtk_box_pack_start (GTK_BOX (vbox5), labelSRT, FALSE, FALSE, 0);

  buttonSRT = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonSRT);
  gtk_box_pack_start (GTK_BOX (vbox5), buttonSRT, FALSE, FALSE, 0);

  label17 = gtk_label_new (_("<b>Output Srt</b>"));
  gtk_widget_show (label17);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label17);
  gtk_label_set_use_markup (GTK_LABEL (label17), TRUE);

  label4 = gtk_label_new (_("<b>Files Select</b>"));
  gtk_widget_show (label4);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label4);
  gtk_label_set_use_markup (GTK_LABEL (label4), TRUE);

  buttonStart = gtk_button_new_with_mnemonic (_("Start OCR"));
  gtk_widget_show (buttonStart);
  gtk_box_pack_start (GTK_BOX (vbox1), buttonStart, FALSE, FALSE, 0);

  frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox1), frame5, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame5), GTK_SHADOW_NONE);

  alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (frame5), alignment5);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment5), 0, 0, 12, 0);

  drawingareaVobSub = gtk_drawing_area_new ();
  gtk_widget_show (drawingareaVobSub);
  gtk_container_add (GTK_CONTAINER (alignment5), drawingareaVobSub);

  label19 = gtk_label_new (_("<b>Bitmap</b>"));
  gtk_widget_show (label19);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label19);
  gtk_label_set_use_markup (GTK_LABEL (label19), TRUE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  Current_Glyph = gtk_frame_new (NULL);
  gtk_widget_show (Current_Glyph);
  gtk_box_pack_start (GTK_BOX (hbox2), Current_Glyph, FALSE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (Current_Glyph), GTK_SHADOW_NONE);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (Current_Glyph), alignment6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 0);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (alignment6), vbox6);

  drawingareaMini = gtk_drawing_area_new ();
  gtk_widget_show (drawingareaMini);
  gtk_box_pack_start (GTK_BOX (vbox6), drawingareaMini, TRUE, TRUE, 0);

  entryEntry = gtk_entry_new ();
  gtk_widget_show (entryEntry);
  gtk_box_pack_start (GTK_BOX (vbox6), entryEntry, FALSE, FALSE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox6), hbox3, TRUE, TRUE, 0);

  buttonIgnore = gtk_button_new_with_mnemonic (_("Ignore"));
  gtk_widget_show (buttonIgnore);
  gtk_box_pack_start (GTK_BOX (hbox3), buttonIgnore, FALSE, FALSE, 0);

  buttonSkip = gtk_button_new_with_mnemonic (_("Skip"));
  gtk_widget_show (buttonSkip);
  gtk_box_pack_start (GTK_BOX (hbox3), buttonSkip, FALSE, FALSE, 0);

  buttonAccept = gtk_button_new_with_mnemonic (_("Ok"));
  gtk_widget_show (buttonAccept);
  gtk_box_pack_start (GTK_BOX (hbox3), buttonAccept, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS (buttonAccept, GTK_CAN_DEFAULT);

  label20 = gtk_label_new (_("<b>Current Glyph</b>"));
  gtk_widget_show (label20);
  gtk_frame_set_label_widget (GTK_FRAME (Current_Glyph), label20);
  gtk_label_set_use_markup (GTK_LABEL (label20), TRUE);

  frame7 = gtk_frame_new (NULL);
  gtk_widget_show (frame7);
  gtk_box_pack_start (GTK_BOX (hbox2), frame7, FALSE, FALSE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame7), GTK_SHADOW_NONE);

  alignment7 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment7);
  gtk_container_add (GTK_CONTAINER (frame7), alignment7);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment7), 0, 0, 12, 0);

  labelText = gtk_label_new ("");
  gtk_widget_show (labelText);
  gtk_container_add (GTK_CONTAINER (alignment7), labelText);

  label21 = gtk_label_new (_("<b>Text:</b>"));
  gtk_widget_show (label21);
  gtk_frame_set_label_widget (GTK_FRAME (frame7), label21);
  gtk_label_set_use_markup (GTK_LABEL (label21), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox7, "vbox7");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, buttonLoad, "buttonLoad");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSave, "buttonSave");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame8, "frame8");
  GLADE_HOOKUP_OBJECT (dialog1, alignment8, "alignment8");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label23, "label23");
  GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
  GLADE_HOOKUP_OBJECT (dialog1, labelNbGLyph, "labelNbGLyph");
  GLADE_HOOKUP_OBJECT (dialog1, labelNbLines, "labelNbLines");
  GLADE_HOOKUP_OBJECT (dialog1, label22, "label22");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (dialog1, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (dialog1, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (dialog1, vbox4, "vbox4");
  GLADE_HOOKUP_OBJECT (dialog1, labelVobSub, "labelVobSub");
  GLADE_HOOKUP_OBJECT (dialog1, labelLanguage, "labelLanguage");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSelectVob, "buttonSelectVob");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog1, frame4, "frame4");
  GLADE_HOOKUP_OBJECT (dialog1, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (dialog1, vbox5, "vbox5");
  GLADE_HOOKUP_OBJECT (dialog1, labelSRT, "labelSRT");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSRT, "buttonSRT");
  GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, buttonStart, "buttonStart");
  GLADE_HOOKUP_OBJECT (dialog1, frame5, "frame5");
  GLADE_HOOKUP_OBJECT (dialog1, alignment5, "alignment5");
  GLADE_HOOKUP_OBJECT (dialog1, drawingareaVobSub, "drawingareaVobSub");
  GLADE_HOOKUP_OBJECT (dialog1, label19, "label19");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, Current_Glyph, "Current_Glyph");
  GLADE_HOOKUP_OBJECT (dialog1, alignment6, "alignment6");
  GLADE_HOOKUP_OBJECT (dialog1, vbox6, "vbox6");
  GLADE_HOOKUP_OBJECT (dialog1, drawingareaMini, "drawingareaMini");
  GLADE_HOOKUP_OBJECT (dialog1, entryEntry, "entryEntry");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, buttonIgnore, "buttonIgnore");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSkip, "buttonSkip");
  GLADE_HOOKUP_OBJECT (dialog1, buttonAccept, "buttonAccept");
  GLADE_HOOKUP_OBJECT (dialog1, label20, "label20");
  GLADE_HOOKUP_OBJECT (dialog1, frame7, "frame7");
  GLADE_HOOKUP_OBJECT (dialog1, alignment7, "alignment7");
  GLADE_HOOKUP_OBJECT (dialog1, labelText, "labelText");
  GLADE_HOOKUP_OBJECT (dialog1, label21, "label21");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  gtk_widget_grab_default (buttonAccept);
  return dialog1;
}

