# include <config.h>

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "callbacks.h"
//#include "avi_vars.h"
#include "default.h"


#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_assert.h" 

#ifdef USE_X264
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encx264.h"
#include "ADM_codecs/ADM_x264param.h"

static GtkWidget       *create_dialog1 (void);  
uint8_t DIA_x264(X264Config *config);
/*********************************************/
uint8_t DIA_x264(X264Config *config)
{
  GtkWidget *dialog;
  uint8_t ret=0;
  
        dialog=create_dialog1();
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
        }
        gtk_widget_destroy(dialog);
        return ret;
  
}

/*********************************************/
GtkWidget       *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *entryBitrate;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *single_pass_bitrate1;
  GtkWidget *single_pass_quantizer1;
  GtkWidget *TwoPass;
  GtkObject *spinbuttonQz_adj;
  GtkWidget *spinbuttonQz;
  GtkWidget *label4;
  GtkWidget *frame2;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *label9;
  GtkObject *spinbuttonMinkF_adj;
  GtkWidget *spinbuttonMinkF;
  GtkWidget *label10;
  GtkObject *spinbuttonMaxKf_adj;
  GtkWidget *spinbuttonMaxKf;
  GtkWidget *hbox2;
  GtkWidget *label11;
  GtkObject *spinbuttonBframes_adj;
  GtkWidget *spinbuttonBframes;
  GtkWidget *label12;
  GtkObject *spinbutton6_adj;
  GtkWidget *spinbutton6;
  GtkWidget *label8;
  GtkWidget *label1;
  GtkWidget *empty_notebook_page;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("X264 encoding options"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox1);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  label5 = gtk_label_new (_("Encoding type:"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new (_("Bitrate (kb/s):"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Quantizer:"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  entryBitrate = gtk_entry_new ();
  gtk_widget_show (entryBitrate);
  gtk_table_attach (GTK_TABLE (table1), entryBitrate, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  single_pass_bitrate1 = gtk_menu_item_new_with_mnemonic (_("Single Pass Bitrate"));
  gtk_widget_show (single_pass_bitrate1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass_bitrate1);

  single_pass_quantizer1 = gtk_menu_item_new_with_mnemonic (_("Single Pass Quantizer"));
  gtk_widget_show (single_pass_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass_quantizer1);

  TwoPass = gtk_menu_item_new_with_mnemonic (_("Two Pass"));
  gtk_widget_show (TwoPass);
  gtk_container_add (GTK_CONTAINER (menu1), TwoPass);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  spinbuttonQz_adj = gtk_adjustment_new (4, 2, 51, 1, 10, 10);
  spinbuttonQz = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQz_adj), 1, 0);
  gtk_widget_show (spinbuttonQz);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonQz, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQz), TRUE);

  label4 = gtk_label_new (_("RateControl"));
  gtk_widget_show (label4);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label4);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame2), vbox2);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  label9 = gtk_label_new (_("I frame interval - Min :"));
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox1), label9, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  spinbuttonMinkF_adj = gtk_adjustment_new (1, 1, 10, 1, 10, 10);
  spinbuttonMinkF = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinkF_adj), 1, 0);
  gtk_widget_show (spinbuttonMinkF);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonMinkF, TRUE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMinkF), TRUE);

  label10 = gtk_label_new (_("Max: "));
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox1), label10, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);

  spinbuttonMaxKf_adj = gtk_adjustment_new (250, 1, 500, 1, 10, 10);
  spinbuttonMaxKf = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxKf_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxKf);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonMaxKf, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxKf), TRUE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

  label11 = gtk_label_new (_("Nb B frames:"));
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox2), label11, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  spinbuttonBframes_adj = gtk_adjustment_new (1, 0, 5, 1, 10, 10);
  spinbuttonBframes = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBframes_adj), 1, 0);
  gtk_widget_show (spinbuttonBframes);
  gtk_box_pack_start (GTK_BOX (hbox2), spinbuttonBframes, FALSE, FALSE, 0);

  label12 = gtk_label_new (_("Nb Ref frames"));
  gtk_widget_show (label12);
  gtk_box_pack_start (GTK_BOX (hbox2), label12, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

  spinbutton6_adj = gtk_adjustment_new (1, 1, 5, 1, 10, 10);
  spinbutton6 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton6_adj), 1, 0);
  gtk_widget_show (spinbutton6);
  gtk_box_pack_start (GTK_BOX (hbox2), spinbutton6, FALSE, FALSE, 0);

  label8 = gtk_label_new (_("Frames"));
  gtk_widget_show (label8);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label8);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  label1 = gtk_label_new (_("Main"));
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  empty_notebook_page = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (empty_notebook_page);
  gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

  label2 = gtk_label_new (_("xx"));
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  empty_notebook_page = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (empty_notebook_page);
  gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);

  label3 = gtk_label_new (_("xx"));
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, entryBitrate, "entryBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass_bitrate1, "single_pass_bitrate1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass_quantizer1, "single_pass_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, TwoPass, "TwoPass");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQz, "spinbuttonQz");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinkF, "spinbuttonMinkF");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxKf, "spinbuttonMaxKf");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBframes, "spinbuttonBframes");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton6, "spinbutton6");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

#endif


