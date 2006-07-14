
#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "ADM_assert.h"
#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_assert.h" 


#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioencoder.h"
#include "ADM_audiofilter/audioencoder_vorbis_param.h"



#define NB_BITRATE 11
static int BTR[] = {
		48,
		56,
		64,
		80,
    		96,
    		112,
    		128,
   		 160,
    		192,
   		 224,
		 384
};
  
static GtkWidget	*create_dialog1 (void);
#define CHECK_HAS_CHANGED 100
int DIA_getVorbisSettings(ADM_audioEncoderDescriptor *descriptor)
{
  GtkWidget *dialog;
  
  int ret=0;
  char string[400];
  GtkComboBox *combo_box;

  VORBIS_encoderParam *vorbisParam;
  ADM_assert(sizeof(VORBIS_encoderParam)==descriptor->paramSize);
  
  vorbisParam=(VORBIS_encoderParam*)descriptor->param;
  
  dialog=create_dialog1();
  gtk_register_dialog(dialog);

  //set initial value
  // *******  set bitrate ************
  combo_box=GTK_COMBO_BOX(WID(comboboxBitrate));
  gtk_combo_box_remove_text(combo_box,0);
        
  for(unsigned int i=0;i<NB_BITRATE;i++)
  {
    sprintf(string,"%d kbits",BTR[i]);
    gtk_combo_box_append_text(combo_box,string);
    if(descriptor->bitrate==BTR[i])
      gtk_combo_box_set_active(combo_box,i);
  }
  // set quality
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbuttonQuality)),(gfloat)vorbisParam->quality) ;

  //
  int cbr=0;
  if(vorbisParam->mode==ADM_VORBIS_CBR) 
  {
    RADIO_SET(radiobuttonCBR,1);
    cbr=1;
  }
  else
  {
      RADIO_SET(radiobuttonVBR,1);
  }

  gtk_widget_set_sensitive(WID(comboboxBitrate),cbr);
  gtk_widget_set_sensitive(WID(spinbuttonQuality),cbr^1);
#define ASSOCIATE(x)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),CHECK_HAS_CHANGED)
  ASSOCIATE(radiobuttonVBR);
  ASSOCIATE(radiobuttonCBR);

  int run=1;
  while(run)
  {
  switch(gtk_dialog_run(GTK_DIALOG(dialog)))
  {
    case GTK_RESPONSE_OK:
    combo_box=GTK_COMBO_BOX(WID(comboboxBitrate));
    descriptor->bitrate=BTR[gtk_combo_box_get_active(combo_box)];
    vorbisParam->quality=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(spinbuttonQuality))) ;
    run=0;
    cbr=RADIO_GET(radiobuttonCBR);
    if(cbr)
    {
      vorbisParam->mode=ADM_VORBIS_CBR;
      combo_box=GTK_COMBO_BOX(WID(comboboxBitrate));
      descriptor->bitrate=BTR[gtk_combo_box_get_active(combo_box)];
    }else
    {
      vorbisParam->mode=ADM_VORBIS_VBR;
      vorbisParam->quality=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(spinbuttonQuality))) ;
    }
    break;
    case CHECK_HAS_CHANGED:
    {
      cbr=RADIO_GET(radiobuttonCBR);

      gtk_widget_set_sensitive(WID(comboboxBitrate),cbr);
      gtk_widget_set_sensitive(WID(spinbuttonQuality),cbr^1);
    }
      break;
    default:
      run=0;
      break;
  }
  }
      
  gtk_unregister_dialog(dialog);
  gtk_widget_destroy(dialog);
  return ret;
  }

  
  //*******************************************


  GtkWidget*
      create_dialog1 (void)
  {
    GtkWidget *dialog1;
    GtkWidget *dialog_vbox1;
    GtkWidget *table1;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *label3;
    GtkWidget *vbox1;
    GtkWidget *radiobuttonCBR;
    GSList *radiobuttonCBR_group = NULL;
    GtkWidget *radiobuttonVBR;
    GtkWidget *comboboxBitrate;
    GtkObject *spinbuttonQuality_adj;
    GtkWidget *spinbuttonQuality;
    GtkWidget *dialog_action_area1;
    GtkWidget *cancelbutton1;
    GtkWidget *okbutton1;

    dialog1 = gtk_dialog_new ();
    gtk_window_set_title (GTK_WINDOW (dialog1), _("Vorbis encoder settings"));
    gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
    gtk_widget_show (dialog_vbox1);

    table1 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (table1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

    label1 = gtk_label_new (_("<b>Mode :</b>"));
    gtk_widget_show (label1);
    gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

    label2 = gtk_label_new (_("<b>Bitrate :</b>"));
    gtk_widget_show (label2);
    gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label2), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

    label3 = gtk_label_new (_("<b>Quality :</b>"));
    gtk_widget_show (label3);
    gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label3), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_table_attach (GTK_TABLE (table1), vbox1, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);

    radiobuttonCBR = gtk_radio_button_new_with_mnemonic (NULL, _("CBR (bitrate based)"));
    gtk_widget_show (radiobuttonCBR);
    gtk_box_pack_start (GTK_BOX (vbox1), radiobuttonCBR, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonCBR), radiobuttonCBR_group);
    radiobuttonCBR_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonCBR));

    radiobuttonVBR = gtk_radio_button_new_with_mnemonic (NULL, _("VBR (Quality based)"));
    gtk_widget_show (radiobuttonVBR);
    gtk_box_pack_start (GTK_BOX (vbox1), radiobuttonVBR, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonVBR), radiobuttonCBR_group);
    radiobuttonCBR_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonVBR));

    comboboxBitrate = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxBitrate);
    gtk_table_attach (GTK_TABLE (table1), comboboxBitrate, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);

    spinbuttonQuality_adj = gtk_adjustment_new (-1, -1, 10, 1, 1, 1);
    spinbuttonQuality = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQuality_adj), 1, 1);
    gtk_widget_show (spinbuttonQuality);
    gtk_table_attach (GTK_TABLE (table1), spinbuttonQuality, 1, 2, 2, 3,
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
    GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT (dialog1, radiobuttonCBR, "radiobuttonCBR");
    GLADE_HOOKUP_OBJECT (dialog1, radiobuttonVBR, "radiobuttonVBR");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxBitrate, "comboboxBitrate");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQuality, "spinbuttonQuality");
    GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
    GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
    GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

    return dialog1;
  }

