/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"

#include <ADM_assert.h>

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"

#include "ADM_audiocodec/ADM_audiocodeclist.h"

#include "ADM_gui2/support.h"


#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)
  
static GtkWidget	*create_dialogAudioCodec (void);

 static AUDIOENCODER findCodec( void );
 static GtkWidget *dialog;
 static void okCallback(GtkButton * button, gpointer user_data);
 
  
 void okCallback(GtkButton * button, gpointer user_data)
{
	AUDIOENCODER cur;


		UNUSED_ARG(button);
		UNUSED_ARG(user_data);

		cur=findCodec();
		audioCodecSetcodec(cur);
		audioCodecConfigure();

}

uint8_t DIA_audioCodec( AUDIOENCODER *codec )
{

	uint8_t ret=0;
	AUDIOENCODER old=*codec;
	
	dialog=create_dialogAudioCodec();
	gtk_transient(dialog);

	
	// now set the input one
	for(uint32_t i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
		if(*codec==myCodecList[i].codec)
			{
				// set 
				gtk_option_menu_set_history(GTK_OPTION_MENU(lookup_widget(dialog,"optionmenu_CodecList")), i);
			}
	if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
	{
		
			*codec=findCodec();
			ret=1;
	
	}
	else
	{
		*codec=old;
	}
	gtk_widget_destroy(dialog);
	
	return ret;
} 

AUDIOENCODER findCodec( void )
{
uint8_t j;
			j=getRangeInMenu(lookup_widget(dialog,"optionmenu_CodecList"));
		
			if(j>=sizeof(myCodecList)/sizeof(CODECLIST)) ADM_assert(0);
			return myCodecList[j].codec;
			

}
//-------------------------

GtkWidget	*create_dialogAudioCodec (void)
{
  GtkWidget *dialogAudioCodec;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *optionmenu_CodecList;
  GtkWidget *menu1;
  GtkWidget *uncompressed;
  GtkWidget *ffmpeg_mpeg_audio;
  GtkWidget *ffmpeg_ac3_2_channels1;
  #ifdef HAVE_LIBMP3LAME
  GtkWidget *lame_mp1;
  #endif
    #ifdef USE_VORBIS
  GtkWidget *vorbis;
  #endif

#ifdef USE_FAAC
  GtkWidget *faac;
#endif
  GtkWidget *toolame,*libtoolame;
  GtkWidget *buttonConfigure;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkWidget *acodec[10];
  int acodecNb=0;

  dialogAudioCodec = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialogAudioCodec), _("AudioCodec"));

  dialog_vbox1 = GTK_DIALOG (dialogAudioCodec)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  optionmenu_CodecList = gtk_option_menu_new ();
  gtk_widget_show (optionmenu_CodecList);
  gtk_box_pack_start (GTK_BOX (vbox1), optionmenu_CodecList, FALSE, FALSE, 0);

  menu1 = gtk_menu_new ();

 /***************/
        for(int i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
        {
                acodec[i]=gtk_menu_item_new_with_mnemonic(myCodecList[i].menuName);
                gtk_widget_show(acodec[i]);
                gtk_container_add(GTK_CONTAINER(menu1),acodec[i]);
        }  
 /***************/ 
  gtk_option_menu_set_menu (GTK_OPTION_MENU (optionmenu_CodecList), menu1);

  buttonConfigure = gtk_button_new_with_mnemonic (_("Configure codec"));
  gtk_widget_show (buttonConfigure);
  gtk_box_pack_start (GTK_BOX (vbox1), buttonConfigure, FALSE, FALSE, 0);

  dialog_action_area1 = GTK_DIALOG (dialogAudioCodec)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialogAudioCodec), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialogAudioCodec), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (dialogAudioCodec, dialogAudioCodec, "dialogAudioCodec");
  GLADE_HOOKUP_OBJECT_NO_REF (dialogAudioCodec, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, optionmenu_CodecList, "optionmenu_CodecList");
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, menu1, "menu1");
/****/
   for(int i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
        {
                GLADE_HOOKUP_OBJECT (dialogAudioCodec, acodec[i],myCodecList[i].name );
        }

/****/
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, buttonConfigure, "buttonConfigure");
  GLADE_HOOKUP_OBJECT_NO_REF (dialogAudioCodec, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialogAudioCodec, okbutton1, "okbutton1");
	 gtk_signal_connect(GTK_OBJECT(buttonConfigure), "clicked",
                      GTK_SIGNAL_FUNC(okCallback),                   (void *) 0);
  return dialogAudioCodec;
}

