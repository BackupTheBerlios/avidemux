
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#include "default.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"

static GtkWidget	*create_dialog1 (void);

uint8_t DIA_4entries(char *title,uint32_t *left,uint32_t *right,uint32_t *top,uint32_t *bottom)
{
	GtkWidget *dialog;

	char str[100];
	uint8_t ret=0;

	gint r;

#define FILL_ENTRY(widget_name,value) 		{sprintf(str,"%ld",*value);r=-1;   \
gtk_editable_delete_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), 0,-1);\
gtk_editable_insert_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), str, strlen(str), &r);}

	dialog=create_dialog1();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);
  	gtk_window_set_title (GTK_WINDOW (dialog), title);
	FILL_ENTRY(entryLeft,left);
	FILL_ENTRY(entryRight,right);
	FILL_ENTRY(entryTop,top);
	FILL_ENTRY(entryBottom,bottom);


	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

			*left=gtk_read_entry(WID(entryLeft));
			*right=gtk_read_entry(WID(entryRight));
			*top=gtk_read_entry(WID(entryTop));
			*bottom=gtk_read_entry(WID(entryBottom));
			ret=1;
	}
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);

	return ret;

}


GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *entryLeft;
  GtkWidget *hbox2;
  GtkWidget *label2;
  GtkWidget *entryRight;
  GtkWidget *hbox3;
  GtkWidget *label3;
  GtkWidget *entryTop;
  GtkWidget *hbox4;
  GtkWidget *label4;
  GtkWidget *entryBottom;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("dialog1"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_table_attach (GTK_TABLE (table1), hbox1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label1 = gtk_label_new (_("Left:"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  entryLeft = gtk_entry_new ();
  gtk_widget_show (entryLeft);
  gtk_box_pack_start (GTK_BOX (hbox1), entryLeft, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entryLeft, 60, -1);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_table_attach (GTK_TABLE (table1), hbox2, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label2 = gtk_label_new (_("Right:"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox2), label2, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  entryRight = gtk_entry_new ();
  gtk_widget_show (entryRight);
  gtk_box_pack_start (GTK_BOX (hbox2), entryRight, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entryRight, 60, -1);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_table_attach (GTK_TABLE (table1), hbox3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label3 = gtk_label_new (_("Top:"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox3), label3, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  entryTop = gtk_entry_new ();
  gtk_widget_show (entryTop);
  gtk_box_pack_start (GTK_BOX (hbox3), entryTop, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entryTop, 60, -1);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_table_attach (GTK_TABLE (table1), hbox4, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label4 = gtk_label_new (_("Bottom:"));
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox4), label4, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  entryBottom = gtk_entry_new ();
  gtk_widget_show (entryBottom);
  gtk_box_pack_start (GTK_BOX (hbox4), entryBottom, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entryBottom, 60, -1);

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
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, entryLeft, "entryLeft");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, entryRight, "entryRight");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, entryTop, "entryTop");
  GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, entryBottom, "entryBottom");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

