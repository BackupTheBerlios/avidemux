
/***************************************************************************
                         DIA_resizeWiz
                             -------------------

                           Ui for hue & sat
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
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h"

#include "DIA_resizeWiz.h"

static GtkWidget       *create_dialog1 (void);

uint8_t DIA_resizeWiz(RESWIZ_FORMAT *format, RESWIZ_AR *source, RESWIZ_AR *destination)
{
uint8_t r=0;
GtkWidget *dialog=create_dialog1();
        
        

        // Initialize
#define COMBINE(x,y) gtk_combo_box_set_active(GTK_COMBO_BOX(WID(x)),(int)*y)

                COMBINE(comboboxSource,source);
                COMBINE(comboboxFinal,destination);
                COMBINE(comboboxTarget,format);

        gtk_register_dialog(dialog);
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                r=1;
#undef COMBINE
#define COMBINE(x) gtk_combo_box_get_active(GTK_COMBO_BOX(WID(x)));
                *format=(RESWIZ_FORMAT) COMBINE(comboboxTarget);
                *source=(RESWIZ_AR)     COMBINE(comboboxSource);
                *destination=(RESWIZ_AR)COMBINE(comboboxFinal);

        }


        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return r;
}


GtkWidget*
        create_dialog1 (void)
{
    GtkWidget *dialog1;
    GtkWidget *dialog_vbox1;
    GtkWidget *table1;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *comboboxSource;
    GtkWidget *label3;
    GtkWidget *comboboxFinal;
    GtkWidget *comboboxTarget;
    GtkWidget *dialog_action_area1;
    GtkWidget *cancelbutton1;
    GtkWidget *okbutton1;

    dialog1 = gtk_dialog_new ();
    gtk_window_set_title (GTK_WINDOW (dialog1), _("Resize for VCD/SVCD/...."));
    gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
    gtk_widget_show (dialog_vbox1);

    table1 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (table1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

    label1 = gtk_label_new (_("Target type : "));
    gtk_widget_show (label1);
    gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

    label2 = gtk_label_new (_("Source Aspect Ratio :"));
    gtk_widget_show (label2);
    gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

    comboboxSource = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxSource);
    gtk_table_attach (GTK_TABLE (table1), comboboxSource, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (comboboxSource), 1);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxSource), _("1:1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxSource), _("4:3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxSource), _("16:9"));

    label3 = gtk_label_new (_("Destination Aspect Ratio :"));
    gtk_widget_show (label3);
    gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

    comboboxFinal = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxFinal);
    gtk_table_attach (GTK_TABLE (table1), comboboxFinal, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (comboboxFinal), 1);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxFinal), _("1:1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxFinal), _("4:3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxFinal), _("16:9"));

    comboboxTarget = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxTarget);
    gtk_table_attach (GTK_TABLE (table1), comboboxTarget, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (comboboxTarget), 1);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxTarget), _("VCD"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxTarget), _("SVCD"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxTarget), _("DVD"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxTarget), _("DVD, half D1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxTarget), _("PSP"));

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
    GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
    GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
    GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxSource, "comboboxSource");
    GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxFinal, "comboboxFinal");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxTarget, "comboboxTarget");
    GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
    GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
    GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

    return dialog1;
}

