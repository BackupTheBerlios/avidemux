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
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_video/ADM_vidBlendRemoval_param.h"

static GtkWidget       *create_dialog1 (void);

#define SPIN_GET(x,y) {mosaic->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)mosaic->y) ;}



uint8_t  DIA_blendRemoval(BLEND_REMOVER_PARAM *mosaic)
{
  GtkWidget *dialog;
  uint8_t ret=0;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        
        SPIN_SET(spinbuttonTreshold,threshold);
        SPIN_SET(spinbuttonNoise,noise);
        SPIN_SET(spinbuttonIdentical,identical);
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonShow)),mosaic->show);

        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
                {
                        ret=1;
                        SPIN_GET(spinbuttonTreshold,threshold);
                        SPIN_GET(spinbuttonNoise,noise);
                        SPIN_GET(spinbuttonIdentical,identical);
                        mosaic->show=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonShow)));
                }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;

}


GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkObject *spinbuttonTreshold_adj;
  GtkWidget *spinbuttonTreshold;
  GtkObject *spinbuttonNoise_adj;
  GtkWidget *spinbuttonNoise;
  GtkObject *spinbuttonIdentical_adj;
  GtkWidget *spinbuttonIdentical;
  GtkWidget *checkbuttonShow;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Blend removal / Hard pulldown removal"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (4, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("%"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("%"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("%"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_(" "));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 2, 3, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Threshold :"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new (_("Noise threshold :"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Identical threshold :"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label8 = gtk_label_new (_("Show metrics"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table1), label8, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  spinbuttonTreshold_adj = gtk_adjustment_new (1, 1, 99, 1, 10, 10);
  spinbuttonTreshold = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTreshold_adj), 1, 0);
  gtk_widget_show (spinbuttonTreshold);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonTreshold, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTreshold, _("If the image is closer than treshold, is it considered valid, smaller means more false detection"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTreshold), TRUE);

  spinbuttonNoise_adj = gtk_adjustment_new (1, 1, 99, 1, 10, 10);
  spinbuttonNoise = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonNoise_adj), 1, 0);
  gtk_widget_show (spinbuttonNoise);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonNoise, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonNoise, _("If pixels are closer than noise threshold, they are considered identical"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonNoise), TRUE);

  spinbuttonIdentical_adj = gtk_adjustment_new (1, 1, 99, 1, 10, 10);
  spinbuttonIdentical = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonIdentical_adj), 1, 0);
  gtk_widget_show (spinbuttonIdentical);
  gtk_table_attach (GTK_TABLE (table1), spinbuttonIdentical, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonIdentical, _("If the picture are less than identical threshold, they are considered identical"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonIdentical), TRUE);

  checkbuttonShow = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonShow);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonShow, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonShow, _("Show metrics in picture (debug only)"), NULL);

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
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTreshold, "spinbuttonTreshold");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonNoise, "spinbuttonNoise");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonIdentical, "spinbuttonIdentical");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonShow, "checkbuttonShow");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

  return dialog1;
}


