
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <config.h>

#include "ADM_library/default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


static GtkWidget	*create_dialog1 (void);

#define GETSPIN(x,y)  y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(dialog,#x)))
#define SETSPIN(x,y)  gtk_spin_button_set_value(GTK_SPIN_BUTTON((lookup_widget(dialog,#x))),(gfloat)y)

uint8_t DIA_dnr(uint32_t *llock,uint32_t *lthresh, uint32_t *clock,
			uint32_t *cthresh, uint32_t *scene)
{
GtkWidget *dialog;

	uint8_t ret=0;

	dialog=create_dialog1();
	gtk_register_dialog(dialog);
	//gtk_transient(dialog);
	SETSPIN(spin_lumalock,*llock);
	SETSPIN(spin_lumathresh,*lthresh);
	SETSPIN(spin_chromalock,*clock);
	SETSPIN(spin_chromathresh,*cthresh);
	SETSPIN(spin_scene,*scene);

	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		GETSPIN(spin_lumalock,*llock);
		GETSPIN(spin_lumathresh,*lthresh);
		GETSPIN(spin_chromalock,*clock);
		GETSPIN(spin_chromathresh,*cthresh);
		GETSPIN(spin_scene,*scene);
			ret=1;
	}
	gtk_unregister_dialog(dialog);
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
  GtkWidget *label5;
  GtkObject *spin_lumalock_adj;
  GtkWidget *spin_lumalock;
  GtkObject *spin_lumathresh_adj;
  GtkWidget *spin_lumathresh;
  GtkObject *spin_chromalock_adj;
  GtkWidget *spin_chromalock;
  GtkObject *spin_chromathresh_adj;
  GtkWidget *spin_chromathresh;
  GtkObject *spin_scene_adj;
  GtkWidget *spin_scene;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Dynamic Noise Reduction"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Luma Lock"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Luma Threshold"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Chroma Lock"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Chroma Threshold"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  label5 = gtk_label_new (_("Scene change detect"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  spin_lumalock_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spin_lumalock = gtk_spin_button_new (GTK_ADJUSTMENT (spin_lumalock_adj), 1, 0);
  gtk_widget_show (spin_lumalock);
  gtk_table_attach (GTK_TABLE (table1), spin_lumalock, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_lumalock), TRUE);

  spin_lumathresh_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spin_lumathresh = gtk_spin_button_new (GTK_ADJUSTMENT (spin_lumathresh_adj), 1, 0);
  gtk_widget_show (spin_lumathresh);
  gtk_table_attach (GTK_TABLE (table1), spin_lumathresh, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_lumathresh), TRUE);

  spin_chromalock_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spin_chromalock = gtk_spin_button_new (GTK_ADJUSTMENT (spin_chromalock_adj), 1, 0);
  gtk_widget_show (spin_chromalock);
  gtk_table_attach (GTK_TABLE (table1), spin_chromalock, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_chromalock), TRUE);

  spin_chromathresh_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  spin_chromathresh = gtk_spin_button_new (GTK_ADJUSTMENT (spin_chromathresh_adj), 1, 0);
  gtk_widget_show (spin_chromathresh);
  gtk_table_attach (GTK_TABLE (table1), spin_chromathresh, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_chromathresh), TRUE);

  spin_scene_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spin_scene = gtk_spin_button_new (GTK_ADJUSTMENT (spin_scene_adj), 1, 0);
  gtk_widget_show (spin_scene);
  gtk_table_attach (GTK_TABLE (table1), spin_scene, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin_scene), TRUE);

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
  GLADE_HOOKUP_OBJECT (dialog1, spin_lumalock, "spin_lumalock");
  GLADE_HOOKUP_OBJECT (dialog1, spin_lumathresh, "spin_lumathresh");
  GLADE_HOOKUP_OBJECT (dialog1, spin_chromalock, "spin_chromalock");
  GLADE_HOOKUP_OBJECT (dialog1, spin_chromathresh, "spin_chromathresh");
  GLADE_HOOKUP_OBJECT (dialog1, spin_scene, "spin_scene");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


