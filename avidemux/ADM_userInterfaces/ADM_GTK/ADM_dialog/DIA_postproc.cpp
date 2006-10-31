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
# include <config.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


static GtkWidget	*create_dialog1 (void);


int DIA_getMPParams( uint32_t *pplevel, uint32_t *ppstrength,uint32_t *swap)
{
	GtkWidget *dialog;
	GtkAdjustment *pp_adj;
	int ret=0;

	dialog=create_dialog1();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);
#define SET_TOG(x,y) { if(*pplevel & x)  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(y),1);    else \
                                     gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(y),0); }
#define GET_TOG(x,y) { if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(y)))   \
                                *pplevel+=x;     }
#define WID(x) lookup_widget(dialog,#x)

		pp_adj=gtk_range_get_adjustment (GTK_RANGE (WID(hscale1)));

 		gtk_adjustment_set_value( GTK_ADJUSTMENT(pp_adj),*ppstrength);
    		SET_TOG(1, WID(Horizontal));
    		SET_TOG(2,  WID(Vertical));
     		SET_TOG(4,  WID(Dering));
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(SwapUV)),*swap);



	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

	         *ppstrength=   (int)floor(GTK_ADJUSTMENT( pp_adj) ->value);
        	*pplevel=0;
    		GET_TOG(1, WID(Horizontal));
    		GET_TOG(2,  WID(Vertical));
     		GET_TOG(4,  WID(Dering));
		*swap=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(SwapUV)));
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
  GtkWidget *vbox1;
  GtkWidget *Horizontal;
  GtkWidget *Vertical;
  GtkWidget *Dering;
  GtkWidget *vbox2;
  GtkWidget *SwapUV;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label2;
  GtkWidget *label1;
  GtkWidget *hscale1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Mplayer PostProc options"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  Horizontal = gtk_check_button_new_with_mnemonic (_("Horizontal Deblocking"));
  gtk_widget_show (Horizontal);
  gtk_box_pack_start (GTK_BOX (vbox1), Horizontal, FALSE, FALSE, 0);

  Vertical = gtk_check_button_new_with_mnemonic (_("Vertical Deblocking"));
  gtk_widget_show (Vertical);
  gtk_box_pack_start (GTK_BOX (vbox1), Vertical, FALSE, FALSE, 0);

  Dering = gtk_check_button_new_with_mnemonic (_("Deringing"));
  gtk_widget_show (Dering);
  gtk_box_pack_start (GTK_BOX (vbox1), Dering, FALSE, FALSE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

  SwapUV = gtk_check_button_new ();
  gtk_widget_show (SwapUV);
  gtk_box_pack_start (GTK_BOX (vbox2), SwapUV, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (SwapUV), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic (_("Swap U & V"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  label1 = gtk_label_new (_("Filter Strength"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  hscale1 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (5, 0, 5, 1, 1, 0)));
  gtk_widget_show (hscale1);
  gtk_box_pack_start (GTK_BOX (vbox1), hscale1, TRUE, TRUE, 0);
  gtk_scale_set_digits (GTK_SCALE (hscale1), 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, Horizontal, "Horizontal");
  GLADE_HOOKUP_OBJECT (dialog1, Vertical, "Vertical");
  GLADE_HOOKUP_OBJECT (dialog1, Dering, "Dering");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, SwapUV, "SwapUV");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, hscale1, "hscale1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

