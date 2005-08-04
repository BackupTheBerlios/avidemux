#include <config.h>
#include "ADM_lavcodec.h"

#include <stdio.h>
#include <stdlib.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>
# include <config.h>

#include "ADM_library/default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_codecs/ADM_ffmpegConfig.h"
#include "ADM_assert.h" 
static GtkWidget	*create_dialog1 (void);

				
#define ENTRY_SET(x,y) {gtk_write_entry(WID(x),(int)conf->y);}
#define ENTRY_GET(x,y) {localSettings.y=gtk_read_entry(WID(x));}


#define HIST_SET(x) 	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu_mode)), x)
#define VAL_SET(x) 	gtk_write_entry(WID(entry_bitrate), x)

static GtkWidget 	*dialog=NULL;		      
static uint32_t 	mQ,mB,mS;
static COMPRESSION_MODE mMode;
static FFcodecSetting localSettings;

static void 		updateMode( void );
static int 		cb_mod(GtkObject * object, gpointer user_data);
static void 		upload(GtkWidget *dialog, FFcodecSetting *conf);
static void 		download(GtkWidget *dialog, FFcodecSetting *conf);

typedef struct bufferSize
{
	int num;
	int size;
}bufferSize;

bufferSize myBuffer[3]=
{
	{0,40},
	{1,112},
	{2,224}
};
//____________________________________________

uint8_t DIA_DVDffParam(COMPRES_PARAMS *incoming)
{
	

	int ret;
	FFcodecSetting *conf=(FFcodecSetting *)incoming->extraSettings;
	ADM_assert(incoming->extraSettingsLen==sizeof(FFcodecSetting));
	
	gint r,b;

	dialog=create_dialog1();
	gtk_register_dialog(dialog);
	
	mQ=incoming->qz;
	mB=incoming->bitrate;
	mS=incoming->finalsize;	
	mMode=incoming->mode;

	memcpy(&localSettings,conf,sizeof(localSettings));
	
	updateMode();	
	
	gtk_signal_connect(GTK_OBJECT(WID(optionmenu_mode)), "changed",
                      GTK_SIGNAL_FUNC(cb_mod),                   (void *) 0);
	

	upload(dialog,conf);
	
	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		int r=-1,value=-1;
		
		ret=1;
		download(dialog,conf);	
		r=getRangeInMenu(WID(optionmenu_mode));				
		switch(r)
			{
				case 0:
					incoming->mode = COMPRESS_CBR;				      
		      			value = (uint32_t) gtk_read_entry(WID(entry_bitrate));
		      			if (value < 9900)
			  			value *= 1000;
		      			if (value > 16 && value < 9900000)
					{
			    			incoming->bitrate = value;
			    			ret = 1;
		      			}
					
					break;
				case 1:
					incoming->mode = COMPRESS_CQ;		      			
					value = (uint32_t) gtk_spin_button_get_value_as_int(
								GTK_SPIN_BUTTON(WID(spinbutton_quant)));
		      			if (value >= 2 && value <= 32)
					{
			    			incoming->qz = value;
		      			}
		      			break;

				case 2:
		     			incoming->mode = COMPRESS_2PASS;	
					value = (uint32_t)
						gtk_read_entry(WID(entry_bitrate));
        				if((value>0)&&(value<8200)) // enough for DL DVDs
          				{
       						incoming->finalsize=value;	
           				}
            				break;
		  		default:
		      			ADM_assert(0);
				}
		
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);

	return ret;
}	


void upload(GtkWidget *dialog, FFcodecSetting *conf)
{
 	gtk_option_menu_set_history (GTK_OPTION_MENU (WID(optionmenu_matrix)),conf->user_matrix);
// 	gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton1)),(float)conf->gop_size) ;
	ENTRY_SET(entry_gopsize,gop_size);
	
	if(conf->interlaced)
	{
		if(conf->bff)
			RADIO_SET(radiobutton_bff,1);
		else
			RADIO_SET(radiobutton_tff,1);			
			
	}
	if(conf->widescreen)
		RADIO_SET(radiobutton_ar169,1);			
	gtk_write_entry(WID(entry_maxbitrate),(conf->maxBitrate*8)/1000);
	gtk_write_entry(WID(entry_minbitrate),(conf->minBitrate*8)/1000);
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbutton_xvid)),conf->use_xvid_ratecontrol);
	// Search closer 
	int h;
	int vbv=conf->bufferSize;
	
	printf("incoming vbv:%d\n",vbv);
	if(vbv>(myBuffer[2].size+myBuffer[1].size)/2) h=2;
	else
		if(vbv>(myBuffer[1].size+myBuffer[0].size)/2) h=1;
		else
			h=0;
	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenu1)),h);
	
}
void download(GtkWidget *dialog, FFcodecSetting *conf)
{
	ENTRY_GET(entry_gopsize,gop_size);
	conf->user_matrix=getRangeInMenu(WID(optionmenu_matrix));
	
	
	
	if(RADIO_GET(radiobutton_progr))
	{
		conf->interlaced=0;
		conf->bff=0;
	}
	else if(RADIO_GET(radiobutton_tff))
	{
		conf->interlaced=1;
		conf->bff=0;
	}
	else if(RADIO_GET(radiobutton_bff))
	{
		conf->interlaced=1;
		conf->bff=1;
	}

	if(RADIO_GET(radiobutton_ar169))
	{
		conf->widescreen=1;
	}
	else	conf->widescreen=0;
 	
	conf->maxBitrate=(gtk_read_entry(WID(entry_maxbitrate))/8)*1000;
	conf->minBitrate=(gtk_read_entry(WID(entry_minbitrate))/8)*1000;
	conf->gop_size=(gtk_read_entry(WID(entry_gopsize)));
	conf->use_xvid_ratecontrol=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbutton_xvid)));
	
	conf->bufferSize=myBuffer[getRangeInMenu((WID(optionmenu1)))].size;
}

int cb_mod(GtkObject * object, gpointer user_data)
{
int r;
	r=getRangeInMenu(WID(optionmenu_mode));
	switch(r)
	{
		case 0: mMode=COMPRESS_CBR ;break;
		case 1: mMode=COMPRESS_CQ ;break;
		case 2: mMode=COMPRESS_2PASS ;break;
	
	}
	updateMode();

}

/**

*/	 
void updateMode( void )
{
uint32_t b;
		// set the right select button
 	switch (mMode)
	    {
	    	case COMPRESS_CBR:
			HIST_SET(0);			
			b=mB/1000;
			VAL_SET(b);
			gtk_widget_set_sensitive(WID(spinbutton_quant),0);
			gtk_widget_set_sensitive(WID(entry_bitrate),1);
			gtk_label_set_text(GTK_LABEL(WID(label3)),"Bitrate (kb/s):");
			break;

		case COMPRESS_2PASS:
			HIST_SET(2);
			VAL_SET(mS);
			gtk_widget_set_sensitive(WID(spinbutton_quant),0);
			gtk_widget_set_sensitive(WID(entry_bitrate),1);
			gtk_label_set_text(GTK_LABEL(WID(label3)),"Size (MBytes):");
			break;

	    	case COMPRESS_CQ:
			HIST_SET(1);
			gtk_widget_set_sensitive(WID(entry_bitrate),0);
			gtk_widget_set_sensitive(WID(spinbutton_quant),1);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton_quant)),(gfloat)mQ);
			break;
		}
}

//

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *optionmenu_mode;
  GtkWidget *menu1;
  GtkWidget *single_pass___bitrate1;
  GtkWidget *single_pass___quantizer1;
  GtkWidget *two_pass__1;
  GtkWidget *entry_bitrate;
  GtkObject *spinbutton_quant_adj;
  GtkWidget *spinbutton_quant;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table2;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *entry_minbitrate;
  GtkWidget *entry_maxbitrate;
  GtkWidget *label8;
  GtkWidget *checkbutton_xvid;
  GtkWidget *label14;
  GtkWidget *optionmenu1;
  GtkWidget *menu3;
  GtkWidget *vcd;
  GtkWidget *svcd;
  GtkWidget *dvd;
  GtkWidget *label5;
  GtkWidget *frame6;
  GtkWidget *hbox5;
  GtkWidget *radiobutton_progr;
  GSList *radiobutton_progr_group = NULL;
  GtkWidget *radiobutton_tff;
  GtkWidget *radiobutton_bff;
  GtkWidget *label15;
  GtkWidget *hbox1;
  GtkWidget *frame3;
  GtkWidget *hbox2;
  GtkWidget *radiobutton_ar43;
  GSList *radiobutton_ar43_group = NULL;
  GtkWidget *radiobutton_ar169;
  GtkWidget *label9;
  GtkWidget *frame4;
  GtkWidget *optionmenu_matrix;
  GtkWidget *menu2;
  GtkWidget *default1;
  GtkWidget *tmpgenc1;
  GtkWidget *anime1;
  GtkWidget *kvcd1;
  GtkWidget *hrtmpgenc1;
  GtkWidget *label10;
  GtkWidget *hbox3;
  GtkWidget *label11;
  GtkWidget *entry_gopsize;
  GtkWidget *frame5;
  GtkWidget *hbox4;
  GtkWidget *label13;
  GtkWidget *label_audiosize;
  GtkWidget *label12;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("DVD FFmpeg codec settings"));

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

  label2 = gtk_label_new (_("Encoding Type"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Target Bitrate (kb/s)   "));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Quantizer"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  optionmenu_mode = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_mode);
  gtk_table_attach (GTK_TABLE (table1), optionmenu_mode, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  single_pass___bitrate1 = gtk_menu_item_new_with_mnemonic (_("Single Pass - Bitrate"));
  gtk_widget_show (single_pass___bitrate1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass___bitrate1);

  single_pass___quantizer1 = gtk_menu_item_new_with_mnemonic (_("Single Pass - Quantizer"));
  gtk_widget_show (single_pass___quantizer1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass___quantizer1);

  two_pass__1 = gtk_menu_item_new_with_mnemonic (_("Two Pass  "));
  gtk_widget_show (two_pass__1);
  gtk_container_add (GTK_CONTAINER (menu1), two_pass__1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_mode), menu1);

  entry_bitrate = gtk_entry_new ();
  gtk_widget_show (entry_bitrate);
  gtk_table_attach (GTK_TABLE (table1), entry_bitrate, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  spinbutton_quant_adj = gtk_adjustment_new (2, 2, 32, 1, 10, 10);
  spinbutton_quant = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_quant_adj), 1, 0);
  gtk_widget_show (spinbutton_quant);
  gtk_table_attach (GTK_TABLE (table1), spinbutton_quant, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_quant), TRUE);

  label1 = gtk_label_new (_("<b>Encoding mode </b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  table2 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame2), table2);

  label6 = gtk_label_new (_("Minimum Bitrate (kb/s) "));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table2), label6, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Maximum Bitrate (kb/s)"));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table2), label7, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  entry_minbitrate = gtk_entry_new ();
  gtk_widget_show (entry_minbitrate);
  gtk_table_attach (GTK_TABLE (table2), entry_minbitrate, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry_maxbitrate = gtk_entry_new ();
  gtk_widget_show (entry_maxbitrate);
  gtk_table_attach (GTK_TABLE (table2), entry_maxbitrate, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label8 = gtk_label_new (_("Use Xvid Ratecontrol"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table2), label8, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  checkbutton_xvid = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbutton_xvid);
  gtk_table_attach (GTK_TABLE (table2), checkbutton_xvid, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label14 = gtk_label_new (_("VBV buffer size"));
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table2), label14, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table2), optionmenu1, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu3 = gtk_menu_new ();

  vcd = gtk_menu_item_new_with_mnemonic (_("VCD: 40 kB"));
  gtk_widget_show (vcd);
  gtk_container_add (GTK_CONTAINER (menu3), vcd);

  svcd = gtk_menu_item_new_with_mnemonic (_("SVCD: 112 kB"));
  gtk_widget_show (svcd);
  gtk_container_add (GTK_CONTAINER (menu3), svcd);

  dvd = gtk_menu_item_new_with_mnemonic (_("DVD: 224 kB"));
  gtk_widget_show (dvd);
  gtk_container_add (GTK_CONTAINER (menu3), dvd);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu3);

  label5 = gtk_label_new (_("<b>RateControl</b>"));
  gtk_widget_show (label5);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label5);
  gtk_label_set_use_markup (GTK_LABEL (label5), TRUE);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

  frame6 = gtk_frame_new (NULL);
  gtk_widget_show (frame6);
  gtk_box_pack_start (GTK_BOX (vbox1), frame6, TRUE, TRUE, 0);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (frame6), hbox5);

  radiobutton_progr = gtk_radio_button_new_with_mnemonic (NULL, _("Progressive"));
  gtk_widget_show (radiobutton_progr);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_progr, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_progr), radiobutton_progr_group);
  radiobutton_progr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_progr));

  radiobutton_tff = gtk_radio_button_new_with_mnemonic (NULL, _("Interlaced TFF"));
  gtk_widget_show (radiobutton_tff);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_tff, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_tff), radiobutton_progr_group);
  radiobutton_progr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_tff));

  radiobutton_bff = gtk_radio_button_new_with_mnemonic (NULL, _("Interlaced BFF"));
  gtk_widget_show (radiobutton_bff);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton_bff, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_bff), radiobutton_progr_group);
  radiobutton_progr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_bff));

  label15 = gtk_label_new (_("<b>Interlacing</b>"));
  gtk_widget_show (label15);
  gtk_frame_set_label_widget (GTK_FRAME (frame6), label15);
  gtk_label_set_use_markup (GTK_LABEL (label15), TRUE);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (hbox1), frame3, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (frame3), hbox2);

  radiobutton_ar43 = gtk_radio_button_new_with_mnemonic (NULL, _("4:3"));
  gtk_widget_show (radiobutton_ar43);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobutton_ar43, TRUE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_ar43), radiobutton_ar43_group);
  radiobutton_ar43_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_ar43));

  radiobutton_ar169 = gtk_radio_button_new_with_mnemonic (NULL, _("16:9"));
  gtk_widget_show (radiobutton_ar169);
  gtk_box_pack_start (GTK_BOX (hbox2), radiobutton_ar169, TRUE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_ar169), radiobutton_ar43_group);
  radiobutton_ar43_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_ar169));

  label9 = gtk_label_new (_("<b>Aspect Ratio</b>"));
  gtk_widget_show (label9);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label9);
  gtk_label_set_use_markup (GTK_LABEL (label9), TRUE);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (hbox1), frame4, TRUE, TRUE, 0);

  optionmenu_matrix = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_matrix);
  gtk_container_add (GTK_CONTAINER (frame4), optionmenu_matrix);

  menu2 = gtk_menu_new ();

  default1 = gtk_menu_item_new_with_mnemonic (_("Default"));
  gtk_widget_show (default1);
  gtk_container_add (GTK_CONTAINER (menu2), default1);

  tmpgenc1 = gtk_menu_item_new_with_mnemonic (_("Tmpgenc"));
  gtk_widget_show (tmpgenc1);
  gtk_container_add (GTK_CONTAINER (menu2), tmpgenc1);

  anime1 = gtk_menu_item_new_with_mnemonic (_("Anime"));
  gtk_widget_show (anime1);
  gtk_container_add (GTK_CONTAINER (menu2), anime1);

  kvcd1 = gtk_menu_item_new_with_mnemonic (_("KVCD"));
  gtk_widget_show (kvcd1);
  gtk_container_add (GTK_CONTAINER (menu2), kvcd1);

  hrtmpgenc1 = gtk_menu_item_new_with_mnemonic (_("HR-Tmpgenc"));
  gtk_widget_show (hrtmpgenc1);
  gtk_container_add (GTK_CONTAINER (menu2), hrtmpgenc1);
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_matrix), menu2);

  label10 = gtk_label_new (_("<b>Matrices</b>"));
  gtk_widget_show (label10);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label10);
  gtk_label_set_use_markup (GTK_LABEL (label10), TRUE);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, FALSE, 0);

  label11 = gtk_label_new (_("Gop Size:"));
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (hbox3), label11, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);

  entry_gopsize = gtk_entry_new ();
  gtk_widget_show (entry_gopsize);
  gtk_box_pack_start (GTK_BOX (hbox3), entry_gopsize, TRUE, TRUE, 0);

  frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox1), frame5, FALSE, FALSE, 0);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (frame5), hbox4);

  label13 = gtk_label_new (_("For info, size is : "));
  gtk_widget_show (label13);
  gtk_box_pack_start (GTK_BOX (hbox4), label13, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  label_audiosize = gtk_label_new (_("0"));
  gtk_widget_show (label_audiosize);
  gtk_box_pack_start (GTK_BOX (hbox4), label_audiosize, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label_audiosize), GTK_JUSTIFY_LEFT);

  label12 = gtk_label_new (_("<b>Indicative audio size</b>"));
  gtk_widget_show (label12);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label12);
  gtk_label_set_use_markup (GTK_LABEL (label12), TRUE);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu_mode, "optionmenu_mode");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass___bitrate1, "single_pass___bitrate1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass___quantizer1, "single_pass___quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, two_pass__1, "two_pass__1");
  GLADE_HOOKUP_OBJECT (dialog1, entry_bitrate, "entry_bitrate");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton_quant, "spinbutton_quant");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, entry_minbitrate, "entry_minbitrate");
  GLADE_HOOKUP_OBJECT (dialog1, entry_maxbitrate, "entry_maxbitrate");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton_xvid, "checkbutton_xvid");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu3, "menu3");
  GLADE_HOOKUP_OBJECT (dialog1, vcd, "vcd");
  GLADE_HOOKUP_OBJECT (dialog1, svcd, "svcd");
  GLADE_HOOKUP_OBJECT (dialog1, dvd, "dvd");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, frame6, "frame6");
  GLADE_HOOKUP_OBJECT (dialog1, hbox5, "hbox5");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_progr, "radiobutton_progr");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_tff, "radiobutton_tff");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_bff, "radiobutton_bff");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_ar43, "radiobutton_ar43");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton_ar169, "radiobutton_ar169");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, frame4, "frame4");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu_matrix, "optionmenu_matrix");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, default1, "default1");
  GLADE_HOOKUP_OBJECT (dialog1, tmpgenc1, "tmpgenc1");
  GLADE_HOOKUP_OBJECT (dialog1, anime1, "anime1");
  GLADE_HOOKUP_OBJECT (dialog1, kvcd1, "kvcd1");
  GLADE_HOOKUP_OBJECT (dialog1, hrtmpgenc1, "hrtmpgenc1");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, entry_gopsize, "entry_gopsize");
  GLADE_HOOKUP_OBJECT (dialog1, frame5, "frame5");
  GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog1, label_audiosize, "label_audiosize");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

