/***************************************************************************
                          ADM_guiSRT.cpp  -  description
                             -------------------
    begin                : Wed Dec 18 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <iconv.h>


#include "fourcc.h"
#include "avio.hxx"

#ifdef USE_FREETYPE

#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"



#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidFont.h"
#include "ADM_video/ADM_vidSRT.h"

#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"

#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_colorspace/colorspace.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_osSupport/ADM_debug.h"

#include "prototype.h"
#include "prefs.h"
#include <ADM_assert.h>

typedef struct unicd
{
	char *display,*name;
}unicd;
#define CHECKSPIN(x,y) y= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(x)) ;
//
//	If you want to add languages just add them here, nothing else to do
//	First is the displayed name, second is the iconv --list name use internally
//
static unicd  names[]=
{
	{(char *)"Ascii"	,(char *)"ISO-8859-1"},
	{(char *)"Cyrillic"	,(char *)"WINDOWS-1251"}, // ru
	{(char *)"Czech"	,(char *)"ISO-8859-2"},	// cz
	{(char *)"German"	,(char *)"ISO-8859-9"}		// german ?
	,{(char *)"Slovene"	,(char *)"CP1250"}		// UTF8
	,{(char *)"UTF8"	,(char *)"UTF8"}		// UTF8
	,{(char *)"Chinese Traditionnal(Big5)"	,(char *)"CP950"}		// UTF8
	,{(char *)"Chinese Simplified (GB2312)"	,(char *)"CP936"}		// UTF8
};



static void on_callback_sub(GtkButton * button, gpointer user_data);
static void on_callback_font(GtkButton * button, gpointer user_data);
static void on_callback_color(GtkButton * button, gpointer user_data);

static gboolean on_slider( void );

static void indexSUB(char *name);
static void update(uint32_t value);
static void read(void);

static GtkWidget* create_dialog1 (void);
static GtkWidget *dialog=NULL;
static GtkWidget *drawing=NULL;
static GtkAdjustment *sliderAdjustment;

static uint32_t glob_font=0;
static uint32_t glob_base=0;


static int  GUI_subtitleParam(char *font,char *sub,int  *charset,int *size,uint32_t *baseline,
                              int32_t *coly,int32_t *colu,int32_t *colv,uint32_t *autocut,int32_t *delay
			      ,uint32_t *bg);

static gboolean gui_draw( void );
static void draw( void);

#define MAX_STRING 300
static char subString[MAX_STRING];
static char fontString[MAX_STRING];

static ADMImage  *sourceImage=NULL;
static uint8_t *targetImage=NULL;
static uint8_t *targetImageRGB=NULL;

static uint32_t _w,_h;
static int32_t myY,myU,myV;

extern int DIA_colorSel(uint8_t *r, uint8_t *g, uint8_t *b);
ColYuvRgb *rgbConv=NULL;
uint8_t ADMVideoSubtitle::configure(AVDMGenericVideoStream *instream)
{
UNUSED_ARG(instream);
//char c;
uint8_t ret=0;
int charset=0;
uint32_t l,f;

		// look up old one
		if(_conf->_charset)
		{
			for(uint32_t j=0;j<sizeof(names)/sizeof(unicd);j++)
			 if(!strcmp(_conf->_charset,names[j].name)) charset=j;

		}
		_w=_info.width;
		_h=_info.height;

		sourceImage		=new ADMImage(_w,_h);//uint8_t[_w*_h*2];
		targetImage		=new uint8_t[_w*_h*2];
		targetImageRGB		=new uint8_t[_w*_h*4];
		
        rgbConv=new ColYuvRgb(_w,_h);
        rgbConv->reset(_w,_h);

		ADM_assert(instream->getFrameNumberNoAlloc(curframe,
						&l,
						sourceImage,
						&f));
				
		 if(GUI_subtitleParam(	
                        (char *)_conf->_fontname,
                        (char *)_conf->_subname,
                        &charset,
                        (int *)&_conf->_fontsize,
                        &(_conf->_baseLine),
                        &(_conf->_Y_percent),
                        &(_conf->_U_percent),
                        &(_conf->_V_percent),
                        &(_conf->_selfAdjustable),
                        &(_conf->_delay),
                        &(_conf->_useBackgroundColor)
							))
		 {
                    printf("\n Font : %s", _conf->_fontname);
                    printf("\n Sub  : %s", _conf->_subname);
                    printf("\n Font size : %ld",_conf->_fontsize);
                    printf("\n Charset : %d",charset);
                    printf("\n Y : %ld",_conf->_Y_percent);
                    printf("\n U : %ld",_conf->_U_percent);
                    printf("\n V : %ld",_conf->_V_percent);
                    if(charset<(int)(sizeof(names)/sizeof(unicd)))
                    {
                        strcpy(_conf->_charset,names[charset].name    );
                        printf("\n Charset : %s\n",   names[charset].name    );
                    }

                    loadSubtitle();
                    loadFont();
				
                    prefs->set(FILTERS_SUBTITLE_FONTNAME,
                            (ADM_filename *)_conf->_fontname);
                    prefs->set(FILTERS_SUBTITLE_CHARSET,
                            _conf->_charset);
                    prefs->set(FILTERS_SUBTITLE_FONTSIZE,_conf->_fontsize);
                    prefs->set(FILTERS_SUBTITLE_YPERCENT,_conf->_Y_percent);
                    prefs->set(FILTERS_SUBTITLE_UPERCENT,_conf->_U_percent);
                    prefs->set(FILTERS_SUBTITLE_VPERCENT,_conf->_V_percent);
                    prefs->set(FILTERS_SUBTITLE_SELFADJUSTABLE,
                                _conf->_selfAdjustable);
                    prefs->set(FILTERS_SUBTITLE_USEBACKGROUNDCOLOR,
                                _conf->_useBackgroundColor);
                    ret=1;
		}
		delete  sourceImage;
		delete [] targetImage;
		delete [] targetImageRGB;

		sourceImage=NULL;
		targetImage=targetImageRGB=NULL;
		delete rgbConv;
		rgbConv=NULL;
		return ret;

}

int  GUI_subtitleParam(char *font,char *sub,int  *charset,int *size,uint32_t *baseline,
                       int32_t *coly,int32_t *colu,int32_t *colv,uint32_t *autocut,int32_t *delay,
		       uint32_t *bgcolor)
{

  

int ret=0;
gint answer;
#define WID(x) lookup_widget(dialog,#x)
#define CHECK_GET(x,y) {y=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(x)));}
#define CHECK_SET(x,y) {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(x)),y);}		

	printf("In:Y:%d U:%d V:%d\n",*coly,*colu,*colv);

       	dialog = create_dialog1();
	    gtk_register_dialog(dialog);
	
	if(sub)    	strcpy( subString, sub);
	if(font) 	 strcpy( fontString, font);

	myY=*coly;
	myU=*colu;
	myV=*colv;



        gtk_label_set_text(GTK_LABEL(WID(label_sub)),sub);
        gtk_label_set_text(GTK_LABEL(WID(label_font)),font);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton_fontsize)),(gfloat)*size) ;
	CHECK_SET(checkbutton_autosplit,*autocut);
	CHECK_SET(checkbuttonbg,*bgcolor);
	
	gtk_write_entry(WID(entry_delay), *delay);
	
	int nb=sizeof(names)/sizeof(unicd);

	GtkWidget *w;
	for(uint32_t j=1;j<(uint32_t)nb;j++)
	{

 		w = gtk_menu_item_new_with_mnemonic ( names[j].display  );
  		gtk_widget_show (w);
  		gtk_container_add (GTK_CONTAINER (WID(menu1)),w);
	}

	 gtk_option_menu_set_history (GTK_OPTION_MENU (WID(optionmenu1)), *charset);

  	 gtk_signal_connect(GTK_OBJECT(WID(button_sub)), "clicked",
		       GTK_SIGNAL_FUNC(on_callback_sub), (void *) 1);
     	gtk_signal_connect(GTK_OBJECT(WID(button_font)), "clicked",
		       GTK_SIGNAL_FUNC(on_callback_font), (void *) 1);

 	gtk_signal_connect(GTK_OBJECT(WID(button_color)), "clicked",
		       GTK_SIGNAL_FUNC(on_callback_color), (void *) 1);

	// Draw a green square to show where it's gonna be
	CHECKSPIN( WID(spinbutton_fontsize),glob_font);


	drawing=WID(drawingarea1);

	gtk_widget_set_usize(drawing, _w,_h);

	gtk_widget_show(dialog);

	glob_font=*size;
	glob_base=*baseline;

	sliderAdjustment=gtk_range_get_adjustment (GTK_RANGE(WID(vscale1)));
  	gtk_adjustment_set_value( GTK_ADJUSTMENT(sliderAdjustment),(  gdouble  ) glob_base );
/*-------*/

	gtk_signal_connect(GTK_OBJECT(sliderAdjustment),"value_changed", \
		       GTK_SIGNAL_FUNC(on_slider), NULL);


/*-------*/


	update(*baseline);
	draw();

	 gtk_signal_connect(GTK_OBJECT(WID(drawingarea1)), "expose_event",
		       GTK_SIGNAL_FUNC(gui_draw),
		       NULL);

	ret=0;
	gtk_register_dialog(dialog);
	while( (answer=gtk_dialog_run(GTK_DIALOG(dialog)))==GTK_RESPONSE_APPLY)
	{
		read();
		update(glob_base);
		draw();
	}
	gtk_unregister_dialog(dialog);
	if(answer==GTK_RESPONSE_OK)
	{
				strcpy(sub,subString);
				strcpy(font,fontString);
				*charset= getRangeInMenu(WID(optionmenu1));;

          			read();
				*size=glob_font;
				*baseline=glob_base;

				*coly=myY;
				*colu=myU;
				*colv=myV;
				printf("Y:%d U:%d V:%d\n",*coly,*colu,*colv);
				CHECK_GET(checkbutton_autosplit,*autocut);
				CHECK_GET(checkbuttonbg,*bgcolor);
				*delay= (int32_t)gtk_read_entry(WID(entry_delay));
				ret=1;
	}
	gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	dialog=NULL;
	return ret;

}
void on_callback_color(GtkButton * button, gpointer user_data)
{
	uint8_t r,g,b;
	uint8_t y;
	int8_t u,v;

		y=(myY);
		u=(myU);
		v=(myV);


	aprintf("YUV: %d %d %d \n",y,u,v);
	COL_YuvToRgb(   y,  u,  v, &b,&g,&r);
	aprintf("RGB %d %d %d -->\n\n",r,g,b);


	if(DIA_colorSel(&r,&g,&b))
	{
		aprintf("RGB %d %d %d -->\n",r,g,b);
		COL_RgbToYuv(b,  g,  r, &y, &u,&v);
		aprintf("YUV:%d %d %d \n",y,u,v);
		myY=y;
		myU=(u);
		myV=(v);

		if(abs(myU)<2) myU=0;
		if(abs(myV)<2) myV=0;

	}

}

gboolean on_slider( void )
{
		read();
		update(glob_base);
		draw();
	return TRUE;
}
void on_callback_sub(GtkButton * button, gpointer user_data)
{
	      UNUSED_ARG(button);
	UNUSED_ARG(user_data);
        //GUI_FileSelRead("TTF font to use", fontCB);
        FileSel_SelectRead(_("Subtitle to load"), subString,
		MAX_STRING,subString);
	gtk_label_set_text(GTK_LABEL(WID(label_sub)),subString);
}

void on_callback_font(GtkButton * button, gpointer user_data)
{
        UNUSED_ARG(button);
	UNUSED_ARG(user_data);
        //GUI_FileSelRead("TTF font to use", fontCB);
        FileSel_SelectRead(_("TTF font to use"), fontString,
		MAX_STRING,fontString);
	gtk_label_set_text(GTK_LABEL(WID(label_font)),fontString);
	
}
/*--------------------------------------*/
gboolean gui_draw( void )
{
	draw();
	return true;
}
void draw (void  )
{
	GUI_RGBDisplay(targetImageRGB, _w,_h, (void *)drawing);
}

void read ( void )
{

	// Draw a green square to show where it's gonna be
	CHECKSPIN( WID(spinbutton_fontsize),glob_font);

	glob_base=(uint32_t)GTK_ADJUSTMENT(sliderAdjustment)->value;

	if(glob_base<1) glob_base=1;
	if(glob_base>(_h - glob_font*SRT_MAX_LINE)) glob_base=_h - glob_font*SRT_MAX_LINE;


}

/*
	Add our stuff and display the result
*/
void update(uint32_t value)
{
uint32_t h;
uint32_t page=_w*_h;
	//printf("\n Updating with %lu base line\n",value);
	// grab it
	
	memcpy(targetImage,YPLANE(sourceImage),(_w*_h));
	memcpy(targetImage+page,UPLANE(sourceImage),(_w*_h)>>2);
	memcpy(targetImage+((page*5)>>2),VPLANE(sourceImage),(_w*_h)>>2);
	
	h=glob_font;
	if(h>8) h-=4;

	for(uint32_t line=0;line<SRT_MAX_LINE;line++)
	{
		uint8_t *src=targetImage+(glob_base+line*glob_font)*_w;

		for(uint32_t y=0;y<h;y+=2)
		{
			memset(src,255,_w);
			src+=2*_w;
		}
	}
	// convert to rgb
	//ADM_assert(COL_yv12rgb(_w,_h,targetImage,targetImageRGB));
	rgbConv->scale(targetImage,targetImageRGB);

}


/*
	Almost straigh out of glade2

*/

//-------------------------------


GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *hbox1;
  GtkWidget *label_sub;
  GtkWidget *button_sub;
  GtkWidget *image1;
  GtkWidget *hbox2;
  GtkWidget *label_font;
  GtkWidget *button_font;
  GtkWidget *image2;
  GtkWidget *optionmenu1;
  GtkWidget *menu1;
  GtkWidget *enc_ascii;
  /*
  GtkWidget *enc_8859;
  GtkWidget *ebc_cyrillic;
  GtkWidget *enc_german;
  */
  GtkObject *spinbutton_fontsize_adj;
  GtkWidget *spinbutton_fontsize;
  GtkWidget *label5;
  GtkWidget *hbox4;
  GtkWidget *button_color;
  GtkWidget *checkbutton_autosplit;
  GtkWidget *label6;
  GtkWidget *entry_delay;
  GtkWidget *checkbuttonbg;
  GtkWidget *hbox3;
  GtkWidget *drawingarea1;
  GtkWidget *vscale1;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *applybutton1;
  GtkWidget *okbutton1;

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("Subtitle selector"));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  table1 = gtk_table_new (7, 2, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("Subtitle file "));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Font (TTF)"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new (_("Encoding "));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  label4 = gtk_label_new (_("Font Size"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label4), 0, 0.5);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_table_attach (GTK_TABLE (table1), hbox1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_sub = gtk_label_new (_("sub"));
  gtk_widget_show (label_sub);
  gtk_box_pack_start (GTK_BOX (hbox1), label_sub, TRUE, FALSE, 0);

  button_sub = gtk_button_new ();
  gtk_widget_show (button_sub);
  gtk_box_pack_start (GTK_BOX (hbox1), button_sub, FALSE, FALSE, 0);

  image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_container_add (GTK_CONTAINER (button_sub), image1);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_table_attach (GTK_TABLE (table1), hbox2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_font = gtk_label_new (_("font"));
  gtk_widget_show (label_font);
  gtk_box_pack_start (GTK_BOX (hbox2), label_font, TRUE, FALSE, 0);

  button_font = gtk_button_new ();
  gtk_widget_show (button_font);
  gtk_box_pack_start (GTK_BOX (hbox2), button_font, FALSE, FALSE, 0);

  image2 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_container_add (GTK_CONTAINER (button_font), image2);

  optionmenu1 = gtk_option_menu_new ();
  gtk_widget_show (optionmenu1);
  gtk_table_attach (GTK_TABLE (table1), optionmenu1, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  menu1 = gtk_menu_new ();

  enc_ascii = gtk_menu_item_new_with_mnemonic (_("Ascii"));
  gtk_widget_show (enc_ascii);
  gtk_container_add (GTK_CONTAINER (menu1), enc_ascii);
/*
  enc_8859 = gtk_menu_item_new_with_mnemonic (_("Iso 8859-1 (Czech...)"));
  gtk_widget_show (enc_8859);
  gtk_container_add (GTK_CONTAINER (menu1), enc_8859);

  ebc_cyrillic = gtk_menu_item_new_with_mnemonic (_("Cyrillic"));
  gtk_widget_show (ebc_cyrillic);
  gtk_container_add (GTK_CONTAINER (menu1), ebc_cyrillic);

  enc_german = gtk_menu_item_new_with_mnemonic (_("German"));
  gtk_widget_show (enc_german);
  gtk_container_add (GTK_CONTAINER (menu1), enc_german);
*/
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu1), menu1);

  spinbutton_fontsize_adj = gtk_adjustment_new (26, 8, 100, 1, 10, 10);
  spinbutton_fontsize = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_fontsize_adj), 1, 0);
  gtk_widget_show (spinbutton_fontsize);
  gtk_table_attach (GTK_TABLE (table1), spinbutton_fontsize, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton_fontsize), TRUE);

  label5 = gtk_label_new (_("Select color"));
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_table_attach (GTK_TABLE (table1), hbox4, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  button_color = gtk_button_new_with_mnemonic (_("Select"));
  gtk_widget_show (button_color);
  gtk_box_pack_start (GTK_BOX (hbox4), button_color, FALSE, FALSE, 0);

  checkbutton_autosplit = gtk_check_button_new_with_mnemonic (_("Auto split"));
  gtk_widget_show (checkbutton_autosplit);
  gtk_table_attach (GTK_TABLE (table1), checkbutton_autosplit, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label6 = gtk_label_new (_("Delay in ms"));
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  entry_delay = gtk_entry_new ();
  gtk_widget_show (entry_delay);
  gtk_table_attach (GTK_TABLE (table1), entry_delay, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (entry_delay, 90, -1);
  gtk_entry_set_text (GTK_ENTRY (entry_delay), _("0"));

  checkbuttonbg = gtk_check_button_new_with_mnemonic (_("Force background"));
  gtk_widget_show (checkbuttonbg);
  gtk_table_attach (GTK_TABLE (table1), checkbuttonbg, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  drawingarea1 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea1);
  gtk_box_pack_start (GTK_BOX (hbox3), drawingarea1, TRUE, TRUE, 0);
  gtk_widget_set_size_request (drawingarea1, 352, 288);

  vscale1 = gtk_vscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 1, 576, 1, 1, 1)));
  gtk_widget_show (vscale1);
  gtk_box_pack_start (GTK_BOX (hbox3), vscale1, FALSE, TRUE, 0);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  applybutton1 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (applybutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), applybutton1, GTK_RESPONSE_APPLY);
  GTK_WIDGET_SET_FLAGS (applybutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog1, "dialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, table1, "table1");
  GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
  GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
  GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
  GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, label_sub, "label_sub");
  GLADE_HOOKUP_OBJECT (dialog1, button_sub, "button_sub");
  GLADE_HOOKUP_OBJECT (dialog1, image1, "image1");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, label_font, "label_font");
  GLADE_HOOKUP_OBJECT (dialog1, button_font, "button_font");
  GLADE_HOOKUP_OBJECT (dialog1, image2, "image2");
  GLADE_HOOKUP_OBJECT (dialog1, optionmenu1, "optionmenu1");
  GLADE_HOOKUP_OBJECT (dialog1, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (dialog1, enc_ascii, "enc_ascii");
  /*
  GLADE_HOOKUP_OBJECT (dialog1, enc_8859, "enc_8859");
  GLADE_HOOKUP_OBJECT (dialog1, ebc_cyrillic, "ebc_cyrillic");
  GLADE_HOOKUP_OBJECT (dialog1, enc_german, "enc_german");
  */
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton_fontsize, "spinbutton_fontsize");
  GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
  GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (dialog1, button_color, "button_color");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton_autosplit, "checkbutton_autosplit");
  GLADE_HOOKUP_OBJECT (dialog1, label6, "label6");
  GLADE_HOOKUP_OBJECT (dialog1, entry_delay, "entry_delay");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonbg, "checkbuttonbg");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, drawingarea1, "drawingarea1");
  GLADE_HOOKUP_OBJECT (dialog1, vscale1, "vscale1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, applybutton1, "applybutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

  return dialog1;
}


#endif

