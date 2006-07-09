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


#define MENU_SET(x,y) { gtk_option_menu_set_history (GTK_OPTION_MENU(WID(x)),y);}
#define MENU_GET(x)   getRangeInMenu(WID(x))

#define TOGGLE_SET(x,y) {gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(WID(x)),specific->y);}
#define TOGGLE_GET(x,y) {specific->y=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(WID(x)));;}
                                
#define ENABLE(x)    gtk_widget_set_sensitive(WID(x),1)
#define DISABLE(x)    gtk_widget_set_sensitive(WID(x),0)


uint8_t DIA_x264(COMPRES_PARAMS *config);

static GtkWidget       *create_dialog1 (void);  

static COMPRES_PARAMS generic={CodecDummy,"dummy","dummy","dummy",COMPRESS_CQ,4,1500,700,1000,0,0,NULL,0};
static GtkWidget *dialog;

static void updateMode( void );
static int cb_mod(GtkObject * object, gpointer user_data);
static ADM_x264Param *specific;
static void upload(GtkWidget *dialog,ADM_x264Param *param);
static void download(GtkWidget *dialog,ADM_x264Param *param);
extern ADM_x264Param x264ExtraDefault;
/*********************************************/
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton##x)),y) ;}
#define SPIN_GET(x,y)  {y=(int32_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(spinbutton##x))) ;}
#define ENTRY_SET(x,y) {gtk_write_entry(WID(entry##x),y);}        
#define ENTRY_GET(x,y) {y=gtk_read_entry(WID(entry##x));}        


typedef enum x264_actions
{
    ACTION_LOAD_DEFAULT=10,
    ACTION_LOAD_INVALID
};
uint8_t DIA_x264(COMPRES_PARAMS *config)
{
  
  uint8_t ret=0;
  
        ADM_assert(config->extraSettingsLen==sizeof(ADM_x264Param));
  
        memcpy(&generic,config,sizeof(generic));
        specific=(ADM_x264Param *)config->extraSettings;
       
        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonResetDefaults),ACTION_LOAD_DEFAULT);
        DISABLE(buttonLoadDefaults);
        DISABLE(buttonSaveDefaults);
        upload(dialog,specific);
        
        updateMode();
        
        gtk_signal_connect(GTK_OBJECT(WID(comboboxMode)), "changed",
                           GTK_SIGNAL_FUNC(cb_mod), NULL);
        int reply=0;
        while(1)
        {
            reply=gtk_dialog_run(GTK_DIALOG(dialog));
            if(reply==ACTION_LOAD_DEFAULT)
            {
                printf("Resetting to default\n");
                upload(dialog,&x264ExtraDefault);
                continue;
            }
            
            break;
        };
        if(reply==GTK_RESPONSE_OK)
        {
            uint32_t b;
          ret=1;
          
          ENTRY_GET(Target,b);
          switch(generic.mode)
          {
            case COMPRESS_CBR:
              generic.bitrate=b*1000;
              break;

            case COMPRESS_2PASS:              
              generic.finalsize=b;
              break;
          case COMPRESS_2PASS_BITRATE:
              generic.avg_bitrate=b;
              break;

            case COMPRESS_AQ:
            case COMPRESS_CQ:              
              SPIN_GET(Quantizer,b);
              generic.qz=b;
              break;            

            default:
              ADM_assert(0);
          }
          memcpy(config,&generic,sizeof(generic));
          config->extraSettings=specific;
          download(dialog,specific);
           
        }
        gtk_unregister_dialog(dialog);
        gtk_widget_destroy(dialog);
        return ret;
  
}

void updateMode( void )
{
  uint32_t b;
                // set the right select button
#define COMBO(x) gtk_combo_box_set_active(GTK_COMBO_BOX(WID(comboboxMode)),x);
  switch (generic.mode)
  {
    case COMPRESS_CBR:
        COMBO(2);
      b=generic.bitrate/1000;
      ENTRY_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelTarget)),"Target bitrate (kb/s):");
      DISABLE(spinbuttonQuantizer);
      ENABLE(entryTarget);
      
      break;

    case COMPRESS_2PASS:
        COMBO(3);
      b=generic.finalsize;
      ENTRY_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelTarget)),"Target FinalSize (MB):");
      DISABLE(spinbuttonQuantizer);
      ENABLE(entryTarget);
      break;
 case COMPRESS_2PASS_BITRATE:
      COMBO(4);
      b=generic.avg_bitrate;
      ENTRY_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelTarget)),"Dual pass, avg bitate (kb/s):");
      DISABLE(spinbuttonQuantizer);
      ENABLE(entryTarget);
      break;

    case COMPRESS_CQ:
        COMBO(1);
      b=generic.qz;
      SPIN_SET(Quantizer,b);     
      ENABLE(spinbuttonQuantizer);
      DISABLE(entryTarget);
      break;

    case COMPRESS_AQ:
        COMBO(0);
      b=generic.qz;
      SPIN_SET(Quantizer,b);
      ENABLE(spinbuttonQuantizer);
      DISABLE(entryTarget);
      break;

    default:
      ADM_assert(0);
#undef COMBO
  }
}               
int cb_mod(GtkObject * object, gpointer user_data)
{
  int r;
  r=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxMode)));
  switch(r)
  {
    case 0: generic.mode=COMPRESS_AQ ;break;
    case 1: generic.mode=COMPRESS_CQ ;break;
    case 2: generic.mode=COMPRESS_CBR ;break;
    case 3: generic.mode=COMPRESS_2PASS ;break;
    case 4: generic.mode=COMPRESS_2PASS_BITRATE ;break;
    default: ADM_assert(0);
        
  }
  updateMode();
}
#undef SPIN_SET
#undef SPIN_GET
#define CHECK_SET(x)  {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbutton##x)),param->x);}
#define CHECK_GET(x)  {param->x=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbutton##x)));}
#define SPIN_SET(x)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton##x)),(gfloat)param->x) ;}
#define SPIN_GET(x)  {param->x=(int32_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(spinbutton##x))) ;}
#undef ENTRY_SET
#undef ENTRY_GET
#define ENTRY_SET(x) {gtk_write_entry(WID(entry##x),(int)param->x);}
#define ENTRY_GET(x) {param->x=gtk_read_entry(WID(entry##x));}
#define COMBO_SET(x) {gtk_combo_box_set_active(GTK_COMBO_BOX(WID(combobox##x)),param->x);}
#define COMBO_GET(x) {param->x=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(combobox##x)));}

#define PSEUDO_SET(x)  {gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(WID(checkbutton##x)),param->_##x);}
#define PSEUDO_GET(x)  {param->_##x=gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(WID(checkbutton##x)));}

#define LIST_OF_WIDGET \
    CHECK(BasReference); \
    CHECK(BidirME);\
    CHECK(Adaptative);\
    CHECK(Weighted);\
    CHECK(MixedRefs); \
    CHECK(CABAC);\
    SPIN(Trellis);\
    SPIN(NoiseReduction);\
    CHECK(DeblockingFilter); \
    CHECK(ChromaME); \
    \
    SPIN(KeyframeBoost);\
    SPIN(BframeReduction);\
    SPIN(BitrateVariability);\
    SPIN(MinQp);\
    SPIN(MaxQp);\
    SPIN(QpStep);\
    SPIN(SceneCut);\
    SPIN(MinIdr);\
    SPIN(MaxIdr);\
    \
    SPIN(MaxRefFrames); \
    SPIN(MaxBFrame);\
    SPIN(Bias);\
    SPIN(Strength);\
    SPIN(Threshold); \
    ENTRY(AR_Num);\
    ENTRY(AR_Den);\
    \
    COMBO(PartitionDecision); \
    SPIN(Range);\
    COMBO(Method); \
    COMBO(DirectMode); \
    PSEUDO(8x8); \
    PSEUDO(8x8P); \
    PSEUDO(8x8B);\
    PSEUDO(4x4);\
    PSEUDO(8x8I);\
    PSEUDO(4x4I);\
    
void upload(GtkWidget *dialog,ADM_x264Param *param)
{
#define SPIN SPIN_SET
#define CHECK CHECK_SET
#define ENTRY ENTRY_SET
#define COMBO COMBO_SET
#define PSEUDO PSEUDO_SET
    LIST_OF_WIDGET
#undef SPIN
#undef CHECK
#undef ENTRY
#undef PSEUDO
}
void download(GtkWidget *dialog,ADM_x264Param *param)
{
#define SPIN SPIN_GET
#define CHECK CHECK_GET
#define ENTRY ENTRY_GET
#undef COMBO
#define COMBO COMBO_GET
#define PSEUDO PSEUDO_GET
    LIST_OF_WIDGET
#undef SPIN
#undef CHECK
}

/*********************************************/
GtkWidget*
create_dialog1 (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *tableBitrate;
  GtkWidget *labelEncodingMode;
  GtkWidget *entryTarget;
  GtkWidget *labelQuantizer;
  GtkObject *spinbuttonQuantizer_adj;
  GtkWidget *spinbuttonQuantizer;
  GtkWidget *hbuttonbox1;
  GtkWidget *buttonLoadDefaults;
  GtkWidget *buttonSaveDefaults;
  GtkWidget *buttonResetDefaults;
  GtkWidget *labelTarget;
  GtkWidget *comboboxMode;
  GtkWidget *labelPageBitrate;
  GtkWidget *vbox5;
  GtkWidget *frame6;
  GtkWidget *alignment6;
  GtkWidget *vbox6;
  GtkWidget *table7;
  GtkWidget *labelPartitionDecision;
  GtkWidget *labelMethod;
  GtkWidget *comboboxPartitionDecision;
  GtkWidget *comboboxMethod;
  GtkWidget *table11;
  GtkObject *spinbuttonMaxRefFrames_adj;
  GtkWidget *spinbuttonMaxRefFrames;
  GtkWidget *labelMaxRefFrames;
  GtkWidget *labelRange;
  GtkWidget *checkbuttonChromaME;
  GtkWidget *checkbuttonMixedRefs;
  GtkObject *spinbuttonRange_adj;
  GtkWidget *spinbuttonRange;
  GtkWidget *label24;
  GtkWidget *frame7;
  GtkWidget *alignment7;
  GtkWidget *vbox7;
  GtkWidget *hbox2;
  GtkWidget *labelSampleAR;
  GtkWidget *entryAR_Num;
  GtkWidget *label38;
  GtkWidget *entryAR_Den;
  GtkWidget *hbox3;
  GtkWidget *checkbuttonDeblockingFilter;
  GtkWidget *table8;
  GtkObject *spinbuttonStrength_adj;
  GtkWidget *spinbuttonStrength;
  GtkObject *spinbuttonThreshold_adj;
  GtkWidget *spinbuttonThreshold;
  GtkWidget *labelThreshold;
  GtkWidget *labelStrength;
  GtkWidget *hbox4;
  GtkWidget *checkbuttonCABAC;
  GtkWidget *labelTrellis;
  GtkObject *spinbuttonTrellis_adj;
  GtkWidget *spinbuttonTrellis;
  GtkWidget *labelNoiseReduction;
  GtkObject *spinbuttonNoiseReduction_adj;
  GtkWidget *spinbuttonNoiseReduction;
  GtkWidget *label25;
  GtkWidget *labelPageMotion_More;
  GtkWidget *vbox2;
  GtkWidget *frame4;
  GtkWidget *alignment4;
  GtkWidget *vbox3;
  GtkWidget *checkbutton8x8;
  GtkWidget *checkbutton8x8P;
  GtkWidget *checkbutton8x8B;
  GtkWidget *checkbutton4x4;
  GtkWidget *checkbutton8x8I;
  GtkWidget *checkbutton4x4I;
  GtkWidget *label19;
  GtkWidget *frame5;
  GtkWidget *alignment5;
  GtkWidget *vbox4;
  GtkWidget *table5;
  GtkWidget *labelBias;
  GtkObject *spinbuttonBias_adj;
  GtkWidget *spinbuttonBias;
  GtkWidget *labelMaxConsecutive;
  GtkObject *spinbuttonMaxBFrame_adj;
  GtkWidget *spinbuttonMaxBFrame;
  GtkWidget *table6;
  GtkWidget *checkbuttonBidirME;
  GtkWidget *checkbuttonWeighted;
  GtkWidget *checkbuttonAdaptative;
  GtkWidget *checkbuttonBasReference;
  GtkWidget *hbox1;
  GtkWidget *labelBFrameDirectMode;
  GtkWidget *comboboxDirectMode;
  GtkWidget *label20;
  GtkWidget *labelPagePartion_Frame;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *alignment1;
  GtkWidget *table2;
  GtkObject *spinbuttonKeyframeBoost_adj;
  GtkWidget *spinbuttonKeyframeBoost;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkObject *spinbuttonBframeReduction_adj;
  GtkWidget *spinbuttonBframeReduction;
  GtkObject *spinbuttonBitrateVariability_adj;
  GtkWidget *spinbuttonBitrateVariability;
  GtkWidget *label7;
  GtkWidget *frame2;
  GtkWidget *alignment2;
  GtkWidget *table3;
  GtkWidget *label13;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkObject *spinbuttonMinQp_adj;
  GtkWidget *spinbuttonMinQp;
  GtkObject *spinbuttonMaxQp_adj;
  GtkWidget *spinbuttonMaxQp;
  GtkObject *spinbuttonQpStep_adj;
  GtkWidget *spinbuttonQpStep;
  GtkWidget *label8;
  GtkWidget *frame3;
  GtkWidget *alignment3;
  GtkWidget *table4;
  GtkWidget *label16;
  GtkObject *spinbuttonSceneCut_adj;
  GtkWidget *spinbuttonSceneCut;
  GtkWidget *label17;
  GtkObject *spinbuttonMinIdr_adj;
  GtkWidget *spinbuttonMinIdr;
  GtkObject *spinbuttonMaxIdr_adj;
  GtkWidget *spinbuttonMaxIdr;
  GtkWidget *label18;
  GtkWidget *labelMoreEncodingSettings;
  GtkWidget *labelPageRateControl;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), _("X264 configuration"));
  gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  tableBitrate = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (tableBitrate);
  gtk_container_add (GTK_CONTAINER (notebook1), tableBitrate);
  gtk_table_set_row_spacings (GTK_TABLE (tableBitrate), 2);

  labelEncodingMode = gtk_label_new (_("Encoding Mode:"));
  gtk_widget_show (labelEncodingMode);
  gtk_table_attach (GTK_TABLE (tableBitrate), labelEncodingMode, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 1);
  gtk_misc_set_alignment (GTK_MISC (labelEncodingMode), 0, 0.5);

  entryTarget = gtk_entry_new ();
  gtk_widget_show (entryTarget);
  gtk_table_attach (GTK_TABLE (tableBitrate), entryTarget, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 2, 0);
  gtk_tooltips_set_tip (tooltips, entryTarget, _("Entry input for target file size or bitrate"), NULL);

  labelQuantizer = gtk_label_new (_("Quantizer (0-51):"));
  gtk_widget_show (labelQuantizer);
  gtk_table_attach (GTK_TABLE (tableBitrate), labelQuantizer, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 1);
  gtk_misc_set_alignment (GTK_MISC (labelQuantizer), 0, 0.5);

  spinbuttonQuantizer_adj = gtk_adjustment_new (4, 0, 51, 1, 10, 10);
  spinbuttonQuantizer = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQuantizer_adj), 1, 0);
  gtk_widget_show (spinbuttonQuantizer);
  gtk_table_attach (GTK_TABLE (tableBitrate), spinbuttonQuantizer, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 1, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonQuantizer, _("Constant Quantizer - Each frame will the same compression. Low numbers equal higher quality while high numbers equal lower quality"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQuantizer), TRUE);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_table_attach (GTK_TABLE (tableBitrate), hbuttonbox1, 0, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 40, 65);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  buttonLoadDefaults = gtk_button_new_with_mnemonic (_("Load Defaults"));
  gtk_widget_show (buttonLoadDefaults);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonLoadDefaults);
  GTK_WIDGET_SET_FLAGS (buttonLoadDefaults, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, buttonLoadDefaults, _("Load the stored default settings for the codec"), NULL);

  buttonSaveDefaults = gtk_button_new_with_mnemonic (_("Save Defaults"));
  gtk_widget_show (buttonSaveDefaults);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonSaveDefaults);
  GTK_WIDGET_SET_FLAGS (buttonSaveDefaults, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, buttonSaveDefaults, _("Save the current codec configuration as the stored default settings"), NULL);

  buttonResetDefaults = gtk_button_new_with_mnemonic (_("Reset Defaults"));
  gtk_widget_show (buttonResetDefaults);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), buttonResetDefaults);
  GTK_WIDGET_SET_FLAGS (buttonResetDefaults, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, buttonResetDefaults, _("Reload the initial default codec settings"), NULL);

  labelTarget = gtk_label_new (_("Target Size:"));
  gtk_widget_show (labelTarget);
  gtk_table_attach (GTK_TABLE (tableBitrate), labelTarget, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 0);
  gtk_misc_set_alignment (GTK_MISC (labelTarget), 0, 0.5);

  comboboxMode = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxMode);
  gtk_table_attach (GTK_TABLE (tableBitrate), comboboxMode, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Single Pass - Quality Quantizer (Average)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Single Pass - Quality Quantizer (Constant)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Single Pass - Bitrate (Average)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Two Pass - File Size"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Two Pass - Average bitrate"));

  labelPageBitrate = gtk_label_new (_("Bitrate"));
  gtk_widget_show (labelPageBitrate);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), labelPageBitrate);
  gtk_misc_set_alignment (GTK_MISC (labelPageBitrate), 0, 0.5);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox5);

  frame6 = gtk_frame_new (NULL);
  gtk_widget_show (frame6);
  gtk_box_pack_start (GTK_BOX (vbox5), frame6, TRUE, TRUE, 0);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (frame6), alignment6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 0);

  vbox6 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox6);
  gtk_container_add (GTK_CONTAINER (alignment6), vbox6);

  table7 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table7);
  gtk_box_pack_start (GTK_BOX (vbox6), table7, FALSE, TRUE, 0);

  labelPartitionDecision = gtk_label_new (_("Partition decision"));
  gtk_widget_show (labelPartitionDecision);
  gtk_table_attach (GTK_TABLE (table7), labelPartitionDecision, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 1);
  gtk_misc_set_alignment (GTK_MISC (labelPartitionDecision), 0, 0.5);

  labelMethod = gtk_label_new (_("Method"));
  gtk_widget_show (labelMethod);
  gtk_table_attach (GTK_TABLE (table7), labelMethod, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 1);
  gtk_misc_set_alignment (GTK_MISC (labelMethod), 0, 0.5);

  comboboxPartitionDecision = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxPartitionDecision);
  gtk_table_attach (GTK_TABLE (table7), comboboxPartitionDecision, 1, 3, 0, 1,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL), 1, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("1  - Extremely Low (Fastest)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("2  - Very Low"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("3  - Low"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("4  - Medium"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("5  - High (Default)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("6  - Very High"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("6B - Very High (RDO on Bframes)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("7 - Ultra High"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("7B - Ultra High (RDO on Bframes)"));

  comboboxMethod = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxMethod);
  gtk_table_attach (GTK_TABLE (table7), comboboxMethod, 1, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 1, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Diamond search"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Hexagonal search"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Uneven multi hexagon"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Exhaustive search"));

  table11 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table11);
  gtk_box_pack_start (GTK_BOX (vbox6), table11, TRUE, FALSE, 0);

  spinbuttonMaxRefFrames_adj = gtk_adjustment_new (1, 0, 16, 1, 10, 10);
  spinbuttonMaxRefFrames = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxRefFrames_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxRefFrames);
  gtk_table_attach (GTK_TABLE (table11), spinbuttonMaxRefFrames, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxRefFrames, _("Set how many previous frames can be referenced by a P/B-frame. Numbers above 5 do not seem to improve quality greatly. Numbers are 3 to 5 are recommended"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxRefFrames), TRUE);

  labelMaxRefFrames = gtk_label_new (_("Max. Ref. frames"));
  gtk_widget_show (labelMaxRefFrames);
  gtk_table_attach (GTK_TABLE (table11), labelMaxRefFrames, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 7, 0);
  gtk_misc_set_alignment (GTK_MISC (labelMaxRefFrames), 0, 0.5);

  labelRange = gtk_label_new (_("Range"));
  gtk_widget_show (labelRange);
  gtk_table_attach (GTK_TABLE (table11), labelRange, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 7, 0);
  gtk_misc_set_alignment (GTK_MISC (labelRange), 0, 0.5);

  checkbuttonChromaME = gtk_check_button_new_with_mnemonic (_("Chroma ME"));
  gtk_widget_show (checkbuttonChromaME);
  gtk_table_attach (GTK_TABLE (table11), checkbuttonChromaME, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 15, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonChromaME, _("Uses chroma (color) components for motion for compression and helps the quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonChromaME), TRUE);

  checkbuttonMixedRefs = gtk_check_button_new_with_mnemonic (_("Mixed Refs"));
  gtk_widget_show (checkbuttonMixedRefs);
  gtk_table_attach (GTK_TABLE (table11), checkbuttonMixedRefs, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 15, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonMixedRefs, _("Calculate referencing individually based on each partition"), NULL);

  spinbuttonRange_adj = gtk_adjustment_new (16, 0, 64, 1, 10, 10);
  spinbuttonRange = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonRange_adj), 1, 0);
  gtk_widget_show (spinbuttonRange);
  gtk_table_attach (GTK_TABLE (table11), spinbuttonRange, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonRange, _("Define how many pixels are analysised. The higher the range the more accurate the analysis but the slower the encoding time"), NULL);

  label24 = gtk_label_new (_("<b>Motion Estimation</b>"));
  gtk_widget_show (label24);
  gtk_frame_set_label_widget (GTK_FRAME (frame6), label24);
  gtk_label_set_use_markup (GTK_LABEL (label24), TRUE);

  frame7 = gtk_frame_new (NULL);
  gtk_widget_show (frame7);
  gtk_box_pack_start (GTK_BOX (vbox5), frame7, TRUE, TRUE, 1);

  alignment7 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment7);
  gtk_container_add (GTK_CONTAINER (frame7), alignment7);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment7), 0, 0, 12, 0);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox7);
  gtk_container_add (GTK_CONTAINER (alignment7), vbox7);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox2, TRUE, TRUE, 0);

  labelSampleAR = gtk_label_new (_("Sample A/R "));
  gtk_widget_show (labelSampleAR);
  gtk_box_pack_start (GTK_BOX (hbox2), labelSampleAR, FALSE, FALSE, 0);

  entryAR_Num = gtk_entry_new ();
  gtk_widget_show (entryAR_Num);
  gtk_box_pack_start (GTK_BOX (hbox2), entryAR_Num, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, entryAR_Num, _("Enforce the size of a decoded pixel decoded to a certain value. Leave this at 1:1 for non-anamorphic video"), NULL);
  gtk_entry_set_text (GTK_ENTRY (entryAR_Num), _("1"));

  label38 = gtk_label_new (_(":"));
  gtk_widget_show (label38);
  gtk_box_pack_start (GTK_BOX (hbox2), label38, FALSE, FALSE, 2);
  gtk_label_set_justify (GTK_LABEL (label38), GTK_JUSTIFY_CENTER);

  entryAR_Den = gtk_entry_new ();
  gtk_widget_show (entryAR_Den);
  gtk_box_pack_start (GTK_BOX (hbox2), entryAR_Den, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, entryAR_Den, _("Enforce the size of a decoded pixel decoded to a certain value. Leave this at 1:1 for non-anamorphic video"), NULL);
  gtk_entry_set_text (GTK_ENTRY (entryAR_Den), _("1"));

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox3, TRUE, TRUE, 0);

  checkbuttonDeblockingFilter = gtk_check_button_new_with_mnemonic (_("Deblocking Filter :"));
  gtk_widget_show (checkbuttonDeblockingFilter);
  gtk_box_pack_start (GTK_BOX (hbox3), checkbuttonDeblockingFilter, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonDeblockingFilter, _("Enable in-loop deblocking to filter the video"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonDeblockingFilter), TRUE);

  table8 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table8);
  gtk_box_pack_start (GTK_BOX (hbox3), table8, TRUE, TRUE, 0);

  spinbuttonStrength_adj = gtk_adjustment_new (0, -6, 6, 1, 10, 10);
  spinbuttonStrength = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonStrength_adj), 1, 0);
  gtk_widget_show (spinbuttonStrength);
  gtk_table_attach (GTK_TABLE (table8), spinbuttonStrength, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonStrength, _("Set the amount of blurring to use for block edges when they are found"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonStrength), TRUE);

  spinbuttonThreshold_adj = gtk_adjustment_new (0, -6, 6, 1, 10, 10);
  spinbuttonThreshold = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonThreshold_adj), 1, 0);
  gtk_widget_show (spinbuttonThreshold);
  gtk_table_attach (GTK_TABLE (table8), spinbuttonThreshold, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonThreshold, _("Define the level for when deblocking is applied"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonThreshold), TRUE);

  labelThreshold = gtk_label_new (_("<i>Threshold</i>"));
  gtk_widget_show (labelThreshold);
  gtk_table_attach (GTK_TABLE (table8), labelThreshold, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 0);
  gtk_label_set_use_markup (GTK_LABEL (labelThreshold), TRUE);
  gtk_misc_set_alignment (GTK_MISC (labelThreshold), 0, 0.5);

  labelStrength = gtk_label_new (_("<i>Strength</i>"));
  gtk_widget_show (labelStrength);
  gtk_table_attach (GTK_TABLE (table8), labelStrength, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 0);
  gtk_label_set_use_markup (GTK_LABEL (labelStrength), TRUE);
  gtk_misc_set_alignment (GTK_MISC (labelStrength), 0, 0.5);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox4, TRUE, TRUE, 0);

  checkbuttonCABAC = gtk_check_button_new_with_mnemonic (_("CABAC"));
  gtk_widget_show (checkbuttonCABAC);
  gtk_box_pack_start (GTK_BOX (hbox4), checkbuttonCABAC, FALSE, FALSE, 4);
  gtk_tooltips_set_tip (tooltips, checkbuttonCABAC, _("Lossless compression method which usually improves the bitrate usage by 15% (especially on high bitrate). Helps the quality a lot but is slower. "), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonCABAC), TRUE);

  labelTrellis = gtk_label_new (_("Trellis"));
  gtk_widget_show (labelTrellis);
  gtk_box_pack_start (GTK_BOX (hbox4), labelTrellis, TRUE, TRUE, 4);
  gtk_misc_set_alignment (GTK_MISC (labelTrellis), 1, 0.5);

  spinbuttonTrellis_adj = gtk_adjustment_new (1, 0, 2, 1, 10, 10);
  spinbuttonTrellis = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTrellis_adj), 1, 0);
  gtk_widget_show (spinbuttonTrellis);
  gtk_box_pack_start (GTK_BOX (hbox4), spinbuttonTrellis, FALSE, TRUE, 1);
  gtk_tooltips_set_tip (tooltips, spinbuttonTrellis, _("Use rate distortion quantization to find optimal encoding for each block. Level 0 equals disabled, level 1 equals normal and level 2 equals high"), NULL);

  labelNoiseReduction = gtk_label_new (_("Noise Reduction"));
  gtk_widget_show (labelNoiseReduction);
  gtk_box_pack_start (GTK_BOX (hbox4), labelNoiseReduction, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (labelNoiseReduction), 0.8, 0.5);

  spinbuttonNoiseReduction_adj = gtk_adjustment_new (0, 0, 1000000000, 1, 10, 10);
  spinbuttonNoiseReduction = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonNoiseReduction_adj), 1, 0);
  gtk_widget_show (spinbuttonNoiseReduction);
  gtk_box_pack_start (GTK_BOX (hbox4), spinbuttonNoiseReduction, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonNoiseReduction, _("Set the amount of noise reduction"), NULL);

  label25 = gtk_label_new (_("<b>Misc. Options</b>"));
  gtk_widget_show (label25);
  gtk_frame_set_label_widget (GTK_FRAME (frame7), label25);
  gtk_label_set_use_markup (GTK_LABEL (label25), TRUE);

  labelPageMotion_More = gtk_label_new (_("Motion & Misc"));
  gtk_widget_show (labelPageMotion_More);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), labelPageMotion_More);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (vbox2), frame4, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame4), GTK_SHADOW_IN);

  alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (frame4), alignment4);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment4), 0, 0, 12, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (alignment4), vbox3);

  checkbutton8x8 = gtk_check_button_new_with_mnemonic (_("8x8 Transform"));
  gtk_widget_show (checkbutton8x8);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton8x8, _("General block breakdown transformation"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton8x8), TRUE);

  checkbutton8x8P = gtk_check_button_new_with_mnemonic (_("8x8, 8x16 and 16x8 P-frame search"));
  gtk_widget_show (checkbutton8x8P);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8P, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton8x8P, _("Improve the P-frame quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton8x8P), TRUE);

  checkbutton8x8B = gtk_check_button_new_with_mnemonic (_("8x8, 8x16 and 16x8 B-frame search"));
  gtk_widget_show (checkbutton8x8B);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8B, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton8x8B, _("Improve the B-frame quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton8x8B), TRUE);

  checkbutton4x4 = gtk_check_button_new_with_mnemonic (_("4x4, 4x8 and 8x4 P-frame search"));
  gtk_widget_show (checkbutton4x4);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton4x4, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton4x4, _("Further improve the P-frame quality"), NULL);

  checkbutton8x8I = gtk_check_button_new_with_mnemonic (_("8x8 Intra search"));
  gtk_widget_show (checkbutton8x8I);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8I, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton8x8I, _("Enable DCT Intra block search to improve quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton8x8I), TRUE);

  checkbutton4x4I = gtk_check_button_new_with_mnemonic (_("4x4 Intra search"));
  gtk_widget_show (checkbutton4x4I);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton4x4I, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbutton4x4I, _("Further enable DCT Intra block search to improve quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton4x4I), TRUE);

  label19 = gtk_label_new (_("<b>Partition Macroblocks</b>"));
  gtk_widget_show (label19);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label19);
  gtk_label_set_use_markup (GTK_LABEL (label19), TRUE);

  frame5 = gtk_frame_new (NULL);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox2), frame5, TRUE, TRUE, 1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame5), GTK_SHADOW_IN);

  alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (frame5), alignment5);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment5), 0, 0, 12, 0);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (alignment5), vbox4);

  table5 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table5);
  gtk_box_pack_start (GTK_BOX (vbox4), table5, TRUE, TRUE, 0);

  labelBias = gtk_label_new (_("Bias"));
  gtk_widget_show (labelBias);
  gtk_table_attach (GTK_TABLE (table5), labelBias, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelBias), 0, 0.5);

  spinbuttonBias_adj = gtk_adjustment_new (0, -100, 100, 1, 10, 10);
  spinbuttonBias = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBias_adj), 1, 0);
  gtk_widget_show (spinbuttonBias);
  gtk_table_attach (GTK_TABLE (table5), spinbuttonBias, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonBias, _("Increase / decrease probability for how often B-frames are used. Will not violate the maximum consecutive frame limit"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBias), TRUE);

  labelMaxConsecutive = gtk_label_new (_("Max Consecutive"));
  gtk_widget_show (labelMaxConsecutive);
  gtk_table_attach (GTK_TABLE (table5), labelMaxConsecutive, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelMaxConsecutive), 0, 0.5);

  spinbuttonMaxBFrame_adj = gtk_adjustment_new (3, 0, 15, 1, 10, 10);
  spinbuttonMaxBFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxBFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxBFrame);
  gtk_table_attach (GTK_TABLE (table5), spinbuttonMaxBFrame, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxBFrame, _("Set the maximum number of consecutive B-frames. This defines how many duplicate frames can be droped. Numbers 2 to 5 are recommended. This greatly improves the use of bitrate and quality"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxBFrame), TRUE);

  table6 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table6);
  gtk_box_pack_start (GTK_BOX (vbox4), table6, TRUE, TRUE, 0);

  checkbuttonBidirME = gtk_check_button_new_with_mnemonic (_("Bidirectional ME"));
  gtk_widget_show (checkbuttonBidirME);
  gtk_table_attach (GTK_TABLE (table6), checkbuttonBidirME, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonBidirME, _("Jointly optimize both Motion Vector's in B-frames. This will improve quality but take more time for encoding"), NULL);

  checkbuttonWeighted = gtk_check_button_new_with_mnemonic (_("Weighted biprediction"));
  gtk_widget_show (checkbuttonWeighted);
  gtk_table_attach (GTK_TABLE (table6), checkbuttonWeighted, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonWeighted, _("Enables weighting of B-frames to help fades"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonWeighted), TRUE);

  checkbuttonAdaptative = gtk_check_button_new_with_mnemonic (_("Adaptative DCT"));
  gtk_widget_show (checkbuttonAdaptative);
  gtk_table_attach (GTK_TABLE (table6), checkbuttonAdaptative, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 4, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonAdaptative, _("Use fewer B-frame if needed. This is always recommended. If not enabled, the codec will always use the maximum number of consecutive B-frames"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonAdaptative), TRUE);

  checkbuttonBasReference = gtk_check_button_new_with_mnemonic (_("Use as reference"));
  gtk_widget_show (checkbuttonBasReference);
  gtk_table_attach (GTK_TABLE (table6), checkbuttonBasReference, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 5, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonBasReference, _("Allow B-frames to make references non-linearly to another B-frame (instead of creating a duplicate copy)"), NULL);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox1, TRUE, TRUE, 0);

  labelBFrameDirectMode = gtk_label_new (_("B-Frame Direct Mode:  "));
  gtk_widget_show (labelBFrameDirectMode);
  gtk_box_pack_start (GTK_BOX (hbox1), labelBFrameDirectMode, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (labelBFrameDirectMode), 1, 0.5);

  comboboxDirectMode = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxDirectMode);
  gtk_box_pack_start (GTK_BOX (hbox1), comboboxDirectMode, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Temporal"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Spatial"));

  label20 = gtk_label_new (_("<b>B-Frames</b>"));
  gtk_widget_show (label20);
  gtk_frame_set_label_widget (GTK_FRAME (frame5), label20);
  gtk_label_set_use_markup (GTK_LABEL (label20), TRUE);

  labelPagePartion_Frame = gtk_label_new (_("Partitions & Frames"));
  gtk_widget_show (labelPagePartion_Frame);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), labelPagePartion_Frame);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox1);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 1);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

  table2 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table2);
  gtk_container_add (GTK_CONTAINER (alignment1), table2);

  spinbuttonKeyframeBoost_adj = gtk_adjustment_new (40, 0, 100, 1, 10, 10);
  spinbuttonKeyframeBoost = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonKeyframeBoost_adj), 1, 0);
  gtk_widget_show (spinbuttonKeyframeBoost);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonKeyframeBoost, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (spinbuttonKeyframeBoost, 115, -1);
  gtk_tooltips_set_tip (tooltips, spinbuttonKeyframeBoost, _("Set how much \"bitrate bonus\" a keyframe can get"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonKeyframeBoost), TRUE);

  label10 = gtk_label_new (_("Keyframe boots (%)"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

  label11 = gtk_label_new (_("B-frame reduction (%)"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table2), label11, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new (_("Bitrate variability (%)"));
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  spinbuttonBframeReduction_adj = gtk_adjustment_new (30, 0, 100, 1, 10, 10);
  spinbuttonBframeReduction = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBframeReduction_adj), 1, 0);
  gtk_widget_show (spinbuttonBframeReduction);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonBframeReduction, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonBframeReduction, _("Set how much bitrate is deducted from a B-frame as compared to the previous P-frame"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBframeReduction), TRUE);

  spinbuttonBitrateVariability_adj = gtk_adjustment_new (60, 0, 100, 1, 10, 10);
  spinbuttonBitrateVariability = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBitrateVariability_adj), 1, 0);
  gtk_widget_show (spinbuttonBitrateVariability);
  gtk_table_attach (GTK_TABLE (table2), spinbuttonBitrateVariability, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonBitrateVariability, _("Define how much the bitrate can fluctuate over the entire video. 0% results in a constant bitrate stream, while 100% results in a pure quality based bitrate stream"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBitrateVariability), TRUE);

  label7 = gtk_label_new (_("<b>Bitrate</b>"));
  gtk_widget_show (label7);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label7);
  gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 1);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

  table3 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (alignment2), table3);

  label13 = gtk_label_new (_("Min Qp"));
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table3), label13, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  label14 = gtk_label_new (_("Max Qp"));
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table3), label14, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  label15 = gtk_label_new (_("Max QP Step"));
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table3), label15, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  spinbuttonMinQp_adj = gtk_adjustment_new (10, 10, 51, 1, 10, 10);
  spinbuttonMinQp = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinQp_adj), 1, 0);
  gtk_widget_show (spinbuttonMinQp);
  gtk_table_attach (GTK_TABLE (table3), spinbuttonMinQp, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (spinbuttonMinQp, 54, -1);
  gtk_tooltips_set_tip (tooltips, spinbuttonMinQp, _("Enforce a minimum quantizer level"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMinQp), TRUE);

  spinbuttonMaxQp_adj = gtk_adjustment_new (51, 10, 51, 1, 10, 10);
  spinbuttonMaxQp = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxQp_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxQp);
  gtk_table_attach (GTK_TABLE (table3), spinbuttonMaxQp, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxQp, _("Enforce a maximum quantizer level"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxQp), TRUE);

  spinbuttonQpStep_adj = gtk_adjustment_new (4, 0, 10, 1, 10, 10);
  spinbuttonQpStep = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQpStep_adj), 1, 0);
  gtk_widget_show (spinbuttonQpStep);
  gtk_table_attach (GTK_TABLE (table3), spinbuttonQpStep, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonQpStep, _("Define how much the quantizer rate can change between two consecutive frames"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQpStep), TRUE);

  label8 = gtk_label_new (_("<b>Quantization Limits</b>"));
  gtk_widget_show (label8);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label8);
  gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox1), frame3, TRUE, TRUE, 1);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (frame3), alignment3);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment3), 0, 0, 12, 0);

  table4 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table4);
  gtk_container_add (GTK_CONTAINER (alignment3), table4);

  label16 = gtk_label_new (_("Scene Cut Threshold"));
  gtk_widget_show (label16);
  gtk_table_attach (GTK_TABLE (table4), label16, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

  spinbuttonSceneCut_adj = gtk_adjustment_new (40, 0, 100, 1, 10, 10);
  spinbuttonSceneCut = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonSceneCut_adj), 1, 0);
  gtk_widget_show (spinbuttonSceneCut);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonSceneCut, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (spinbuttonSceneCut, 115, -1);
  gtk_tooltips_set_tip (tooltips, spinbuttonSceneCut, _("Increase / decrease sensitivity for detecting Scene Changes to compensate for. Improves I-frame usage and helps quality"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonSceneCut), TRUE);

  label17 = gtk_label_new (_("Min IDR frame interval"));
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table4), label17, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

  spinbuttonMinIdr_adj = gtk_adjustment_new (25, 0, 100, 1, 10, 10);
  spinbuttonMinIdr = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinIdr_adj), 1, 0);
  gtk_widget_show (spinbuttonMinIdr);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonMinIdr, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMinIdr, _("Set minimum frame interval between IDR frames. Defines the minimum amount a frame can be reused and referenced by other frames before a new clean one is established"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMinIdr), TRUE);

  spinbuttonMaxIdr_adj = gtk_adjustment_new (250, 1, 1000, 1, 10, 10);
  spinbuttonMaxIdr = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxIdr_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxIdr);
  gtk_table_attach (GTK_TABLE (table4), spinbuttonMaxIdr, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxIdr, _("Set maximum frame interval between IDR frames. Defines the maximum amount a frame can be reused and referenced by other frames before a new clean one is established"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxIdr), TRUE);

  label18 = gtk_label_new (_("Max IDR frame interval"));
  gtk_widget_show (label18);
  gtk_table_attach (GTK_TABLE (table4), label18, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

  labelMoreEncodingSettings = gtk_label_new (_("<b>More Rate Settings</b>"));
  gtk_widget_show (labelMoreEncodingSettings);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), labelMoreEncodingSettings);
  gtk_label_set_use_markup (GTK_LABEL (labelMoreEncodingSettings), TRUE);

  labelPageRateControl = gtk_label_new (_("Rate Control"));
  gtk_widget_show (labelPageRateControl);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), labelPageRateControl);

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
  GLADE_HOOKUP_OBJECT (dialog1, tableBitrate, "tableBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, labelEncodingMode, "labelEncodingMode");
  GLADE_HOOKUP_OBJECT (dialog1, entryTarget, "entryTarget");
  GLADE_HOOKUP_OBJECT (dialog1, labelQuantizer, "labelQuantizer");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQuantizer, "spinbuttonQuantizer");
  GLADE_HOOKUP_OBJECT (dialog1, hbuttonbox1, "hbuttonbox1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonLoadDefaults, "buttonLoadDefaults");
  GLADE_HOOKUP_OBJECT (dialog1, buttonSaveDefaults, "buttonSaveDefaults");
  GLADE_HOOKUP_OBJECT (dialog1, buttonResetDefaults, "buttonResetDefaults");
  GLADE_HOOKUP_OBJECT (dialog1, labelTarget, "labelTarget");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxMode, "comboboxMode");
  GLADE_HOOKUP_OBJECT (dialog1, labelPageBitrate, "labelPageBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, vbox5, "vbox5");
  GLADE_HOOKUP_OBJECT (dialog1, frame6, "frame6");
  GLADE_HOOKUP_OBJECT (dialog1, alignment6, "alignment6");
  GLADE_HOOKUP_OBJECT (dialog1, vbox6, "vbox6");
  GLADE_HOOKUP_OBJECT (dialog1, table7, "table7");
  GLADE_HOOKUP_OBJECT (dialog1, labelPartitionDecision, "labelPartitionDecision");
  GLADE_HOOKUP_OBJECT (dialog1, labelMethod, "labelMethod");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxPartitionDecision, "comboboxPartitionDecision");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxMethod, "comboboxMethod");
  GLADE_HOOKUP_OBJECT (dialog1, table11, "table11");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxRefFrames, "spinbuttonMaxRefFrames");
  GLADE_HOOKUP_OBJECT (dialog1, labelMaxRefFrames, "labelMaxRefFrames");
  GLADE_HOOKUP_OBJECT (dialog1, labelRange, "labelRange");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChromaME, "checkbuttonChromaME");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonMixedRefs, "checkbuttonMixedRefs");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonRange, "spinbuttonRange");
  GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
  GLADE_HOOKUP_OBJECT (dialog1, frame7, "frame7");
  GLADE_HOOKUP_OBJECT (dialog1, alignment7, "alignment7");
  GLADE_HOOKUP_OBJECT (dialog1, vbox7, "vbox7");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, labelSampleAR, "labelSampleAR");
  GLADE_HOOKUP_OBJECT (dialog1, entryAR_Num, "entryAR_Num");
  GLADE_HOOKUP_OBJECT (dialog1, label38, "label38");
  GLADE_HOOKUP_OBJECT (dialog1, entryAR_Den, "entryAR_Den");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDeblockingFilter, "checkbuttonDeblockingFilter");
  GLADE_HOOKUP_OBJECT (dialog1, table8, "table8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonStrength, "spinbuttonStrength");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonThreshold, "spinbuttonThreshold");
  GLADE_HOOKUP_OBJECT (dialog1, labelThreshold, "labelThreshold");
  GLADE_HOOKUP_OBJECT (dialog1, labelStrength, "labelStrength");
  GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCABAC, "checkbuttonCABAC");
  GLADE_HOOKUP_OBJECT (dialog1, labelTrellis, "labelTrellis");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTrellis, "spinbuttonTrellis");
  GLADE_HOOKUP_OBJECT (dialog1, labelNoiseReduction, "labelNoiseReduction");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonNoiseReduction, "spinbuttonNoiseReduction");
  GLADE_HOOKUP_OBJECT (dialog1, label25, "label25");
  GLADE_HOOKUP_OBJECT (dialog1, labelPageMotion_More, "labelPageMotion_More");
  GLADE_HOOKUP_OBJECT (dialog1, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (dialog1, frame4, "frame4");
  GLADE_HOOKUP_OBJECT (dialog1, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (dialog1, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton8x8, "checkbutton8x8");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton8x8P, "checkbutton8x8P");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton8x8B, "checkbutton8x8B");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton4x4, "checkbutton4x4");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton8x8I, "checkbutton8x8I");
  GLADE_HOOKUP_OBJECT (dialog1, checkbutton4x4I, "checkbutton4x4I");
  GLADE_HOOKUP_OBJECT (dialog1, label19, "label19");
  GLADE_HOOKUP_OBJECT (dialog1, frame5, "frame5");
  GLADE_HOOKUP_OBJECT (dialog1, alignment5, "alignment5");
  GLADE_HOOKUP_OBJECT (dialog1, vbox4, "vbox4");
  GLADE_HOOKUP_OBJECT (dialog1, table5, "table5");
  GLADE_HOOKUP_OBJECT (dialog1, labelBias, "labelBias");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBias, "spinbuttonBias");
  GLADE_HOOKUP_OBJECT (dialog1, labelMaxConsecutive, "labelMaxConsecutive");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxBFrame, "spinbuttonMaxBFrame");
  GLADE_HOOKUP_OBJECT (dialog1, table6, "table6");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBidirME, "checkbuttonBidirME");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonWeighted, "checkbuttonWeighted");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonAdaptative, "checkbuttonAdaptative");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBasReference, "checkbuttonBasReference");
  GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (dialog1, labelBFrameDirectMode, "labelBFrameDirectMode");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxDirectMode, "comboboxDirectMode");
  GLADE_HOOKUP_OBJECT (dialog1, label20, "label20");
  GLADE_HOOKUP_OBJECT (dialog1, labelPagePartion_Frame, "labelPagePartion_Frame");
  GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonKeyframeBoost, "spinbuttonKeyframeBoost");
  GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
  GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
  GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBframeReduction, "spinbuttonBframeReduction");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBitrateVariability, "spinbuttonBitrateVariability");
  GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
  GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (dialog1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
  GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
  GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
  GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinQp, "spinbuttonMinQp");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxQp, "spinbuttonMaxQp");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQpStep, "spinbuttonQpStep");
  GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
  GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (dialog1, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (dialog1, table4, "table4");
  GLADE_HOOKUP_OBJECT (dialog1, label16, "label16");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonSceneCut, "spinbuttonSceneCut");
  GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinIdr, "spinbuttonMinIdr");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxIdr, "spinbuttonMaxIdr");
  GLADE_HOOKUP_OBJECT (dialog1, label18, "label18");
  GLADE_HOOKUP_OBJECT (dialog1, labelMoreEncodingSettings, "labelMoreEncodingSettings");
  GLADE_HOOKUP_OBJECT (dialog1, labelPageRateControl, "labelPageRateControl");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

//  gtk_widget_grab_default (comboboxDirectMode);
  return dialog1;
}

#endif
