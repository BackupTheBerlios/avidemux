//
//
// C++ Implementation: DIA_SVCD
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <config.h>
#include "ADM_lavcodec.h"


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

#include "callbacks.h"
#include "avi_vars.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"


#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_codecs/ADM_ffmpegConfig.h"
#include "ADM_toolkit/toolkit.hxx"

static GtkWidget	*create_dialog1 (void);
static void 		updateMode( void );
static int 		cb_mod(GtkObject * object, gpointer user_data);
static void 		upload(GtkWidget *dialog, FFcodecSetting *conf);
static void 		download(GtkWidget *dialog, FFcodecSetting *conf);

static GtkWidget 	*dialog=NULL;		      
static uint32_t 	mQ,mB,mS;
static COMPRESSION_MODE mMode;


uint8_t DIA_SVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   	  uint32_t * br,uint32_t *fsize,FFcodecSetting *conf)
{	
	int ret=0;
	gint b;



#define WID(x) lookup_widget(dialog,#x)


#define SPIN_GETF(x,y) {conf->y= gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(x))) ;\
				printf(#x":%d\n",conf->y);}
#define SPIN_GET(x,y) {conf->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(x))) ;\
				printf(#x":%d\n",conf->y);}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(x)),(gfloat)conf->y) ; \
				printf(#x":%d\n",conf->y);}

#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),conf->y);}
#define MENU_GET(x,y) { conf->y	= getRangeInMenu(WID(x));}
#define TOGGLE_SET(x,y) {gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WID(x)),conf->y);}
#define TOGGLE_GET(x,y) {conf->y=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(WID(x)));\
				printf(#y":%d\n",conf->y);}
				
#define ENTRY_SET(x,y) {gtk_write_entry(WID(x),(int)conf->y);}
#define ENTRY_GET(x,y) {localParam.y=gtk_read_entry(WID(x));\
				printf(#y":%d\n",conf->y);}

#define CHECK_GET(x,y) {conf->y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),conf->y);}	

#define HIST_SET(x) gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenuType)), x)
#define VAL_SET(x) gtk_write_entry(WID(entryBitrate), x)


	dialog=create_dialog1();
	gtk_transient(dialog);
  	gtk_window_set_title (GTK_WINDOW (dialog), title);
	
	mQ=*qz;
	mB=*br;
	mS=*fsize;	
	mMode=*mode;

	updateMode();	
	
	gtk_signal_connect(GTK_OBJECT(WID(optionmenuType)), "changed",
                      GTK_SIGNAL_FUNC(cb_mod),                   (void *) 0);
	

	upload(dialog,conf);
	
	ret=0;
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		int r=-1,value=-1;
		
		ret=1;
		download(dialog,conf);	
		r=getRangeInMenu(WID(optionmenuType));				
		switch(r)
			{
				case 0:
					*mode = COMPRESS_CBR;				      
		      			value = (uint32_t) gtk_read_entry(WID(entryBitrate));
		      			if (value < 3000)
			  			value *= 1000;
		      			if (value > 16 && value < 6000000)
					{
			    			*br = value;
			    			ret = 1;
		      			}
					
					break;
				case 1:
					*mode = COMPRESS_CQ;		      			
					value = (uint32_t) gtk_spin_button_get_value_as_int(
								GTK_SPIN_BUTTON(WID(spinbuttonQz)));
		      			if (value >= 2 && value <= 32)
					{
			    			*qz = value;
		      			}
		      			break;

				case 2:
		     			*mode = COMPRESS_2PASS;	
					value = (uint32_t)
						gtk_read_entry(WID(entryBitrate));
        				if((value>0)&&(value<4000))
          				{
       						*fsize=value;	
           				}
            				break;
		  		default:
		      			assert(0);
				}
		
	}
	gtk_widget_destroy(dialog);

	return ret;
}	


void upload(GtkWidget *dialog, FFcodecSetting *conf)
{
 	gtk_option_menu_set_history (GTK_OPTION_MENU (WID(optionmenu2)),conf->user_matrix);
// 	gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton1)),(float)conf->gop_size) ;
	SPIN_SET(spinbuttonGop,gop_size);
	if(conf->interlaced)
	{
		if(conf->bff)
			RADIO_SET(radiobutton3,1);
		else
			RADIO_SET(radiobutton2,1);			
			
	}
	if(conf->widescreen)
		RADIO_SET(radiobutton5,1);			
	gtk_write_entry(WID(entryMax),(conf->maxBitrate*8)/1000);
	
}
void download(GtkWidget *dialog, FFcodecSetting *conf)
{
	SPIN_GET(spinbuttonGop,gop_size);
	conf->user_matrix=getRangeInMenu(WID(optionmenu2));
	
	
	if(RADIO_GET(radiobutton1))
		{
			conf->interlaced=0;
			conf->bff=0;
		}
	else if(RADIO_GET(radiobutton2))
		{
			conf->interlaced=1;
			conf->bff=0;
		}
	else if(RADIO_GET(radiobutton3))
		{
			conf->interlaced=1;
			conf->bff=1;
		}
	if(RADIO_GET(radiobutton5))
	{
		conf->widescreen=1;
	}
	else	conf->widescreen=0;
 	
	conf->maxBitrate=(gtk_read_entry(WID(entryMax))/8)*1000;
}

int cb_mod(GtkObject * object, gpointer user_data)
{
int r;
	r=getRangeInMenu(WID(optionmenuType));
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
			gtk_widget_set_sensitive(WID(spinbuttonQz),0);
			gtk_widget_set_sensitive(WID(entryBitrate),1);
			gtk_label_set_text(GTK_LABEL(WID(labelBitrate)),"Bitrate (kb/s):");
			break;

		case COMPRESS_2PASS:
			HIST_SET(2);
			VAL_SET(mS);
			gtk_widget_set_sensitive(WID(spinbuttonQz),0);
			gtk_widget_set_sensitive(WID(entryBitrate),1);
			gtk_label_set_text(GTK_LABEL(WID(labelBitrate)),"Size (MBytes):");
			break;

	    	case COMPRESS_CQ:
			HIST_SET(1);
			gtk_widget_set_sensitive(WID(entryBitrate),0);
			gtk_widget_set_sensitive(WID(spinbuttonQz),1);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbuttonQz)),(gfloat)mQ);
			break;
		}
}


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *label2;
  GtkWidget *optionmenuType;
  GtkWidget *menu1;
  GtkWidget *single_pass___constant_quantizer1;
  GtkWidget *single_pass___constant_bitrate1;
  GtkWidget *two_pass1;
  GtkWidget *hbox2;
  GtkWidget *labelBitrate;
  GtkWidget *entryBitrate;
  GtkWidget *hbox3;
  GtkWidget *label4;
  GtkObject *spinbuttonQz_adj;
  GtkWidget *spinbuttonQz;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *hbox4;
  GtkWidget *radiobutton1;
  GSList *radiobutton1_group = NULL;
  GtkWidget *radiobutton2;
  GtkWidget *radiobutton3;
  GtkWidget *label5;
  GtkWidget *hbox5;
  GtkWidget *frame3;
  GtkWidget *hbox6;
  GtkWidget *radiobutton4;
  GSList *radiobutton4_group = NULL;
  GtkWidget *radiobutton5;
  GtkWidget *label6;
  GtkWidget *frame4;
  GtkWidget *optionmenu2;
  GtkWidget *menu2;
  GtkWidget *default1;
  GtkWidget *tmpgenc1;
  GtkWidget *anime1;
  GtkWidget *kvcd1;
  GtkWidget *label7;
  GtkWidget *hbox7;
  GtkWidget *label8;
  GtkObject *spinbuttonGop_adj;
  GtkWidget *spinbuttonGop;
  GtkWidget *hbox8;
  GtkWidget *label9;
  GtkWidget *entryMax;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("DVD Parameters"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame1), vbox2);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  label2 = gtk_label_new (_("Encoding Type:"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label2, 100, -1);

  optionmenuType = gtk_option_menu_new ();
  gtk_widget_show (optionmenuType);
  gtk_box_pack_start (GTK_BOX (hbox1), optionmenuType, FALSE, FALSE, 0);

  menu1 = gtk_menu_new ();

  single_pass___constant_quantizer1 = gtk_menu_item_new_with_mnemonic (_("Single pass:Bitrate"));
  gtk_widget_show (single_pass___constant_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass___constant_quantizer1);

  single_pass___constant_bitrate1 = gtk_menu_item_new_with_mnemonic (_("Single pass:Quantizer"));
  gtk_widget_show (single_pass___constant_bitrate1);
  gtk_container_add (GTK_CONTAINER (menu1), single_pass___constant_bitrate1);

  two_pass1 = gtk_menu_item_new_with_mnemonic (_("Two pass"));
  gtk_widget_show (two_pass1);
  gtk_container_add (GTK_CONTAINER (menu1), two_pass1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuType), menu1);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, TRUE, TRUE, 0);

  labelBitrate = gtk_label_new (_("Bitrate (kb/s)"));
  gtk_widget_show (labelBitrate);
  gtk_box_pack_start (GTK_BOX (hbox2), labelBitrate, FALSE, FALSE, 0);
  gtk_widget_set_size_request (labelBitrate, 100, -1);

  entryBitrate = gtk_entry_new ();
  gtk_widget_show (entryBitrate);
  gtk_box_pack_start (GTK_BOX (hbox2), entryBitrate, FALSE, TRUE, 0);
  gtk_widget_set_size_request (entryBitrate, 100, -1);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox3, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("Quantizer:"));
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox3), label4, FALSE, TRUE, 0);
  gtk_widget_set_size_request (label4, 100, -1);

  spinbuttonQz_adj = gtk_adjustment_new (4, 2, 31, 1, 10, 10);
  spinbuttonQz = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQz_adj), 1, 0);
  gtk_widget_show (spinbuttonQz);
  gtk_box_pack_start (GTK_BOX (hbox3), spinbuttonQz, FALSE, TRUE, 0);
  gtk_widget_set_size_request (spinbuttonQz, 100, -1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQz), TRUE);

  label1 = gtk_label_new (_("Encoding mode"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (frame2), hbox4);

  radiobutton1 = gtk_radio_button_new_with_mnemonic (NULL, _("Progressive"));
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton1, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton1), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton1));

  radiobutton2 = gtk_radio_button_new_with_mnemonic (NULL, _("Interlaced TFF"));
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton2, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton2), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton2));

  radiobutton3 = gtk_radio_button_new_with_mnemonic (NULL, _("Interlaced BFF"));
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton3, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton3), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton3));

  label5 = gtk_label_new (_("Interlacing"));
  gtk_widget_show (label5);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label5);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox5, TRUE, TRUE, 0);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (hbox5), frame3, FALSE, TRUE, 0);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox6);
  gtk_container_add (GTK_CONTAINER (frame3), hbox6);

  radiobutton4 = gtk_radio_button_new_with_mnemonic (NULL, _("4:3"));
  gtk_widget_show (radiobutton4);
  gtk_box_pack_start (GTK_BOX (hbox6), radiobutton4, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton4), radiobutton4_group);
  radiobutton4_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton4));

  radiobutton5 = gtk_radio_button_new_with_mnemonic (NULL, _("16:9"));
  gtk_widget_show (radiobutton5);
  gtk_box_pack_start (GTK_BOX (hbox6), radiobutton5, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton5), radiobutton4_group);
  radiobutton4_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton5));

  label6 = gtk_label_new (_("Aspect Ratio"));
  gtk_widget_show (label6);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label6);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (hbox5), frame4, TRUE, TRUE, 0);

  optionmenu2 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu2);
  gtk_container_add (GTK_CONTAINER (frame4), optionmenu2);

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

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu2), menu2);

  label7 = gtk_label_new (_("Matrix"));
  gtk_widget_show (label7);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label7);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox7, TRUE, TRUE, 0);

  label8 = gtk_label_new (_("Gop Size:"));
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (hbox7), label8, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label8, 100, -1);

  spinbuttonGop_adj = gtk_adjustment_new (1, 1, 25, 1, 10, 10);
  spinbuttonGop = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonGop_adj), 1, 0);
  gtk_widget_show (spinbuttonGop);
  gtk_box_pack_start (GTK_BOX (hbox7), spinbuttonGop, FALSE, FALSE, 0);
  gtk_widget_set_size_request (spinbuttonGop, 100, -1);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonGop), TRUE);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox8, TRUE, TRUE, 0);

  label9 = gtk_label_new (_("Max Bitrate:"));
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox8), label9, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label9, 100, -1);

  entryMax = gtk_entry_new ();
  gtk_widget_show (entryMax);
  gtk_box_pack_start (GTK_BOX (hbox8), entryMax, FALSE, TRUE, 0);
  gtk_widget_set_size_request (entryMax, 100, -1);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuType, "optionmenuType");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass___constant_quantizer1, "single_pass___constant_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, single_pass___constant_bitrate1, "single_pass___constant_bitrate1");
  GLADE_HOOKUP_OBJECT (dialog1, two_pass1, "two_pass1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, labelBitrate, "labelBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, entryBitrate, "entryBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQz, "spinbuttonQz");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton1, "radiobutton1");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton2, "radiobutton2");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton3, "radiobutton3");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, hbox5, "hbox5");
  GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (dialog1, hbox6, "hbox6");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton4, "radiobutton4");
  GLADE_HOOKUP_OBJECT (dialog1, radiobutton5, "radiobutton5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, frame4, "frame4");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu2, "optionmenu2");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, default1, "default1");
  GLADE_HOOKUP_OBJECT (dialog1, tmpgenc1, "tmpgenc1");
  GLADE_HOOKUP_OBJECT (dialog1, anime1, "anime1");
  GLADE_HOOKUP_OBJECT (dialog1, kvcd1, "kvcd1");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, hbox7, "hbox7");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonGop, "spinbuttonGop");
  GLADE_HOOKUP_OBJECT (dialog1, hbox8, "hbox8");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, entryMax, "entryMax");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

