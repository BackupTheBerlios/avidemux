#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>
# include "config.h"
#include "default.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_video/ADM_vidTDeint_param.h"

static GtkWidget       *create_dialog1 (void);
uint8_t  DIA_tdeint(TDEINT_PARAM *param);

static void upload(GtkWidget *dialog,TDEINT_PARAM *param);
static void download(GtkWidget *dialog,TDEINT_PARAM *param);

uint8_t  DIA_tdeint(TDEINT_PARAM *param)
{
        uint8_t ret=0;


        GtkWidget *dialog;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);

        upload(dialog,param);

        switch(gtk_dialog_run(GTK_DIALOG(dialog)))
        {
                case GTK_RESPONSE_OK: 
                {
                        download(dialog,param);
                        ret=1;
                }
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;
}

#define SET_SPIN(x,y) gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(dialog,#x)),(gfloat)param->y) 
#define SET_MENU(x,y) gtk_combo_box_set_active(GTK_COMBO_BOX(WID(x)),param->y)
#define SET_CHECK(x,y) gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),param->y)

void upload(GtkWidget *dialog,TDEINT_PARAM *param)
{
        SET_SPIN(spinbuttonmthreshL,mthreshL);
        SET_SPIN(spinbuttonmthreshC,mthreshC);
        SET_SPIN(spinbuttoncthresh,cthresh);
        SET_SPIN(spinbuttonMI,MI);
        SET_SPIN(spinbuttonAP,AP);
        
        SET_MENU(comboboxOrder,order);
        SET_MENU(comboboxInterpolate,field);
        SET_MENU(comboboxType,type);
        SET_MENU(comboboxMnt,mtnmode);
        SET_MENU(combobox5,link);
        SET_MENU(combobox6,APType);

        SET_CHECK(checkbuttonChroma,chroma);
        SET_CHECK(checkbuttonTryweave,tryWeave);
        SET_CHECK(checkbuttonDenoise,denoise);
        SET_CHECK(checkbuttonSharp,sharp);
        SET_CHECK(checkbuttonFull,full);

}
#undef SET_SPIN
#undef SET_MENU
#undef SET_CHECK
#define SET_SPIN(x,y) param->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(dialog,#x))) 
#define SET_MENU(x,y) param->y=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(x)))
#define SET_CHECK(x,y) param->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)))

void download(GtkWidget *dialog,TDEINT_PARAM *param)
{
        SET_SPIN(spinbuttonmthreshL,mthreshL);
        SET_SPIN(spinbuttonmthreshC,mthreshC);
        SET_SPIN(spinbuttoncthresh,cthresh);
        SET_SPIN(spinbuttonMI,MI);
        SET_SPIN(spinbuttonAP,AP);

        SET_MENU(comboboxOrder,order);
        SET_MENU(comboboxInterpolate,field);
        SET_MENU(comboboxType,type);
        SET_MENU(comboboxMnt,mtnmode);
        SET_MENU(combobox5,link);
        SET_MENU(combobox6,APType);

        SET_CHECK(checkbuttonChroma,chroma);
        SET_CHECK(checkbuttonTryweave,tryWeave);
        SET_CHECK(checkbuttonDenoise,denoise);
        SET_CHECK(checkbuttonSharp,sharp);
        SET_CHECK(checkbuttonFull,full);
}

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *comboboxOrder;
  GtkWidget *label2;
  GtkWidget *comboboxInterpolate;
  GtkWidget *label3;
  GtkWidget *comboboxType;
  GtkWidget *label4;
  GtkWidget *comboboxMnt;
  GtkWidget *label7;
  GtkWidget *checkbuttonChroma;
  GtkWidget *label8;
  GtkWidget *checkbuttonTryweave;
  GtkWidget *label10;
  GtkWidget *checkbuttonDenoise;
  GtkWidget *label9;
  GtkWidget *combobox5;
  GtkWidget *label5;
  GtkWidget *checkbuttonSharp;
  GtkWidget *label16;
  GtkWidget *checkbuttonFull;
  GtkWidget *label6;
  GtkWidget *combobox6;
  GtkWidget *table2;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkObject *spinbuttonmthreshL_adj;
  GtkWidget *spinbuttonmthreshL;
  GtkObject *spinbuttonmthreshC_adj;
  GtkWidget *spinbuttonmthreshC;
  GtkWidget *label13;
  GtkObject *spinbuttoncthresh_adj;
  GtkWidget *spinbuttoncthresh;
  GtkWidget *label14;
  GtkObject *spinbuttonMI_adj;
  GtkWidget *spinbuttonMI;
  GtkWidget *label15;
  GtkObject *spinbuttonAP_adj;
  GtkWidget *spinbuttonAP;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("TDeint param"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (11, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Field Order :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  comboboxOrder = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxOrder);
  gtk_table_attach (GTK_TABLE (table1), comboboxOrder, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxOrder), _("Bottom Field First"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxOrder), _("Top Field First"));

  label2 = gtk_label_new (_("Interpolate :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  comboboxInterpolate = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxInterpolate);
  gtk_table_attach (GTK_TABLE (table1), comboboxInterpolate, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxInterpolate), _("Top field (keep bottom)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxInterpolate), _("Bottom field (keep top)"));

  label3 = gtk_label_new (_("Type"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  comboboxType = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxType);
  gtk_table_attach (GTK_TABLE (table1), comboboxType, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxType), _("Cubic Interpolation"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxType), _("Modified ELA"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxType), _("Kernerl interpolation"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxType), _("Modified ELA-2"));

  label4 = gtk_label_new (_("MntMode"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  comboboxMnt = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxMnt);
  gtk_table_attach (GTK_TABLE (table1), comboboxMnt, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMnt), _("4 field check"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMnt), _("5 field check"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMnt), _("4 field check (no average)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMnt), _("5 field check (no average)"));

  label7 = gtk_label_new (_("Use Chroma to evalute"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  checkbuttonChroma = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonChroma);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonChroma, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label8 = gtk_label_new (_("Try weave "));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table1), label8, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  checkbuttonTryweave = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonTryweave);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonTryweave, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label10 = gtk_label_new (_("Denoise"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table1), label10, 0, 1, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  checkbuttonDenoise = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonDenoise);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonDenoise, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label9 = gtk_label_new (_("Link"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table1), label9, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  combobox5 = gtk_combo_box_new_text ();
  gtk_widget_show (combobox5);
  gtk_table_attach (GTK_TABLE (table1), combobox5, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox5), _("No link"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox5), _("Full linking"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox5), _("Y to UV link"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox5), _("UV to Y link"));

  label5 = gtk_label_new (_("Sharp :"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  checkbuttonSharp = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonSharp);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonSharp, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label16 = gtk_label_new (_("AP type"));
  gtk_widget_show (label16);
  gtk_table_attach (GTK_TABLE (table1), label16, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

  checkbuttonFull = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonFull);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonFull, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label6 = gtk_label_new (_("Evaluate ALL frames"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  combobox6 = gtk_combo_box_new_text ();
  gtk_widget_show (combobox6);
  gtk_table_attach (GTK_TABLE (table1), combobox6, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox6), _("0 (read manual)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox6), _("1 (read manual)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (combobox6), _("2 (read manual)"));

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (vbox1), table2, TRUE, TRUE, 0);

  label11 = gtk_label_new (_("motion Threshold, Luma"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new (_("motion Threshold, Chroma"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  spinbuttonmthreshL_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spinbuttonmthreshL = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonmthreshL_adj), 1, 0);
  gtk_widget_show (spinbuttonmthreshL);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonmthreshL, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonmthreshL), TRUE);

  spinbuttonmthreshC_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spinbuttonmthreshC = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonmthreshC_adj), 1, 0);
  gtk_widget_show (spinbuttonmthreshC);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonmthreshC, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonmthreshC), TRUE);

  label13 = gtk_label_new (_("Area Combing Threshold"));
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table2), label13, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  spinbuttoncthresh_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spinbuttoncthresh = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttoncthresh_adj), 1, 0);
  gtk_widget_show (spinbuttoncthresh);
  gtk_table_attach (GTK_TABLE (table2), spinbuttoncthresh, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label14 = gtk_label_new (_("Combed Treshold"));
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table2), label14, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  spinbuttonMI_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spinbuttonMI = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMI_adj), 1, 0);
  gtk_widget_show (spinbuttonMI);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonMI, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMI), TRUE);

  label15 = gtk_label_new (_("Artefact Protection threshold "));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table2), label15, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  spinbuttonAP_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spinbuttonAP = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonAP_adj), 1, 0);
  gtk_widget_show (spinbuttonAP);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonAP, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonAP), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxOrder, "comboboxOrder");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxInterpolate, "comboboxInterpolate");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxType, "comboboxType");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxMnt, "comboboxMnt");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChroma, "checkbuttonChroma");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonTryweave, "checkbuttonTryweave");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDenoise, "checkbuttonDenoise");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, combobox5, "combobox5");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonSharp, "checkbuttonSharp");
  GLADE_HOOKUP_OBJECT (dialog1, label16, "label16");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonFull, "checkbuttonFull");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, combobox6, "combobox6");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonmthreshL, "spinbuttonmthreshL");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonmthreshC, "spinbuttonmthreshC");
  GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttoncthresh, "spinbuttoncthresh");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMI, "spinbuttonMI");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonAP, "spinbuttonAP");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

