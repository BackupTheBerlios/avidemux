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

static GtkWidget       *create_dialog1 (void);
static void             updateStatus(void);
static int              getSelection(GtkWidget *dialog);
extern bool parseECMAScript(const char *name);
static const char *StringStatus[]={"Ready","Succeeded","Failed","Deleted","Running"};


typedef enum JOB_STATUS
{
        STATUS_READY=0,
        STATUS_SUCCEED,
        STATUS_FAILED,
        STATUS_DELETED,
        STATUS_RUNNING
};

typedef enum 
{
        COMMAND_DELETE_ALL=1,
        COMMAND_DELETE=2,
        COMMAND_RUN_ALL=3,
        COMMAND_RUN=4
};


typedef struct JobsDescriptor
{
        GtkWidget *dialog;
        GtkListStore *store;
        uint32_t  nb;
        char      **name;
        JOB_STATUS *status;
}JobsDescriptor;

static JobsDescriptor jobs;

uint8_t  DIA_job(uint32_t nb, char **name)
{
GtkListStore *store;

GtkTreeViewColumn *column,*column2;
GtkCellRenderer *renderer;

        int ret=0;
        


        GtkWidget *dialog;

        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        store=gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
        
        // initialize our job structure
        jobs.dialog=dialog;
        jobs.nb=nb;
        jobs.name=name;
        jobs.status=new JOB_STATUS[nb];
        jobs.store=store;
        memset(jobs.status,0,jobs.nb*sizeof(JOB_STATUS));

        gtk_tree_view_set_model(GTK_TREE_VIEW(WID(treeview1)),GTK_TREE_MODEL (store));


        // Add columns

        renderer = gtk_cell_renderer_text_new ();

        column = gtk_tree_view_column_new_with_attributes ("Name", renderer,
                                                      "text", (GdkModifierType) 0,
                                                      NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (WID(treeview1)), column);

        column2 = gtk_tree_view_column_new_with_attributes ("Status", renderer,
                                                      "text", (GdkModifierType) 1,
                                                      NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (WID(treeview1)), column2);

        //
        #define ASSOCIATE(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)
            ASSOCIATE(buttonDelete,COMMAND_DELETE);
            ASSOCIATE(buttonDeleteAll,COMMAND_DELETE_ALL);
            ASSOCIATE(buttonRunAll,COMMAND_RUN_ALL);
            ASSOCIATE(buttonRun,COMMAND_RUN);
        //
         
        int running=1;
        while(running)
        {
                int sel=0;
                updateStatus();
                switch(gtk_dialog_run(GTK_DIALOG(dialog)))
                {
                        case GTK_RESPONSE_OK : running=0;break;
                        case GTK_RESPONSE_APPLY : running=0;break;
                        case GTK_RESPONSE_CANCEL : running=0;break;
                        case COMMAND_DELETE_ALL:
                                        if(GUI_Confirmation_HIG("Sure!","Delete ALL jobs","Are you sure you want to delete all jobs ?"))
                                        {
                                                for(int i=0;i<jobs.nb;i++) jobs.status[i]=STATUS_DELETED;
                                        }
                                        break;
                        case COMMAND_RUN: 
                                        sel=getSelection(jobs.dialog);
                                        if(sel>=jobs.nb) break;
                                        jobs.status[sel]=STATUS_RUNNING;
                                        updateStatus();

                                          if(parseECMAScript(jobs.name[sel])) jobs.status[sel]=STATUS_SUCCEED;
                                                        else jobs.status[sel]=STATUS_FAILED;
                                        break;
                        case COMMAND_RUN_ALL: 
                                        for(int i=0;i<jobs.nb;i++)
                                        {
                                                if(jobs.status[i]==STATUS_DELETED) continue;
                                                if(jobs.status[i]==STATUS_SUCCEED) continue;
                                                jobs.status[i]=STATUS_RUNNING;
                                                updateStatus();
                                                if(parseECMAScript(jobs.name[i])) jobs.status[i]=STATUS_SUCCEED;
                                                        else jobs.status[i]=STATUS_FAILED;

                                        }
                                        break;
                        case COMMAND_DELETE: 
                                        sel=getSelection(jobs.dialog);
                                        if(sel>=jobs.nb) break;
                                        if(GUI_Confirmation_HIG("Sure!","Delete job","Are you sure you want to delete %s job ?",GetFileName(jobs.name[sel])))
                                        {
                                                jobs.status[sel]=STATUS_DELETED;
                                        }
                                        break;

                        
                        default: GUI_Alert("Unknown event");break;
                }
                
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);

        // Now delete the "deleted" jobs
        for(int i=0;i<jobs.nb;i++)
        {
                if(jobs.status[i]==STATUS_DELETED)
                {
                        unlink(jobs.name[i]);
                }

        }

        delete [] jobs.status;

        return ret;
}
//**************************************************
int              getSelection(GtkWidget *dialog)
{
uint32_t n=0xffff;
        if(! getSelectionNumber(jobs.nb,WID(treeview1) , jobs.store,&n)) return 0xffff;
        return n;
}
//*************************************
void updateStatus(void)
{
GtkTreeIter iter;
        gtk_list_store_clear (jobs.store);
        for (uint32_t i = 0; i < jobs.nb; i++)
        {
                gtk_list_store_append (jobs.store, &iter);
                gtk_list_store_set (jobs.store, &iter, 0,GetFileName(jobs.name[i]),-1);
                gtk_list_store_set (jobs.store, &iter, 1,StringStatus[jobs.status[i]], -1);
                
                

        }
}
//*************************************

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *hbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview1;
  GtkWidget *vbuttonbox1;
  GtkWidget *buttonDeleteAll;
  GtkWidget *buttonDelete;
  GtkWidget *buttonRunAll;
  GtkWidget *buttonRun;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Jobs"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox1, TRUE, TRUE, 0);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  treeview1 = gtk_tree_view_new ();
  gtk_widget_show (treeview1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview1);
  gtk_widget_set_size_request (treeview1, 300, -1);

  vbuttonbox1 = gtk_vbutton_box_new ();
  gtk_widget_show (vbuttonbox1);
  gtk_box_pack_start (GTK_BOX (hbox1), vbuttonbox1, FALSE, FALSE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (vbuttonbox1), GTK_BUTTONBOX_START);

  buttonDeleteAll = gtk_button_new_with_mnemonic (_("Delete All Jobs"));
  gtk_widget_show (buttonDeleteAll);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDeleteAll);
  GTK_WIDGET_SET_FLAGS (buttonDeleteAll, GTK_CAN_DEFAULT);

  buttonDelete = gtk_button_new_with_mnemonic (_("Delete Job"));
  gtk_widget_show (buttonDelete);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonDelete);
  GTK_WIDGET_SET_FLAGS (buttonDelete, GTK_CAN_DEFAULT);

  buttonRunAll = gtk_button_new_with_mnemonic (_("Run all jobs"));
  gtk_widget_show (buttonRunAll);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonRunAll);
  GTK_WIDGET_SET_FLAGS (buttonRunAll, GTK_CAN_DEFAULT);

  buttonRun = gtk_button_new_with_mnemonic (_("Run Job"));
  gtk_widget_show (buttonRun);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), buttonRun);
  GTK_WIDGET_SET_FLAGS (buttonRun, GTK_CAN_DEFAULT);

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
  GLADE_HOOKUP_OBJECT (dialog1, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (dialog1, treeview1, "treeview1");
  GLADE_HOOKUP_OBJECT (dialog1, vbuttonbox1, "vbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDeleteAll, "buttonDeleteAll");
  GLADE_HOOKUP_OBJECT (dialog1, buttonDelete, "buttonDelete");
  GLADE_HOOKUP_OBJECT (dialog1, buttonRunAll, "buttonRunAll");
  GLADE_HOOKUP_OBJECT (dialog1, buttonRun, "buttonRun");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

