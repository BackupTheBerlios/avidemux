# include <config.h>

#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "callbacks.h"
//#include "avi_vars.h"
#include "default.h"


#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"

#include "ADM_assert.h" 

#ifdef USE_X264
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encx264.h"
#include "ADM_codecs/ADM_x264param.h"

#define SPIN_GET(x,y) {specific->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)specific->y) ;}

#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),y);}
#define MENU_GET(x)   getRangeInMenu(WID(x))

#define TOGGLE_SET(x,y) {gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WID(x)),specific->y);}
#define TOGGLE_GET(x,y) {specific->y=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(WID(x)));;}
                                
#define ENTRY_SET(x,y) {gtk_write_entry(WID(x),(int)y);}
#define ENTRY_GET(x,y) {y=gtk_read_entry(WID(x));}

#define CHECK_GET(x,y) {specific->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),specific->y);}                                         

uint8_t DIA_x264(COMPRES_PARAMS *config);

static GtkWidget       *create_dialog1 (void);  

static COMPRES_PARAMS generic={CodecDummy,"dummy","dummy","dummy",COMPRESS_CQ,4,1500,700,0,0,NULL,0};
static GtkWidget *dialog;

static void updateMode( void );
static int cb_mod(GtkObject * object, gpointer user_data);
static ADM_x264Param *specific;

/*********************************************/
uint8_t DIA_x264(COMPRES_PARAMS *config)
{
  
  uint8_t ret=0;
  
        ADM_assert(config->extraSettingsLen==sizeof(ADM_x264Param));
  
        memcpy(&generic,config,sizeof(generic));
        specific=(ADM_x264Param *)config->extraSettings;
       
        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        // Update
        SPIN_SET(spinbuttonQmin,qmin);
        SPIN_SET(spinbuttonQmax,qmax);
        
        SPIN_SET(spinbuttonKfmin,minKf);
        SPIN_SET(spinbuttonKfmax,maxKf);
        
        SPIN_SET(spinbuttonBframes,nbBframe);
        
        CHECK_SET(checkbuttonCabac,cabac);
        
        updateMode();
        
        gtk_signal_connect(GTK_OBJECT(WID(optionmenu1)), "changed",
                           GTK_SIGNAL_FUNC(cb_mod), NULL);
        
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
          uint32_t b;
          
          
          SPIN_GET(spinbuttonQmin,qmin);
          SPIN_GET(spinbuttonQmax,qmax);
        
          SPIN_GET(spinbuttonKfmin,minKf);
          SPIN_GET(spinbuttonKfmax,maxKf);
        
          SPIN_GET(spinbuttonBframes,nbBframe);
        
          CHECK_GET(checkbuttonCabac,cabac);
          ret=1;
          ENTRY_GET(entryQz,b);
          switch(generic.mode)
          {
            case COMPRESS_CBR:
              generic.bitrate=b*1000;
              break;

            case COMPRESS_2PASS:              
              b=generic.finalsize=b;
              break;

            case COMPRESS_CQ:              
              generic.qz=b;
              break;            
            default:
              ADM_assert(0);
          }
          memcpy(config,&generic,sizeof(generic));
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;
  
}
void updateMode( void )
{
  uint32_t b;
                // set the right select button
  switch (generic.mode)
  {
    case COMPRESS_CBR:
      MENU_SET(optionmenu1,1);                  
      b=generic.bitrate/1000;
      ENTRY_SET(entryQz,b);     
      gtk_label_set_text(GTK_LABEL(WID(label3)),"Target bitrate (kb/s):");
      break;

    case COMPRESS_2PASS:
      MENU_SET(optionmenu1,2);                  
      b=generic.finalsize;
      ENTRY_SET(entryQz,b);     
      gtk_label_set_text(GTK_LABEL(WID(label3)),"Target FinalSize (MB):");
            
      break;

    case COMPRESS_CQ:
      MENU_SET(optionmenu1,0);                  
      b=generic.qz;
      ENTRY_SET(entryQz,b);     
      gtk_label_set_text(GTK_LABEL(WID(label3)),"Quantizer (1-50):");
      break;
    default:
      ADM_assert(0);

  }
}               
int cb_mod(GtkObject * object, gpointer user_data)
{
  int r;
  r=MENU_GET(optionmenu1);
  switch(r)
  {
    case 1: generic.mode=COMPRESS_CBR ;break;
    case 0: generic.mode=COMPRESS_CQ ;break;
    case 2: generic.mode=COMPRESS_2PASS ;break;
    default: ADM_assert(0);
        
  }
  updateMode();
  printf("Changed!!!\n");

}
/*********************************************/

GtkWidget*
    create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *constant_quantizer1;
  GtkWidget *constant_bitrate1;
  GtkWidget *_2_pass_average_bitrate1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *entryQz;
  GtkWidget *hseparator1;
  GtkWidget *hseparator2;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *vbox2;
  GtkWidget *table2;
  GtkObject *spinbuttonQmin_adj;
  GtkWidget *spinbuttonQmin;
  GtkObject *spinbuttonQmax_adj;
  GtkWidget *spinbuttonQmax;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *hbox1;
  GtkWidget *label7;
  GtkObject *spinbuttonKfmin_adj;
  GtkWidget *spinbuttonKfmin;
  GtkWidget *label8;
  GtkObject *spinbuttonKfmax_adj;
  GtkWidget *spinbuttonKfmax;
  GtkWidget *hbox2;
  GtkWidget *label9;
  GtkObject *spinbuttonBframes_adj;
  GtkWidget *spinbuttonBframes;
  GtkWidget *checkbuttonCabac;
  GtkWidget *hseparator3;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("X264 Settings"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  constant_quantizer1 = gtk_menu_item_new_with_mnemonic (_("Constant Quantizer"));
  gtk_widget_show (constant_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu1), constant_quantizer1);

  constant_bitrate1 = gtk_menu_item_new_with_mnemonic (_("Constant Bitrate"));
  gtk_widget_show (constant_bitrate1);
  gtk_container_add (GTK_CONTAINER (menu1), constant_bitrate1);

  _2_pass_average_bitrate1 = gtk_menu_item_new_with_mnemonic (_("2 pass Average Bitrate"));
  gtk_widget_show (_2_pass_average_bitrate1);
  gtk_container_add (GTK_CONTAINER (menu1), _2_pass_average_bitrate1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  label2 = gtk_label_new (_("Mode :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Quantizer :"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  entryQz = gtk_entry_new ();
  gtk_widget_show (entryQz);
  gtk_table_attach (GTK_TABLE (table1), entryQz, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator1);
  gtk_table_attach (GTK_TABLE (table1), hseparator1, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  hseparator2 = gtk_hseparator_new ();
  gtk_widget_show (hseparator2);
  gtk_table_attach (GTK_TABLE (table1), hseparator2, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label1 = gtk_label_new (_("<b>Encoding Mode</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame2), vbox2);

  table2 = gtk_table_new (1, 4, FALSE);
  gtk_widget_show (table2);
  gtk_box_pack_start (GTK_BOX (vbox2), table2, TRUE, TRUE, 0);

  spinbuttonQmin_adj = gtk_adjustment_new (1, 1, 50, 1, 10, 10);
  spinbuttonQmin = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQmin_adj), 1, 0);
  gtk_widget_show (spinbuttonQmin);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonQmin, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQmin), TRUE);

  spinbuttonQmax_adj = gtk_adjustment_new (1, 1, 50, 1, 10, 10);
  spinbuttonQmax = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQmax_adj), 1, 0);
  gtk_widget_show (spinbuttonQmax);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonQmax, 3, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQmax), TRUE);

  label5 = gtk_label_new (_("Qmin :"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table2), label5, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new (_("Qmax :"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table2), label6, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  label7 = gtk_label_new (_("Kf Min :"));
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox1), label7, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

  spinbuttonKfmin_adj = gtk_adjustment_new (5, 5, 5000, 1, 10, 10);
  spinbuttonKfmin = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonKfmin_adj), 1, 0);
  gtk_widget_show (spinbuttonKfmin);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonKfmin, FALSE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonKfmin), TRUE);

  label8 = gtk_label_new (_("Kf Max :"));
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox1), label8, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  spinbuttonKfmax_adj = gtk_adjustment_new (5, 5, 5000, 1, 10, 10);
  spinbuttonKfmax = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonKfmax_adj), 1, 0);
  gtk_widget_show (spinbuttonKfmax);
  gtk_box_pack_start (GTK_BOX (hbox1), spinbuttonKfmax, FALSE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonKfmax), TRUE);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

  label9 = gtk_label_new (_("# B Frames :"));
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox2), label9, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  spinbuttonBframes_adj = gtk_adjustment_new (1, 0, 4, 1, 10, 10);
  spinbuttonBframes = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBframes_adj), 1, 0);
  gtk_widget_show (spinbuttonBframes);
  gtk_box_pack_start (GTK_BOX (hbox2), spinbuttonBframes, FALSE, FALSE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBframes), TRUE);

  checkbuttonCabac = gtk_check_button_new_with_mnemonic (_("Cabac"));
  gtk_widget_show (checkbuttonCabac);
  gtk_box_pack_start (GTK_BOX (hbox2), checkbuttonCabac, FALSE, FALSE, 0);

  hseparator3 = gtk_hseparator_new ();
  gtk_widget_show (hseparator3);
  gtk_box_pack_start (GTK_BOX (hbox2), hseparator3, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("<b>Settings</b>"));
  gtk_widget_show (label4);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label4);
  gtk_label_set_use_markup (GTK_LABEL (label4), TRUE);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, constant_quantizer1, "constant_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, constant_bitrate1, "constant_bitrate1");
  GLADE_HOOKUP_OBJECT (dialog1, _2_pass_average_bitrate1, "_2_pass_average_bitrate1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, entryQz, "entryQz");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator1, "hseparator1");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator2, "hseparator2");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQmin, "spinbuttonQmin");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQmax, "spinbuttonQmax");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonKfmin, "spinbuttonKfmin");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonKfmax, "spinbuttonKfmax");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBframes, "spinbuttonBframes");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCabac, "checkbuttonCabac");
  GLADE_HOOKUP_OBJECT (dialog1, hseparator3, "hseparator3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}
#endif
