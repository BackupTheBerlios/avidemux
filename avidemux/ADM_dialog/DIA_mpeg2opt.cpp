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
#include "ADM_assert.h"

static GtkWidget	*create_dialog1 (void);

/**
		*maxbitrate : bitrate in/out
		qz			: quantisation in/out
		opt1			: initial options in
		opts			: options : out


*/

uint8_t  DIA_mpeg2opt(uint32_t *maxbitrate, uint32_t *qz, char *opt1,char **opts)
{

GtkWidget *dialog;
char str[500];
uint8_t  ret=0;
gint r;

#define FILL_ENTRY(widget_name) {		 \
gtk_editable_delete_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), 0,-1);\
gtk_editable_insert_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), str, strlen(str), &r);}

			dialog=create_dialog1();
			gtk_transient(dialog);
			sprintf(str,"%ld",*maxbitrate);
			FILL_ENTRY(entryBitrate);
			sprintf(str,"%ld",*qz);
			FILL_ENTRY(entryQuant);
			strcpy(str,opt1);
			FILL_ENTRY(entryArgs);


			ret=0;
			if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
			{

				gchar *s;
				uint32_t val1;
				ret=1;
#define READ_ENTRY(widget_name)   gtk_editable_get_chars(GTK_EDITABLE (lookup_widget(dialog,#widget_name)), 0, -1);

			s=READ_ENTRY(entryBitrate);
			val1=atoi(s);
			if(!val1) ret=0;
			else
					*maxbitrate=val1;

			s=READ_ENTRY(entryQuant);
			val1=atoi(s);
			if(!val1||val1>31) ret=0;
			else
					*qz=val1;
			s=READ_ENTRY(entryArgs);
			if(ret)
					*opts=ADM_strdup(s);
				else
					*opts=NULL;

			}
		gtk_widget_destroy(dialog);		
		return ret;
}



GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *entryBitrate;
  GtkWidget *entryQuant;
  GtkWidget *entryArgs;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mpeg2 options"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Other arg"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Quantisation (2-31)"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Maximum bitrate (kbps)"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  entryBitrate = gtk_entry_new ();
  gtk_widget_show (entryBitrate);
  gtk_table_attach (GTK_TABLE (table1), entryBitrate, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryQuant = gtk_entry_new ();
  gtk_widget_show (entryQuant);
  gtk_table_attach (GTK_TABLE (table1), entryQuant, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryArgs = gtk_entry_new ();
  gtk_widget_show (entryArgs);
  gtk_table_attach (GTK_TABLE (table1), entryArgs, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
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
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, entryBitrate, "entryBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, entryQuant, "entryQuant");
  GLADE_HOOKUP_OBJECT (dialog1, entryArgs, "entryArgs");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

