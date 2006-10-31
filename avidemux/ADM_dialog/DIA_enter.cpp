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
#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


#include "ADM_dialog/DIA_enter.h"
static GtkWidget        *create_dialog3 (void);
static GtkWidget	*create_dialog1 (void);
static GtkWidget	*create_dialog2 (int mmin, int mmax);

uint8_t  		GUI_getIntegerValue(int *valye, int min, int max, const char *title)
{
	return DIA_GetIntegerValue(valye,min,max,title,title);
}
uint8_t  DIA_GetIntegerValue(int *value, int min, int max, const char *title, const char *legend)
{

    	int d;
    	char *str;
    	char string[250];
      	char s[250];
      	sprintf(s,"%s (%d -- %d)",legend,min,max);

	GtkWidget *dialog;
	int ret=-1;

	while(ret==-1)
	{
		dialog=create_dialog2(min,max);
		gtk_register_dialog(dialog);
                gtk_widget_grab_focus (WID(spinbutton1));
                gtk_widget_grab_default(WID(okbutton1));
		sprintf(string,"%d",*value);

        	//gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog,"entry1")),string);
		gtk_window_set_title (GTK_WINDOW (dialog), title);
		gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"label1")),s);

		gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton1)),(gfloat)*value) ;
		
		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
		{
			d = (uint32_t) gtk_spin_button_get_value_as_int(
								GTK_SPIN_BUTTON(WID(spinbutton1)));
  			if((d>=min) && ( d<=max))
     				{
           	 	 	*value=d;
              			ret= 1;
	         		}
			else
				ret=-1;
		}
		else
			ret=0;
                gtk_unregister_dialog(dialog);
                gtk_widget_destroy(dialog);
	};
	return ret;
}

uint8_t  DIA_GetFloatValue(float *value, float min, float max, const char *title, const char *legend)
{


	float d;
    	char *str;
    	char string[250];
      	char s[250];
        	sprintf(s,"%s (%f -- %f)",legend,min,max);

	GtkWidget *dialog;
	int ret=-1;

	while(ret==-1)
	{
		dialog=create_dialog1();
		gtk_register_dialog(dialog);
		sprintf(string,"%f",*value);

        	gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog,"entry1")),string);
		gtk_window_set_title (GTK_WINDOW (dialog), title);
		gtk_label_set_text(GTK_LABEL(lookup_widget(dialog,"legend")),s);

		if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
		{

  			str = gtk_editable_get_chars(GTK_EDITABLE(lookup_widget(dialog,"entry1")), 0, -1);
	  		d = atof(str);
    			if((d>=min) && ( d<=max))
     				{
           	 	 	*value=d;
              			ret= 1;
	         		}
			else
				ret=-1;
		}
		else
			ret=0;
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	};
	return ret;
}

//***************************************************
uint8_t  DIA_enterString_HIG(const char *title, const char *second, char **outname)
{
        int ret=0;
        GtkWidget *dialog;
        

        char *alertstring;
        *outname=NULL;
     
        alertstring = g_strconcat("<span size=\"larger\" weight=\"bold\">", title, "</span>\n\n", NULL);
                

        dialog=create_dialog3();
        gtk_label_set_text(GTK_LABEL(WID(label1)), alertstring);
        g_free(alertstring);

        gtk_label_set_text(GTK_LABEL(WID(label2)), second);

        gtk_label_set_use_markup(GTK_LABEL(WID(label1)), TRUE);
        gtk_register_dialog(dialog);

        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
                ret=1;
                *outname=ADM_strdup( gtk_editable_get_chars(GTK_EDITABLE ((WID(entry1))), 0, -1));
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        UI_purge();
        return ret;
}
//***************************************************
GtkWidget	*create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *legend;
  GtkWidget *entry1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Title"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, FALSE, FALSE, 0);

  legend = gtk_label_new (_("label1"));
  gtk_widget_show (legend);
  gtk_box_pack_start (GTK_BOX (vbox1), legend, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (legend), GTK_JUSTIFY_LEFT);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (vbox1), entry1, FALSE, FALSE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, legend, "legend");
  GLADE_HOOKUP_OBJECT (dialog1, entry1, "entry1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

GtkWidget	*create_dialog2 (int mmin, int mmax)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkObject *spinbutton1_adj;
  GtkWidget *spinbutton1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("dialog1"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("label1"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  spinbutton1_adj = gtk_adjustment_new (1, mmin, mmax, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_show (spinbutton1);
  gtk_box_pack_start (GTK_BOX (vbox1), spinbutton1, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);
  GTK_WIDGET_UNSET_FLAGS (spinbutton1, GTK_CAN_DEFAULT);

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
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton1, "spinbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  gtk_widget_grab_focus (okbutton1);
  gtk_widget_grab_default (okbutton1);
  return dialog1;
}


GtkWidget *create_dialog3 (void)
{
  GtkWidget *dialog3;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *hbox1;
  GtkWidget *label2;
  GtkWidget *entry1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog3 = gtk_dialog_new ();
  gtk_window_set_type_hint (GTK_WINDOW (dialog3), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog3)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("label1"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("label2"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 0);

  entry1 = gtk_entry_new ();
  gtk_widget_show (entry1);
  gtk_box_pack_start (GTK_BOX (hbox1), entry1, TRUE, TRUE, 0);
  GTK_WIDGET_SET_FLAGS (entry1, GTK_CAN_DEFAULT);

  dialog_action_area1 = GTK_DIALOG (dialog3)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog3), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog3), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog3, dialog3, "dialog3");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog3, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog3, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog3, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog3, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog3, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog3, entry1, "entry1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog3, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog3, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog3, okbutton1, "okbutton1");

  gtk_widget_grab_default (okbutton1);
  return dialog3;
}

