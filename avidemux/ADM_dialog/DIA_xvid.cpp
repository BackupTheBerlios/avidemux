#include <config.h>
#include "ADM_lavcodec.h"



#include <string.h>
#include <stdio.h>

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
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_library/default.h"
#include "ADM_assert.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_gui/GUI_xvidparam.h"


#ifdef USE_XX_XVID 
#include "xvid.h"

#define FILL_ENTRY(widget_name,value) 		{sprintf(string,"%ld",value);r=-1;   \
gtk_editable_delete_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), 0,-1);\
gtk_editable_insert_text(GTK_EDITABLE(lookup_widget(dialog,#widget_name)), string, strlen(string), &r);}


#define WID(x) lookup_widget(dialog,#x)

#define SPIN_GET(x,y) {myParam->y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(dialog,#x))) ;\
				printf(#x":%d\n",myParam->y);}

#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(dialog,#x)),(gfloat)myParam->y) ; \
				printf(#x":%d\n",myParam->y);}

#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),myParam->y);}

static GtkWidget	*create_dialog1 (void);
static void xvidFill(GtkWidget *dialog,xvidEncParam *myparam);
static void xvidRead(GtkWidget *dialog,xvidEncParam *myParam);

int  DIA_getXvidCompressParams(COMPRESSION_MODE * mode, uint32_t * qz,
		      uint32_t * br,uint32_t *fsize,xvidEncParam *param)
{
	GtkWidget *dialog;

	gchar *str;
	char string[200];
	int ret=0;

	gint r,b;

	dialog=create_dialog1();
	gtk_transient(dialog);
	// set the right select button
 	switch (*mode)
	    {
	    	case COMPRESS_CBR:

			RADIO_SET(radioCBR,1);
			b=*br/1000;
			FILL_ENTRY(entryCBR,(uint32_t)b);
			break;

		case COMPRESS_2PASS:
			RADIO_SET(radio2Pass,1);
			FILL_ENTRY(entry2Pass, *fsize);
			break;

	    	case COMPRESS_CQ:
			RADIO_SET(radioCQ,1);
			FILL_ENTRY(entryCQ, *qz);

			break;
		}

		xvidFill(dialog,param);

	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{

		xvidRead(dialog,param);
		int r,value=0;
		ret=1;
		r=RADIO_GET(radioCQ)+(2*RADIO_GET(radioCBR))+(4*RADIO_GET(radio2Pass));

		switch(r)
			{
				case 2:
					*mode = COMPRESS_CBR;
				      str =		  gtk_editable_get_chars(GTK_EDITABLE (WID(entryCBR)), 0, -1);
		      			value = (uint32_t) atoi(str);
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
		      			str =	  gtk_editable_get_chars(GTK_EDITABLE(WID(entryCQ)), 0,						 -1);
		      			value = (uint32_t) atoi(str);
		      			if (value >= 2 && value <= 32)
						{
			    			*qz = value;
		      				}
		      			break;

				case 4:
		     				*mode = COMPRESS_2PASS;
		       				str=gtk_editable_get_chars(GTK_EDITABLE(WID(entry2Pass)), 0,	 -1);
		      				value = (uint32_t) atoi(str);
        					if((value>0)&&(value<4000))
          					{
       							*fsize=value;
				      			ret=1;
           					}
            					break;
		  		default:
		      				ADM_assert(0);
				}
	}
	gtk_widget_destroy(dialog);
	return ret;

}
static 					uint32_t  stQuant[2]={XVID_H263QUANT,XVID_MPEGQUANT};

/*

*/
void xvidFill(GtkWidget *dialog,xvidEncParam *myParam)
{

	 SPIN_SET(spinImin,imin);
	 SPIN_SET(spinImax,imax);
	 SPIN_SET(spinPmin,pmin);
	 SPIN_SET(spinPmax,pmax);

	 SPIN_SET(spinbuttonMaxIFrame,max_key_interval);
	 SPIN_SET(spinbuttonMinIFrame,min_key_interval);

	 if(myParam->quantizer==stQuant[0])
	 	gtk_option_menu_set_history (GTK_OPTION_MENU(WID(optionQzer)),0);
	else
		gtk_option_menu_set_history (GTK_OPTION_MENU(WID(optionQzer)),1);

	MENU_SET(optionME,gui_option);

}
void xvidRead(GtkWidget *dialog,xvidEncParam *myParam)
{

	 SPIN_GET(spinImin,imin);
	 SPIN_GET(spinImax,imax);
	 SPIN_GET(spinPmin,pmin);
	 SPIN_GET(spinPmax,pmax);

	 SPIN_GET(spinbuttonMaxIFrame,max_key_interval);
	 SPIN_GET(spinbuttonMinIFrame,min_key_interval);

	myParam->quantizer	= stQuant[getRangeInMenu(WID(optionQzer))];
	myParam->gui_option	= getRangeInMenu(WID(optionME));

}
GtkWidget *create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *table1;
  GtkWidget *radioCQ;
  GSList *radioCQ_group = NULL;
  GtkWidget *radioCBR;
  GtkWidget *radio2Pass;
  GtkWidget *entryCQ;
  GtkWidget *entryCBR;
  GtkWidget *entry2Pass;
  GtkWidget *label1;
  GtkWidget *table2;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkObject *spinbuttonMaxIFrame_adj;
  GtkWidget *spinbuttonMaxIFrame;
  GtkWidget *label8;
  GtkObject *spinbuttonMinIFrame_adj;
  GtkWidget *spinbuttonMinIFrame;
  GtkWidget *optionME;
  GtkWidget *menu4;
  GtkWidget *_0___none1;
  GtkWidget *_1__very_low1;
  GtkWidget *_2__low1;
  GtkWidget *_3__medium1;
  GtkWidget *_4__high1;
  GtkWidget *_5__very_high1;
  GtkWidget *_6__ultra_high1;
  GtkWidget *optionQzer;
  GtkWidget *menu5;
  GtkWidget *h263_quantizer1;
  GtkWidget *mpeg_quantizer1;
  GtkWidget *label2;
  GtkWidget *table3;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkObject *spinImin_adj;
  GtkWidget *spinImin;
  GtkObject *spinImax_adj;
  GtkWidget *spinImax;
  GtkObject *spinPmin_adj;
  GtkWidget *spinPmin;
  GtkObject *spinPmax_adj;
  GtkWidget *spinPmax;
  GtkWidget *label3;
  GtkWidget *label13;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Xvid Encoder"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (notebook1), table1);

  radioCQ = gtk_radio_button_new_with_mnemonic (NULL, _("Constant Quantizer"));
  gtk_widget_show (radioCQ);
  gtk_table_attach (GTK_TABLE (table1), radioCQ, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioCQ), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioCQ));

  radioCBR = gtk_radio_button_new_with_mnemonic (NULL, _("Constant Bitrate (kbps)"));
  gtk_widget_show (radioCBR);
  gtk_table_attach (GTK_TABLE (table1), radioCBR, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radioCBR), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radioCBR));

  radio2Pass = gtk_radio_button_new_with_mnemonic (NULL, _("Dual pass (MBytes)"));
  gtk_widget_show (radio2Pass);
  gtk_table_attach (GTK_TABLE (table1), radio2Pass, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radio2Pass), radioCQ_group);
  radioCQ_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio2Pass));

  entryCQ = gtk_entry_new ();
  gtk_widget_show (entryCQ);
  gtk_table_attach (GTK_TABLE (table1), entryCQ, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryCBR = gtk_entry_new ();
  gtk_widget_show (entryCBR);
  gtk_table_attach (GTK_TABLE (table1), entryCBR, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry2Pass = gtk_entry_new ();
  gtk_widget_show (entry2Pass);
  gtk_table_attach (GTK_TABLE (table1), entry2Pass, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label1 = gtk_label_new (_("Basic"));
  gtk_widget_show (label1);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

  table2 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (notebook1), table2);

  label5 = gtk_label_new (_("Motion Search"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table2), label5, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new (_("Quantization"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table2), label6, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new (_("Max I frame interval "));
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table2), label7, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  spinbuttonMaxIFrame_adj = gtk_adjustment_new (300, 0, 600, 1, 10, 10);
  spinbuttonMaxIFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxIFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxIFrame);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonMaxIFrame, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxIFrame), TRUE);

  label8 = gtk_label_new (_("Min I frame interval"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table2), label8, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  spinbuttonMinIFrame_adj = gtk_adjustment_new (1, 0, 600, 1, 10, 10);
  spinbuttonMinIFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinIFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonMinIFrame);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonMinIFrame, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMinIFrame), TRUE);

  optionME = gtk_option_menu_new ();
  gtk_widget_show (optionME);
  gtk_table_attach (GTK_TABLE (table2), optionME, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu4 = gtk_menu_new ();

  _0___none1 = gtk_menu_item_new_with_mnemonic (_("0 - None"));
  gtk_widget_show (_0___none1);
  gtk_container_add (GTK_CONTAINER (menu4), _0___none1);

  _1__very_low1 = gtk_menu_item_new_with_mnemonic (_("1- Very low"));
  gtk_widget_show (_1__very_low1);
  gtk_container_add (GTK_CONTAINER (menu4), _1__very_low1);

  _2__low1 = gtk_menu_item_new_with_mnemonic (_("2- Low"));
  gtk_widget_show (_2__low1);
  gtk_container_add (GTK_CONTAINER (menu4), _2__low1);

  _3__medium1 = gtk_menu_item_new_with_mnemonic (_("3- Medium"));
  gtk_widget_show (_3__medium1);
  gtk_container_add (GTK_CONTAINER (menu4), _3__medium1);

  _4__high1 = gtk_menu_item_new_with_mnemonic (_("4- High"));
  gtk_widget_show (_4__high1);
  gtk_container_add (GTK_CONTAINER (menu4), _4__high1);

  _5__very_high1 = gtk_menu_item_new_with_mnemonic (_("5- Very High"));
  gtk_widget_show (_5__very_high1);
  gtk_container_add (GTK_CONTAINER (menu4), _5__very_high1);

  _6__ultra_high1 = gtk_menu_item_new_with_mnemonic (_("6- Ultra High"));
  gtk_widget_show (_6__ultra_high1);
  gtk_container_add (GTK_CONTAINER (menu4), _6__ultra_high1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionME), menu4);

  optionQzer = gtk_option_menu_new ();
  gtk_widget_show (optionQzer);
  gtk_table_attach (GTK_TABLE (table2), optionQzer, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu5 = gtk_menu_new ();

 h263_quantizer1 = gtk_menu_item_new_with_mnemonic (_("H263 Quantizer"));
  gtk_widget_show (h263_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu5), h263_quantizer1);


  mpeg_quantizer1 = gtk_menu_item_new_with_mnemonic (_("MPEG Quantizer"));
  gtk_widget_show (mpeg_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu5), mpeg_quantizer1);


  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionQzer), menu5);

  label2 = gtk_label_new (_("Advanced"));
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  table3 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (notebook1), table3);

  label9 = gtk_label_new (_("Min I Frame Qzer"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table3), label9, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  label10 = gtk_label_new (_("Max I Frame Qzer"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table3), label10, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label11 = gtk_label_new (_("Min P Frame Qzer"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table3), label11, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new (_("Max P Frame Qzer"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table3), label12, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  spinImin_adj = gtk_adjustment_new (2, 2, 31, 1, 10, 10);
  spinImin = gtk_spin_button_new (GTK_ADJUSTMENT (spinImin_adj), 1, 0);
  gtk_widget_show (spinImin);
  gtk_table_attach (GTK_TABLE (table3), spinImin, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinImin), TRUE);

  spinImax_adj = gtk_adjustment_new (2, 2, 31, 1, 10, 10);
  spinImax = gtk_spin_button_new (GTK_ADJUSTMENT (spinImax_adj), 1, 0);
  gtk_widget_show (spinImax);
  gtk_table_attach (GTK_TABLE (table3), spinImax, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinImax), TRUE);

  spinPmin_adj = gtk_adjustment_new (2, 2, 31, 1, 10, 10);
  spinPmin = gtk_spin_button_new (GTK_ADJUSTMENT (spinPmin_adj), 1, 0);
  gtk_widget_show (spinPmin);
  gtk_table_attach (GTK_TABLE (table3), spinPmin, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinPmin), TRUE);

  spinPmax_adj = gtk_adjustment_new (2, 2, 31, 1, 10, 10);
  spinPmax = gtk_spin_button_new (GTK_ADJUSTMENT (spinPmax_adj), 1, 0);
  gtk_widget_show (spinPmax);
  gtk_table_attach (GTK_TABLE (table3), spinPmax, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinPmax), TRUE);

  label3 = gtk_label_new (_("Quantizer"));
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  label13 = gtk_label_new (_("Not Yet!"));
  gtk_widget_show (label13);
  gtk_container_add (GTK_CONTAINER (notebook1), label13);
  gtk_label_set_justify (GTK_LABEL (label13), GTK_JUSTIFY_LEFT);

  label4 = gtk_label_new (_("B Frames"));
  gtk_widget_show (label4);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label4);
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
  GLADE_HOOKUP_OBJECT (dialog1, notebook1, "notebook1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, radioCQ, "radioCQ");
  GLADE_HOOKUP_OBJECT (dialog1, radioCBR, "radioCBR");
  GLADE_HOOKUP_OBJECT (dialog1, radio2Pass, "radio2Pass");
  GLADE_HOOKUP_OBJECT (dialog1, entryCQ, "entryCQ");
  GLADE_HOOKUP_OBJECT (dialog1, entryCBR, "entryCBR");
  GLADE_HOOKUP_OBJECT (dialog1, entry2Pass, "entry2Pass");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxIFrame, "spinbuttonMaxIFrame");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinIFrame, "spinbuttonMinIFrame");
  GLADE_HOOKUP_OBJECT (dialog1, optionME, "optionME");
  GLADE_HOOKUP_OBJECT (dialog1, menu4, "menu4");
  GLADE_HOOKUP_OBJECT (dialog1, _0___none1, "_0___none1");
  GLADE_HOOKUP_OBJECT (dialog1, _1__very_low1, "_1__very_low1");
  GLADE_HOOKUP_OBJECT (dialog1, _2__low1, "_2__low1");
  GLADE_HOOKUP_OBJECT (dialog1, _3__medium1, "_3__medium1");
  GLADE_HOOKUP_OBJECT (dialog1, _4__high1, "_4__high1");
  GLADE_HOOKUP_OBJECT (dialog1, _5__very_high1, "_5__very_high1");
  GLADE_HOOKUP_OBJECT (dialog1, _6__ultra_high1, "_6__ultra_high1");
  GLADE_HOOKUP_OBJECT (dialog1, optionQzer, "optionQzer");
  GLADE_HOOKUP_OBJECT (dialog1, menu5, "menu5");
  GLADE_HOOKUP_OBJECT (dialog1, h263_quantizer1, "h263_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, mpeg_quantizer1, "mpeg_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
  GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, spinImin, "spinImin");
  GLADE_HOOKUP_OBJECT (dialog1, spinImax, "spinImax");
  GLADE_HOOKUP_OBJECT (dialog1, spinPmin, "spinPmin");
  GLADE_HOOKUP_OBJECT (dialog1, spinPmax, "spinPmax");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


#endif
