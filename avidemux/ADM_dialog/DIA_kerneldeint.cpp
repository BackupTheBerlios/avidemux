//

# include <config.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "config.h"
#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

static GtkWidget	*create_dialog1 (void);

#define CHECK_GET(x,y) {*y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),*y);}	

#define SPIN_GET(x,y) {*y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;\
				printf(#x":%d\n",*y);}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)*y) ; \
				printf(#x":%d\n",*y);}


uint8_t DIA_kerneldeint(uint32_t *order, uint32_t *threshold, uint32_t *sharp, 
		uint32_t *twoway, uint32_t *map);
uint8_t DIA_kerneldeint(uint32_t *order, uint32_t *threshold, uint32_t *sharp, 
		uint32_t *twoway, uint32_t *map)
{
	GtkWidget *dialog;

	char str[100];
	uint8_t ret=0;

	gint r;

	dialog=create_dialog1();
	gtk_transient(dialog);
	// Now update the entries
	if(*order==0) // BFF
	{
		RADIO_SET(radiobuttonBFF,1);	
	}
	else
	{
		RADIO_SET(radiobuttonTFF,1);			
	}
	CHECK_SET(checkbuttonSharp,sharp);
	CHECK_SET(checkbuttonTwoWay,twoway);
	CHECK_SET(checkbuttonMap,map);
	SPIN_SET(spinbuttonThresold,threshold);
	
	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		CHECK_GET(checkbuttonSharp,sharp);
		CHECK_GET(checkbuttonTwoWay,twoway);
		CHECK_GET(checkbuttonMap,map);
		SPIN_GET(spinbuttonThresold,threshold);
		*order=RADIO_GET(radiobuttonTFF);	
		ret=1;
	}

	gtk_widget_destroy(dialog);

	return ret;

}

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *hbox2;
  GtkWidget *radiobuttonBFF;
  GSList *radiobuttonBFF_group = NULL;
  GtkWidget *radiobuttonTFF;
  GtkWidget *hbox3;
  GtkWidget *label2;
  GtkObject *spinbuttonThresold_adj;
  GtkWidget *spinbuttonThresold;
  GtkWidget *checkbuttonSharp;
  GtkWidget *checkbuttonTwoWay;
  GtkWidget *checkbuttonMap;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Kernel Deinterlacer"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Field Order :"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, TRUE, FALSE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), hbox2, TRUE, TRUE, 0);

  radiobuttonBFF = gtk_radio_button_new_with_mnemonic (NULL, _("Bottom Field First"));
  gtk_widget_show (radiobuttonBFF);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobuttonBFF, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonBFF), radiobuttonBFF_group);
  radiobuttonBFF_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonBFF));

  radiobuttonTFF = gtk_radio_button_new_with_mnemonic (NULL, _("Top Field First"));
  gtk_widget_show (radiobuttonTFF);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobuttonTFF, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonTFF), radiobuttonBFF_group);
  radiobuttonBFF_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonTFF));

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Threshold :"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox3), label2, TRUE, FALSE, 0);

  spinbuttonThresold_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbuttonThresold = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonThresold_adj), 1, 0);
  gtk_widget_show (spinbuttonThresold);
  gtk_box_pack_start (GTK_BOX (hbox3), spinbuttonThresold, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonThresold, _("smaller means deinterlace more!"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonThresold), TRUE);

  checkbuttonSharp = gtk_check_button_new_with_mnemonic (_("Sharp"));
  gtk_widget_show (checkbuttonSharp);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbuttonSharp, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonSharp, _("Sharper engine"), NULL);

  checkbuttonTwoWay = gtk_check_button_new_with_mnemonic (_("Two Way"));
  gtk_widget_show (checkbuttonTwoWay);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbuttonTwoWay, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonTwoWay, _("Extrapolate two ways (better not use it)"), NULL);

  checkbuttonMap = gtk_check_button_new_with_mnemonic (_("Map"));
  gtk_widget_show (checkbuttonMap);
  gtk_box_pack_start (GTK_BOX (vbox1), checkbuttonMap, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonMap, _("Show the interlaced areas (test only!)"), NULL);

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
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonBFF, "radiobuttonBFF");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonTFF, "radiobuttonTFF");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonThresold, "spinbuttonThresold");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonSharp, "checkbuttonSharp");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonTwoWay, "checkbuttonTwoWay");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonMap, "checkbuttonMap");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

  return dialog1;
}

