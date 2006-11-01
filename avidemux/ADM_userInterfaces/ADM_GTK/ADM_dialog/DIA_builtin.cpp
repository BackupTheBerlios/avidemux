//
/**/
/***************************************************************************
                          DIA_hue
                             -------------------

                           Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/5 by mean
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



#include <config.h>


#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>


#include "default.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "default.h"
#include "ADM_assert.h"
static GtkWidget *create_dialog1 (void);
#define CHECK_SET(x) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),1);}
uint8_t DIA_builtin(void)
{
GtkWidget *dialog;
        dialog=create_dialog1();

#ifdef USE_ALTIVEC
        CHECK_SET(checkbutton1)
#endif
#ifdef USE_MP3
        CHECK_SET(checkbutton2)
#endif
#ifdef USE_AC3
        CHECK_SET(checkbutton3)
#endif
#ifdef USE_XVID_4
        CHECK_SET(checkbutton4)
#endif
#ifdef USE_X264
        CHECK_SET(checkbutton5)
#endif
#ifdef USE_FREETYPE
        CHECK_SET(checkbutton6)
#endif
#ifdef USE_ESD
        CHECK_SET(checkbutton7)
#endif
#ifdef USE_ARTS
        CHECK_SET(checkbutton8)
#endif
#ifdef USE_VORBIS
        CHECK_SET(checkbutton9)
#endif
#ifdef CYG_MANGLING
        CHECK_SET(checkbutton10)
#endif
#ifdef USE_FAAC
        CHECK_SET(checkbutton11)
#endif
#ifdef USE_FAAD
        CHECK_SET(checkbutton12)
#endif
#ifdef USE_LIBDCA
        CHECK_SET(checkbutton13)
#endif


        gtk_register_dialog(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));

        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return 1;

}

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *label1;
  GtkWidget *hseparator1;
  GtkWidget *table2;
  GtkWidget *label6;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label11;
  GtkWidget *label7;
  GtkWidget *label5;
  GtkWidget *checkbutton1;
  GtkWidget *checkbutton2;
  GtkWidget *checkbutton3;
  GtkWidget *checkbutton4;
  GtkWidget *checkbutton5;
  GtkWidget *checkbutton6;
  GtkWidget *checkbutton7;
  GtkWidget *checkbutton8;
  GtkWidget *checkbutton9;
  GtkWidget *checkbutton10;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *checkbutton11;
  GtkWidget *checkbutton12;
  GtkWidget *label14;
  GtkWidget *checkbutton13;
  GtkWidget *dialog_action_area1;
  GtkWidget *closebutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Builtin support for"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("If an option is not checked, it means\nyou must RECOMPILE avidemux to have\nsupport for it!"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_FILL);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox2), hseparator1, TRUE, TRUE, 0);

  table2 = gtk_table_new (13, 2, FALSE);
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (vbox1), table2, TRUE, TRUE, 0);

  label6 = gtk_label_new (_("X264"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table2), label6, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label8 = gtk_label_new (_("ESD"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table2), label8, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  label9 = gtk_label_new (_("Arts"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table2), label9, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  label10 = gtk_label_new (_("Vorbis"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label2 = gtk_label_new (_("Altivec"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table2), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Mad"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table2), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("A52"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table2), label4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label11 = gtk_label_new (_("Win32"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label7 = gtk_label_new (_("Freetype"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table2), label7, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label5 = gtk_label_new (_("Xvid"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table2), label5, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  checkbutton1 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton1);
  gtk_table_attach (GTK_TABLE (table2), checkbutton1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton2 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton2);
  gtk_table_attach (GTK_TABLE (table2), checkbutton2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton3 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton3);
  gtk_table_attach (GTK_TABLE (table2), checkbutton3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton4 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton4);
  gtk_table_attach (GTK_TABLE (table2), checkbutton4, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton5 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton5);
  gtk_table_attach (GTK_TABLE (table2), checkbutton5, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton6 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton6);
  gtk_table_attach (GTK_TABLE (table2), checkbutton6, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton7 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton7);
  gtk_table_attach (GTK_TABLE (table2), checkbutton7, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton8 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton8);
  gtk_table_attach (GTK_TABLE (table2), checkbutton8, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton9 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton9);
  gtk_table_attach (GTK_TABLE (table2), checkbutton9, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton10 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton10);
  gtk_table_attach (GTK_TABLE (table2), checkbutton10, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label12 = gtk_label_new (_("FAAC"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  label13 = gtk_label_new (_("FAAD"));
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table2), label13, 0, 1, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  checkbutton11 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton11);
  gtk_table_attach (GTK_TABLE (table2), checkbutton11, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbutton12 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton12);
  gtk_table_attach (GTK_TABLE (table2), checkbutton12, 1, 2, 11, 12,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label14 = gtk_label_new (_("libDCA/libDTS"));
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table2), label14, 0, 1, 12, 13,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  checkbutton13 = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton13);
  gtk_table_attach (GTK_TABLE (table2), checkbutton13, 1, 2, 12, 13,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton1, "checkbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton2, "checkbutton2");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton3, "checkbutton3");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton4, "checkbutton4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton5, "checkbutton5");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton6, "checkbutton6");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton7, "checkbutton7");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton8, "checkbutton8");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton9, "checkbutton9");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton10, "checkbutton10");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton11, "checkbutton11");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton12, "checkbutton12");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton13, "checkbutton13");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, closebutton1, "closebutton1");

  return dialog1;
}

