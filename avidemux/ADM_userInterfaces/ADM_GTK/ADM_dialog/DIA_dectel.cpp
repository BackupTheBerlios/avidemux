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

#include "default.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"


#include "ADM_video/ADM_vidDecTel_param.h"
#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),param->y);}
#define MENU_GET(x,y) { param->y	= getRangeInMenu(WID(x));}

#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y);}
#define CHECK_GET(x,y) {param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}


static GtkWidget	*create_dialog1 (void);

uint8_t DIA_getDecombTelecide(TelecideParam *param)
{
GtkWidget *dialog;
int ret=0;
	dialog=create_dialog1();
	
	// Update
	gtk_write_entry_float(WID(entryVthresh),param->vthresh);
	gtk_write_entry_float(WID(entryBthresh),param->bthresh);
	gtk_write_entry_float(WID(entryDthresh),param->dthresh);
	gtk_write_entry_float(WID(entryNT),param->nt);
	
	MENU_SET(optionmenuField,order);
	MENU_SET(optionmenuBack,back);
	MENU_SET(optionmenuGuide,guide);
	MENU_SET(optionmenuPost,post);
	MENU_SET(optionmenuBlend,blend);
	MENU_SET(optionmenuChroma,chroma);
        CHECK_SET(checkbuttonShow,show);
	gtk_register_dialog(dialog);
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		MENU_GET(optionmenuField,order);
		MENU_GET(optionmenuBack,back);
		MENU_GET(optionmenuGuide,guide);
		MENU_GET(optionmenuPost,post);
		MENU_GET(optionmenuBlend,blend);
		MENU_GET(optionmenuChroma,chroma);
                CHECK_GET(checkbuttonShow,show);
		#define RD_ENTRY(x,y) {param->y=gtk_read_entry_float(WID(x));}
		RD_ENTRY(entryVthresh,vthresh);
		RD_ENTRY(entryBthresh,bthresh);
		RD_ENTRY(entryDthresh,dthresh);
		RD_ENTRY(entryNT,nt);
		ret=1;
	}
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	return ret;
}

//________________________________________

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *optionmenuField;
  GtkWidget *menu1;
  GtkWidget *bottom_field_first1;
  GtkWidget *top_field_first1;
  GtkWidget *label2;
  GtkWidget *optionmenuBack;
  GtkWidget *menu2;
  GtkWidget *never1;
  GtkWidget *when_forward_is_still_combed1;
  GtkWidget *always_try1;
  GtkWidget *optionmenuGuide;
  GtkWidget *menu3;
  GtkWidget *no_strategy1;
  GtkWidget *telecine_3_1;
  GtkWidget *pal_secam1;
  GtkWidget *ntsc_converted_from_pal1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *optionmenuPost;
  GtkWidget *menu4;
  GtkWidget *no_postprocessing1;
  GtkWidget *item1;
  GtkWidget *postproc_on_best_match1;
  GtkWidget *label5;
  GtkWidget *optionmenuChroma;
  GtkWidget *menu5;
  GtkWidget *ignore_chroma_to_decide1;
  GtkWidget *use_chroma_to_decide1;
  GtkWidget *label6;
  GtkWidget *optionmenuBlend;
  GtkWidget *menu6;
  GtkWidget *interpolate1;
  GtkWidget *blend1;
  GtkWidget *label7;
  GtkWidget *entryBthresh;
  GtkWidget *label8;
  GtkWidget *entryDthresh;
  GtkWidget *label9;
  GtkWidget *entryVthresh;
  GtkWidget *label10;
  GtkWidget *entryNT;
  GtkWidget *label11;
  GtkWidget *checkbuttonShow;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Decomb Telecide"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (11, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Field Order"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  optionmenuField = gtk_option_menu_new ();
  gtk_widget_show (optionmenuField);
  gtk_table_attach (GTK_TABLE (table1), optionmenuField, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  bottom_field_first1 = gtk_menu_item_new_with_mnemonic (_("Bottom Field First"));
  gtk_widget_show (bottom_field_first1);
  gtk_container_add (GTK_CONTAINER (menu1), bottom_field_first1);

  top_field_first1 = gtk_menu_item_new_with_mnemonic (_("Top Field First"));
  gtk_widget_show (top_field_first1);
  gtk_container_add (GTK_CONTAINER (menu1), top_field_first1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuField), menu1);

  label2 = gtk_label_new (_("Try backward field"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  optionmenuBack = gtk_option_menu_new ();
  gtk_widget_show (optionmenuBack);
  gtk_table_attach (GTK_TABLE (table1), optionmenuBack, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu2 = gtk_menu_new ();

  never1 = gtk_menu_item_new_with_mnemonic (_("Never"));
  gtk_widget_show (never1);
  gtk_container_add (GTK_CONTAINER (menu2), never1);

  when_forward_is_still_combed1 = gtk_menu_item_new_with_mnemonic (_("When forward is still combed"));
  gtk_widget_show (when_forward_is_still_combed1);
  gtk_container_add (GTK_CONTAINER (menu2), when_forward_is_still_combed1);

  always_try1 = gtk_menu_item_new_with_mnemonic (_("Always try"));
  gtk_widget_show (always_try1);
  gtk_container_add (GTK_CONTAINER (menu2), always_try1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuBack), menu2);

  optionmenuGuide = gtk_option_menu_new ();
  gtk_widget_show (optionmenuGuide);
  gtk_table_attach (GTK_TABLE (table1), optionmenuGuide, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu3 = gtk_menu_new ();

  no_strategy1 = gtk_menu_item_new_with_mnemonic (_("No strategy"));
  gtk_widget_show (no_strategy1);
  gtk_container_add (GTK_CONTAINER (menu3), no_strategy1);

  telecine_3_1 = gtk_menu_item_new_with_mnemonic (_("Telecine 3:2"));
  gtk_widget_show (telecine_3_1);
  gtk_container_add (GTK_CONTAINER (menu3), telecine_3_1);

  pal_secam1 = gtk_menu_item_new_with_mnemonic (_("Pal/Secam"));
  gtk_widget_show (pal_secam1);
  gtk_container_add (GTK_CONTAINER (menu3), pal_secam1);

  ntsc_converted_from_pal1 = gtk_menu_item_new_with_mnemonic (_("NTSC converted from Pal"));
  gtk_widget_show (ntsc_converted_from_pal1);
  gtk_container_add (GTK_CONTAINER (menu3), ntsc_converted_from_pal1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuGuide), menu3);

  label3 = gtk_label_new (_("Decomb strategy"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Postprocessing"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  optionmenuPost = gtk_option_menu_new ();
  gtk_widget_show (optionmenuPost);
  gtk_table_attach (GTK_TABLE (table1), optionmenuPost, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu4 = gtk_menu_new ();

  no_postprocessing1 = gtk_menu_item_new_with_mnemonic (_("No postprocessing"));
  gtk_widget_show (no_postprocessing1);
  gtk_container_add (GTK_CONTAINER (menu4), no_postprocessing1);

  item1 = gtk_menu_item_new_with_mnemonic (_("No postproc, but compute"));
  gtk_widget_show (item1);
  gtk_container_add (GTK_CONTAINER (menu4), item1);

  postproc_on_best_match1 = gtk_menu_item_new_with_mnemonic (_("Postproc on best match"));
  gtk_widget_show (postproc_on_best_match1);
  gtk_container_add (GTK_CONTAINER (menu4), postproc_on_best_match1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuPost), menu4);

  label5 = gtk_label_new (_("Chroma"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  optionmenuChroma = gtk_option_menu_new ();
  gtk_widget_show (optionmenuChroma);
  gtk_table_attach (GTK_TABLE (table1), optionmenuChroma, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu5 = gtk_menu_new ();

  ignore_chroma_to_decide1 = gtk_menu_item_new_with_mnemonic (_("Ignore chroma to decide"));
  gtk_widget_show (ignore_chroma_to_decide1);
  gtk_container_add (GTK_CONTAINER (menu5), ignore_chroma_to_decide1);

  use_chroma_to_decide1 = gtk_menu_item_new_with_mnemonic (_("Use chroma to decide"));
  gtk_widget_show (use_chroma_to_decide1);
  gtk_container_add (GTK_CONTAINER (menu5), use_chroma_to_decide1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuChroma), menu5);

  label6 = gtk_label_new (_("Blend or interpolate"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  optionmenuBlend = gtk_option_menu_new ();
  gtk_widget_show (optionmenuBlend);
  gtk_table_attach (GTK_TABLE (table1), optionmenuBlend, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu6 = gtk_menu_new ();

  interpolate1 = gtk_menu_item_new_with_mnemonic (_("Interpolate"));
  gtk_widget_show (interpolate1);
  gtk_container_add (GTK_CONTAINER (menu6), interpolate1);

  blend1 = gtk_menu_item_new_with_mnemonic (_("Blend"));
  gtk_widget_show (blend1);
  gtk_container_add (GTK_CONTAINER (menu6), blend1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuBlend), menu6);

  label7 = gtk_label_new (_("Backward thresh"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  entryBthresh = gtk_entry_new ();
  gtk_widget_show (entryBthresh);
  gtk_table_attach (GTK_TABLE (table1), entryBthresh, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label8 = gtk_label_new (_("Direct thresh"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table1), label8, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  entryDthresh = gtk_entry_new ();
  gtk_widget_show (entryDthresh);
  gtk_table_attach (GTK_TABLE (table1), entryDthresh, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label9 = gtk_label_new (_("Postproc thresh"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table1), label9, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  entryVthresh = gtk_entry_new ();
  gtk_widget_show (entryVthresh);
  gtk_table_attach (GTK_TABLE (table1), entryVthresh, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label10 = gtk_label_new (_("Noise Threshold"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table1), label10, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  entryNT = gtk_entry_new ();
  gtk_widget_show (entryNT);
  gtk_table_attach (GTK_TABLE (table1), entryNT, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label11 = gtk_label_new (_("Show"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table1), label11, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  checkbuttonShow = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonShow);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonShow, 1, 2, 10, 11,
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
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuField, "optionmenuField");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, bottom_field_first1, "bottom_field_first1");
  GLADE_HOOKUP_OBJECT (dialog1, top_field_first1, "top_field_first1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuBack, "optionmenuBack");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, never1, "never1");
  GLADE_HOOKUP_OBJECT (dialog1, when_forward_is_still_combed1, "when_forward_is_still_combed1");
  GLADE_HOOKUP_OBJECT (dialog1, always_try1, "always_try1");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuGuide, "optionmenuGuide");
  GLADE_HOOKUP_OBJECT (dialog1, menu3, "menu3");
  GLADE_HOOKUP_OBJECT (dialog1, no_strategy1, "no_strategy1");
  GLADE_HOOKUP_OBJECT (dialog1, telecine_3_1, "telecine_3_1");
  GLADE_HOOKUP_OBJECT (dialog1, pal_secam1, "pal_secam1");
  GLADE_HOOKUP_OBJECT (dialog1, ntsc_converted_from_pal1, "ntsc_converted_from_pal1");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuPost, "optionmenuPost");
  GLADE_HOOKUP_OBJECT (dialog1, menu4, "menu4");
  GLADE_HOOKUP_OBJECT (dialog1, no_postprocessing1, "no_postprocessing1");
  GLADE_HOOKUP_OBJECT (dialog1, item1, "item1");
  GLADE_HOOKUP_OBJECT (dialog1, postproc_on_best_match1, "postproc_on_best_match1");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuChroma, "optionmenuChroma");
  GLADE_HOOKUP_OBJECT (dialog1, menu5, "menu5");
  GLADE_HOOKUP_OBJECT (dialog1, ignore_chroma_to_decide1, "ignore_chroma_to_decide1");
  GLADE_HOOKUP_OBJECT (dialog1, use_chroma_to_decide1, "use_chroma_to_decide1");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuBlend, "optionmenuBlend");
  GLADE_HOOKUP_OBJECT (dialog1, menu6, "menu6");
  GLADE_HOOKUP_OBJECT (dialog1, interpolate1, "interpolate1");
  GLADE_HOOKUP_OBJECT (dialog1, blend1, "blend1");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, entryBthresh, "entryBthresh");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, entryDthresh, "entryDthresh");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, entryVthresh, "entryVthresh");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, entryNT, "entryNT");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonShow, "checkbuttonShow");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

//EOF
