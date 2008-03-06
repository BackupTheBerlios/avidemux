#include "../ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "avidemutils.h"

static GtkWidget       *create_dialog1 (void);

#define actionBrowse 31


uint8_t  DIA_job_select(char **jobname, char **filename)
{
uint8_t ret=0;
char *tmp=NULL,*tmp2=NULL;
        *jobname=NULL;
        *filename=NULL;

        GtkWidget *dialog;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonBrowse),actionBrowse);
        while(1)
        {
                switch(gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                        case actionBrowse:
                                        {
                                        
                                        int r;
                                        char *escape=NULL;
                                                GUI_FileSelWrite(QT_TR_NOOP("Select Video to Write"),&tmp);
                                                if(!tmp) continue;
                                                escape=ADM_escape((ADM_filename *)tmp);
                                                gtk_editable_delete_text(GTK_EDITABLE(WID(entryOutputFileName)), 0,-1);
                                                gtk_editable_insert_text(GTK_EDITABLE(WID(entryOutputFileName)), escape, strlen(escape), &r);
                                                ADM_dealloc(tmp);
                                                delete [] escape;
                                        }
                                        break;
                        case GTK_RESPONSE_OK: 
                                        {
                                           tmp=gtk_editable_get_chars(GTK_EDITABLE (WID(entryOutputFileName)), 0, -1);
                                           if(!tmp || !*tmp)
                                           {
                                             GUI_Error_HIG(QT_TR_NOOP("Invalid filename"),QT_TR_NOOP("Please select or enter a valid filename."));
                                                        continue;
                                            }
                                           tmp2=gtk_editable_get_chars(GTK_EDITABLE (WID(entryJobName)), 0, -1);
                                           if(!tmp2 || !*tmp2)
                                           {
                                             GUI_Error_HIG(QT_TR_NOOP("Invalid jobname"),QT_TR_NOOP("Please select or enter a valid jobname."));
                                                        continue;
                                            }
                                            *jobname=ADM_strdup(tmp2);
                                            *filename=ADM_strdup(tmp);
                                            ret=1;
                                        }
                        default: goto _nxt;
                }
        }
_nxt:
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;
}
//**************************************************
//*************************************

GtkWidget       *create_dialog1 (void)
{
  GtkWidget *SaveJob;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *buttonBrowse;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label3;
  GtkWidget *entryJobName;
  GtkWidget *entryOutputFileName;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  SaveJob = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (SaveJob), 6);
  gtk_window_set_title (GTK_WINDOW (SaveJob), QT_TR_NOOP("Save Job"));
  gtk_window_set_type_hint (GTK_WINDOW (SaveJob), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_window_set_resizable (GTK_WINDOW (SaveJob), FALSE);
  gtk_dialog_set_has_separator (GTK_DIALOG (SaveJob), FALSE);

  dialog_vbox1 = GTK_DIALOG (SaveJob)->vbox;
  gtk_box_set_spacing (GTK_BOX(dialog_vbox1), 12);
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 6);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 6);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 12);

  buttonBrowse = gtk_button_new ();
  gtk_widget_show (buttonBrowse);
  gtk_table_attach (GTK_TABLE (table1), buttonBrowse, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, buttonBrowse, QT_TR_NOOP("Select the output file"), NULL);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (buttonBrowse), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic (QT_TR_NOOP("_Browse..."));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox1), label3, FALSE, FALSE, 0);

  entryJobName = gtk_entry_new ();
  gtk_widget_show (entryJobName);
  gtk_table_attach (GTK_TABLE (table1), entryJobName, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, entryJobName, QT_TR_NOOP("Job name displayed in the job list and used as the script filename"), NULL);
  gtk_entry_set_max_length (GTK_ENTRY (entryJobName), 40);
  gtk_entry_set_width_chars (GTK_ENTRY (entryJobName), 40);

  entryOutputFileName = gtk_entry_new ();
  gtk_widget_show (entryOutputFileName);
  gtk_table_attach (GTK_TABLE (table1), entryOutputFileName, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_tooltips_set_tip (tooltips, entryOutputFileName, QT_TR_NOOP("Filename of the audio/video output"), NULL);
  gtk_entry_set_width_chars (GTK_ENTRY (entryOutputFileName), 40);

  label1 = gtk_label_new_with_mnemonic (QT_TR_NOOP("_Job name:"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new_with_mnemonic (QT_TR_NOOP("Output _file:"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  dialog_action_area1 = GTK_DIALOG (SaveJob)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (SaveJob), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (SaveJob), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label1), entryJobName);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label2), entryOutputFileName);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (SaveJob, SaveJob, "SaveJob");
  GLADE_HOOKUP_OBJECT_NO_REF (SaveJob, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (SaveJob, table1, "table1");
  GLADE_HOOKUP_OBJECT (SaveJob, buttonBrowse, "buttonBrowse");
  GLADE_HOOKUP_OBJECT (SaveJob, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (SaveJob, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (SaveJob, image1, "image1");
  GLADE_HOOKUP_OBJECT (SaveJob, label3, "label3");
  GLADE_HOOKUP_OBJECT (SaveJob, entryJobName, "entryJobName");
  GLADE_HOOKUP_OBJECT (SaveJob, entryOutputFileName, "entryOutputFileName");
  GLADE_HOOKUP_OBJECT (SaveJob, label1, "label1");
  GLADE_HOOKUP_OBJECT (SaveJob, label2, "label2");
  GLADE_HOOKUP_OBJECT_NO_REF (SaveJob, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (SaveJob, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (SaveJob, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (SaveJob, tooltips, "tooltips");

  gtk_widget_grab_focus (entryJobName);
  gtk_widget_grab_default (okbutton1);
  return SaveJob;
}

