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
#include "ADM_toolkit/filesel.h"

static GtkWidget       *create_dialog1 (void);
uint8_t  DIA_v2v(char **vobname, char **ifoname,char **vobsubname);
#define actionIFO       31
#define actionVOB       32
#define actionVOBSUB    33


uint8_t  DIA_v2v(char **vobname, char **ifoname,char **vobsubname)
{
uint8_t ret=0;
char *tmp=NULL,*tmp2=NULL,*tmp3=NULL;

        GtkWidget *dialog;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonVob),actionVOB);
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonIfo),actionIFO);
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonVobSub),actionVOBSUB);

#define ENTRY_SET(x,y) {gtk_write_entry_string(WID(x),*y);}

        ENTRY_SET(entryIfo,ifoname);
        ENTRY_SET(entryVob,vobname);
        ENTRY_SET(entryVobSub,vobsubname);

        while(1)
        {
                switch(gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                        case actionVOB:
                                        {
                                        
                                        int r;
                                                GUI_FileSelRead(_("Select Vob file to scan"),&tmp);
                                                if(!tmp) continue;
                                                gtk_editable_delete_text(GTK_EDITABLE(WID(entryVob)), 0,-1);
                                                gtk_editable_insert_text(GTK_EDITABLE(WID(entryVob)), tmp, strlen(tmp), &r);
                                                ADM_dealloc(tmp);
                                        }
                                        break;
                        case actionIFO:
                                        {
                                        
                                        int r;
                                                GUI_FileSelRead(_("Select ifo file to use"),&tmp);
                                                if(!tmp) continue;
                                                gtk_editable_delete_text(GTK_EDITABLE(WID(entryIfo)), 0,-1);
                                                gtk_editable_insert_text(GTK_EDITABLE(WID(entryIfo)), tmp, strlen(tmp), &r);
                                                ADM_dealloc(tmp);
                                        }
                                        break;
                        case actionVOBSUB:
                                        {
                                        
                                        int r;
                                                GUI_FileSelWrite(_("Select vobsub to write"),&tmp);
                                                if(!tmp) continue;
                                                gtk_editable_delete_text(GTK_EDITABLE(WID(entryVobSub)), 0,-1);
                                                gtk_editable_insert_text(GTK_EDITABLE(WID(entryVobSub)), tmp, strlen(tmp), &r);
                                                ADM_dealloc(tmp);
                                        }
                                        break;

                                        break;
                        case GTK_RESPONSE_OK: 
                                        {
                                           tmp=gtk_editable_get_chars(GTK_EDITABLE (WID(entryVob)), 0, -1);
                                           if(!tmp || !*tmp)
                                           {
                                             GUI_Error_HIG(_("Invalid vobname"),_("Please select or enter a valid vob name"));
                                                        continue;
                                            }
                                           tmp2=gtk_editable_get_chars(GTK_EDITABLE (WID(entryIfo)), 0, -1);
                                           if(!tmp2 || !*tmp2)
                                           {
                                             GUI_Error_HIG(_("Invalid ifo"),_("Please select or enter a valid ifo file"));
                                                        continue;
                                            }
                                           tmp3=gtk_editable_get_chars(GTK_EDITABLE (WID(entryVobSub)), 0, -1);
                                           if(!tmp3 || !*tmp3 )
                                           {
                                             GUI_Error_HIG(_("Invalid vobsubname"),_("Please select or enter a valid vobsub file"));
                                                        continue;
                                            }
                                            if(*vobname) ADM_dealloc(*vobname);
                                            if(*ifoname) ADM_dealloc(*ifoname);
                                            if(*vobsubname) ADM_dealloc(*vobsubname);

                                             *vobname=*ifoname=*vobsubname=NULL;

                                            *vobname=ADM_strdup(tmp);
                                            *ifoname=ADM_strdup(tmp2);
                                            *vobsubname=(char *)ADM_alloc(strlen(tmp3)+5); //ADM_strdup(tmp3);
                                            strcpy(*vobsubname,tmp3);
                                            if(tmp3[strlen(tmp3)-1]!='x') strcat(*vobsubname,".idx");
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


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *entryVob;
  GtkWidget *entryIfo;
  GtkWidget *entryVobSub;
  GtkWidget *buttonVob;
  GtkWidget *buttonIfo;
  GtkWidget *buttonVobSub;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Vob to Vobsub"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Vob :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Ifo :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Output vobsub (.idx) :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  entryVob = gtk_entry_new ();
  gtk_widget_show (entryVob);
  gtk_table_attach (GTK_TABLE (table1), entryVob, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryIfo = gtk_entry_new ();
  gtk_widget_show (entryIfo);
  gtk_table_attach (GTK_TABLE (table1), entryIfo, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryVobSub = gtk_entry_new ();
  gtk_widget_show (entryVobSub);
  gtk_table_attach (GTK_TABLE (table1), entryVobSub, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  buttonVob = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonVob);
  gtk_table_attach (GTK_TABLE (table1), buttonVob, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  buttonIfo = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonIfo);
  gtk_table_attach (GTK_TABLE (table1), buttonIfo, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  buttonVobSub = gtk_button_new_from_stock ("gtk-open");
  gtk_widget_show (buttonVobSub);
  gtk_table_attach (GTK_TABLE (table1), buttonVobSub, 2, 3, 2, 3,
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
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, entryVob, "entryVob");
  GLADE_HOOKUP_OBJECT (dialog1, entryIfo, "entryIfo");
  GLADE_HOOKUP_OBJECT (dialog1, entryVobSub, "entryVobSub");
  GLADE_HOOKUP_OBJECT (dialog1, buttonVob, "buttonVob");
  GLADE_HOOKUP_OBJECT (dialog1, buttonIfo, "buttonIfo");
  GLADE_HOOKUP_OBJECT (dialog1, buttonVobSub, "buttonVobSub");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

