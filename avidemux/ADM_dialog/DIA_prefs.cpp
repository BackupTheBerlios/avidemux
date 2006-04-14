//
/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


# include <config.h>

#include "ADM_library/default.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_toolkit/toolkit_gtk_include.h"
#include "ADM_toolkit/filesel.h"

# include "prefs.h"
#include "ADM_audiodevice/audio_out.h"

#include "ADM_assert.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_toolkit/ADM_cpuCap.h"
#include "ADM_toolkit/toolkit.hxx"

static GtkWidget	*create_dialog1 (void);
static void setpp(void);

static GtkWidget *dialog=NULL;

extern void 		AVDM_audioPref( void );

typedef struct stVideoDevice
{
        ADM_RENDER_TYPE type;
        char            *name;

}stVideoDevice;

static stVideoDevice myVideoDevice[]
={
        {RENDER_GTK,"Gtk (no accel)"},
#ifdef USE_XV
        {RENDER_XV,"Xvideo accel (best)"},
#endif
#ifdef USE_SDL
        {RENDER_SDL,"SDL accel"},
#endif

};

uint8_t DIA_Preferences(void);


uint8_t DIA_Preferences(void)
{
uint8_t ret=0;
gint r;
char *str;
const char *nullstring="";
AUDIO_DEVICE olddevice,newdevice;
uint32_t	lavcodec_mpeg=0;
uint32_t        use_odml=0;
uint32_t	autosplit=0;
GtkWidget *wids[10];
GtkWidget *videowids[10];
uint32_t k;
unsigned int renderI;
ADM_RENDER_TYPE render;
uint32_t useTray=0;
uint32_t useMaster=0;
uint32_t useAutoIndex=0;
uint32_t useSwap=0;
uint32_t useNuv=0;
uint32_t mthreads=0;	
uint32_t downmix;
	dialog=create_dialog1();
//	gtk_transient(dialog);
        gtk_register_dialog(dialog);

        if(!prefs->get(FEATURE_USE_SYSTRAY,&useTray)) useTray=0;
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonSystray)),useTray);
        
//****************************	
#if 0
#define SET_CPU(x,y) gtk_widget_set_sensitive(WID(check##x),0); \
        if(CpuCaps::has##y()) gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(check##x)),1);
#else
        #define SET_CPU(x,y)
#endif
        SET_CPU(MMX,MMX);
        SET_CPU(MMXEXT,MMXEXT);
        SET_CPU(3DNOW,3DNOW);
        SET_CPU(SSE,SSE);
        SET_CPU(SSE2,SSE2);
        //gtk_widget_set_sensitive( (WID(checkAltivec)),0);
        gtk_widget_set_sensitive( (WID(checkbuttonReuseLog)),0);
//****************************
	olddevice=newdevice=AVDM_getCurrentDevice();
	
	#define CONNECT(A,B)  gtk_signal_connect (GTK_OBJECT(lookup_widget(dialog,#A)), "clicked", \
		      GTK_SIGNAL_FUNC (B), (void *) NULL);

	
#define SPIN_GET(x,y) {x= gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(WID(y))) ;}
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(y)),(gfloat)x) ;}

	// Alsa
        if( prefs->get(DEVICE_AUDIO_ALSA_DEVICE, &str) != RC_OK )
               str = ADM_strdup("plughw:0,0");
        gtk_write_entry_string(WID(entryALSADevice), str);
        ADM_dealloc(str);
	// Multithreads
	if(!prefs->get(FEATURE_MULTI_THREAD, &mthreads))
	{
		mthreads=0;		
	}
        SPIN_SET(mthreads,spinbuttonThread);
	// VCD/SVCD split point		
	if(!prefs->get(SETTINGS_MPEGSPLIT, &autosplit))
	{
		autosplit=690;		
	}
	// Fill entry
         SPIN_SET(autosplit,spinbuttonMPEGSplit);
        		
	if(!prefs->get(FEATURE_USE_LAVCODEC_MPEG, &lavcodec_mpeg))
	{
		lavcodec_mpeg=0;		
	}		
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonLibavcodec)),
			lavcodec_mpeg);
        
        // Open DML (Gmv)
        if(!prefs->get(FEATURE_USE_ODML, &use_odml))
        {
          use_odml=0;                
        }               
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonOpenDML)),
                                      use_odml);

        if(!prefs->get(FEATURE_AUDIOBAR_USES_MASTER, &useMaster))
                useMaster=0;
        
        if(useMaster) RADIO_SET(radiobuttonMaster,1);
                else RADIO_SET(radiobuttonMaster,0);


        if(!prefs->get(FEATURE_TRYAUTOIDX, &useAutoIndex))
                useAutoIndex=0;
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonAutoindex)),useAutoIndex);

        
        if(!prefs->get(FEATURE_SWAP_IF_A_GREATER_THAN_B, &useSwap))
                useSwap=0;
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonSwapAB)),useSwap);


        if(!prefs->get(FEATURE_DISABLE_NUV_RESYNC, &useNuv))
                useNuv=0;
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbuttonNuvResync)),useNuv);
        // _____________Message level_____________
        //________________________________________
        GtkComboBox     *combo_box;
        unsigned int msg=2;
        prefs->get(MESSAGE_LEVEL,&msg);
        combo_box=GTK_COMBO_BOX(WID(comboboxMessageLevel));
        gtk_combo_box_set_active(combo_box,msg);
        
        // ___________Video accel device ______________________________________________
        
        
        combo_box=GTK_COMBO_BOX(WID(comboboxVideoOutput));
        int vd=0;
        if(prefs->get(DEVICE_VIDEODEVICE,&renderI)!=RC_OK)
        {       
                render=RENDER_GTK;
        }else
        {
                render=(ADM_RENDER_TYPE)renderI;
        }
        for(uint32_t i=0;i<sizeof(myVideoDevice)/sizeof(stVideoDevice);i++)
        {
                if(myVideoDevice[i].type==render)
                        {
                                vd=i;
                        }
                gtk_combo_box_append_text      (combo_box,myVideoDevice[i].name);
        }               
        gtk_combo_box_set_active(combo_box,vd);
        // ___________ Downmixing ______________________________________________
        if(prefs->get(DOWNMIXING_PROLOGIC,&downmix)!=RC_OK)
        {       
            downmix=0;
        }
        combo_box=GTK_COMBO_BOX(WID(comboboxDownMix));
        gtk_combo_box_set_active(combo_box,downmix);
	// ___________ Audio accel device ______________________________________________
        
        combo_box=GTK_COMBO_BOX(WID(comboboxAudioOutput));
        for(uint32_t i=0;i<sizeof(audioDeviceList)/sizeof(DEVICELIST);i++)
        {
                if(audioDeviceList[i].id==olddevice)
                {
                        k=i;
                }
                gtk_combo_box_append_text      (combo_box,audioDeviceList[i].name);
        }
        gtk_combo_box_set_active(combo_box,k);
        //______________________________________________________
        // Callback for button
        gtk_signal_connect(GTK_OBJECT(WID(buttonPostprocLevel)), "clicked",GTK_SIGNAL_FUNC(setpp),   NULL);
	 // __________ run _____________________

	gtk_dialog_run(GTK_DIALOG(dialog));
	{
		ret=1;
                // Get downmix
                k=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxDownMix)));
                prefs->set(DOWNMIXING_PROLOGIC,k);
                
		// Get device
		k=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxAudioOutput)));
		newdevice=audioDeviceList[k].id;
		if(newdevice!=olddevice)
		{
			AVDM_switch(newdevice);
		}
                //
                uint32_t s;
                s=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxMessageLevel)));
                prefs->set(MESSAGE_LEVEL,s);
                // video device

                k=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxVideoOutput)));
                render=myVideoDevice[k].type;
                renderI=(int)render;
                prefs->set(DEVICE_VIDEODEVICE,renderI);
        
                //**************
                useTray=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonSystray)));
                prefs->set(FEATURE_USE_SYSTRAY,useTray);
                
                //*************
                if(RADIO_GET(radiobuttonMaster))
                        prefs->set(FEATURE_AUDIOBAR_USES_MASTER,(uint32_t) 1);
                else
                        prefs->set(FEATURE_AUDIOBAR_USES_MASTER,(uint32_t) 0);

                ///*********
		lavcodec_mpeg=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonLibavcodec)));
		prefs->set(FEATURE_USE_LAVCODEC_MPEG, lavcodec_mpeg);
                
                use_odml=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonOpenDML)));
                prefs->set(FEATURE_USE_ODML, use_odml);
                
                SPIN_GET(autosplit,spinbuttonMPEGSplit);
		prefs->set(SETTINGS_MPEGSPLIT, autosplit);
                
                //
                SPIN_GET(mthreads,spinbuttonThread);
                if(mthreads<2) mthreads=0;
                prefs->set(FEATURE_MULTI_THREAD, mthreads);
                //

                useAutoIndex=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonAutoindex)));
                prefs->set(FEATURE_TRYAUTOIDX, useAutoIndex);

                useSwap=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonSwapAB)));
                prefs->set(FEATURE_SWAP_IF_A_GREATER_THAN_B, useSwap);

                useNuv=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbuttonNuvResync)));
                prefs->set(FEATURE_DISABLE_NUV_RESYNC, useNuv);

                //alsa device
                str=gtk_editable_get_chars(GTK_EDITABLE (WID(entryALSADevice)), 0, -1);
                if(str)
                        prefs->set(DEVICE_AUDIO_ALSA_DEVICE, str);
		
	}
        gtk_unregister_dialog(dialog);
	gtk_widget_destroy(dialog);
	dialog=NULL;
	return ret;
}
extern int DIA_getMPParams( uint32_t *pplevel, uint32_t *ppstrength,uint32_t *swap);
void setpp(void)
{
        uint32_t type,strength,uv=0;

        if(!prefs->get(DEFAULT_POSTPROC_TYPE,&type)) type=3;
        if(!prefs->get(DEFAULT_POSTPROC_VALUE,&strength)) strength=3;
        if( DIA_getMPParams( &type,&strength,&uv))
        {
                prefs->set(DEFAULT_POSTPROC_TYPE,type);
                prefs->set(DEFAULT_POSTPROC_VALUE,strength);

        }
}
/*
void on_callback_lame(GtkButton * button, gpointer user_data)
{
char *lame=NULL;

	 GUI_FileSelRead("Where is lame ?", &lame);
	 if(lame)
	 {
	 	gtk_write_entry_string(WID(entryLame), lame);
	 	
	 }

}
*/
void on_callback_toolame(GtkButton * button, gpointer user_data)
{
char *lame=NULL;
gint r;
	 GUI_FileSelRead("Where is toolame ?", &lame);
	 if(lame)
	 {
	 	gtk_write_entry_string(WID(entryToolame), lame);		
	 }


}
//*************************
GtkWidget*
        create_dialog1 (void)
{
    GtkWidget *Preferences;
    GtkWidget *dialog_vbox1;
    GtkWidget *notebook1;
    GtkWidget *vbox12;
    GtkWidget *hbox3;
    GtkWidget *label11;
    GtkWidget *comboboxMessageLevel;
    GtkWidget *checkbuttonSwapAB;
    GtkWidget *checkbuttonSystray;
    GtkWidget *checkbuttonSavePrefs;
    GtkWidget *label21;
    GtkWidget *vbox16;
    GtkWidget *checkbuttonAutoindex;
    GtkWidget *checkbuttonNuvResync;
    GtkWidget *checkbuttonLibavcodec;
    GtkWidget *buttonPostprocLevel;
    GtkWidget *label23;
    GtkWidget *vbox14;
    GtkWidget *hbox4;
    GtkWidget *label13;
    GtkObject *spinbuttonMPEGSplit_adj;
    GtkWidget *spinbuttonMPEGSplit;
    GtkWidget *checkbuttonOpenDML;
    GtkWidget *checkbuttonReuseLog;
    GtkWidget *label22;
    GtkWidget *table1;
    GtkWidget *label18;
    GtkWidget *label19;
    GtkWidget *comboboxAudioOutput;
    GtkWidget *entryALSADevice;
    GtkWidget *radiobuttonMaster;
    GSList *radiobuttonMaster_group = NULL;
    GtkWidget *radiobuttonPCM;
    GtkWidget *label20;
    GtkWidget *label29;
    GtkWidget *comboboxDownMix;
    GtkWidget *label25;
    GtkWidget *table2;
    GtkWidget *hbox5;
    GtkWidget *label16;
    GtkWidget *comboboxVideoOutput;
    GtkWidget *label24;
    GtkWidget *hbox6;
    GtkWidget *label27;
    GtkObject *spinbuttonThread_adj;
    GtkWidget *spinbuttonThread;
    GtkWidget *label28;
    GtkWidget *label26;
    GtkWidget *dialog_action_area1;
    GtkWidget *okbutton1;

    Preferences = gtk_dialog_new ();
    gtk_container_set_border_width (GTK_CONTAINER (Preferences), 6);
    gtk_window_set_title (GTK_WINDOW (Preferences), _("Preferences"));
    gtk_window_set_resizable (GTK_WINDOW (Preferences), FALSE);
    gtk_window_set_type_hint (GTK_WINDOW (Preferences), GDK_WINDOW_TYPE_HINT_DIALOG);
    gtk_dialog_set_has_separator (GTK_DIALOG (Preferences), FALSE);

    dialog_vbox1 = GTK_DIALOG (Preferences)->vbox;
    gtk_widget_show (dialog_vbox1);

    notebook1 = gtk_notebook_new ();
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (notebook1), 6);

    vbox12 = gtk_vbox_new (FALSE, 6);
    gtk_widget_show (vbox12);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox12);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox12,
                                        FALSE, FALSE, GTK_PACK_START);
    gtk_container_set_border_width (GTK_CONTAINER (vbox12), 12);

    hbox3 = gtk_hbox_new (FALSE, 12);
    gtk_widget_show (hbox3);
    gtk_box_pack_start (GTK_BOX (vbox12), hbox3, TRUE, TRUE, 0);

    label11 = gtk_label_new_with_mnemonic (_("_Message level:"));
    gtk_widget_show (label11);
    gtk_box_pack_start (GTK_BOX (hbox3), label11, FALSE, FALSE, 0);

    comboboxMessageLevel = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxMessageLevel);
    gtk_box_pack_start (GTK_BOX (hbox3), comboboxMessageLevel, TRUE, TRUE, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMessageLevel), _("No alerts"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMessageLevel), _("Display only error alerts"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMessageLevel), _("Display all alerts"));

    checkbuttonSwapAB = gtk_check_button_new_with_mnemonic (_("_Swap marks A and B if A > B"));
    gtk_widget_show (checkbuttonSwapAB);
    gtk_box_pack_start (GTK_BOX (vbox12), checkbuttonSwapAB, FALSE, FALSE, 0);

    checkbuttonSystray = gtk_check_button_new_with_mnemonic (_("_Go to systray when encoding"));
    gtk_widget_show (checkbuttonSystray);
    gtk_box_pack_start (GTK_BOX (vbox12), checkbuttonSystray, FALSE, FALSE, 0);

    checkbuttonSavePrefs = gtk_check_button_new_with_mnemonic (_("S_ave preferences on exit"));
    gtk_widget_show (checkbuttonSavePrefs);
    gtk_box_pack_start (GTK_BOX (vbox12), checkbuttonSavePrefs, FALSE, FALSE, 0);

    label21 = gtk_label_new (_("User Interface"));
    gtk_widget_show (label21);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label21);

    vbox16 = gtk_vbox_new (FALSE, 6);
    gtk_widget_show (vbox16);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox16);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox16,
                                        FALSE, FALSE, GTK_PACK_START);
    gtk_container_set_border_width (GTK_CONTAINER (vbox16), 12);

    checkbuttonAutoindex = gtk_check_button_new_with_mnemonic (_("_Automatically index MPEG files"));
    gtk_widget_show (checkbuttonAutoindex);
    gtk_box_pack_start (GTK_BOX (vbox16), checkbuttonAutoindex, FALSE, FALSE, 0);

    checkbuttonNuvResync = gtk_check_button_new_with_mnemonic (_("_Disable NuppelVideo resync"));
    gtk_widget_show (checkbuttonNuvResync);
    gtk_box_pack_start (GTK_BOX (vbox16), checkbuttonNuvResync, FALSE, FALSE, 0);

    checkbuttonLibavcodec = gtk_check_button_new_with_mnemonic (_("_Use libavcodec MPEG decoder"));
    gtk_widget_show (checkbuttonLibavcodec);
    gtk_box_pack_start (GTK_BOX (vbox16), checkbuttonLibavcodec, FALSE, FALSE, 0);

    buttonPostprocLevel = gtk_button_new_with_mnemonic (_("_Set Default Postprocessing Level"));
    gtk_widget_show (buttonPostprocLevel);
    gtk_box_pack_start (GTK_BOX (vbox16), buttonPostprocLevel, FALSE, FALSE, 0);

    label23 = gtk_label_new (_("Input"));
    gtk_widget_show (label23);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label23);

    vbox14 = gtk_vbox_new (FALSE, 6);
    gtk_widget_show (vbox14);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox14);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), vbox14,
                                        FALSE, FALSE, GTK_PACK_START);
    gtk_container_set_border_width (GTK_CONTAINER (vbox14), 12);

    hbox4 = gtk_hbox_new (FALSE, 12);
    gtk_widget_show (hbox4);
    gtk_box_pack_start (GTK_BOX (vbox14), hbox4, FALSE, FALSE, 0);

    label13 = gtk_label_new_with_mnemonic (_("_MPEG auto split (MB):"));
    gtk_widget_show (label13);
    gtk_box_pack_start (GTK_BOX (hbox4), label13, FALSE, FALSE, 0);

    spinbuttonMPEGSplit_adj = gtk_adjustment_new (790, 400, 5000, 1, 10, 10);
    spinbuttonMPEGSplit = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMPEGSplit_adj), 1, 0);
    gtk_widget_show (spinbuttonMPEGSplit);
    gtk_box_pack_start (GTK_BOX (hbox4), spinbuttonMPEGSplit, FALSE, FALSE, 0);

    checkbuttonOpenDML = gtk_check_button_new_with_mnemonic (_("_Use OpenDML (don't split large AVI files)"));
    gtk_widget_show (checkbuttonOpenDML);
    gtk_box_pack_start (GTK_BOX (vbox14), checkbuttonOpenDML, FALSE, FALSE, 0);

    checkbuttonReuseLog = gtk_check_button_new_with_mnemonic (_("_Reuse 2-pass log"));
    gtk_widget_show (checkbuttonReuseLog);
    gtk_box_pack_start (GTK_BOX (vbox14), checkbuttonReuseLog, FALSE, FALSE, 0);

    label22 = gtk_label_new (_("Output"));
    gtk_widget_show (label22);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label22);

    table1 = gtk_table_new (4, 3, FALSE);
    gtk_widget_show (table1);
    gtk_container_add (GTK_CONTAINER (notebook1), table1);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), table1,
                                        FALSE, FALSE, GTK_PACK_START);
    gtk_container_set_border_width (GTK_CONTAINER (table1), 12);
    gtk_table_set_row_spacings (GTK_TABLE (table1), 6);
    gtk_table_set_col_spacings (GTK_TABLE (table1), 12);

    label18 = gtk_label_new_with_mnemonic (_("_Audio output:"));
    gtk_widget_show (label18);
    gtk_table_attach (GTK_TABLE (table1), label18, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

    label19 = gtk_label_new_with_mnemonic (_("ALSA _device:"));
    gtk_widget_show (label19);
    gtk_table_attach (GTK_TABLE (table1), label19, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label19), 0, 0.5);

    comboboxAudioOutput = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxAudioOutput);
    gtk_table_attach (GTK_TABLE (table1), comboboxAudioOutput, 1, 3, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);

    entryALSADevice = gtk_entry_new ();
    gtk_widget_show (entryALSADevice);
    gtk_table_attach (GTK_TABLE (table1), entryALSADevice, 1, 3, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    radiobuttonMaster = gtk_radio_button_new_with_mnemonic (NULL, _("_Master"));
    gtk_widget_show (radiobuttonMaster);
    gtk_table_attach (GTK_TABLE (table1), radiobuttonMaster, 2, 3, 3, 4,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonMaster), radiobuttonMaster_group);
    radiobuttonMaster_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonMaster));

    radiobuttonPCM = gtk_radio_button_new_with_mnemonic (NULL, _("_PCM"));
    gtk_widget_show (radiobuttonPCM);
    gtk_table_attach (GTK_TABLE (table1), radiobuttonPCM, 1, 2, 3, 4,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonPCM), radiobuttonMaster_group);
    radiobuttonMaster_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonPCM));

    label20 = gtk_label_new_with_mnemonic (_("_Volume bar controls:"));
    gtk_widget_show (label20);
    gtk_table_attach (GTK_TABLE (table1), label20, 0, 1, 3, 4,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label20), 0, 0.5);

    label29 = gtk_label_new (_("Local Playback Downmixing"));
    gtk_widget_show (label29);
    gtk_table_attach (GTK_TABLE (table1), label29, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label29), 0, 0.5);

    comboboxDownMix = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxDownMix);
    gtk_table_attach (GTK_TABLE (table1), comboboxDownMix, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDownMix), _("No downmixing (multichannel)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDownMix), _("Dolby Prologic"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDownMix), _("Dolby Prologic 2"));

    label25 = gtk_label_new (_("Audio"));
    gtk_widget_show (label25);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label25);

    table2 = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table2);
    gtk_container_add (GTK_CONTAINER (notebook1), table2);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook1), table2,
                                        FALSE, FALSE, GTK_PACK_START);

    hbox5 = gtk_hbox_new (FALSE, 12);
    gtk_widget_show (hbox5);
    gtk_table_attach (GTK_TABLE (table2), hbox5, 0, 2, 0, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (hbox5), 12);

    label16 = gtk_label_new_with_mnemonic (_("_Video output:"));
    gtk_widget_show (label16);
    gtk_box_pack_start (GTK_BOX (hbox5), label16, FALSE, FALSE, 0);

    comboboxVideoOutput = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxVideoOutput);
    gtk_box_pack_start (GTK_BOX (hbox5), comboboxVideoOutput, TRUE, TRUE, 0);

    label24 = gtk_label_new (_("Video"));
    gtk_widget_show (label24);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), label24);

    hbox6 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox6);
    gtk_container_add (GTK_CONTAINER (notebook1), hbox6);

    label27 = gtk_label_new (_("Enable multithreading : "));
    gtk_widget_show (label27);
    gtk_box_pack_start (GTK_BOX (hbox6), label27, FALSE, FALSE, 0);

    spinbuttonThread_adj = gtk_adjustment_new (0, 0, 4, 1, 10, 10);
    spinbuttonThread = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonThread_adj), 1, 0);
    gtk_widget_show (spinbuttonThread);
    gtk_box_pack_start (GTK_BOX (hbox6), spinbuttonThread, FALSE, TRUE, 0);
    gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonThread), TRUE);

    label28 = gtk_label_new (_("Threads"));
    gtk_widget_show (label28);
    gtk_box_pack_start (GTK_BOX (hbox6), label28, FALSE, FALSE, 0);

    label26 = gtk_label_new (_("MultiThread"));
    gtk_widget_show (label26);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 5), label26);

    dialog_action_area1 = GTK_DIALOG (Preferences)->action_area;
    gtk_widget_show (dialog_action_area1);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

    okbutton1 = gtk_button_new_from_stock ("gtk-close");
    gtk_widget_show (okbutton1);
    gtk_dialog_add_action_widget (GTK_DIALOG (Preferences), okbutton1, GTK_RESPONSE_CLOSE);
    GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

    gtk_label_set_mnemonic_widget (GTK_LABEL (label13), spinbuttonMPEGSplit);
    gtk_label_set_mnemonic_widget (GTK_LABEL (label19), entryALSADevice);
    gtk_label_set_mnemonic_widget (GTK_LABEL (label20), radiobuttonPCM);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF (Preferences, Preferences, "Preferences");
    GLADE_HOOKUP_OBJECT_NO_REF (Preferences, dialog_vbox1, "dialog_vbox1");
    GLADE_HOOKUP_OBJECT (Preferences, notebook1, "notebook1");
    GLADE_HOOKUP_OBJECT (Preferences, vbox12, "vbox12");
    GLADE_HOOKUP_OBJECT (Preferences, hbox3, "hbox3");
    GLADE_HOOKUP_OBJECT (Preferences, label11, "label11");
    GLADE_HOOKUP_OBJECT (Preferences, comboboxMessageLevel, "comboboxMessageLevel");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonSwapAB, "checkbuttonSwapAB");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonSystray, "checkbuttonSystray");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonSavePrefs, "checkbuttonSavePrefs");
    GLADE_HOOKUP_OBJECT (Preferences, label21, "label21");
    GLADE_HOOKUP_OBJECT (Preferences, vbox16, "vbox16");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonAutoindex, "checkbuttonAutoindex");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonNuvResync, "checkbuttonNuvResync");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonLibavcodec, "checkbuttonLibavcodec");
    GLADE_HOOKUP_OBJECT (Preferences, buttonPostprocLevel, "buttonPostprocLevel");
    GLADE_HOOKUP_OBJECT (Preferences, label23, "label23");
    GLADE_HOOKUP_OBJECT (Preferences, vbox14, "vbox14");
    GLADE_HOOKUP_OBJECT (Preferences, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT (Preferences, label13, "label13");
    GLADE_HOOKUP_OBJECT (Preferences, spinbuttonMPEGSplit, "spinbuttonMPEGSplit");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonOpenDML, "checkbuttonOpenDML");
    GLADE_HOOKUP_OBJECT (Preferences, checkbuttonReuseLog, "checkbuttonReuseLog");
    GLADE_HOOKUP_OBJECT (Preferences, label22, "label22");
    GLADE_HOOKUP_OBJECT (Preferences, table1, "table1");
    GLADE_HOOKUP_OBJECT (Preferences, label18, "label18");
    GLADE_HOOKUP_OBJECT (Preferences, label19, "label19");
    GLADE_HOOKUP_OBJECT (Preferences, comboboxAudioOutput, "comboboxAudioOutput");
    GLADE_HOOKUP_OBJECT (Preferences, entryALSADevice, "entryALSADevice");
    GLADE_HOOKUP_OBJECT (Preferences, radiobuttonMaster, "radiobuttonMaster");
    GLADE_HOOKUP_OBJECT (Preferences, radiobuttonPCM, "radiobuttonPCM");
    GLADE_HOOKUP_OBJECT (Preferences, label20, "label20");
    GLADE_HOOKUP_OBJECT (Preferences, label29, "label29");
    GLADE_HOOKUP_OBJECT (Preferences, comboboxDownMix, "comboboxDownMix");
    GLADE_HOOKUP_OBJECT (Preferences, label25, "label25");
    GLADE_HOOKUP_OBJECT (Preferences, table2, "table2");
    GLADE_HOOKUP_OBJECT (Preferences, hbox5, "hbox5");
    GLADE_HOOKUP_OBJECT (Preferences, label16, "label16");
    GLADE_HOOKUP_OBJECT (Preferences, comboboxVideoOutput, "comboboxVideoOutput");
    GLADE_HOOKUP_OBJECT (Preferences, label24, "label24");
    GLADE_HOOKUP_OBJECT (Preferences, hbox6, "hbox6");
    GLADE_HOOKUP_OBJECT (Preferences, label27, "label27");
    GLADE_HOOKUP_OBJECT (Preferences, spinbuttonThread, "spinbuttonThread");
    GLADE_HOOKUP_OBJECT (Preferences, label28, "label28");
    GLADE_HOOKUP_OBJECT (Preferences, label26, "label26");
    GLADE_HOOKUP_OBJECT_NO_REF (Preferences, dialog_action_area1, "dialog_action_area1");
    GLADE_HOOKUP_OBJECT (Preferences, okbutton1, "okbutton1");

    return Preferences;
}

