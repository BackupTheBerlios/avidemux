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

#include "ADM_library/default.h"

#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_assert.h" 
#include "ADM_lavcodec.h"
#include "ADM_codecs/ADM_ffmpegConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

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


static 	GtkWidget *create_dialog1 (void);
static  void 	upload(GtkWidget *dialog, FFcodecSetting *conf);
static  void 	download(GtkWidget *dialog, FFcodecSetting *conf);
static  void updateMode( void );
static  int cb_mod(GtkObject * object, gpointer user_data);


uint8_t getFFCompressParams(COMPRES_PARAMS *incoming);

static GtkWidget *dialog=NULL;		      
static uint32_t mQ,mB,mS;
static COMPRESSION_MODE mMode;
		      
		      
/*


*/		      		      
uint8_t getFFCompressParams(COMPRES_PARAMS *incoming)
{
int ret=0;	
	FFcodecSetting *conf=(FFcodecSetting *)incoming->extraSettings;
	ADM_assert(incoming->extraSettingsLen==sizeof(FFcodecSetting));
	dialog=create_dialog1();
	upload(dialog,conf);
	
	gtk_register_dialog(dialog);
	
#define HIST_SET(x) gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenuType)), x)
#define VAL_SET(x) gtk_write_entry(WID(entryEntry), x)

		mQ=incoming->qz;
		mB=incoming->bitrate;
		mS=incoming->finalsize;	
		mMode=incoming->mode;
		
	updateMode();	

	
 	gtk_signal_connect(GTK_OBJECT(WID(optionmenuType)), "changed",
                      GTK_SIGNAL_FUNC(cb_mod),                   (void *) 0);
	
	
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		int r=-1,value=-1;
		
		ret=1;
		download(dialog,conf);	
		r=getRangeInMenu(WID(optionmenuType));				
		switch(r)
			{
				case 0:
					incoming->mode = COMPRESS_CBR;				      
		      			value = (uint32_t) gtk_read_entry(WID(entryEntry));
		      			if (value < 3000)
			  			value *= 1000;
		      			if (value > 16 && value < 6000000)
					{
			    			incoming->bitrate = value;
			    			ret = 1;						
		      			}
					
					break;
				case 1:
					incoming->mode = COMPRESS_CQ;		      			
					value = (uint32_t) gtk_spin_button_get_value_as_int(
								GTK_SPIN_BUTTON(WID(spinbuttonQuant)));
		      			if (value >= 2 && value <= 32)
						{
			    			incoming->qz = value;
		      				}
		      			break;

				case 2:
		     				incoming->mode = COMPRESS_2PASS;		       				
						value = (uint32_t) gtk_read_entry(WID(entryEntry));
        					if((value>0)&&(value<4000))
          					{
       							incoming->finalsize=value;
				      			
           					}						
            					break;
				case 3:
						incoming->mode=COMPRESS_SAME;
						ret=1;
						break;
		  		default:
		      				ADM_assert(0);
				}
		
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);

	return ret;
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
			gtk_widget_set_sensitive(WID(spinbuttonQuant),0);
			gtk_widget_set_sensitive(WID(entryEntry),1);
			gtk_label_set_text(GTK_LABEL(WID(label11)),"Target bitrate (kb/s):");
			break;

		case COMPRESS_2PASS:
			HIST_SET(2);
			VAL_SET(mS);
			gtk_widget_set_sensitive(WID(spinbuttonQuant),0);
			gtk_widget_set_sensitive(WID(entryEntry),1);
			gtk_label_set_text(GTK_LABEL(WID(label11)),"Target size (MBytes):");
			break;

	    	case COMPRESS_CQ:
			HIST_SET(1);
			gtk_widget_set_sensitive(WID(entryEntry),0);
			gtk_widget_set_sensitive(WID(spinbuttonQuant),1);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbuttonQuant)),(gfloat)mQ);
			break;
		case COMPRESS_SAME:
			HIST_SET(3);
			gtk_widget_set_sensitive(WID(entryEntry),0);
			gtk_widget_set_sensitive(WID(spinbuttonQuant),0);
			break;
		}
}
/**

*/		
int cb_mod(GtkObject * object, gpointer user_data)
{
int r;
	r=getRangeInMenu(WID(optionmenuType));
	switch(r)
	{
		case 0: mMode=COMPRESS_CBR ;break;
		case 1: mMode=COMPRESS_CQ ;break;
		case 2: mMode=COMPRESS_2PASS ;break;
		case 3: mMode=COMPRESS_SAME ;break;
	
	}
	updateMode();
	printf("Changed!!!\n");

}
void upload(GtkWidget *dialog, FFcodecSetting *conf)
{

	// Disable some
#define GTK_DISABLE(x) gtk_widget_set_sensitive(WID(x),0);	
	//
	GTK_DISABLE(checkbuttonPMask);
	GTK_DISABLE(checkbuttonLumaElimDC);
	GTK_DISABLE(checkbuttonChromaElimDC);
	GTK_DISABLE(spinbuttonPMask);
	GTK_DISABLE(checkbuttonCbp );
	GTK_DISABLE(checkbuttonMV0);
	GTK_DISABLE(table1);
	GTK_DISABLE(table2);
	GTK_DISABLE(checkbuttonAspect );
	//---------------------------------------
	
	CHECK_SET(checkbuttonGMC,_GMC);
	CHECK_SET(checkbuttonQPel,_QPEL);
	CHECK_SET(checkbutton4MV,_4MV);
	CHECK_SET(checkbuttonTrellis,_TRELLIS_QUANT);
	CHECK_SET(checkbuttonInterlaced,interlaced);
	
	CHECK_SET(checkbuttonNaq,_NORMALIZE_AQP);
	CHECK_SET(checkbuttonAspect,widescreen);
	
	
	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenuMS)), conf->me_method-1);
	gtk_option_menu_set_history(GTK_OPTION_MENU(WID(optionmenuMbd)), conf->mb_eval);
	
	
	//SPIN_SET(spinbuttonIMaxPeriod, 
	
	SPIN_SET(spinbuttonBFrame, max_b_frames);
	SPIN_SET(spinbuttonIMin, qmin);
	SPIN_SET(spinbuttonIMax, qmax);
	SPIN_SET(spinbuttonMaxQDiff, max_qdiff);
	
	SPIN_SET(spinbuttonLumaElim,luma_elim_threshold);
	SPIN_SET(spinbuttonChromaElim,chroma_elim_threshold);
	
	SPIN_SET(spinbuttonLumiMask,lumi_masking);
	SPIN_SET(spinbuttonDarkMask,dark_masking );
	
	SPIN_SET( spinbuttonQComp,qcompress);
	SPIN_SET( spinbuttonQBlur ,qblur );
	
	SPIN_SET( spinbuttonScplxMask ,spatial_cplx_masking );
	SPIN_SET( spinbuttonTcplxMask,temporal_cplx_masking );
	
	
	if(conf->mpeg_quant)
		RADIO_SET(radiobuttonMpeg,		1);
	else
		RADIO_SET(radiobuttonH263,		1);	

	TOGGLE_SET(checkbuttonScplxMask,is_spatial_cplx_masking);		
	TOGGLE_SET(checkbuttonTcplxMask,is_temporal_cplx_masking);
	
	TOGGLE_SET(checkbuttonLumiMask ,is_lumi_masking);		
	TOGGLE_SET(checkbuttonDarkMask ,is_dark_masking);
	
	TOGGLE_SET(checkbuttonLumaElim  ,is_luma_elim_threshold);		
	TOGGLE_SET(checkbuttonChromaElim ,is_chroma_elim_threshold);
		
}
void download(GtkWidget *dialog,FFcodecSetting *conf)
{
	CHECK_GET(checkbuttonGMC,_GMC);
	CHECK_GET(checkbuttonQPel,_QPEL);
	CHECK_GET(checkbutton4MV,_4MV);
	CHECK_GET(checkbuttonTrellis,_TRELLIS_QUANT);
	CHECK_GET(checkbuttonInterlaced,interlaced);
	
	
	CHECK_GET(checkbuttonNaq,_NORMALIZE_AQP);
	CHECK_GET(checkbuttonAspect,widescreen);
	
	SPIN_GET(spinbuttonBFrame, max_b_frames);
	SPIN_GET(spinbuttonIMin, qmin);
	SPIN_GET(spinbuttonIMax, qmax);
	SPIN_GET(spinbuttonMaxQDiff, max_qdiff);
	
	SPIN_GET(spinbuttonLumaElim,luma_elim_threshold);
	SPIN_GET(spinbuttonChromaElim,chroma_elim_threshold);
	
	SPIN_GETF(spinbuttonLumiMask,lumi_masking);
	SPIN_GETF(spinbuttonDarkMask,dark_masking );
	
	SPIN_GETF( spinbuttonQComp,qcompress);
	SPIN_GETF( spinbuttonQBlur ,qblur );
	
	SPIN_GETF( spinbuttonScplxMask ,spatial_cplx_masking );
	SPIN_GETF( spinbuttonTcplxMask,temporal_cplx_masking );
	
	
	conf->me_method=(Motion_Est_ID)(1+getRangeInMenu(WID(optionmenuMS)));		
	conf->mb_eval=(Motion_Est_ID)getRangeInMenu(WID(optionmenuMbd));

	
		
	conf->mpeg_quant=RADIO_GET(radiobuttonMpeg);
	TOGGLE_GET(checkbuttonScplxMask,is_spatial_cplx_masking);		
	TOGGLE_GET(checkbuttonTcplxMask,is_temporal_cplx_masking);
	
	TOGGLE_GET(checkbuttonLumiMask ,is_lumi_masking);		
	TOGGLE_GET(checkbuttonDarkMask ,is_dark_masking);
	
	TOGGLE_GET(checkbuttonLumaElim  ,is_luma_elim_threshold);		
	TOGGLE_GET(checkbuttonChromaElim ,is_chroma_elim_threshold);
	
	
}

// Glade     


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *vbox2;
  GtkWidget *frame10;
  GtkWidget *table3;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *optionmenuType;
  GtkWidget *menu1;
  GtkWidget *one_pass_cbr1;
  GtkWidget *one_pass_quantizer1;
  GtkWidget *two_pass1;
  GtkWidget *same_qz_as_input1;
  GtkObject *spinbuttonQuant_adj;
  GtkWidget *spinbuttonQuant;
  GtkWidget *entryEntry;
  GtkWidget *label51;
  GtkWidget *hbox7;
  GtkWidget *vbox4;
  GtkWidget *checkbuttonInterlaced;
  GtkWidget *checkbuttonGreyScale;
  GtkWidget *hbox20;
  GtkWidget *checkbuttonAspect;
  GtkWidget *entryAspectX;
  GtkWidget *label40;
  GtkWidget *entryAspectY;
  GtkWidget *label8;
  GtkWidget *vbox6;
  GtkWidget *table7;
  GtkWidget *label41;
  GtkWidget *label15;
  GtkWidget *hbox30;
  GtkWidget *checkbutton4MV;
  GtkWidget *checkbuttonMV0;
  GtkWidget *optionmenuMS;
  GtkWidget *menu2;
  GtkWidget *_0___none1;
  GtkWidget *_1___full1;
  GtkWidget *_2___log2;
  GtkWidget *_3___phods1;
  GtkWidget *_4___epzs1;
  GtkWidget *_5___x1;
  GtkObject *spinbuttonIMaxPeriod_adj;
  GtkWidget *spinbuttonIMaxPeriod;
  GtkWidget *frame2;
  GtkWidget *hbox11;
  GtkWidget *label18;
  GtkObject *spinbuttonBFrame_adj;
  GtkWidget *spinbuttonBFrame;
  GtkWidget *checkbuttonQPel;
  GtkWidget *checkbuttonGMC;
  GtkWidget *label17;
  GtkWidget *hbox24;
  GtkWidget *frame8;
  GtkWidget *table1;
  GtkWidget *label43;
  GtkWidget *optionmenuPrecmp;
  GtkWidget *menu4;
  GtkWidget *_0__sad1;
  GtkWidget *_1__sse1;
  GtkWidget *_2__satd1;
  GtkWidget *_3__dct1;
  GtkWidget *_4__psnr1;
  GtkWidget *_5__bit1;
  GtkWidget *_6__rd1;
  GtkWidget *_7__zero1;
  GtkWidget *label44;
  GtkWidget *optionmenuCmp;
  GtkWidget *menu5;
  GtkWidget *menuitem1;
  GtkWidget *menuitem2;
  GtkWidget *menuitem3;
  GtkWidget *menuitem4;
  GtkWidget *menuitem5;
  GtkWidget *menuitem6;
  GtkWidget *menuitem7;
  GtkWidget *menuitem8;
  GtkWidget *label45;
  GtkWidget *optionmenuSubcmp;
  GtkWidget *menu6;
  GtkWidget *menuitem9;
  GtkWidget *menuitem10;
  GtkWidget *menuitem11;
  GtkWidget *menuitem12;
  GtkWidget *menuitem13;
  GtkWidget *menuitem14;
  GtkWidget *menuitem15;
  GtkWidget *menuitem16;
  GtkWidget *label68;
  GtkWidget *optionmenuMBcmp;
  GtkWidget *menu10;
  GtkWidget *menuitem28;
  GtkWidget *menuitem29;
  GtkWidget *menuitem30;
  GtkWidget *menuitem31;
  GtkWidget *menuitem32;
  GtkWidget *menuitem33;
  GtkWidget *menuitem34;
  GtkWidget *menuitem35;
  GtkWidget *checkbuttonPrecmpChroma;
  GtkWidget *checkbuttonCmpChroma;
  GtkWidget *checkbuttonSubcmpChroma;
  GtkWidget *checkbuttonMBcmpChroma;
  GtkWidget *label49;
  GtkWidget *frame9;
  GtkWidget *table2;
  GtkWidget *label47;
  GtkWidget *label48;
  GtkWidget *optionmenuPreDia;
  GtkWidget *menu7;
  GtkWidget *_3__adaptive_with_size_1;
  GtkWidget *_2__adaptive_with_size_1;
  GtkWidget *experimental1;
  GtkWidget *_1__normal_with_size_1__epzs_1;
  GtkWidget *_2__normal_with_size_1;
  GtkWidget *optionmenuDia;
  GtkWidget *menu8;
  GtkWidget *menuitem23;
  GtkWidget *menuitem24;
  GtkWidget *menuitem25;
  GtkWidget *menuitem26;
  GtkWidget *menuitem27;
  GtkWidget *label50;
  GtkWidget *label2;
  GtkWidget *vbox10;
  GtkWidget *table6;
  GtkWidget *label19;
  GtkWidget *label52;
  GtkWidget *label62;
  GtkWidget *optionmenuMbd;
  GtkWidget *menu9;
  GtkWidget *_0__mbcmp1;
  GtkWidget *_1__fewest_bits1;
  GtkWidget *_2__rate_distortion1;
  GtkWidget *hbox28;
  GtkWidget *radiobuttonH263;
  GSList *radiobuttonH263_group = NULL;
  GtkWidget *radiobuttonMpeg;
  GtkWidget *hbox29;
  GtkObject *spinbuttonIMin_adj;
  GtkWidget *spinbuttonIMin;
  GtkWidget *label24;
  GtkObject *spinbuttonIMax_adj;
  GtkWidget *spinbuttonIMax;
  GtkWidget *checkbuttonTrellis;
  GtkWidget *checkbuttonCbp;
  GtkWidget *label3;
  GtkWidget *vbox11;
  GtkWidget *table4;
  GtkObject *spinbuttonDarkMask_adj;
  GtkWidget *spinbuttonDarkMask;
  GtkObject *spinbuttonLumiMask_adj;
  GtkWidget *spinbuttonLumiMask;
  GtkObject *spinbuttonScplxMask_adj;
  GtkWidget *spinbuttonScplxMask;
  GtkObject *spinbuttonTcplxMask_adj;
  GtkWidget *spinbuttonTcplxMask;
  GtkWidget *checkbuttonDarkMask;
  GtkWidget *checkbuttonScplxMask;
  GtkWidget *checkbuttonTcplxMask;
  GtkWidget *checkbuttonLumiMask;
  GtkObject *spinbuttonPMask_adj;
  GtkWidget *spinbuttonPMask;
  GtkWidget *checkbuttonPMask;
  GtkWidget *checkbuttonNaq;
  GtkWidget *frame11;
  GtkWidget *table5;
  GtkObject *spinbuttonLumaElim_adj;
  GtkWidget *spinbuttonLumaElim;
  GtkObject *spinbuttonChromaElim_adj;
  GtkWidget *spinbuttonChromaElim;
  GtkWidget *checkbuttonLumaElim;
  GtkWidget *checkbuttonChromaElim;
  GtkWidget *checkbuttonLumaElimDC;
  GtkWidget *checkbuttonChromaElimDC;
  GtkWidget *label59;
  GtkWidget *label53;
  GtkWidget *table8;
  GtkWidget *label64;
  GtkWidget *label65;
  GtkWidget *label66;
  GtkWidget *label67;
  GtkObject *spinbuttonMaxQDiff_adj;
  GtkWidget *spinbuttonMaxQDiff;
  GtkObject *spinbuttonQBlur_adj;
  GtkWidget *spinbuttonQBlur;
  GtkObject *spinbuttonQComp_adj;
  GtkWidget *spinbuttonQComp;
  GtkWidget *entryRateTol;
  GtkWidget *labelRateTol;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dialog1 = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 2);
  gtk_window_set_title (GTK_WINDOW (dialog1), _("FFmpeg encoding options"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox2,
                                      FALSE, FALSE, GTK_PACK_START);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 5);

  frame10 = gtk_frame_new (NULL);
  gtk_widget_show (frame10);
  gtk_box_pack_start (GTK_BOX (vbox2), frame10, FALSE, FALSE, 0);

  table3 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (frame10), table3);
  gtk_container_set_border_width (GTK_CONTAINER (table3), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table3), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table3), 10);

  label10 = gtk_label_new (_("Encoding type:"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table3), label10, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label10), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label11 = gtk_label_new (_("Bitrate (kb/s):"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table3), label11, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label11), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new (_("Quantizer:"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table3), label12, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label12), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  optionmenuType = gtk_option_menu_new ();
  gtk_widget_show (optionmenuType);
  gtk_table_attach (GTK_TABLE (table3), optionmenuType, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuType, _("Select 1-pass or 2-pass encoding"), NULL);

  menu1 = gtk_menu_new ();

  one_pass_cbr1 = gtk_menu_item_new_with_mnemonic (_("Single pass - bitrate"));
  gtk_widget_show (one_pass_cbr1);
  gtk_container_add (GTK_CONTAINER (menu1), one_pass_cbr1);

  one_pass_quantizer1 = gtk_menu_item_new_with_mnemonic (_("Single pass - quantizer"));
  gtk_widget_show (one_pass_quantizer1);
  gtk_container_add (GTK_CONTAINER (menu1), one_pass_quantizer1);

  two_pass1 = gtk_menu_item_new_with_mnemonic (_("Two pass"));
  gtk_widget_show (two_pass1);
  gtk_container_add (GTK_CONTAINER (menu1), two_pass1);

  same_qz_as_input1 = gtk_menu_item_new_with_mnemonic (_("Same Qz as input"));
  gtk_widget_show (same_qz_as_input1);
  gtk_container_add (GTK_CONTAINER (menu1), same_qz_as_input1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuType), menu1);

  spinbuttonQuant_adj = gtk_adjustment_new (4, 1, 31, 1, 10, 10);
  spinbuttonQuant = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQuant_adj), 1, 0);
  gtk_widget_show (spinbuttonQuant);
  gtk_table_attach (GTK_TABLE (table3), spinbuttonQuant, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonQuant, _("Set quantizer for a constant quantizer/quality encoding"), NULL);

  entryEntry = gtk_entry_new ();
  gtk_widget_show (entryEntry);
  gtk_table_attach (GTK_TABLE (table3), entryEntry, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, entryEntry, _("Target video bitrate"), NULL);
  gtk_entry_set_width_chars (GTK_ENTRY (entryEntry), 10);

  label51 = gtk_label_new (_("Variable bitrate mode"));
  gtk_widget_show (label51);
  gtk_frame_set_label_widget (GTK_FRAME (frame10), label51);
  gtk_label_set_justify (GTK_LABEL (label51), GTK_JUSTIFY_LEFT);

  hbox7 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox7, FALSE, FALSE, 0);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (hbox7), vbox4, FALSE, FALSE, 0);

  checkbuttonInterlaced = gtk_check_button_new_with_mnemonic (_("_Interlaced"));
  gtk_widget_show (checkbuttonInterlaced);
  gtk_box_pack_start (GTK_BOX (vbox4), checkbuttonInterlaced, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonInterlaced, _("Enable interlaced encoding"), NULL);

  checkbuttonGreyScale = gtk_check_button_new_with_mnemonic (_("_Greyscale"));
  gtk_widget_show (checkbuttonGreyScale);
  gtk_box_pack_start (GTK_BOX (vbox2), checkbuttonGreyScale, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonGreyScale, _("Encode in black & white"), NULL);

  hbox20 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox20);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox20, FALSE, FALSE, 0);

  checkbuttonAspect = gtk_check_button_new_with_mnemonic (_("_Aspect ratio"));
  gtk_widget_show (checkbuttonAspect);
  gtk_box_pack_start (GTK_BOX (hbox20), checkbuttonAspect, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonAspect, _("Set MPEG-4 aspect ratio (warning: MPlayer supports it, but most other players don't)"), NULL);

  entryAspectX = gtk_entry_new ();
  gtk_widget_show (entryAspectX);
  gtk_box_pack_start (GTK_BOX (hbox20), entryAspectX, FALSE, FALSE, 0);
  gtk_entry_set_width_chars (GTK_ENTRY (entryAspectX), 8);

  label40 = gtk_label_new (_(":"));
  gtk_widget_show (label40);
  gtk_box_pack_start (GTK_BOX (hbox20), label40, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label40), GTK_JUSTIFY_LEFT);

  entryAspectY = gtk_entry_new ();
  gtk_widget_show (entryAspectY);
  gtk_box_pack_start (GTK_BOX (hbox20), entryAspectY, FALSE, FALSE, 0);
  gtk_entry_set_width_chars (GTK_ENTRY (entryAspectY), 9);

  label8 = gtk_label_new (_("Main"));
  gtk_widget_show (label8);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label8);
  gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

  vbox6 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox6);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox6,
                                      FALSE, FALSE, GTK_PACK_START);
  gtk_container_set_border_width (GTK_CONTAINER (vbox6), 5);

  table7 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table7);
  gtk_box_pack_start (GTK_BOX (vbox6), table7, FALSE, FALSE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table7), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table7), 10);

  label41 = gtk_label_new (_("ME method:"));
  gtk_widget_show (label41);
  gtk_table_attach (GTK_TABLE (table7), label41, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label41), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label41), 0, 0.5);

  label15 = gtk_label_new (_("Max I-frame interval:"));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table7), label15, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  hbox30 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox30);
  gtk_table_attach (GTK_TABLE (table7), hbox30, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  checkbutton4MV = gtk_check_button_new_with_mnemonic (_("_4MV"));
  gtk_widget_show (checkbutton4MV);
  gtk_box_pack_start (GTK_BOX (hbox30), checkbutton4MV, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton4MV, _("Allow 4 motion vectors per macroblock (slightly better quality)"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton4MV), TRUE);

  checkbuttonMV0 = gtk_check_button_new_with_mnemonic (_("MV0"));
  gtk_widget_show (checkbuttonMV0);
  gtk_box_pack_start (GTK_BOX (hbox30), checkbuttonMV0, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonMV0, _("Try  to encode each MB with MV=<0,0> and choose the better one"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonMV0), TRUE);

  optionmenuMS = gtk_option_menu_new ();
  gtk_widget_show (optionmenuMS);
  gtk_table_attach (GTK_TABLE (table7), optionmenuMS, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuMS, _("Motion estimation method, 0-3 currently ignores the amount of bits spent, so quality may be low"), NULL);

  menu2 = gtk_menu_new ();

  _0___none1 = gtk_menu_item_new_with_mnemonic (_("0: None"));
  gtk_widget_show (_0___none1);
  gtk_container_add (GTK_CONTAINER (menu2), _0___none1);

  _1___full1 = gtk_menu_item_new_with_mnemonic (_("1: Full"));
  gtk_widget_show (_1___full1);
  gtk_container_add (GTK_CONTAINER (menu2), _1___full1);

  _2___log2 = gtk_menu_item_new_with_mnemonic (_("2: Log"));
  gtk_widget_show (_2___log2);
  gtk_container_add (GTK_CONTAINER (menu2), _2___log2);

  _3___phods1 = gtk_menu_item_new_with_mnemonic (_("3: Phods"));
  gtk_widget_show (_3___phods1);
  gtk_container_add (GTK_CONTAINER (menu2), _3___phods1);

  _4___epzs1 = gtk_menu_item_new_with_mnemonic (_("4: EPZS"));
  gtk_widget_show (_4___epzs1);
  gtk_container_add (GTK_CONTAINER (menu2), _4___epzs1);

  _5___x1 = gtk_menu_item_new_with_mnemonic (_("5: X1"));
  gtk_widget_show (_5___x1);
  gtk_container_add (GTK_CONTAINER (menu2), _5___x1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuMS), menu2);

  spinbuttonIMaxPeriod_adj = gtk_adjustment_new (250, 0, 300, 1, 10, 10);
  spinbuttonIMaxPeriod = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonIMaxPeriod_adj), 1, 0);
  gtk_widget_show (spinbuttonIMaxPeriod);
  gtk_table_attach (GTK_TABLE (table7), spinbuttonIMaxPeriod, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonIMaxPeriod, _("Maximum interval between keyframes in frames"), NULL);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox6), frame2, FALSE, FALSE, 5);

  hbox11 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox11);
  gtk_container_add (GTK_CONTAINER (frame2), hbox11);
  gtk_container_set_border_width (GTK_CONTAINER (hbox11), 5);

  label18 = gtk_label_new (_("Number of B-frames:"));
  gtk_widget_show (label18);
  gtk_box_pack_start (GTK_BOX (hbox11), label18, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label18), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

  spinbuttonBFrame_adj = gtk_adjustment_new (1, 0, 4, 1, 1, 1);
  spinbuttonBFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonBFrame);
  gtk_box_pack_start (GTK_BOX (hbox11), spinbuttonBFrame, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonBFrame, _("Maximum number of B-frames between non B-frames, 0-2 is a sane range"), NULL);

  checkbuttonQPel = gtk_check_button_new_with_mnemonic (_("_Qpel"));
  gtk_widget_show (checkbuttonQPel);
  gtk_box_pack_start (GTK_BOX (hbox11), checkbuttonQPel, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonQPel, _("Use quarter pixel motion compensation (useful at higher bitrates)"), NULL);

  checkbuttonGMC = gtk_check_button_new_with_mnemonic (_("_GMC"));
  gtk_widget_show (checkbuttonGMC);
  gtk_box_pack_start (GTK_BOX (hbox11), checkbuttonGMC, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonGMC, _("Global Motion Compensation can save bits on camera pans, zooming and rotation"), NULL);

  label17 = gtk_label_new (_("Advanced Simple Profile"));
  gtk_widget_show (label17);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label17);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);

  hbox24 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox24);
  gtk_box_pack_start (GTK_BOX (vbox6), hbox24, FALSE, FALSE, 0);

  frame8 = gtk_frame_new (NULL);
  gtk_widget_show (frame8);
  gtk_box_pack_start (GTK_BOX (hbox24), frame8, FALSE, FALSE, 0);

  table1 = gtk_table_new (4, 3, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame8), table1);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 10);

  label43 = gtk_label_new (_("Prepass:"));
  gtk_widget_show (label43);
  gtk_table_attach (GTK_TABLE (table1), label43, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label43), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label43), 0, 0.5);

  optionmenuPrecmp = gtk_option_menu_new ();
  gtk_widget_show (optionmenuPrecmp);
  gtk_table_attach (GTK_TABLE (table1), optionmenuPrecmp, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuPrecmp, _("Comparison function for motion estimation pre pass. Tip: SAD is fast, SATD is good"), NULL);

  menu4 = gtk_menu_new ();

  _0__sad1 = gtk_menu_item_new_with_mnemonic (_("0: SAD"));
  gtk_widget_show (_0__sad1);
  gtk_container_add (GTK_CONTAINER (menu4), _0__sad1);

  _1__sse1 = gtk_menu_item_new_with_mnemonic (_("1: SSE"));
  gtk_widget_show (_1__sse1);
  gtk_container_add (GTK_CONTAINER (menu4), _1__sse1);

  _2__satd1 = gtk_menu_item_new_with_mnemonic (_("2: SATD"));
  gtk_widget_show (_2__satd1);
  gtk_container_add (GTK_CONTAINER (menu4), _2__satd1);

  _3__dct1 = gtk_menu_item_new_with_mnemonic (_("3: DCT"));
  gtk_widget_show (_3__dct1);
  gtk_container_add (GTK_CONTAINER (menu4), _3__dct1);

  _4__psnr1 = gtk_menu_item_new_with_mnemonic (_("4: PSNR"));
  gtk_widget_show (_4__psnr1);
  gtk_container_add (GTK_CONTAINER (menu4), _4__psnr1);

  _5__bit1 = gtk_menu_item_new_with_mnemonic (_("5: BIT"));
  gtk_widget_show (_5__bit1);
  gtk_container_add (GTK_CONTAINER (menu4), _5__bit1);

  _6__rd1 = gtk_menu_item_new_with_mnemonic (_("6: RD"));
  gtk_widget_show (_6__rd1);
  gtk_container_add (GTK_CONTAINER (menu4), _6__rd1);

  _7__zero1 = gtk_menu_item_new_with_mnemonic (_("7: ZERO"));
  gtk_widget_show (_7__zero1);
  gtk_container_add (GTK_CONTAINER (menu4), _7__zero1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuPrecmp), menu4);

  label44 = gtk_label_new (_("Full pixel:"));
  gtk_widget_show (label44);
  gtk_table_attach (GTK_TABLE (table1), label44, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label44), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label44), 0, 0.5);

  optionmenuCmp = gtk_option_menu_new ();
  gtk_widget_show (optionmenuCmp);
  gtk_table_attach (GTK_TABLE (table1), optionmenuCmp, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuCmp, _("Comparison function for full pel motion estimation"), NULL);

  menu5 = gtk_menu_new ();

  menuitem1 = gtk_menu_item_new_with_mnemonic (_("0: SAD"));
  gtk_widget_show (menuitem1);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem1);

  menuitem2 = gtk_menu_item_new_with_mnemonic (_("1: SSE"));
  gtk_widget_show (menuitem2);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem2);

  menuitem3 = gtk_menu_item_new_with_mnemonic (_("2: SATD"));
  gtk_widget_show (menuitem3);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem3);

  menuitem4 = gtk_menu_item_new_with_mnemonic (_("3: DCT"));
  gtk_widget_show (menuitem4);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem4);

  menuitem5 = gtk_menu_item_new_with_mnemonic (_("4: PSNR"));
  gtk_widget_show (menuitem5);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem5);

  menuitem6 = gtk_menu_item_new_with_mnemonic (_("5: BIT"));
  gtk_widget_show (menuitem6);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem6);

  menuitem7 = gtk_menu_item_new_with_mnemonic (_("6: RD"));
  gtk_widget_show (menuitem7);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem7);

  menuitem8 = gtk_menu_item_new_with_mnemonic (_("7: ZERO"));
  gtk_widget_show (menuitem8);
  gtk_container_add (GTK_CONTAINER (menu5), menuitem8);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuCmp), menu5);

  label45 = gtk_label_new (_("Subpixel:"));
  gtk_widget_show (label45);
  gtk_table_attach (GTK_TABLE (table1), label45, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label45), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label45), 0, 0.5);

  optionmenuSubcmp = gtk_option_menu_new ();
  gtk_widget_show (optionmenuSubcmp);
  gtk_table_attach (GTK_TABLE (table1), optionmenuSubcmp, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuSubcmp, _("Comparison function for sub pel motion estimation"), NULL);

  menu6 = gtk_menu_new ();

  menuitem9 = gtk_menu_item_new_with_mnemonic (_("0: SAD"));
  gtk_widget_show (menuitem9);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem9);

  menuitem10 = gtk_menu_item_new_with_mnemonic (_("1: SSE"));
  gtk_widget_show (menuitem10);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem10);

  menuitem11 = gtk_menu_item_new_with_mnemonic (_("2: SATD"));
  gtk_widget_show (menuitem11);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem11);

  menuitem12 = gtk_menu_item_new_with_mnemonic (_("3: DCT"));
  gtk_widget_show (menuitem12);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem12);

  menuitem13 = gtk_menu_item_new_with_mnemonic (_("4: PSNR"));
  gtk_widget_show (menuitem13);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem13);

  menuitem14 = gtk_menu_item_new_with_mnemonic (_("5: BIT"));
  gtk_widget_show (menuitem14);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem14);

  menuitem15 = gtk_menu_item_new_with_mnemonic (_("6: RD"));
  gtk_widget_show (menuitem15);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem15);

  menuitem16 = gtk_menu_item_new_with_mnemonic (_("7: ZERO"));
  gtk_widget_show (menuitem16);
  gtk_container_add (GTK_CONTAINER (menu6), menuitem16);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuSubcmp), menu6);

  label68 = gtk_label_new (_("Macroblock:"));
  gtk_widget_show (label68);
  gtk_table_attach (GTK_TABLE (table1), label68, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label68), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label68), 0, 0.5);

  optionmenuMBcmp = gtk_option_menu_new ();
  gtk_widget_show (optionmenuMBcmp);
  gtk_table_attach (GTK_TABLE (table1), optionmenuMBcmp, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuMBcmp, _("Comparison  function for the macroblock decision, used only when macroblock decision is set to 0"), NULL);

  menu10 = gtk_menu_new ();

  menuitem28 = gtk_menu_item_new_with_mnemonic (_("0: SAD"));
  gtk_widget_show (menuitem28);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem28);

  menuitem29 = gtk_menu_item_new_with_mnemonic (_("1: SSE"));
  gtk_widget_show (menuitem29);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem29);

  menuitem30 = gtk_menu_item_new_with_mnemonic (_("2: SATD"));
  gtk_widget_show (menuitem30);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem30);

  menuitem31 = gtk_menu_item_new_with_mnemonic (_("3: DCT"));
  gtk_widget_show (menuitem31);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem31);

  menuitem32 = gtk_menu_item_new_with_mnemonic (_("4: PSNR"));
  gtk_widget_show (menuitem32);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem32);

  menuitem33 = gtk_menu_item_new_with_mnemonic (_("5: BIT"));
  gtk_widget_show (menuitem33);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem33);

  menuitem34 = gtk_menu_item_new_with_mnemonic (_("6: RD"));
  gtk_widget_show (menuitem34);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem34);

  menuitem35 = gtk_menu_item_new_with_mnemonic (_("7: ZERO"));
  gtk_widget_show (menuitem35);
  gtk_container_add (GTK_CONTAINER (menu10), menuitem35);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuMBcmp), menu10);

  checkbuttonPrecmpChroma = gtk_check_button_new_with_mnemonic (_("Chroma"));
  gtk_widget_show (checkbuttonPrecmpChroma);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonPrecmpChroma, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonPrecmpChroma, _("Use chroma, too, currently doesn't work (correctly) with B-frames"), NULL);

  checkbuttonCmpChroma = gtk_check_button_new_with_mnemonic (_("Chroma"));
  gtk_widget_show (checkbuttonCmpChroma);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonCmpChroma, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonCmpChroma, _("Use chroma, too, currently doesn't work (correctly) with B-frames"), NULL);

  checkbuttonSubcmpChroma = gtk_check_button_new_with_mnemonic (_("Chroma"));
  gtk_widget_show (checkbuttonSubcmpChroma);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonSubcmpChroma, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonSubcmpChroma, _("Use chroma, too, currently doesn't work (correctly) with B-frames"), NULL);

  checkbuttonMBcmpChroma = gtk_check_button_new_with_mnemonic (_("Chroma"));
  gtk_widget_show (checkbuttonMBcmpChroma);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonMBcmpChroma, 2, 3, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonMBcmpChroma, _("Use chroma, too, currently doesn't work (correctly) with B-frames"), NULL);

  label49 = gtk_label_new (_("ME comparison function"));
  gtk_widget_show (label49);
  gtk_frame_set_label_widget (GTK_FRAME (frame8), label49);
  gtk_label_set_justify (GTK_LABEL (label49), GTK_JUSTIFY_LEFT);

  frame9 = gtk_frame_new (NULL);
  gtk_widget_show (frame9);
  gtk_box_pack_start (GTK_BOX (hbox24), frame9, FALSE, FALSE, 0);

  table2 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (frame9), table2);
  gtk_container_set_border_width (GTK_CONTAINER (table2), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table2), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table2), 10);

  label47 = gtk_label_new (_("Prepass:"));
  gtk_widget_show (label47);
  gtk_table_attach (GTK_TABLE (table2), label47, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label47), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label47), 0, 0.5);

  label48 = gtk_label_new (_("ME:"));
  gtk_widget_show (label48);
  gtk_table_attach (GTK_TABLE (table2), label48, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label48), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label48), 0, 0.5);

  optionmenuPreDia = gtk_option_menu_new ();
  gtk_widget_show (optionmenuPreDia);
  gtk_table_attach (GTK_TABLE (table2), optionmenuPreDia, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuPreDia, _("Diamond type and size for motion estimation pre pass"), NULL);

  menu7 = gtk_menu_new ();

  _3__adaptive_with_size_1 = gtk_menu_item_new_with_mnemonic (_("adaptive, size 3"));
  gtk_widget_show (_3__adaptive_with_size_1);
  gtk_container_add (GTK_CONTAINER (menu7), _3__adaptive_with_size_1);

  _2__adaptive_with_size_1 = gtk_menu_item_new_with_mnemonic (_("adaptive, size 2"));
  gtk_widget_show (_2__adaptive_with_size_1);
  gtk_container_add (GTK_CONTAINER (menu7), _2__adaptive_with_size_1);

  experimental1 = gtk_menu_item_new_with_mnemonic (_("experimental"));
  gtk_widget_show (experimental1);
  gtk_container_add (GTK_CONTAINER (menu7), experimental1);

  _1__normal_with_size_1__epzs_1 = gtk_menu_item_new_with_mnemonic (_("normal, size 1"));
  gtk_widget_show (_1__normal_with_size_1__epzs_1);
  gtk_container_add (GTK_CONTAINER (menu7), _1__normal_with_size_1__epzs_1);

  _2__normal_with_size_1 = gtk_menu_item_new_with_mnemonic (_("normal, size 2"));
  gtk_widget_show (_2__normal_with_size_1);
  gtk_container_add (GTK_CONTAINER (menu7), _2__normal_with_size_1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuPreDia), menu7);

  optionmenuDia = gtk_option_menu_new ();
  gtk_widget_show (optionmenuDia);
  gtk_table_attach (GTK_TABLE (table2), optionmenuDia, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuDia, _("Diamond type and size for motion estimation"), NULL);

  menu8 = gtk_menu_new ();

  menuitem23 = gtk_menu_item_new_with_mnemonic (_("adaptive, size 3"));
  gtk_widget_show (menuitem23);
  gtk_container_add (GTK_CONTAINER (menu8), menuitem23);

  menuitem24 = gtk_menu_item_new_with_mnemonic (_("adaptive, size 2"));
  gtk_widget_show (menuitem24);
  gtk_container_add (GTK_CONTAINER (menu8), menuitem24);

  menuitem25 = gtk_menu_item_new_with_mnemonic (_("experimental"));
  gtk_widget_show (menuitem25);
  gtk_container_add (GTK_CONTAINER (menu8), menuitem25);

  menuitem26 = gtk_menu_item_new_with_mnemonic (_("normal, size 1"));
  gtk_widget_show (menuitem26);
  gtk_container_add (GTK_CONTAINER (menu8), menuitem26);

  menuitem27 = gtk_menu_item_new_with_mnemonic (_("normal, size 2"));
  gtk_widget_show (menuitem27);
  gtk_container_add (GTK_CONTAINER (menu8), menuitem27);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuDia), menu8);

  label50 = gtk_label_new (_("Diamond type and size"));
  gtk_widget_show (label50);
  gtk_frame_set_label_widget (GTK_FRAME (frame9), label50);
  gtk_label_set_justify (GTK_LABEL (label50), GTK_JUSTIFY_LEFT);

  label2 = gtk_label_new (_("Motion Estimation"));
  gtk_widget_show (label2);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  vbox10 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox10);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox10);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox10,
                                      FALSE, FALSE, GTK_PACK_START);
  gtk_container_set_border_width (GTK_CONTAINER (vbox10), 5);

  table6 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table6);
  gtk_box_pack_start (GTK_BOX (vbox10), table6, FALSE, FALSE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table6), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table6), 10);

  label19 = gtk_label_new (_("Quantization type:"));
  gtk_widget_show (label19);
  gtk_table_attach (GTK_TABLE (table6), label19, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label19), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label19), 0, 0.5);

  label52 = gtk_label_new (_("Macroblock decision:"));
  gtk_widget_show (label52);
  gtk_table_attach (GTK_TABLE (table6), label52, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label52), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label52), 0, 0.5);

  label62 = gtk_label_new (_("Quantizer range - Min:"));
  gtk_widget_show (label62);
  gtk_table_attach (GTK_TABLE (table6), label62, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label62), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label62), 0, 0.5);

  optionmenuMbd = gtk_option_menu_new ();
  gtk_widget_show (optionmenuMbd);
  gtk_table_attach (GTK_TABLE (table6), optionmenuMbd, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, optionmenuMbd, _("Macroblock decision algorithm (high quality mode), encode each macroblock in all modes and choose the best"), NULL);

  menu9 = gtk_menu_new ();

  _0__mbcmp1 = gtk_menu_item_new_with_mnemonic (_("0: MB comparison"));
  gtk_widget_show (_0__mbcmp1);
  gtk_container_add (GTK_CONTAINER (menu9), _0__mbcmp1);

  _1__fewest_bits1 = gtk_menu_item_new_with_mnemonic (_("1: Fewest bits (vhq)"));
  gtk_widget_show (_1__fewest_bits1);
  gtk_container_add (GTK_CONTAINER (menu9), _1__fewest_bits1);

  _2__rate_distortion1 = gtk_menu_item_new_with_mnemonic (_("2: Rate distortion"));
  gtk_widget_show (_2__rate_distortion1);
  gtk_container_add (GTK_CONTAINER (menu9), _2__rate_distortion1);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenuMbd), menu9);

  hbox28 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox28);
  gtk_table_attach (GTK_TABLE (table6), hbox28, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  radiobuttonH263 = gtk_radio_button_new_with_mnemonic (NULL, _("_H.263"));
  gtk_widget_show (radiobuttonH263);
  gtk_box_pack_start (GTK_BOX (hbox28), radiobuttonH263, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobuttonH263, _("H.263 quantizers smooth the picture (useful at lower bitrates)"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonH263), radiobuttonH263_group);
  radiobuttonH263_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonH263));

  radiobuttonMpeg = gtk_radio_button_new_with_mnemonic (NULL, _("_MPEG"));
  gtk_widget_show (radiobuttonMpeg);
  gtk_box_pack_start (GTK_BOX (hbox28), radiobuttonMpeg, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, radiobuttonMpeg, _("MPEG quantizers produce sharper picture (useful at higher bitrates)"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonMpeg), radiobuttonH263_group);
  radiobuttonH263_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonMpeg));

  hbox29 = gtk_hbox_new (FALSE, 10);
  gtk_widget_show (hbox29);
  gtk_table_attach (GTK_TABLE (table6), hbox29, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  spinbuttonIMin_adj = gtk_adjustment_new (2, 1, 31, 1, 10, 10);
  spinbuttonIMin = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonIMin_adj), 1, 0);
  gtk_widget_show (spinbuttonIMin);
  gtk_box_pack_start (GTK_BOX (hbox29), spinbuttonIMin, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonIMin, _("Minimum quantizer, 1 is not recommended, should be 2 for normal MPEG-4 encoding"), NULL);

  label24 = gtk_label_new (_("Max:"));
  gtk_widget_show (label24);
  gtk_box_pack_start (GTK_BOX (hbox29), label24, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label24), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  spinbuttonIMax_adj = gtk_adjustment_new (31, 1, 31, 1, 10, 10);
  spinbuttonIMax = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonIMax_adj), 1, 0);
  gtk_widget_show (spinbuttonIMax);
  gtk_box_pack_start (GTK_BOX (hbox29), spinbuttonIMax, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonIMax, _("Maximum quantizer, 10-31 should be a sane range"), NULL);

  checkbuttonTrellis = gtk_check_button_new_with_mnemonic (_("_Trellis quantization"));
  gtk_widget_show (checkbuttonTrellis);
  gtk_box_pack_start (GTK_BOX (vbox10), checkbuttonTrellis, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonTrellis, _("Trellis searched quantization will find the optimal encoding for each 8x8 block"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonTrellis), TRUE);

  checkbuttonCbp = gtk_check_button_new_with_mnemonic (_("Rate distorted CBP"));
  gtk_widget_show (checkbuttonCbp);
  gtk_box_pack_start (GTK_BOX (vbox10), checkbuttonCbp, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonCbp, _("Rate distorted optimal coded block pattern"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonCbp), TRUE);

  label3 = gtk_label_new (_("Quantization"));
  gtk_widget_show (label3);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  vbox11 = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (vbox11);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox11);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox11,
                                      FALSE, FALSE, GTK_PACK_START);
  gtk_container_set_border_width (GTK_CONTAINER (vbox11), 5);

  table4 = gtk_table_new (5, 2, FALSE);
  gtk_widget_show (table4);
  gtk_box_pack_start (GTK_BOX (vbox11), table4, FALSE, FALSE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table4), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table4), 10);

  spinbuttonDarkMask_adj = gtk_adjustment_new (0.01, 0, 1, 0.01, 10, 10);
  spinbuttonDarkMask = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonDarkMask_adj), 1, 2);
  gtk_widget_show (spinbuttonDarkMask);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonDarkMask, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonDarkMask, _("Sane range: 0-0.3. Large values might look good on some monitors, but horrible on others (TV, LCD...)"), NULL);

  spinbuttonLumiMask_adj = gtk_adjustment_new (0.05, 0, 1, 0.01, 10, 10);
  spinbuttonLumiMask = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonLumiMask_adj), 1, 2);
  gtk_widget_show (spinbuttonLumiMask);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonLumiMask, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonLumiMask, _("Sane range: 0-0.3. Large values might look good on some monitors, but horrible on others (TV, LCD...)"), NULL);

  spinbuttonScplxMask_adj = gtk_adjustment_new (0.1, 0, 1, 0.01, 10, 10);
  spinbuttonScplxMask = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonScplxMask_adj), 1, 2);
  gtk_widget_show (spinbuttonScplxMask);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonScplxMask, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonScplxMask, _("Sane range: 0-0.5. Large values help against blockiness, but may introduce ringing/mosquito noise artifacts on sharp edges"), NULL);

  spinbuttonTcplxMask_adj = gtk_adjustment_new (0, 0, 1, 0.01, 10, 10);
  spinbuttonTcplxMask = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTcplxMask_adj), 1, 2);
  gtk_widget_show (spinbuttonTcplxMask);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonTcplxMask, 1, 2, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonDarkMask = gtk_check_button_new_with_mnemonic (_("Darkness masking:"));
  gtk_widget_show (checkbuttonDarkMask);
  gtk_table_attach (GTK_TABLE (table4), checkbuttonDarkMask, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonDarkMask), TRUE);

  checkbuttonScplxMask = gtk_check_button_new_with_mnemonic (_("Spatial complexity masking:"));
  gtk_widget_show (checkbuttonScplxMask);
  gtk_table_attach (GTK_TABLE (table4), checkbuttonScplxMask, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonScplxMask), TRUE);

  checkbuttonTcplxMask = gtk_check_button_new_with_mnemonic (_("Temporal complexity masking:"));
  gtk_widget_show (checkbuttonTcplxMask);
  gtk_table_attach (GTK_TABLE (table4), checkbuttonTcplxMask, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonLumiMask = gtk_check_button_new_with_mnemonic (_("Lumi masking:"));
  gtk_widget_show (checkbuttonLumiMask);
  gtk_table_attach (GTK_TABLE (table4), checkbuttonLumiMask, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonLumiMask), TRUE);

  spinbuttonPMask_adj = gtk_adjustment_new (0, 0, 1, 0.1, 10, 10);
  spinbuttonPMask = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonPMask_adj), 1, 2);
  gtk_widget_show (spinbuttonPMask);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonPMask, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonPMask = gtk_check_button_new_with_mnemonic (_("P block masking:"));
  gtk_widget_show (checkbuttonPMask);
  gtk_table_attach (GTK_TABLE (table4), checkbuttonPMask, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonNaq = gtk_check_button_new_with_mnemonic (_("Normalize adaptive quantization"));
  gtk_widget_show (checkbuttonNaq);
  gtk_box_pack_start (GTK_BOX (vbox11), checkbuttonNaq, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonNaq, _("Attempt  to adjust the per-MB quantizers to maintain the proper average when using adaptive quantization (*masking)"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonNaq), TRUE);

  frame11 = gtk_frame_new (NULL);
  gtk_widget_show (frame11);
  gtk_box_pack_start (GTK_BOX (vbox11), frame11, FALSE, FALSE, 5);

  table5 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table5);
  gtk_container_add (GTK_CONTAINER (frame11), table5);
  gtk_container_set_border_width (GTK_CONTAINER (table5), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table5), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table5), 10);

  spinbuttonLumaElim_adj = gtk_adjustment_new (2, 0, 1000, 1, 10, 10);
  spinbuttonLumaElim = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonLumaElim_adj), 1, 0);
  gtk_widget_show (spinbuttonLumaElim);
  gtk_table_attach (GTK_TABLE (table5), spinbuttonLumaElim, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  spinbuttonChromaElim_adj = gtk_adjustment_new (7, 0, 1000, 1, 10, 10);
  spinbuttonChromaElim = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonChromaElim_adj), 1, 0);
  gtk_widget_show (spinbuttonChromaElim);
  gtk_table_attach (GTK_TABLE (table5), spinbuttonChromaElim, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  checkbuttonLumaElim = gtk_check_button_new_with_mnemonic (_("Luminance:"));
  gtk_widget_show (checkbuttonLumaElim);
  gtk_table_attach (GTK_TABLE (table5), checkbuttonLumaElim, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonLumaElim), TRUE);

  checkbuttonChromaElim = gtk_check_button_new_with_mnemonic (_("Chrominance:"));
  gtk_widget_show (checkbuttonChromaElim);
  gtk_table_attach (GTK_TABLE (table5), checkbuttonChromaElim, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonChromaElim), TRUE);

  checkbuttonLumaElimDC = gtk_check_button_new_with_mnemonic (_("DC"));
  gtk_widget_show (checkbuttonLumaElimDC);
  gtk_table_attach (GTK_TABLE (table5), checkbuttonLumaElimDC, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonLumaElimDC, _("Consider also the dc coefficient"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonLumaElimDC), TRUE);

  checkbuttonChromaElimDC = gtk_check_button_new_with_mnemonic (_("DC"));
  gtk_widget_show (checkbuttonChromaElimDC);
  gtk_table_attach (GTK_TABLE (table5), checkbuttonChromaElimDC, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonChromaElimDC, _("Consider also the dc coefficient"), NULL);

  label59 = gtk_label_new (_("Single coefficient elimination threshold"));
  gtk_widget_show (label59);
  gtk_frame_set_label_widget (GTK_FRAME (frame11), label59);
  gtk_label_set_justify (GTK_LABEL (label59), GTK_JUSTIFY_LEFT);

  label53 = gtk_label_new (_("Masking"));
  gtk_widget_show (label53);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label53);
  gtk_label_set_justify (GTK_LABEL (label53), GTK_JUSTIFY_LEFT);

  table8 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table8);
  gtk_container_add (GTK_CONTAINER (notebook1), table8);
  gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), table8,
                                      FALSE, FALSE, GTK_PACK_START);
  gtk_container_set_border_width (GTK_CONTAINER (table8), 5);
  gtk_table_set_row_spacings (GTK_TABLE (table8), 5);
  gtk_table_set_col_spacings (GTK_TABLE (table8), 10);

  label64 = gtk_label_new (_("Filesize tolerance (kb):"));
  gtk_widget_show (label64);
  gtk_table_attach (GTK_TABLE (table8), label64, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label64), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label64), 0, 0.5);

  label65 = gtk_label_new (_("Quantizer compression:"));
  gtk_widget_show (label65);
  gtk_table_attach (GTK_TABLE (table8), label65, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label65), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label65), 0, 0.5);

  label66 = gtk_label_new (_("Quantizer blur:"));
  gtk_widget_show (label66);
  gtk_table_attach (GTK_TABLE (table8), label66, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label66), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label66), 0, 0.5);

  label67 = gtk_label_new (_("Max quantizer difference:"));
  gtk_widget_show (label67);
  gtk_table_attach (GTK_TABLE (table8), label67, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label67), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label67), 0, 0.5);

  spinbuttonMaxQDiff_adj = gtk_adjustment_new (3, 1, 31, 1, 10, 10);
  spinbuttonMaxQDiff = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxQDiff_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxQDiff);
  gtk_table_attach (GTK_TABLE (table8), spinbuttonMaxQDiff, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxQDiff, _("Maximum  quantizer difference between I or P frames"), NULL);

  spinbuttonQBlur_adj = gtk_adjustment_new (0.5, 0, 1, 0.1, 10, 10);
  spinbuttonQBlur = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQBlur_adj), 1, 1);
  gtk_widget_show (spinbuttonQBlur);
  gtk_table_attach (GTK_TABLE (table8), spinbuttonQBlur, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonQBlur, _("Quantizer  gaussian blur, larger values will average the quantizer more over time (slower change)"), NULL);

  spinbuttonQComp_adj = gtk_adjustment_new (0.5, 0, 1, 1, 10, 10);
  spinbuttonQComp = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQComp_adj), 1, 1);
  gtk_widget_show (spinbuttonQComp);
  gtk_table_attach (GTK_TABLE (table8), spinbuttonQComp, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryRateTol = gtk_entry_new ();
  gtk_widget_show (entryRateTol);
  gtk_table_attach (GTK_TABLE (table8), entryRateTol, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, entryRateTol, _("Filesize tolerance (1 kb = 1000 bits). 1000-100000 is a sane range"), NULL);
  gtk_entry_set_text (GTK_ENTRY (entryRateTol), _("8000"));

  labelRateTol = gtk_label_new (_("Ratecontrol"));
  gtk_widget_show (labelRateTol);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), labelRateTol);
  gtk_label_set_justify (GTK_LABEL (labelRateTol), GTK_JUSTIFY_LEFT);

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
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, frame10, "frame10");
  GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuType, "optionmenuType");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, one_pass_cbr1, "one_pass_cbr1");
  GLADE_HOOKUP_OBJECT (dialog1, one_pass_quantizer1, "one_pass_quantizer1");
  GLADE_HOOKUP_OBJECT (dialog1, two_pass1, "two_pass1");
  GLADE_HOOKUP_OBJECT (dialog1, same_qz_as_input1, "same_qz_as_input1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQuant, "spinbuttonQuant");
  GLADE_HOOKUP_OBJECT (dialog1, entryEntry, "entryEntry");
  GLADE_HOOKUP_OBJECT (dialog1, label51, "label51");
  GLADE_HOOKUP_OBJECT (dialog1, hbox7, "hbox7");
  GLADE_HOOKUP_OBJECT (dialog1, vbox4, "vbox4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonInterlaced, "checkbuttonInterlaced");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonGreyScale, "checkbuttonGreyScale");
  GLADE_HOOKUP_OBJECT (dialog1, hbox20, "hbox20");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonAspect, "checkbuttonAspect");
  GLADE_HOOKUP_OBJECT (dialog1, entryAspectX, "entryAspectX");
  GLADE_HOOKUP_OBJECT (dialog1, label40, "label40");
  GLADE_HOOKUP_OBJECT (dialog1, entryAspectY, "entryAspectY");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, vbox6, "vbox6");
  GLADE_HOOKUP_OBJECT (dialog1, table7, "table7");
  GLADE_HOOKUP_OBJECT (dialog1, label41, "label41");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, hbox30, "hbox30");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton4MV, "checkbutton4MV");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonMV0, "checkbuttonMV0");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuMS, "optionmenuMS");
  GLADE_HOOKUP_OBJECT (dialog1, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (dialog1, _0___none1, "_0___none1");
  GLADE_HOOKUP_OBJECT (dialog1, _1___full1, "_1___full1");
  GLADE_HOOKUP_OBJECT (dialog1, _2___log2, "_2___log2");
  GLADE_HOOKUP_OBJECT (dialog1, _3___phods1, "_3___phods1");
  GLADE_HOOKUP_OBJECT (dialog1, _4___epzs1, "_4___epzs1");
  GLADE_HOOKUP_OBJECT (dialog1, _5___x1, "_5___x1");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonIMaxPeriod, "spinbuttonIMaxPeriod");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, hbox11, "hbox11");
  GLADE_HOOKUP_OBJECT (dialog1, label18, "label18");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBFrame, "spinbuttonBFrame");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonQPel, "checkbuttonQPel");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonGMC, "checkbuttonGMC");
  GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog1, hbox24, "hbox24");
  GLADE_HOOKUP_OBJECT (dialog1, frame8, "frame8");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label43, "label43");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuPrecmp, "optionmenuPrecmp");
  GLADE_HOOKUP_OBJECT (dialog1, menu4, "menu4");
  GLADE_HOOKUP_OBJECT (dialog1, _0__sad1, "_0__sad1");
  GLADE_HOOKUP_OBJECT (dialog1, _1__sse1, "_1__sse1");
  GLADE_HOOKUP_OBJECT (dialog1, _2__satd1, "_2__satd1");
  GLADE_HOOKUP_OBJECT (dialog1, _3__dct1, "_3__dct1");
  GLADE_HOOKUP_OBJECT (dialog1, _4__psnr1, "_4__psnr1");
  GLADE_HOOKUP_OBJECT (dialog1, _5__bit1, "_5__bit1");
  GLADE_HOOKUP_OBJECT (dialog1, _6__rd1, "_6__rd1");
  GLADE_HOOKUP_OBJECT (dialog1, _7__zero1, "_7__zero1");
  GLADE_HOOKUP_OBJECT (dialog1, label44, "label44");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuCmp, "optionmenuCmp");
  GLADE_HOOKUP_OBJECT (dialog1, menu5, "menu5");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem1, "menuitem1");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem2, "menuitem2");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem3, "menuitem3");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem4, "menuitem4");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem5, "menuitem5");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem6, "menuitem6");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem7, "menuitem7");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem8, "menuitem8");
  GLADE_HOOKUP_OBJECT (dialog1, label45, "label45");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuSubcmp, "optionmenuSubcmp");
  GLADE_HOOKUP_OBJECT (dialog1, menu6, "menu6");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem9, "menuitem9");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem10, "menuitem10");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem11, "menuitem11");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem12, "menuitem12");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem13, "menuitem13");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem14, "menuitem14");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem15, "menuitem15");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem16, "menuitem16");
  GLADE_HOOKUP_OBJECT (dialog1, label68, "label68");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuMBcmp, "optionmenuMBcmp");
  GLADE_HOOKUP_OBJECT (dialog1, menu10, "menu10");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem28, "menuitem28");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem29, "menuitem29");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem30, "menuitem30");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem31, "menuitem31");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem32, "menuitem32");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem33, "menuitem33");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem34, "menuitem34");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem35, "menuitem35");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonPrecmpChroma, "checkbuttonPrecmpChroma");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCmpChroma, "checkbuttonCmpChroma");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonSubcmpChroma, "checkbuttonSubcmpChroma");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonMBcmpChroma, "checkbuttonMBcmpChroma");
  GLADE_HOOKUP_OBJECT (dialog1, label49, "label49");
  GLADE_HOOKUP_OBJECT (dialog1, frame9, "frame9");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, label47, "label47");
  GLADE_HOOKUP_OBJECT (dialog1, label48, "label48");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuPreDia, "optionmenuPreDia");
  GLADE_HOOKUP_OBJECT (dialog1, menu7, "menu7");
  GLADE_HOOKUP_OBJECT (dialog1, _3__adaptive_with_size_1, "_3__adaptive_with_size_1");
  GLADE_HOOKUP_OBJECT (dialog1, _2__adaptive_with_size_1, "_2__adaptive_with_size_1");
  GLADE_HOOKUP_OBJECT (dialog1, experimental1, "experimental1");
  GLADE_HOOKUP_OBJECT (dialog1, _1__normal_with_size_1__epzs_1, "_1__normal_with_size_1__epzs_1");
  GLADE_HOOKUP_OBJECT (dialog1, _2__normal_with_size_1, "_2__normal_with_size_1");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuDia, "optionmenuDia");
  GLADE_HOOKUP_OBJECT (dialog1, menu8, "menu8");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem23, "menuitem23");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem24, "menuitem24");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem25, "menuitem25");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem26, "menuitem26");
  GLADE_HOOKUP_OBJECT (dialog1, menuitem27, "menuitem27");
  GLADE_HOOKUP_OBJECT (dialog1, label50, "label50");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, vbox10, "vbox10");
  GLADE_HOOKUP_OBJECT (dialog1, table6, "table6");
  GLADE_HOOKUP_OBJECT (dialog1, label19, "label19");
  GLADE_HOOKUP_OBJECT (dialog1, label52, "label52");
  GLADE_HOOKUP_OBJECT (dialog1, label62, "label62");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenuMbd, "optionmenuMbd");
  GLADE_HOOKUP_OBJECT (dialog1, menu9, "menu9");
  GLADE_HOOKUP_OBJECT (dialog1, _0__mbcmp1, "_0__mbcmp1");
  GLADE_HOOKUP_OBJECT (dialog1, _1__fewest_bits1, "_1__fewest_bits1");
  GLADE_HOOKUP_OBJECT (dialog1, _2__rate_distortion1, "_2__rate_distortion1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox28, "hbox28");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonH263, "radiobuttonH263");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonMpeg, "radiobuttonMpeg");
  GLADE_HOOKUP_OBJECT (dialog1, hbox29, "hbox29");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonIMin, "spinbuttonIMin");
  GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonIMax, "spinbuttonIMax");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonTrellis, "checkbuttonTrellis");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCbp, "checkbuttonCbp");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, vbox11, "vbox11");
  GLADE_HOOKUP_OBJECT (dialog1, table4, "table4");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonDarkMask, "spinbuttonDarkMask");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonLumiMask, "spinbuttonLumiMask");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonScplxMask, "spinbuttonScplxMask");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTcplxMask, "spinbuttonTcplxMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDarkMask, "checkbuttonDarkMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonScplxMask, "checkbuttonScplxMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonTcplxMask, "checkbuttonTcplxMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonLumiMask, "checkbuttonLumiMask");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonPMask, "spinbuttonPMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonPMask, "checkbuttonPMask");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonNaq, "checkbuttonNaq");
  GLADE_HOOKUP_OBJECT (dialog1, frame11, "frame11");
  GLADE_HOOKUP_OBJECT (dialog1, table5, "table5");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonLumaElim, "spinbuttonLumaElim");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonChromaElim, "spinbuttonChromaElim");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonLumaElim, "checkbuttonLumaElim");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChromaElim, "checkbuttonChromaElim");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonLumaElimDC, "checkbuttonLumaElimDC");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChromaElimDC, "checkbuttonChromaElimDC");
  GLADE_HOOKUP_OBJECT (dialog1, label59, "label59");
  GLADE_HOOKUP_OBJECT (dialog1, label53, "label53");
  GLADE_HOOKUP_OBJECT (dialog1, table8, "table8");
  GLADE_HOOKUP_OBJECT (dialog1, label64, "label64");
  GLADE_HOOKUP_OBJECT (dialog1, label65, "label65");
  GLADE_HOOKUP_OBJECT (dialog1, label66, "label66");
  GLADE_HOOKUP_OBJECT (dialog1, label67, "label67");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxQDiff, "spinbuttonMaxQDiff");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQBlur, "spinbuttonQBlur");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQComp, "spinbuttonQComp");
  GLADE_HOOKUP_OBJECT (dialog1, entryRateTol, "entryRateTol");
  GLADE_HOOKUP_OBJECT (dialog1, labelRateTol, "labelRateTol");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

  return dialog1;
}

