//
// C++ Implementation: DIA_decimate
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
// C++ Implementation: DIA_dectel
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include <config.h>

#include <sys/types.h>
#include <sys/stat.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_library/default.h"

#include "ADM_video/ADM_vidDGbob_param.h"

#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),param->y);}
#define MENU_GET(x,y) { param->y        = getRangeInMenu(WID(x));}

#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}


static GtkWidget        *create_dialog1 (void);

uint8_t DIA_getDGbob(DGBobparam *param)
{
GtkWidget *dialog;
GtkAdjustment *adj;

int ret=0;
        dialog=create_dialog1();
        
        // Update
        MENU_SET(optionmenu1,mode);
        CHECK_SET(checkbutton1,ap);
        if(param->order)  RADIO_SET(radiobutton2,1);
                else     RADIO_SET(radiobutton1,1);
         gtk_range_set_value (GTK_RANGE(WID(hscale1)),(gdouble)param->thresh);
        // run
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                MENU_GET(optionmenu1,mode);
                CHECK_GET(checkbutton1,ap);       
                param->order= RADIO_GET(radiobutton2);                
                adj=gtk_range_get_adjustment (GTK_RANGE(WID(hscale1)));
                param->thresh=GTK_ADJUSTMENT(adj)->value;
                ret=1;
        }
        gtk_widget_destroy(dialog);
        return ret;
}


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *hbox1;
  GtkWidget *radiobutton1;
  GSList *radiobutton1_group = NULL;
  GtkWidget *radiobutton2;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *mode_0;
  GtkWidget *one;
  GtkWidget *double1;
  GtkWidget *hscale1;
  GtkWidget *checkbutton1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("DGbob"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Field Order"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Mode"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Threshold"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Extra check"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_table_attach (GTK_TABLE (table1), hbox1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  radiobutton1 = gtk_radio_button_new_with_mnemonic (NULL, _("BFF"));
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton1, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton1), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton1));

  radiobutton2 = gtk_radio_button_new_with_mnemonic (NULL, _("TFF"));
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox1), radiobutton2, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton2), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton2));

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  mode_0 = gtk_menu_item_new_with_mnemonic (_("Keep fps and frame #"));
  gtk_widget_show (mode_0);
  gtk_container_add (GTK_CONTAINER (menu1), mode_0);

  one = gtk_menu_item_new_with_mnemonic (_("Double fps and frame #"));
  gtk_widget_show (one);
  gtk_container_add (GTK_CONTAINER (menu1), one);

  double1 = gtk_menu_item_new_with_mnemonic (_("Double frame # (slow mo)"));
  gtk_widget_show (double1);
  gtk_container_add (GTK_CONTAINER (menu1), double1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  hscale1 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (12, 0, 255, 1, 1, 1)));
  gtk_widget_show (hscale1);
  gtk_table_attach (GTK_TABLE (table1), hscale1, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  checkbutton1 = gtk_check_button_new_with_mnemonic (_("(avoid using it)"));
  gtk_widget_show (checkbutton1);
  gtk_table_attach (GTK_TABLE (table1), checkbutton1, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton1, "radiobutton1");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton2, "radiobutton2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, mode_0, "mode_0");
  GLADE_HOOKUP_OBJECT (dialog1, one, "one");
  GLADE_HOOKUP_OBJECT (dialog1, double1, "double1");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton1, "checkbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

