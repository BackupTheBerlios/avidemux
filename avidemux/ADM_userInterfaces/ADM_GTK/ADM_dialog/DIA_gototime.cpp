//

# include <config.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "config.h"

#include "default.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"

static GtkWidget        *create_dialog1 (void);


#define SPIN_GET(x,y) {*y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;\
                                printf(#x":%d\n",*y);}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)*y) ; \
                                printf(#x":%d\n",*y);}
static GtkWidget *create_dialog1 (void);
uint8_t DIA_gotoTime(uint16_t *hh, uint16_t *mm, uint16_t *ss);
uint8_t DIA_gotoTime(uint16_t *hh, uint16_t *mm, uint16_t *ss)
{
GtkWidget *dialog;
uint8_t ret;
        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        ret=0;

        SPIN_SET(spinbuttonH,hh);
        SPIN_SET(spinbuttonM,mm);
        SPIN_SET(spinbuttonS,ss);
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                SPIN_GET(spinbuttonH,hh);
                SPIN_GET(spinbuttonM,mm);
                SPIN_GET(spinbuttonS,ss);
                ret=1;
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;

}

GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkObject *spinbuttonH_adj;
  GtkWidget *spinbuttonH;
  GtkWidget *label1;
  GtkObject *spinbuttonM_adj;
  GtkWidget *spinbuttonM;
  GtkWidget *label2;
  GtkObject *spinbuttonS_adj;
  GtkWidget *spinbuttonS;
  GtkWidget *label3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Go to time"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  spinbuttonH_adj = gtk_adjustment_new (0, 0, 10, 1, 10, 10);
  spinbuttonH = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonH_adj), 1, 0);
  gtk_widget_show (spinbuttonH);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonH, TRUE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonH), TRUE);

  label1 = gtk_label_new (_("h"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  spinbuttonM_adj = gtk_adjustment_new (0, 0, 59, 1, 10, 10);
  spinbuttonM = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonM_adj), 1, 0);
  gtk_widget_show (spinbuttonM);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonM, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("mn"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  spinbuttonS_adj = gtk_adjustment_new (0, 0, 59, 1, 10, 10);
  spinbuttonS = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonS_adj), 1, 0);
  gtk_widget_show (spinbuttonS);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonS, TRUE, TRUE, 0);

  label3 = gtk_label_new (_("s"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox1), label3, FALSE, FALSE, 0);
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
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonH, "spinbuttonH");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonM, "spinbuttonM");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonS, "spinbuttonS");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  gtk_widget_grab_default (okbutton1);
  return dialog1;
}

