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
#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


#include "ADM_dialog/DIA_enter.h"

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
		gtk_transient(dialog);
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
	gtk_widget_destroy(dialog);
	};
	return ret;
}






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

