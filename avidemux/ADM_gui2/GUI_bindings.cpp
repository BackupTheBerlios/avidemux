/***************************************************************************
                          GUI_binding.cpp  -  description
                             -------------------
    begin                : Fri Jan 17 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 * GtkObject *sliderAdjustment=NULL;
 *sliderAdjustment=gtk_adjustment_new (52.1, 0, 99.99, 0.01, 0, 0);
 *sliderNavigate = gtk_hscale_new (GTK_ADJUSTMENT (sliderAdjustment));
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#  include <config.h>


#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ADM_assert.h>

#include "interface.h"
#include "support.h"
#include "math.h"
#include "default.h"
#include "ADM_gui2/GUI_render.h"
#include "gui_action.hxx"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_UI
#include "ADM_osSupport/ADM_debug.h"
#include "ADM_toolkit/toolkit_gtk.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_gui2/GUI_ui.h"

#include "ADM_toolkit/filesel.h"
#include "ADM_editor/ADM_Video.h"
#include "prefs.h"

#if (defined( HAVE_LIBESD) && defined(HAVE_ESD_H)) || \
 defined(OSS_SUPPORT) || defined (USE_ARTS) || \
  defined(USE_SDL) || defined(CONFIG_DARWIN) || \
  defined(CYG_MANGLING) || defined(ALSA_SUPPORT)
  
#define HAVE_AUDIO
#endif

#define WOD(x) lookup_widget (guiRootWindow,#x)

void frame2time(uint32_t frame, uint32_t fps, uint16_t * hh, uint16_t * mm,
	   uint16_t * ss, uint16_t * ms);

static GtkWidget *guiRootWindow=NULL;

static GtkWidget *guiDrawingArea=NULL;
static GtkWidget *guiSlider=NULL;

static GtkWidget *guiMarkA=NULL;
static GtkWidget *guiMarkB=NULL;
static GtkWidget *guiCurFrame=NULL;
static GtkWidget *guiTotalFrame=NULL;
static GtkWidget *guiCurTime=NULL;
static GtkWidget *guiTotalTime=NULL;

static GtkWidget *guiPreviewToggle=NULL;
static GtkWidget *guiOutputToggle=NULL;
static GtkWidget *guiAudioToggle=NULL;
static GtkWidget *guiVideoToggle=NULL;
static GdkCursor *guiCursorBusy=NULL;
static GdkCursor *guiCursorNormal=NULL;
static gint	  guiCursorEvtMask=0;
static void     volumeChange( void );

static char     *customNames[ADM_MAC_CUSTOM_SCRIPT];
static uint32_t ADM_nbCustom=0;
// heek !
static  GtkAdjustment *sliderAdjustment;
// Needed for DND
extern void A_openAvi (char *name);
extern void A_appendAvi (char *name);


static void on_audio_change(void);
static void on_video_change(void);
static int update_ui=0;

static void GUI_initCustom(void);
const char * GUI_getCustomScript(uint32_t nb);
#ifdef HAVE_AUDIO
extern uint8_t AVDM_setVolume(int volume);
#endif
#define AUDIO_WIDGET "comboboxAudio"
#define VIDEO_WIDGET "comboboxVideo"
#define FORMAT_WIDGET "comboboxFormat"
//
enum 
{
        TARGET_STRING,
        TARGET_ROOTWIN,
        TARGET_URL
};
enum 
{
   TARGET_URI_LIST
};
 
static GtkTargetEntry target_table[] = 
{
  { "STRING",     0, TARGET_STRING },
  { "text/plain", 0, TARGET_STRING },
  { "text/uri-list", 0, TARGET_URL },
  { "application/x-rootwin-drop", 0, TARGET_ROOTWIN }
};
// CYB 2005.02.22: DND (END)


static void DNDDataReceived( GtkWidget *widget, GdkDragContext *dc,
                             gint x, gint y, GtkSelectionData *selection_data, guint info, guint t);

extern aviInfo   *avifileinfo;
extern GtkWidget	*create_mainWindow (void);
extern void guiCallback(GtkMenuItem * menuitem, gpointer user_data);
extern void HandleAction(Action act);
extern gboolean UI_on_key_press(GtkWidget *widget, GdkEventKey* event, gpointer user_data);
extern void fileReadWrite(SELFILE_CB cb, int rw, char *name);

// To build vcodec
extern uint32_t encoderGetNbEncoder(void);
extern const char* encoderGetIndexedName(uint32_t i);
// To build a codec
uint32_t audioFilterGetNbEncoder(void);
const char* audioFilterGetIndexedName(uint32_t i);
//
static uint8_t  bindGUI( void );
static gboolean destroyCallback(GtkWidget * widget,	  GdkEvent * event, gpointer user_data);
static gboolean  on_drawingarea1_expose_event(GtkWidget * widget,  GdkEventExpose * event, gpointer user_data);
// Currentframe taking/loosing focus
static int  UI_grabFocus( void);
static int  UI_looseFocus( void);
static void GUI_initCursor( void );
 void UI_BusyCursor( void );
 void UI_NormalCursor( void );
// For checking if Slider shift key is pressed
gboolean SliderIsShifted = FALSE;
gboolean UI_SliderPressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean UI_SliderReleased(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
// Global
GtkAccelGroup *accel_group;
//
void guiCallback(GtkMenuItem * menuitem, gpointer user_data);

typedef struct buttonCallBack_S
{
	const char *name;
	const char *signal;
	Action action;

}buttonCallBack_S;




buttonCallBack_S buttonCallback[]=
{
	{"buttonPlay"			,"clicked"		,ACT_PlayAvi},
	{"buttonStop"			,"clicked"		,ACT_StopAvi},
	{"buttonPrevFrame"		,"clicked"		,ACT_PreviousFrame},
	{"buttonNextFrame"		,"clicked"		,ACT_NextFrame},
	{"buttonPrevKFrame"		,"clicked"		,ACT_PreviousKFrame},
	{"buttonNextKFrame"		,"clicked"		,ACT_NextKFrame},
	{"buttonMarkA"			,"clicked"		,ACT_MarkA},
	{"buttonMarkB"			,"clicked"		,ACT_MarkB},
	{"buttonBegin"			,"clicked"		,ACT_Begin},
	{"buttonEnd"			,"clicked"		,ACT_End},
	{"menutoolbuttonOpen"		,"clicked"		,ACT_OpenAvi},
	{"toolbuttonInfo"			,"clicked"		,ACT_AviInfo},
	{"toolbuttonSave"		,"clicked"		,ACT_SaveAvi},

	{"buttonFilters"		,"clicked"		,ACT_VideoParameter},
	{"buttonAudioFilter"		,"clicked"		,ACT_AudioFilters},
	{"buttonConfV"			,"clicked"		,ACT_VideoCodec},
	{"buttonConfA"			,"clicked"		,ACT_AudioCodec},

	{"buttonPrevBlack"		,"clicked"		,ACT_PrevBlackFrame},
	{"buttonNextBlack"		,"clicked"		,ACT_NextBlackFrame},
	{"buttonGotoA"			,"clicked"		,ACT_GotoMarkA},
	{"buttonGotoB"			,"clicked"		,ACT_GotoMarkB},	
	{"toolbuttonCalc"		,"clicked"		,ACT_Bitrate},	

	{"toggletoolbuttonPreview"	,"toggled"		,ACT_PreviewToggle},
	{"toggletoolbuttonOutput"      ,"toggled"		,ACT_OuputToggle},

	{"boxCurFrame"			,"editing_done"		,ACT_JumpToFrame},
	{"boxCurFrame"			,"activate"		,ACT_JumpToFrame},
	{"boxCurTime"			,"editing_done"		,ACT_TimeChanged},

        {"CheckButtonTimeshift"         ,"toggled"             ,ACT_TimeShift},
        {"spinbuttonTimeShift"          ,"value_changed"       ,ACT_TimeShift}
       // {"spinbuttonTimeShift"          ,"editing_done"       ,ACT_TimeShift}
  
};

///
///	Create main window and bind to it
///
uint8_t initGUI( void )
{
uint8_t ret=0;
		// create top window
		guiRootWindow=create_mainWindow();
		
		if(!guiRootWindow) return 0;
		gtk_register_dialog(guiRootWindow);
					
		// and seek global sub entity
		ret= bindGUI();		
		if(ret) gtk_widget_show(guiRootWindow);
                UI_purge();
		// Set it as always low level
		//gtk_window_set_keep_below(GTK_WINDOW(guiRootWindow), 1);
		renderInit();
		GUI_initCursor(  );


               
	return ret;
}
/**
    Get the custom entry 

*/
const char * GUI_getCustomScript(uint32_t nb)
{
    ADM_assert(nb<ADM_nbCustom);
    return customNames[nb];

}
/** 
	Set the parameter widget as transient 
	for the main window
*/
///_____________________________________________________
/// retrieve gtkwidget through their name
/// the aim is to avoid having to modify the glade generated file
///_____________________________________________________
uint8_t  bindGUI( void )
{

#define ADM_LOOKUP(a,b) a= lookup_widget (guiRootWindow,#b);if(!a) return 0;


	ADM_LOOKUP(guiDrawingArea,guiDrawing);
	ADM_LOOKUP(guiSlider,sliderNavigate);

	sliderAdjustment=gtk_range_get_adjustment (GTK_RANGE(guiSlider));

	ADM_LOOKUP(guiMarkA,labelMarkA);
	ADM_LOOKUP(guiMarkB,labelMarkB);
	ADM_LOOKUP(guiCurFrame,boxCurFrame);
	ADM_LOOKUP(guiTotalFrame,labelTotalFrame);
	
	ADM_LOOKUP(guiCurTime,boxCurTime);
	ADM_LOOKUP(guiTotalTime,labelTotalTime);

	ADM_LOOKUP(guiPreviewToggle,toggletoolbuttonPreview);
	ADM_LOOKUP(guiOutputToggle,toggletoolbuttonOutput);
#if 0
	ADM_LOOKUP(guiAudioToggle,togglebuttonAudio);
	ADM_LOOKUP(guiVideoToggle,togglebuttonVideo);
#endif
#undef ADM_LOOKUP
  // bind menu
 #define CALLBACK(x,y) gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,#x)), "activate", \
                      GTK_SIGNAL_FUNC(guiCallback),                   (void *) y)

 	#include "GUI_menumap.h"
  #undef CALLBACK
  /// /bind menu
	
// destroy
	 gtk_object_set_data_full(GTK_OBJECT(guiRootWindow),
			     "guiRootWindow",
			     guiRootWindow,
			     (GtkDestroyNotify) destroyCallback);
	
	
//	now add callbacks
	
	gtk_signal_connect(GTK_OBJECT(guiSlider), "button_press_event",
                       GTK_SIGNAL_FUNC(UI_SliderPressed),
                       NULL);

	gtk_signal_connect(GTK_OBJECT(guiSlider), "button_release_event",
                       GTK_SIGNAL_FUNC(UI_SliderReleased),
                       NULL);
#define ADD_SIGNAL(a,b,c)  gtk_signal_connect(GTK_OBJECT(a),b, \
		       GTK_SIGNAL_FUNC(guiCallback), (void *) c);
				       
      	ADD_SIGNAL(guiSlider,"value_changed",ACT_Scale);
	
	// We need to know when the current frame box has the focus to cancel
	// shortcuts
	
	gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,"boxCurFrame")), "focus_in_event", 
                      GTK_SIGNAL_FUNC(UI_grabFocus),                   (void *) NULL);	   
	gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,"boxCurFrame")), "focus_out_event", 	
                      GTK_SIGNAL_FUNC(UI_looseFocus),                   (void *) NULL);	 

        
        // navKey
        gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,"hscaleSensitive")), "value_changed",   
                      GTK_SIGNAL_FUNC(volumeChange),                   (void *) NULL);  


		       
// Callbacks for buttons
		uint32_t nb=sizeof(buttonCallback)/sizeof(buttonCallBack_S); 
		GtkWidget *bt;
		
		
		for(uint32_t i=0;i<nb;i++)
		{
			bt= lookup_widget (guiRootWindow,buttonCallback[i].name);
			if(!bt)
			{
				printf("Binding failed for %s\n",buttonCallback[i].name);
				ADM_assert(0);
			}
			ADD_SIGNAL(bt,buttonCallback[i].signal,buttonCallback[i].action);
			GTK_WIDGET_UNSET_FLAGS (bt, GTK_CAN_FOCUS);			
		}

	GTK_WIDGET_SET_FLAGS (lookup_widget(guiRootWindow,"boxCurFrame"), GTK_CAN_FOCUS);
		       
// set some tuning
    gtk_widget_set_usize(guiDrawingArea, 512, 288);
    
// hscale    
    GTK_WIDGET_UNSET_FLAGS (guiSlider, GTK_CAN_FOCUS);	
    gtk_widget_show(guiSlider);
    // And, the size now scales to the width of the window.
    gtk_widget_set_usize(guiSlider, 0, 0);
    // Plus, two-decimal precision.
    gtk_scale_set_digits(GTK_SCALE(guiSlider), 2);
    // And continuous updates!
    gtk_range_set_update_policy (GTK_RANGE (guiSlider), GTK_UPDATE_CONTINUOUS);	    

    gtk_range_set_range(GTK_RANGE(guiSlider),0,100.00);
    
    // keyboard events
    
   
 	gtk_signal_connect(GTK_OBJECT(guiDrawingArea), "expose_event",
		       GTK_SIGNAL_FUNC(on_drawingarea1_expose_event),
		       NULL);
		       
		       
	// Finally add video codec...
	uint32_t nbVid;
	const char *name;
        GtkComboBox     *combo_box;

                nbVid=encoderGetNbEncoder();
                combo_box=GTK_COMBO_BOX(lookup_widget(guiRootWindow,VIDEO_WIDGET));
                gtk_combo_box_remove_text(combo_box,0);
                printf("Found %d video encoder\n",nbVid);
                for(uint32_t i=0;i<nbVid;i++)
                {
                        name=encoderGetIndexedName(i);
                        gtk_combo_box_append_text      (combo_box,_(name));
                }

        gtk_combo_box_set_active(combo_box,0);
        on_video_change();
        // And A codec
        // Finally add video codec...
        uint32_t nbAud;

                nbAud=audioFilterGetNbEncoder();
                combo_box=GTK_COMBO_BOX(lookup_widget(guiRootWindow,AUDIO_WIDGET));
                gtk_combo_box_remove_text(combo_box,0);
                printf("Found %d audio encoder\n",nbAud);		       
                for(uint32_t i=0;i<nbAud;i++)
                {
                        name=audioFilterGetIndexedName(i);
                        gtk_combo_box_append_text      (combo_box,_(name));	
                }
        gtk_combo_box_set_active(combo_box,0);
	on_audio_change();
        // Format
                 gtk_combo_box_set_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,FORMAT_WIDGET)),0);

    //
        gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,VIDEO_WIDGET)), "changed",
                       GTK_SIGNAL_FUNC(on_video_change),
                       NULL);
        gtk_signal_connect(GTK_OBJECT(lookup_widget(guiRootWindow,AUDIO_WIDGET)), "changed",
                       GTK_SIGNAL_FUNC(on_audio_change),
                       NULL);
        
        // Add initial recent files
        UI_updateRecentMenu(  );
    //
    //CYB 2005.02.22: DND (START)
    // Set up avidemux as an available drag'n'drop target.
    gtk_drag_dest_set(guiRootWindow,
        GTK_DEST_DEFAULT_ALL,
        target_table,sizeof(target_table)/sizeof(GtkTargetEntry),
        (GdkDragAction)(GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_DEFAULT));
    g_signal_connect(GTK_OBJECT(guiRootWindow), "drag_data_received",
        GTK_SIGNAL_FUNC(DNDDataReceived),NULL);
    //CYB 2005.02.22: DND (END)

     // Allow shrink
   //GTK_WINDOW ( guiRootWindow ) ->allow_shrink = FALSE;
   //GTK_WINDOW ( guiDrawingArea ) ->allow_shrink = FALSE;

   // By default enable arrow keys
   UI_arrow_enabled();
  // Add custom menu
 GUI_initCustom();
    return 1;

}
/****

**/
void GUI_initCustom(void )
{
  GtkWidget *menuEntry=lookup_widget(guiRootWindow,"custom1");
  char *customdir=ADM_getCustomDir();
  if(!menuEntry)
  {
      printf("No custom menu...\n");
      return;
  }
  if(!customdir) 
  {
      printf("No custom dir...\n");
      return;
  }
  /* Collect the name */
   if(! buildDirectoryContent(&ADM_nbCustom,customdir, customNames,ADM_MAC_CUSTOM_SCRIPT))
    {
      printf("Failed to build custom dir content");
      return;
    }
  if(!ADM_nbCustom)
  {
      printf("No custom script\n");
  }
  printf("Found %u custom scripts, adding them\n",ADM_nbCustom);
 GtkWidget *go,*menu;
 int rank;

  menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuEntry), menu);

#define CALLBACK(x,y) gtk_signal_connect(GTK_OBJECT(x), "activate", \
                      GTK_SIGNAL_FUNC(guiCallback),                   (void *) y)

  for(int i=0;i<ADM_nbCustom;i++)
  {
    go = gtk_menu_item_new_with_mnemonic (GetFileName(customNames[i]));
    gtk_widget_show (go);
    gtk_container_add (GTK_CONTAINER (menu), go);
    rank=ACT_CUSTOM_BASE+i;
    CALLBACK( go                 ,rank);
  }

  #undef CALLBACK
  printf("Menu built\n");
}
gboolean  on_drawingarea1_expose_event(GtkWidget * widget,  GdkEventExpose * event, gpointer user_data)
{
UNUSED_ARG(widget);
UNUSED_ARG(event);
UNUSED_ARG(user_data);
	
		//renderRefresh();
		renderExpose();
		
    	return true;
}

///
///	Return the widget containing the image
///
GtkWidget *getDrawWidget( void )
{
	return guiDrawingArea;

}
///
///	Set slider position
///
static int _upd_in_progres=0;
void UI_setScale( double val )
{
if(_upd_in_progres) return;
 _upd_in_progres++;
   gtk_adjustment_set_value( GTK_ADJUSTMENT(sliderAdjustment),(  gdouble  ) val );
   gtk_signal_emit_by_name (GTK_OBJECT (sliderAdjustment), "changed");
 _upd_in_progres--;
   
}

void volumeChange( void )
{
#ifdef HAVE_AUDIO
GtkWidget *wid;
GtkAdjustment *adj;
int vol;


if(_upd_in_progres) return;
 _upd_in_progres++;

        wid=lookup_widget(guiRootWindow,"hscaleSensitive");
        adj=gtk_range_get_adjustment (GTK_RANGE(wid));
        vol=(int)floor(adj->value+0.5);
        AVDM_setVolume( vol);
 _upd_in_progres--;
#endif
}



void UI_setTitle(char *name)
{
	char title[1024];

		strcpy(title,"Avidemux 2 ");
		strncat(title,name,200);

 	gtk_window_set_title (GTK_WINDOW (guiRootWindow), title);

}
void UI_setFrameType( uint32_t frametype,uint32_t qp)
{
GtkLabel *wid=(GtkLabel *)lookup_widget(guiRootWindow,"labelFrameType");
static char string[100];
char	c='?';
	switch(frametype)
	{
		case AVI_KEY_FRAME: c='I';break;
		case AVI_B_FRAME: c='B';break;
		case 0: c='P';break;
		default:c='?';break;
	
	}
	sprintf(string,_("Frame:%c(%02d)"),c,qp);
	gtk_label_set_text( wid,string);	

}
///
///	Get slider position
///
double  UI_readScale( void )
{
 
	return (double)GTK_ADJUSTMENT(sliderAdjustment)->value;
 
}
void UI_updateFrameCount(uint32_t curFrame)
{
    //char text[80];   
    // frames 
    //sprintf(text, "%lu ", curFrame);
//    gtk_label_set_text((GtkLabel *) guiCurFrame, text);
	gtk_write_entry(guiCurFrame,curFrame);
    
}
void UI_setFrameCount(uint32_t curFrame,uint32_t total)
{
    char text[80]; 
    if(total) total--; // We display from 0 to X  
    // frames 
   // sprintf(text, "%lu ", curFrame);
   // gtk_label_set_text((GtkLabel *) guiCurFrame, text);
    gtk_write_entry(guiCurFrame,curFrame);
      sprintf(text, "/ %lu ", total);
    gtk_label_set_text((GtkLabel *) guiTotalFrame, text);
    
}
void UI_updateTimeCount(uint32_t curFrame,uint32_t fps)
{
  char text[80];   
 uint16_t mm,hh,ss,ms;
 
 	frame2time(curFrame,fps, &hh, &mm, &ss, &ms);
  	sprintf(text, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
//     gtk_label_set_text((GtkLabel *) guiCurTime, text);
	gtk_write_entry_string(guiCurTime,text);

}
void UI_setTimeCount(uint32_t curFrame,uint32_t total, uint32_t fps)
{
  char text[80];   
 uint16_t mm,hh,ss,ms;
 
 	frame2time(curFrame,fps, &hh, &mm, &ss, &ms);
  	sprintf(text, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
     	//gtk_label_set_text((GtkLabel *) guiCurTime, text);
     	gtk_write_entry_string(guiCurTime,text);
     
 	frame2time(total,fps, &hh, &mm, &ss, &ms);
  	sprintf(text, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
     	gtk_label_set_text((GtkLabel *) guiTotalTime, text);     
     

}
///
/// Called upon destroy event
/// just cleanly exit the application
///

gboolean destroyCallback(GtkWidget * widget,
				  GdkEvent * event, gpointer user_data)
{
    UNUSED_ARG(widget);
    UNUSED_ARG(event);
    UNUSED_ARG(user_data);
    HandleAction(ACT_Exit);
    return 1;
}
int UI_grabFocus( void)
{
//	printf("Grabbing focus\n");
#define RM(x,y)   gtk_widget_remove_accelerator (lookup_widget(guiRootWindow,#x), accel_group, \
                              y, (GdkModifierType) 0  );
	RM(buttonNextFrame,GDK_KP_6);
	RM(buttonPrevFrame,GDK_KP_4);
	RM(buttonNextKFrame,GDK_KP_8);
	RM(buttonPrevKFrame,GDK_KP_2);
	RM(delete1,GDK_Delete);
#define RMCTRL(x,y)   gtk_widget_remove_accelerator (lookup_widget(guiRootWindow,#x), accel_group, \
                              y, (GdkModifierType) GDK_CONTROL_MASK  );	
	RMCTRL(paste1,GDK_V);
	RMCTRL(copy1,GDK_C);
	RMCTRL(cut1,GDK_X);
	return FALSE;
	
}
int UI_looseFocus( void)
{
#define ADD(x,y)  gtk_widget_add_accelerator (lookup_widget(guiRootWindow,#x), "clicked", accel_group, \
                              y, (GdkModifierType) 0, \
                              GTK_ACCEL_VISIBLE);
//	printf("Dropping focus\n");			      
	ADD(buttonNextFrame,GDK_KP_6);
	ADD(buttonPrevFrame,GDK_KP_4);
	ADD(buttonNextKFrame,GDK_KP_8);
	ADD(buttonPrevKFrame,GDK_KP_2);
	
	gtk_widget_add_accelerator (lookup_widget(guiRootWindow,"delete1"), "activate", accel_group, \
                              GDK_Delete, (GdkModifierType) 0, \
                              GTK_ACCEL_VISIBLE);	
	
#define ADDCTRL(x,y) gtk_widget_add_accelerator (lookup_widget(guiRootWindow,#x), "activate", accel_group, \
                              y, (GdkModifierType) GDK_CONTROL_MASK, \
                              GTK_ACCEL_VISIBLE);
	
	ADDCTRL(paste1,GDK_V);
	ADDCTRL(copy1,GDK_C);
	ADDCTRL(cut1,GDK_X);
	
	return FALSE;
}
void UI_setMarkers(uint32_t a, uint32_t b )
{
char string[500];
		sprintf(string," %06lu",a);
        	gtk_label_set_text(GTK_LABEL(guiMarkA),string);
		sprintf(string," %06lu",b);
        	gtk_label_set_text(GTK_LABEL(guiMarkB),string);
}
///
///	Return status of preview button toggle
///	1-> Activated
/// 	0-> deActivated

uint8_t UI_getPreviewToggleStatus( void )
{                         //gtk_toggle_button_get_active

	if(TRUE==gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(guiPreviewToggle))  )
 		return 1;
   	else
    	return 0;
}
///
///	Update the preview button toggle
///
uint8_t UI_setPreviewToggleStatus( uint8_t status )
{
gint b;
        update_ui=1;
	 if(status) b=TRUE;
  	else			b=FALSE;
     gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(guiPreviewToggle),b);
        // Update checkbox
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(WOD(preview1)),status);
        update_ui=0;

}
///
///     Return status of preview button toggle
///     1-> Activated
///     0-> deActivated

uint8_t UI_getOutputToggleStatus( void )
{                         //gtk_toggle_button_get_active

        if(TRUE==gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(guiOutputToggle))  )
                return 1;
        else
        return 0;
}
///
///     Update the preview button toggle
///
uint8_t UI_setOutputToggleStatus( uint8_t status )
{
gint b;
        update_ui=1;
         if(status) b=TRUE;
        else                    b=FALSE;
     gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(guiOutputToggle),b);
        // Update checkbox
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(WOD(display_output1)),status);
        update_ui=0;

}

/**
	Set/unset the toggle button audio process
*/
void UI_setAProcessToggleStatus( uint8_t status )
{
gint b;
	 if(status) b=TRUE;
  	else			b=FALSE;

	if(b!=gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(guiAudioToggle)))
     		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(guiAudioToggle),b);
	aprintf("++ audio toggle : %d(%d)\n",b,status);

}
/*
	Set/unset the toggle button video process
*/
void UI_setVProcessToggleStatus( uint8_t status )
{
gint b;
	 if(status) 		b=TRUE;
  	else			b=FALSE;
	if(b!=gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(guiVideoToggle)))
	{
     		gtk_toggle_tool_button_set_active (GTK_TOGGLE_TOOL_BUTTON(guiVideoToggle),b);
		aprintf("Changing it to %d\n",b);
	}
	aprintf("++ video toggle : %d\n",b);

}

///
///	This is a relay function to do UI events -> gtk_gui.cpp
///
void guiCallback(GtkMenuItem * menuitem, gpointer user_data)
{
    UNUSED_ARG(menuitem);
    Action act;
    uint32_t aint;
    if(update_ui) return; // no event sent

    aint = (long int) user_data;
    act = (Action) aint;
    if(act==ACT_Scale)
    {
    	if( _upd_in_progres) return;
	_upd_in_progres++;
   	 HandleAction(act);
	_upd_in_progres--;
   	return;
    }
     HandleAction(act);
}
void UI_JumpDone(void )
{


}
uint32_t UI_readCurFrame( void )
{
int i;
	i=gtk_read_entry(guiCurFrame);
	if(i<0) i=0;
	return  (uint32_t)i;
	 
}
/**
		in=0 -> arts1
		in=1 -> alsa
		in=2->oss

*/

void UI_iconify( void )
{
	gtk_window_iconify(GTK_WINDOW(guiRootWindow));
}
void UI_deiconify( void )
{
	gtk_window_deiconify(GTK_WINDOW(guiRootWindow));
}
void GUI_initCursor( void )
{
	
	guiCursorBusy=gdk_cursor_new(GDK_WATCH);
	guiCursorNormal=gdk_cursor_new(GDK_X_CURSOR);
}
// Change cursor and drop all events
void UI_BusyCursor( void )
{
	 gdk_window_set_cursor((guiRootWindow->window),
                                          guiCursorBusy);
/*					  
	guiCursorEvtMask=gtk_widget_get_events(guiRootWindow);
	gtk_widget_set_events(guiRootWindow,0);
*/	

	
}
void on_video_change(void)
{
int enable;
        if(update_ui)
        {
                printf("Updating\n");
                 return;
        }

        if(!UI_getCurrentVCodec()) // copy ?
        {
                enable=0;
        }
        else enable=1;
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonConfV"),enable);        
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonFilters"),enable);        
        HandleAction(ACT_VideoCodecChanged);
}
void on_audio_change(void)
{
int enable;
       if(update_ui) return;
        if(!UI_getCurrentACodec()) // copy ?
        {
                enable=0;
        }
        else enable=1;
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonConfA"),enable);        
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonAudioFilter"),enable);        
        HandleAction(ACT_AudioCodecChanged);

}
 int 	UI_getCurrentACodec(void)
 {
        //return getRangeInMenu(lookup_widget(guiRootWindow,AUDIO_WIDGET));
        return gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,AUDIO_WIDGET)));
 
 }
 int 	UI_getCurrentVCodec(void)
 {
 
 	//return getRangeInMenu(lookup_widget(guiRootWindow,VIDEO_WIDGET));
        return gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,VIDEO_WIDGET)));
 
 }

void UI_setAudioCodec( int i)
{
        //gtk_option_menu_set_history(GTK_OPTION_MENU(lookup_widget(guiRootWindow,AUDIO_WIDGET)), i);
        update_ui=1;
        gtk_combo_box_set_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,AUDIO_WIDGET)),i);
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonConfA"),i);        
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonAudioFilter"),i);        
        update_ui=0;
}
void UI_setVideoCodec( int i)
{
        //gtk_option_menu_set_history(GTK_OPTION_MENU(lookup_widget(guiRootWindow,VIDEO_WIDGET)), i);
        update_ui=1;
        gtk_combo_box_set_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,VIDEO_WIDGET)),i);
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonConfV"),i);        
        gtk_widget_set_sensitive(lookup_widget(guiRootWindow,"buttonFilters"),i);        
        update_ui=0;

}

void UI_NormalCursor( void )
{
//	gtk_widget_set_events(guiRootWindow,guiCursorEvtMask);
	gdk_window_set_cursor((guiRootWindow->window),
                                          NULL); //guiCursorNormal);

	
}
void UI_PrintCurrentVCodec(const char *str)
{
 	

}
ADM_OUT_FORMAT UI_GetCurrentFormat( void )
{

	return (ADM_OUT_FORMAT)gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,FORMAT_WIDGET)));
}
uint8_t UI_SetCurrentFormat( ADM_OUT_FORMAT fmt )
{

	 gtk_combo_box_set_active(GTK_COMBO_BOX(lookup_widget(guiRootWindow,FORMAT_WIDGET)),fmt);
	return 1;
}
// DND CYB
void DNDDataReceived( GtkWidget *widget, GdkDragContext *dc,
                                  gint x, gint y, GtkSelectionData *selection_data, guint info, guint t)
{
   void *filename;
   char *start,*end;
   int cont;

    if (info == TARGET_URI_LIST)
    {
     start = strstr((char*)selection_data->data,"file://");
     cont = 0;
     do
     {
       if (start)
       {
        end = strstr((char*)start+1,"file://");
        if (!end)
        {
                      end = start + strlen(start);
                      cont = 1;
                      continue;
        }
        filename = ADM_alloc(end-start); 
        if (filename)
        {
              memset(filename,0,end-start);
              memcpy(filename,start+7,end-start-7-2);
              if (avifileinfo) 
              {
                    // Append video when there's already something
                    fileReadWrite(A_appendAvi, 0, (char*)filename);
               }
               else
               {
                    fileReadWrite(A_openAvi, 0, (char*)filename);
               }
         } 
         ADM_dealloc(filename);
         start = end;
       }
       else
       {
           cont=1;
       } 
     } //do
     while (!cont);  
    }
    gtk_drag_finish(dc,TRUE,FALSE,t);
}
void UI_toogleMain(void)
{
static int show=1;
        show=show^1;
        if(!show)
                gtk_widget_hide(GTK_WIDGET(lookup_widget(guiRootWindow,"toolbar2")) );
        else
                gtk_widget_show(GTK_WIDGET(lookup_widget(guiRootWindow,"toolbar2")) );
}
void UI_toogleSide(void)
{
static int show=1;
        show=show^1;
        if(!show)
                gtk_widget_hide(GTK_WIDGET(lookup_widget(guiRootWindow,"vbox9")));
        else
                gtk_widget_show(GTK_WIDGET(lookup_widget(guiRootWindow,"vbox9")));
}
uint8_t UI_getTimeShift(int *onoff,int *value)
{

        *onoff=gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (lookup_widget(guiRootWindow,"CheckButtonTimeshift")));
        *value=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(guiRootWindow,"spinbuttonTimeShift"))) ;
        return 1;
}
uint8_t UI_setTimeShift(int onoff,int value)
{

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (lookup_widget(guiRootWindow,"CheckButtonTimeshift")),onoff);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(guiRootWindow,"spinbuttonTimeShift")),value) ;
        return 1;
}
uint8_t UI_updateRecentMenu( void )
{
const char **names;
uint32_t nb_item=0;
GtkWidget *button,*menu,*item[4];
static Action recent[4]={ACT_RECENT0,ACT_RECENT1,ACT_RECENT2,ACT_RECENT3};

        names=prefs->get_lastfiles();
// count
        for( nb_item=0;nb_item<4;nb_item++)
        {
                if(!names[nb_item]) break;
        }
        button=lookup_widget(guiRootWindow,"menutoolbuttonOpen");
        if(!nb_item)
        {
                gtk_menu_tool_button_set_menu   (GTK_MENU_TOOL_BUTTON(button),NULL);
                return 1;
        }
        menu=gtk_menu_new();
        for(int i=0;i<nb_item;i++)
        {
                item[i]=gtk_menu_item_new_with_label(names[i]);
                gtk_menu_attach(GTK_MENU(menu),item[i],0,1,i,i+1);
                 gtk_signal_connect (GTK_OBJECT (item[i]), "activate", GTK_SIGNAL_FUNC (guiCallback), 
                                (gpointer) recent[i]);
                gtk_widget_show (item[i]);
        }
        gtk_menu_tool_button_set_menu   (GTK_MENU_TOOL_BUTTON(button),menu);
        return 1;
}

uint8_t UI_shrink(uint32_t w,uint32_t h)
{
        gtk_widget_set_usize(guiRootWindow, w, h);
        return 1;
}
// Override arrow keys to quickly navigate
uint8_t UI_arrow_enabled(void)
{
  g_signal_connect(GTK_OBJECT(guiRootWindow), "key_press_event",
                       GTK_SIGNAL_FUNC(UI_on_key_press),
                       NULL);
   

}
uint8_t UI_arrow_disabled(void)
{

}
gboolean UI_SliderPressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if(event->state&GDK_SHIFT_MASK) SliderIsShifted=TRUE;
	return FALSE;

}
gboolean UI_SliderReleased(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	SliderIsShifted=FALSE;
	return FALSE;
}
// EOF
