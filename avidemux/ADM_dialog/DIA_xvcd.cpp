#include <config.h>
#include "ADM_lavcodec.h"


#include "ADM_assert.h" 

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>
# include <config.h>
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


uint8_t DIA_XVCDParam(char *title,COMPRESSION_MODE * mode, uint32_t * qz,
		   				   uint32_t * br,uint32_t *fsize,FFcodecSetting *conf)
{
	GtkWidget *dialog;

	gchar *str;
	char string[200];
	int ret;

	gint r,b;



#define WID(x) lookup_widget(dialog,#x)
#define CHECK_GET(x,y) {y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),y);}		
	ret=-1;
	while(ret==-1)
	{
	
	dialog=create_dialog1();
	gtk_transient(dialog);	
  	gtk_window_set_title (GTK_WINDOW (dialog), title);
	// set the right select button
 switch (*mode)
	    {
	    	case COMPRESS_CBR:

			RADIO_SET(radioCBR,1);
			b=*br/1000;
			gtk_write_entry(WID(entryCBR),(int)b);
			break;

		case COMPRESS_2PASS:
			RADIO_SET(radio2Pass,1);
			gtk_write_entry(WID(entry2Pass),(int)*fsize);
			break;

	    	case COMPRESS_CQ:
			RADIO_SET(radioCQ,1);
			gtk_write_entry(WID(entryCQ),(int)*qz);

			break;
		}
	gtk_write_entry(WID(entry_min),(int) (conf->minBitrate *8)/1000);
	gtk_write_entry(WID(entry_max),(int) (conf->maxBitrate *8)/1000);
 	gtk_option_menu_set_history (GTK_OPTION_MENU (WID(optionmenu1)),conf->user_matrix);
	gtk_option_menu_set_history (GTK_OPTION_MENU (WID(optionmenu1)),conf->user_matrix);
 	gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton1)),(float)conf->gop_size) ;
	CHECK_SET(checkbuttonxvid,conf->use_xvid_ratecontrol);
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
			int r,value=0;
				ret=1;
    				conf->minBitrate=(gtk_read_entry(WID(entry_min))*1000)>>3;
				conf->maxBitrate=(gtk_read_entry(WID(entry_max))*1000)>>3;
				conf->user_matrix= getRangeInMenu(WID(optionmenu1));
				conf->gop_size= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(spinbutton1)));
				CHECK_GET(checkbuttonxvid,conf->use_xvid_ratecontrol);
				r=RADIO_GET(radioCQ)+(2*RADIO_GET(radioCBR))+(4*RADIO_GET(radio2Pass));
				printf("\n r: %d \n",r);
				switch(r)
				{
				case 2:
					*mode = COMPRESS_CBR;
				      	value =		 gtk_read_entry(WID(entryCBR));

		      			if (value < 9000)
					{
			  			value *= 1000;
					}
		      			if (value > 16 && value < 9000000)
					{
			    			*br = value;
			    			ret = 1;
		      			} else
			  		ret = -1;
		      			break;
				case 1:
					*mode = COMPRESS_CQ;
		      			value =  gtk_read_entry(WID(entryCQ));
		      			if (value >= 2 && value <= 32)
						{
			    			*qz = value;
			    			ret = 1;
		      				}
		      			else
						{
							printf("\n incorrect Q parameter\n");
				    		ret =-1;
						}
		  	  	  	break;

				case 4:
		     				*mode = COMPRESS_2PASS;		       				
		      				value = gtk_read_entry(WID(entry2Pass));
        					if((value>0)&&(value<4000))
          					{
       							*fsize=value;
				      			ret=1;
           					}
            					else
             						ret=-1;
         					break;
		  		default:
		      				ADM_assert(0);
				}
	}
	else
	ret=0;

	gtk_widget_destroy(dialog);
	}
	return ret;

}

//---

GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *radioCQ;
  GSList *radioCQ_group = NULL;
  GtkWidget *entryCQ;
  GtkWidget *radioCBR;
  GtkWidget *entryCBR;
  GtkWidget *radio2Pass;
  GtkWidget *entry2Pass;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *entry_min;
  GtkWidget *entry_max;
  GtkWidget *label3;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *default_mpeg1;
  GtkWidget *tmpeng_1;
  GtkWidget *anim__1;
  GtkWidget *k_vcd1;
  GtkWidget *label4;
  GtkObject *spinbutton1_adj;
  GtkWidget *spinbutton1;
  GtkWidget *label5;
  GtkWidget *checkbuttonxvid;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("XVCD encoding"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (8, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, TRUE, TRUE, 0);

  radioCQ = gtk_radio_button_new_with_mnemonic (NULL, _("Constant Quantizer"));
  gtk_widget_show (radioCQ);
  gtk_table_attach (GTK_TABLE (table1), radioCQ, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioCQ), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioCQ));

  entryCQ = gtk_entry_new ();
  gtk_widget_show (entryCQ);
  gtk_table_attach (GTK_TABLE (table1), entryCQ, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  radioCBR = gtk_radio_button_new_with_mnemonic (NULL, _("Constant Bitrate"));
  gtk_widget_show (radioCBR);
  gtk_table_attach (GTK_TABLE (table1), radioCBR, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioCBR), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioCBR));

  entryCBR = gtk_entry_new ();
  gtk_widget_show (entryCBR);
  gtk_table_attach (GTK_TABLE (table1), entryCBR, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  radio2Pass = gtk_radio_button_new_with_mnemonic (NULL, _("Dual pass"));
  gtk_widget_show (radio2Pass);
  gtk_table_attach (GTK_TABLE (table1), radio2Pass, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radio2Pass), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio2Pass));

  entry2Pass = gtk_entry_new ();
  gtk_widget_show (entry2Pass);
  gtk_table_attach (GTK_TABLE (table1), entry2Pass, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label1 = gtk_label_new (_("Min bitrate (kbps) :"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Max bitrate (kbps) :"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  entry_min = gtk_entry_new ();
  gtk_widget_show (entry_min);
  gtk_table_attach (GTK_TABLE (table1), entry_min, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry_max = gtk_entry_new ();
  gtk_widget_show (entry_max);
  gtk_table_attach (GTK_TABLE (table1), entry_max, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label3 = gtk_label_new (_("Matrices"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  default_mpeg1 = gtk_menu_item_new_with_mnemonic (_("Default mpeg"));
  gtk_widget_show (default_mpeg1);
  gtk_container_add (GTK_CONTAINER (menu1), default_mpeg1);

  tmpeng_1 = gtk_menu_item_new_with_mnemonic (_("Tmpeng "));
  gtk_widget_show (tmpeng_1);
  gtk_container_add (GTK_CONTAINER (menu1), tmpeng_1);

  anim__1 = gtk_menu_item_new_with_mnemonic (_("Anim\303\251"));
  gtk_widget_show (anim__1);
  gtk_container_add (GTK_CONTAINER (menu1), anim__1);

  k_vcd1 = gtk_menu_item_new_with_mnemonic (_("K-VCD"));
  gtk_widget_show (k_vcd1);
  gtk_container_add (GTK_CONTAINER (menu1), k_vcd1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  label4 = gtk_label_new (_("Gop Size  (12):"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  spinbutton1_adj = gtk_adjustment_new (12, 1, 24, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_show (spinbutton1);
  gtk_table_attach (GTK_TABLE (table1), spinbutton1, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);

  label5 = gtk_label_new (_("Use Xvid ratecontrol"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  checkbuttonxvid = gtk_check_button_new_with_mnemonic ("");
  gtk_widget_show (checkbuttonxvid);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonxvid, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
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
  GLADE_HOOKUP_OBJECT (dialog1, radioCQ, "radioCQ");
  GLADE_HOOKUP_OBJECT (dialog1, entryCQ, "entryCQ");
  GLADE_HOOKUP_OBJECT (dialog1, radioCBR, "radioCBR");
  GLADE_HOOKUP_OBJECT (dialog1, entryCBR, "entryCBR");
  GLADE_HOOKUP_OBJECT (dialog1, radio2Pass, "radio2Pass");
  GLADE_HOOKUP_OBJECT (dialog1, entry2Pass, "entry2Pass");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, entry_min, "entry_min");
  GLADE_HOOKUP_OBJECT (dialog1, entry_max, "entry_max");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, default_mpeg1, "default_mpeg1");
  GLADE_HOOKUP_OBJECT (dialog1, tmpeng_1, "tmpeng_1");
  GLADE_HOOKUP_OBJECT (dialog1, anim__1, "anim__1");
  GLADE_HOOKUP_OBJECT (dialog1, k_vcd1, "k_vcd1");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton1, "spinbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonxvid, "checkbuttonxvid");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}

