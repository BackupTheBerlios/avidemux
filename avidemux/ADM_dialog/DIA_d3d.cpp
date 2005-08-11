

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "config.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

# include <config.h>
static GtkWidget	*create_d3doption (void);
uint8_t DIA_d3d(double *luma,double *chroma,double *temporal);
uint8_t DIA_d3d(double *luma,double *chroma,double *temporal)
{
	GtkWidget *dialog;

	char str[100];
	uint8_t ret=0;

	gint r;

#define FILL_ENTRY(widget_name,value) 		{sprintf(str,"%f",*value);r=-1;   \
gtk_editable_delete_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), 0,-1);\
gtk_editable_insert_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), str, strlen(str), &r);}

	dialog=create_d3doption();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);

	FILL_ENTRY(entry1,luma);
	FILL_ENTRY(entry2,chroma);
	FILL_ENTRY(entry3,temporal);



	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

		gchar *s;
#define READ_ENTRY(widget_name)   gtk_editable_get_chars(GTK_EDITABLE (lookup_widget(dialog,#widget_name)), 0, -1);

			s=READ_ENTRY(entry1);;
			*luma=(double)atof(s);
			s=READ_ENTRY(entry2);;
			*chroma=(double)atoi(s);
			s=READ_ENTRY(entry3);;
			*temporal=(double)atoi(s);
			ret=1;
	}
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);

	return ret;

}









GtkWidget	*create_d3doption (void)
{
  GtkWidget *d3doption;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *entry1;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  d3doption = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (d3doption), _("dialog1"));

  dialog_vbox1 = GTK_DIALOG (d3doption)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Spatial Luma Thresh"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Spatial Chroma Thresh"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Temporal Thresh"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_table_attach (GTK_TABLE (table1), entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry2 = gtk_entry_new ();
  gtk_widget_show (entry2);
  gtk_table_attach (GTK_TABLE (table1), entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry3 = gtk_entry_new ();
  gtk_widget_show (entry3);
  gtk_table_attach (GTK_TABLE (table1), entry3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  dialog_action_area1 = GTK_DIALOG (d3doption)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (d3doption), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (d3doption), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (d3doption, d3doption, "d3doption");
  GLADE_HOOKUP_OBJECT_NO_REF (d3doption, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (d3doption, table1, "table1");
  GLADE_HOOKUP_OBJECT (d3doption, label1, "label1");
  GLADE_HOOKUP_OBJECT (d3doption, label2, "label2");
  GLADE_HOOKUP_OBJECT (d3doption, label3, "label3");
  GLADE_HOOKUP_OBJECT (d3doption, entry1, "entry1");
  GLADE_HOOKUP_OBJECT (d3doption, entry2, "entry2");
  GLADE_HOOKUP_OBJECT (d3doption, entry3, "entry3");
  GLADE_HOOKUP_OBJECT_NO_REF (d3doption, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (d3doption, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (d3doption, okbutton1, "okbutton1");

  return d3doption;
}

