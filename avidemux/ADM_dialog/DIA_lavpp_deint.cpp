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
#include "ADM_lavcodec.h"


#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>



#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>


#include "ADM_library/default.h"//#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h" 

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_video/ADM_lavpp_deintparam.h"

#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}                                 

static GtkWidget        *create_dialog1 (void);

uint8_t DIA_lavpp_deint(lavc_pp_param *param)
{
  GtkWidget *dialog;
  uint8_t r=0;
  
  
  dialog=create_dialog1();
  gtk_register_dialog(dialog);
  CHECK_SET(checkbuttonAutoLevel,autolevel);
 
  
  gtk_option_menu_set_history (GTK_OPTION_MENU(WID(optionmenu1)),param->deintType);
  
  if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
  {
    CHECK_GET(checkbuttonAutoLevel,autolevel);
    param->deintType=getRangeInMenu(WID(optionmenu1));
    r=1; 
  }
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return r;
}

//**************************
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *checkbuttonAutoLevel;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *none1;
  GtkWidget *linear_blend1;
  GtkWidget *linear_interpolation1;
  GtkWidget *cubic_interpolation1;
  GtkWidget *median_deinterlacer1;
  GtkWidget *ffmpeg_deinterlacer1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Lavc PP Deint"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Deinterlacing : "));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("AutoLevel :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  checkbuttonAutoLevel = gtk_check_button_new_with_mnemonic (_(" "));
  gtk_widget_show (checkbuttonAutoLevel);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonAutoLevel, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  none1 = gtk_menu_item_new_with_mnemonic (_("None"));
  gtk_widget_show (none1);
  gtk_container_add (GTK_CONTAINER (menu1), none1);

  linear_blend1 = gtk_menu_item_new_with_mnemonic (_("Linear Blend"));
  gtk_widget_show (linear_blend1);
  gtk_container_add (GTK_CONTAINER (menu1), linear_blend1);

  linear_interpolation1 = gtk_menu_item_new_with_mnemonic (_("Linear Interpolation"));
  gtk_widget_show (linear_interpolation1);
  gtk_container_add (GTK_CONTAINER (menu1), linear_interpolation1);

  cubic_interpolation1 = gtk_menu_item_new_with_mnemonic (_("Cubic interpolation"));
  gtk_widget_show (cubic_interpolation1);
  gtk_container_add (GTK_CONTAINER (menu1), cubic_interpolation1);

  median_deinterlacer1 = gtk_menu_item_new_with_mnemonic (_("Median Deinterlacer"));
  gtk_widget_show (median_deinterlacer1);
  gtk_container_add (GTK_CONTAINER (menu1), median_deinterlacer1);

  ffmpeg_deinterlacer1 = gtk_menu_item_new_with_mnemonic (_("FFmpeg Deinterlacer"));
  gtk_widget_show (ffmpeg_deinterlacer1);
  gtk_container_add (GTK_CONTAINER (menu1), ffmpeg_deinterlacer1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

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
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonAutoLevel, "checkbuttonAutoLevel");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, none1, "none1");
  GLADE_HOOKUP_OBJECT (dialog1, linear_blend1, "linear_blend1");
  GLADE_HOOKUP_OBJECT (dialog1, linear_interpolation1, "linear_interpolation1");
  GLADE_HOOKUP_OBJECT (dialog1, cubic_interpolation1, "cubic_interpolation1");
  GLADE_HOOKUP_OBJECT (dialog1, median_deinterlacer1, "median_deinterlacer1");
  GLADE_HOOKUP_OBJECT (dialog1, ffmpeg_deinterlacer1, "ffmpeg_deinterlacer1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

