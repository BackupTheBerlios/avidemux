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

#include "ADM_video/ADM_vidDecDec_param.h"
#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),param->y);}
#define MENU_GET(x,y) { param->y	= getRangeInMenu(WID(x));}
static GtkWidget	*create_dialog1 (void);
#define SPIN_GET(x,y) {param->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)param->y) ;}

uint8_t DIA_getDecombDecimate(DECIMATE_PARAM *param)
{
GtkWidget *dialog;
int ret=0;
	dialog=create_dialog1();
	
	// Update
	
	gtk_write_entry_float(WID(entryThresh),param->threshold);
	gtk_write_entry_float(WID(entry2),param->threshold2);
	
	MENU_SET(optionmenu1,mode);
	MENU_SET(optionmenu2,quality);
	SPIN_SET(spinbuttonCycle,cycle);
	
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		MENU_GET(optionmenu1,mode);
		MENU_GET(optionmenu2,quality);
		SPIN_GET(spinbuttonCycle,cycle);
		#define RD_ENTRY(x,y) {param->y=gtk_read_entry_float(WID(x));}
		RD_ENTRY(entryThresh,threshold);
		RD_ENTRY(entry2,threshold2);
		ret=1;
	
	}
	gtk_widget_destroy(dialog);
	return ret;
}
//________________________________________________
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkObject *spinbuttonCycle_adj;
  GtkWidget *spinbuttonCycle;
  GtkWidget *label2;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *discard_closer1;
  GtkWidget *replace_by_interpolate1;
  GtkWidget *discard_from_longest_dups__anim__s_1;
  GtkWidget *pulldown_dups_removal1;
  GtkWidget *label3;
  GtkWidget *entryThresh;
  GtkWidget *label4;
  GtkWidget *entry2;
  GtkWidget *label5;
  GtkWidget *optionmenu2;
  GtkWidget *menu2;
  GtkWidget *fastest__no_chroma_partial_luma_1;
  GtkWidget *fast__partial_luma_and_chroma_1;
  GtkWidget *medium__full_luma_not_chroma_1;
  GtkWidget *slow__full_luma_and_chroma_1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Decomb Decimate"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Cycle"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  spinbuttonCycle_adj = gtk_adjustment_new (1, 2, 40, 1, 10, 10);
  spinbuttonCycle = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonCycle_adj), 1, 0);
  gtk_widget_show (spinbuttonCycle);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonCycle, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonCycle), TRUE);

  label2 = gtk_label_new (_("Mode"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  discard_closer1 = gtk_menu_item_new_with_mnemonic (_("Discard Closer"));
  gtk_widget_show (discard_closer1);
  gtk_container_add (GTK_CONTAINER (menu1), discard_closer1);

  replace_by_interpolate1 = gtk_menu_item_new_with_mnemonic (_("Replace by interpolate"));
  gtk_widget_show (replace_by_interpolate1);
  gtk_container_add (GTK_CONTAINER (menu1), replace_by_interpolate1);

  discard_from_longest_dups__anim__s_1 = gtk_menu_item_new_with_mnemonic (_("Discard from longest dups (anim\303\251s)"));
  gtk_widget_show (discard_from_longest_dups__anim__s_1);
  gtk_container_add (GTK_CONTAINER (menu1), discard_from_longest_dups__anim__s_1);

  pulldown_dups_removal1 = gtk_menu_item_new_with_mnemonic (_("Pulldown dups removal"));
  gtk_widget_show (pulldown_dups_removal1);
  gtk_container_add (GTK_CONTAINER (menu1), pulldown_dups_removal1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  label3 = gtk_label_new (_("Threshold "));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  entryThresh = gtk_entry_new ();
  gtk_widget_show (entryThresh);
  gtk_table_attach (GTK_TABLE (table1), entryThresh, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label4 = gtk_label_new (_("Threshold2 "));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  entry2 = gtk_entry_new ();
  gtk_widget_show (entry2);
  gtk_table_attach (GTK_TABLE (table1), entry2, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label5 = gtk_label_new (_("Quality "));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  optionmenu2 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu2);
  gtk_table_attach (GTK_TABLE (table1), optionmenu2, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu2 = gtk_menu_new ();

  fastest__no_chroma_partial_luma_1 = gtk_menu_item_new_with_mnemonic (_("Fastest (no chroma,partial luma)"));
  gtk_widget_show (fastest__no_chroma_partial_luma_1);
  gtk_container_add (GTK_CONTAINER (menu2), fastest__no_chroma_partial_luma_1);

  fast__partial_luma_and_chroma_1 = gtk_menu_item_new_with_mnemonic (_("Fast (partial luma and chroma)"));
  gtk_widget_show (fast__partial_luma_and_chroma_1);
  gtk_container_add (GTK_CONTAINER (menu2), fast__partial_luma_and_chroma_1);

  medium__full_luma_not_chroma_1 = gtk_menu_item_new_with_mnemonic (_("Medium (full luma not chroma)"));
  gtk_widget_show (medium__full_luma_not_chroma_1);
  gtk_container_add (GTK_CONTAINER (menu2), medium__full_luma_not_chroma_1);

  slow__full_luma_and_chroma_1 = gtk_menu_item_new_with_mnemonic (_("Slow (full luma and chroma)"));
  gtk_widget_show (slow__full_luma_and_chroma_1);
  gtk_container_add (GTK_CONTAINER (menu2), slow__full_luma_and_chroma_1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu2), menu2);

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
/*
  g_signal_connect ((gpointer) discard_closer1, "activate",
                    G_CALLBACK (on_discard_closer1_activate),
                    NULL);
  g_signal_connect ((gpointer) replace_by_interpolate1, "activate",
                    G_CALLBACK (on_replace_by_interpolate1_activate),
                    NULL);
  g_signal_connect ((gpointer) discard_from_longest_dups__anim__s_1, "activate",
                    G_CALLBACK (on_discard_from_longest_dups__anim__s_1_activate),
                    NULL);
  g_signal_connect ((gpointer) pulldown_dups_removal1, "activate",
                    G_CALLBACK (on_pulldown_dups_removal1_activate),
                    NULL);
  g_signal_connect ((gpointer) fastest__no_chroma_partial_luma_1, "activate",
                    G_CALLBACK (on_fastest__no_chroma_partial_luma_1_activate),
                    NULL);
  g_signal_connect ((gpointer) fast__partial_luma_and_chroma_1, "activate",
                    G_CALLBACK (on_fast__partial_luma_and_chroma_1_activate),
                    NULL);
  g_signal_connect ((gpointer) medium__full_luma_not_chroma_1, "activate",
                    G_CALLBACK (on_medium__full_luma_not_chroma_1_activate),
                    NULL);
  g_signal_connect ((gpointer) slow__full_luma_and_chroma_1, "activate",
                    G_CALLBACK (on_slow__full_luma_and_chroma_1_activate),
                    NULL);
*/
  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonCycle, "spinbuttonCycle");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, discard_closer1, "discard_closer1");
  GLADE_HOOKUP_OBJECT (dialog1, replace_by_interpolate1, "replace_by_interpolate1");
  GLADE_HOOKUP_OBJECT (dialog1, discard_from_longest_dups__anim__s_1, "discard_from_longest_dups__anim__s_1");
  GLADE_HOOKUP_OBJECT (dialog1, pulldown_dups_removal1, "pulldown_dups_removal1");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, entryThresh, "entryThresh");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, entry2, "entry2");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu2, "optionmenu2");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, fastest__no_chroma_partial_luma_1, "fastest__no_chroma_partial_luma_1");
  GLADE_HOOKUP_OBJECT (dialog1, fast__partial_luma_and_chroma_1, "fast__partial_luma_and_chroma_1");
  GLADE_HOOKUP_OBJECT (dialog1, medium__full_luma_not_chroma_1, "medium__full_luma_not_chroma_1");
  GLADE_HOOKUP_OBJECT (dialog1, slow__full_luma_and_chroma_1, "slow__full_luma_and_chroma_1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

