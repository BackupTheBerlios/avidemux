//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <config.h>
#include "ADM_lavcodec.h"

#include <stdio.h>
#include <stdlib.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <math.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h" 
#define CHECK_GET(x,y) {*y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),*y);}	

static GtkWidget	*create_dialog1 (void);
uint8_t DIA_lavDecoder(uint32_t *swapUv, uint32_t *showU);
uint8_t DIA_lavDecoder(uint32_t *swapUv, uint32_t *showU)
{
	
GtkWidget *dialog;
	int ret;

	

	dialog=create_dialog1();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);
	
	CHECK_SET(checkbuttonSwap,swapUv);
	CHECK_SET(checkbutton2,showU);
	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		CHECK_GET(checkbuttonSwap,swapUv);
		CHECK_GET(checkbutton2,showU);
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
  GtkWidget *checkbuttonSwap;
  GtkWidget *checkbutton2;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Libavcodec Decoder Option"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  checkbuttonSwap = gtk_check_button_new_with_mnemonic (_("Swap UV"));
  gtk_widget_show (checkbuttonSwap);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbuttonSwap, FALSE, FALSE, 0);

  checkbutton2 = gtk_check_button_new_with_mnemonic (_("Show motion Vectors"));
  gtk_widget_show (checkbutton2);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbutton2, FALSE, FALSE, 0);

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
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonSwap, "checkbuttonSwap");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton2, "checkbutton2");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

