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

#include "default.h"


#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"

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

typedef struct X264_AR
{
 uint32_t   num,den;
 const char *symbol; 
};

#define MKAR(x,y,z) {x,y,#x":"#y"  ("#z")"}
const X264_AR x264_ar[]=
{
  MKAR(1,1,1),
  MKAR(3,2,1.50),
  MKAR(4,3,1.33),
  MKAR(16,9,1.78),
  MKAR(71,50,1.42),
  MKAR(107,100,1.07)
  
};
#define NB_X264_AR sizeof(x264_ar)/sizeof(X264_AR)
/*
1:1
3:2 (1.50)
4:3 (1.33)
16:9 (1.78)
71:50 (1.4222:1)
107:100 (1.0666:1)
*/
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
        gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(buttonResetDaults),ACTION_LOAD_DEFAULT);
        //DISABLE(buttonLoadDefaults);
        //DISABLE(buttonSaveDefaults);
        upload(dialog,specific);
        
        updateMode();
        
        // Fill in A/R
        for(int i=0;i<NB_X264_AR;i++)
        {
          gtk_combo_box_append_text(GTK_COMBO_BOX(WID(comboboxentry1)),x264_ar[i].symbol); 
          
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(WID(comboboxentry1)),0);
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
              generic.bitrate=b;
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
      b=generic.bitrate;
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
    CHECK(fastPSkip); \
    CHECK(DCTDecimate); \
    CHECK(interlaced); \
    \
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
    SPIN(vbv_max_bitrate);\
    SPIN(vbv_buffer_size);\
    SPIN(vbv_buffer_init);\
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
        
        /* Extra case for aspect ratio */
        if(RADIO_GET(radiobuttonPredefinedAR))
        {
            int rank=gtk_combo_box_get_active(GTK_COMBO_BOX(WID(comboboxentry1)));
              param-> AR_Num=x264_ar[rank].num;
              param-> AR_Den=x264_ar[rank].den;
        }
}

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
  GtkObject *spinbuttonRange_adj;
  GtkWidget *spinbuttonRange;
  GtkWidget *checkbuttonChromaME;
  GtkWidget *checkbuttonMixedRefs;
  GtkWidget *hbox8;
  GtkWidget *checkbuttonfastPSkip;
  GtkWidget *checkbuttonDCTDecimate;
  GtkWidget *checkbuttoninterlaced;
  GtkWidget *label24;
  GtkWidget *frameSampleAR;
  GtkWidget *alignment12;
  GtkWidget *table35;
  GtkWidget *hbox12;
  GtkWidget *entryAR_Num;
  GtkWidget *label53;
  GtkWidget *entryAR_Den;
  GtkWidget *radiobuttonCustomAR;
  GSList *radiobuttonCustomAR_group = NULL;
  GtkWidget *radiobuttonPredefinedAR;
  GtkWidget *comboboxentry1;
  GtkWidget *label39;
  GtkWidget *frame7;
  GtkWidget *alignment7;
  GtkWidget *vbox7;
  GtkWidget *hbox2;
  GtkWidget *checkbuttonCABAC;
  GtkWidget *labelTrellis;
  GtkObject *spinbuttonTrellis_adj;
  GtkWidget *spinbuttonTrellis;
  GtkWidget *hbox11;
  GtkWidget *labelNoiseReduction;
  GtkObject *spinbuttonNoiseReduction_adj;
  GtkWidget *spinbuttonNoiseReduction;
  GtkWidget *hbox3;
  GtkWidget *checkbuttonDeblockingFilter;
  GtkWidget *table8;
  GtkObject *spinbuttonStrength_adj;
  GtkWidget *spinbuttonStrength;
  GtkObject *spinbuttonThreshold_adj;
  GtkWidget *spinbuttonThreshold;
  GtkWidget *labelThreshold;
  GtkWidget *labelStrength;
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
  GtkObject *spinbuttonMaxBFrame_adj;
  GtkWidget *spinbuttonMaxBFrame;
  GtkWidget *labelMaxConsecutive;
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
  GtkWidget *labelKeyframeboost;
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
  GtkWidget *frameVideoBufferVerifier;
  GtkWidget *alignment11;
  GtkWidget *table12;
  GtkWidget *labelVBVBufferSize;
  GtkWidget *labelVBVInitialbuffer;
  GtkWidget *labelVBVMaximumBitrate;
  GtkObject *spinbuttonvbv_max_bitrate_adj;
  GtkWidget *spinbuttonvbv_max_bitrate;
  GtkObject *spinbuttonvbv_buffer_size_adj;
  GtkWidget *spinbuttonvbv_buffer_size;
  GtkObject *spinbuttonvbv_buffer_init_adj;
  GtkWidget *spinbuttonvbv_buffer_init;
  GtkWidget *labelVideoBufferVerifier;
  GtkWidget *labelPageRateControl;
  GtkWidget *vbox8;
  GtkWidget *frame10;
  GtkWidget *alignment10;
  GtkWidget *table15;
  GtkObject *spinbuttonBitrateVariance_adj;
  GtkWidget *spinbuttonBitrateVariance;
  GtkWidget *labelBitrateVariance;
  GtkObject *spinbuttonQuantizerCompression_adj;
  GtkWidget *spinbuttonQuantizerCompression;
  GtkObject *spinbuttonTempBlurFrame_adj;
  GtkWidget *spinbuttonTempBlurFrame;
  GtkObject *spinbuttonTempBlurQuant_adj;
  GtkWidget *spinbuttonTempBlurQuant;
  GtkWidget *labelQuantizerCompression;
  GtkWidget *labelTempBlurEstFrame;
  GtkWidget *labelTempBlueQuant;
  GtkWidget *labelMisc;
  GtkWidget *frame11;
  GtkWidget *alignment13;
  GtkWidget *table16;
  GtkObject *spinbutton8_adj;
  GtkWidget *spinbutton8;
  GtkObject *spinbutton9_adj;
  GtkWidget *spinbutton9;
  GtkObject *spinbuttonChromaQPOffset_adj;
  GtkWidget *spinbuttonChromaQPOffset;
  GtkWidget *labelFactorbetweenIandP;
  GtkWidget *labelFactorbetweenPandB;
  GtkWidget *labelChromaQPOffset;
  GtkWidget *labelQuantizers;
  GtkWidget *frame9;
  GtkWidget *alignment9;
  GtkWidget *table14;
  GtkWidget *radiobuttonCustommatrix;
  GSList *radiobuttonCustommatrix_group = NULL;
  GtkWidget *filechooserbuttonOpenCQMFile;
  GtkWidget *radiobuttonJVTmatrix;
  GtkWidget *radiobuttonFlatmatrix;
  GtkWidget *buttonEditCustomMatrix;
  GtkWidget *vseparator1;
  GtkWidget *labelFrameQuantizationMatrices;
  GtkWidget *labelMore;
  GtkWidget *dialog_action_area1;
  GtkWidget *buttonResetDaults;
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

  spinbuttonMaxRefFrames_adj = gtk_adjustment_new (0, 0, 16, 1, 10, 10);
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

  spinbuttonRange_adj = gtk_adjustment_new (16, 0, 64, 1, 10, 10);
  spinbuttonRange = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonRange_adj), 1, 0);
  gtk_widget_show (spinbuttonRange);
  gtk_table_attach (GTK_TABLE (table11), spinbuttonRange, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonRange, _("Define how many pixels are analysised. The higher the range the more accurate the analysis but the slower the encoding time"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonRange), TRUE);

  checkbuttonChromaME = gtk_check_button_new_with_mnemonic (_("Chroma ME"));
  gtk_widget_show (checkbuttonChromaME);
  gtk_table_attach (GTK_TABLE (table11), checkbuttonChromaME, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonChromaME, _("Uses chroma (color) components for motion for compression and helps the quality"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonChromaME), TRUE);

  checkbuttonMixedRefs = gtk_check_button_new_with_mnemonic (_("Mixed Refs"));
  gtk_widget_show (checkbuttonMixedRefs);
  gtk_table_attach (GTK_TABLE (table11), checkbuttonMixedRefs, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonMixedRefs, _("Calculate referencing individually based on each partition"), NULL);

  hbox8 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox6), hbox8, TRUE, FALSE, 0);

  checkbuttonfastPSkip = gtk_check_button_new_with_mnemonic (_("Fast P-Skip"));
  gtk_widget_show (checkbuttonfastPSkip);
  gtk_box_pack_start (GTK_BOX (hbox8), checkbuttonfastPSkip, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonfastPSkip, _("Uncheck to enable fast p-skip"), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonfastPSkip), TRUE);

  checkbuttonDCTDecimate = gtk_check_button_new_with_mnemonic (_("DCT decimate"));
  gtk_widget_show (checkbuttonDCTDecimate);
  gtk_box_pack_start (GTK_BOX (hbox8), checkbuttonDCTDecimate, FALSE, FALSE, 0);

  checkbuttoninterlaced = gtk_check_button_new_with_mnemonic (_("Interlace"));
  gtk_widget_show (checkbuttoninterlaced);
  gtk_box_pack_start (GTK_BOX (hbox8), checkbuttoninterlaced, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttoninterlaced, _("Encode interlaced"), NULL);

  label24 = gtk_label_new (_("<b>Motion Estimation</b>"));
  gtk_widget_show (label24);
  gtk_frame_set_label_widget (GTK_FRAME (frame6), label24);
  gtk_label_set_use_markup (GTK_LABEL (label24), TRUE);

  frameSampleAR = gtk_frame_new (NULL);
  gtk_widget_show (frameSampleAR);
  gtk_box_pack_start (GTK_BOX (vbox5), frameSampleAR, TRUE, TRUE, 0);

  alignment12 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment12);
  gtk_container_add (GTK_CONTAINER (frameSampleAR), alignment12);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment12), 0, 0, 12, 0);

  table35 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table35);
  gtk_container_add (GTK_CONTAINER (alignment12), table35);

  hbox12 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox12);
  gtk_table_attach (GTK_TABLE (table35), hbox12, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  entryAR_Num = gtk_entry_new ();
  gtk_widget_show (entryAR_Num);
  gtk_box_pack_start (GTK_BOX (hbox12), entryAR_Num, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, entryAR_Num, _("Enforce the size of a decoded pixel decoded to a certain value. Leave this at 1:1 for non-anamorphic video"), NULL);
  gtk_entry_set_text (GTK_ENTRY (entryAR_Num), _("1"));
  gtk_entry_set_width_chars (GTK_ENTRY (entryAR_Num), 3);

  label53 = gtk_label_new (_(":"));
  gtk_widget_show (label53);
  gtk_box_pack_start (GTK_BOX (hbox12), label53, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label53), GTK_JUSTIFY_CENTER);

  entryAR_Den = gtk_entry_new ();
  gtk_widget_show (entryAR_Den);
  gtk_box_pack_start (GTK_BOX (hbox12), entryAR_Den, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, entryAR_Den, _("Enforce the size of a decoded pixel decoded to a certain value. Leave this at 1:1 for non-anamorphic video"), NULL);
  gtk_entry_set_text (GTK_ENTRY (entryAR_Den), _("1"));
  gtk_entry_set_width_chars (GTK_ENTRY (entryAR_Den), 3);

  radiobuttonCustomAR = gtk_radio_button_new_with_mnemonic (NULL, _("Custom"));
  gtk_widget_show (radiobuttonCustomAR);
  gtk_table_attach (GTK_TABLE (table35), radiobuttonCustomAR, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, radiobuttonCustomAR, _("Set a custom aspect ratio. Default 1:1 is recommended for most video."), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonCustomAR), radiobuttonCustomAR_group);
  radiobuttonCustomAR_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonCustomAR));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobuttonCustomAR), TRUE);

  radiobuttonPredefinedAR = gtk_radio_button_new_with_mnemonic (NULL, _("Predefined Aspect Ratios"));
  gtk_widget_show (radiobuttonPredefinedAR);
  gtk_table_attach (GTK_TABLE (table35), radiobuttonPredefinedAR, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, radiobuttonPredefinedAR, _("Set a common predefined aspect ratio"), NULL);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonPredefinedAR), radiobuttonCustomAR_group);
  radiobuttonCustomAR_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonPredefinedAR));

  comboboxentry1 = gtk_combo_box_entry_new_text ();
  gtk_widget_show (comboboxentry1);
  gtk_table_attach (GTK_TABLE (table35), comboboxentry1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label39 = gtk_label_new (_("<b>Sample Aspect Ratio (A/R)</b>"));
  gtk_widget_show (label39);
  gtk_frame_set_label_widget (GTK_FRAME (frameSampleAR), label39);
  gtk_label_set_use_markup (GTK_LABEL (label39), TRUE);

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

  checkbuttonCABAC = gtk_check_button_new_with_mnemonic (_("CABAC"));
  gtk_widget_show (checkbuttonCABAC);
  gtk_box_pack_start (GTK_BOX (hbox2), checkbuttonCABAC, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, checkbuttonCABAC, _("Lossless compression method which usually improves the bitrate usage by 15% (especially on high bitrate). Helps the quality a lot but is slower. "), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbuttonCABAC), TRUE);

  labelTrellis = gtk_label_new (_("Trellis"));
  gtk_widget_show (labelTrellis);
  gtk_box_pack_start (GTK_BOX (hbox2), labelTrellis, TRUE, TRUE, 4);
  gtk_misc_set_alignment (GTK_MISC (labelTrellis), 1, 0.5);

  spinbuttonTrellis_adj = gtk_adjustment_new (1, 0, 2, 1, 10, 10);
  spinbuttonTrellis = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTrellis_adj), 1, 0);
  gtk_widget_show (spinbuttonTrellis);
  gtk_box_pack_start (GTK_BOX (hbox2), spinbuttonTrellis, FALSE, TRUE, 4);
  gtk_tooltips_set_tip (tooltips, spinbuttonTrellis, _("Use rate distortion quantization to find optimal encoding for each block. Level 0 equals disabled, level 1 equals normal and level 2 equals high"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTrellis), TRUE);

  hbox11 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox11);
  gtk_box_pack_start (GTK_BOX (vbox7), hbox11, TRUE, TRUE, 8);

  labelNoiseReduction = gtk_label_new (_("Noise Reduction"));
  gtk_widget_show (labelNoiseReduction);
  gtk_box_pack_start (GTK_BOX (hbox11), labelNoiseReduction, TRUE, TRUE, 7);
  gtk_misc_set_alignment (GTK_MISC (labelNoiseReduction), 1, 0.5);

  spinbuttonNoiseReduction_adj = gtk_adjustment_new (0, 0, 1000000000, 1, 10, 10);
  spinbuttonNoiseReduction = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonNoiseReduction_adj), 1, 0);
  gtk_widget_show (spinbuttonNoiseReduction);
  gtk_box_pack_start (GTK_BOX (hbox11), spinbuttonNoiseReduction, TRUE, TRUE, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonNoiseReduction, _("Set the amount of noise reduction"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonNoiseReduction), TRUE);

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

  spinbuttonMaxBFrame_adj = gtk_adjustment_new (3, 0, 15, 1, 10, 10);
  spinbuttonMaxBFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxBFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonMaxBFrame);
  gtk_table_attach (GTK_TABLE (table5), spinbuttonMaxBFrame, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonMaxBFrame, _("Set the maximum number of consecutive B-frames. This defines how many duplicate frames can be droped. Numbers 2 to 5 are recommended. This greatly improves the use of bitrate and quality"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonMaxBFrame), TRUE);

  labelMaxConsecutive = gtk_label_new (_("Max Consecutive"));
  gtk_widget_show (labelMaxConsecutive);
  gtk_table_attach (GTK_TABLE (table5), labelMaxConsecutive, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelMaxConsecutive), 0, 0.5);

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
  gtk_box_pack_start (GTK_BOX (vbox4), hbox1, TRUE, FALSE, 0);

  labelBFrameDirectMode = gtk_label_new (_("B-Frame Direct Mode:  "));
  gtk_widget_show (labelBFrameDirectMode);
  gtk_box_pack_start (GTK_BOX (hbox1), labelBFrameDirectMode, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (labelBFrameDirectMode), 1, 0.5);

  comboboxDirectMode = gtk_combo_box_new_text ();
  gtk_widget_show (comboboxDirectMode);
  gtk_box_pack_start (GTK_BOX (hbox1), comboboxDirectMode, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Auto"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Temporal"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Spatial"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("None"));

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

  labelKeyframeboost = gtk_label_new (_("Keyframe boost (%)"));
  gtk_widget_show (labelKeyframeboost);
  gtk_table_attach (GTK_TABLE (table2), labelKeyframeboost, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelKeyframeboost), 0, 0.5);

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

  frameVideoBufferVerifier = gtk_frame_new (NULL);
  gtk_widget_show (frameVideoBufferVerifier);
  gtk_box_pack_start (GTK_BOX (vbox1), frameVideoBufferVerifier, TRUE, TRUE, 0);

  alignment11 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment11);
  gtk_container_add (GTK_CONTAINER (frameVideoBufferVerifier), alignment11);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment11), 0, 0, 12, 0);

  table12 = gtk_table_new (3, 2, TRUE);
  gtk_widget_show (table12);
  gtk_container_add (GTK_CONTAINER (alignment11), table12);

  labelVBVBufferSize = gtk_label_new (_("VBV buffer size"));
  gtk_widget_show (labelVBVBufferSize);
  gtk_table_attach (GTK_TABLE (table12), labelVBVBufferSize, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelVBVBufferSize), 0, 0.5);

  labelVBVInitialbuffer = gtk_label_new (_("Initial VBV buffer (%)"));
  gtk_widget_show (labelVBVInitialbuffer);
  gtk_table_attach (GTK_TABLE (table12), labelVBVInitialbuffer, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelVBVInitialbuffer), 0, 0.5);

  labelVBVMaximumBitrate = gtk_label_new (_("Maximum local bitrate"));
  gtk_widget_show (labelVBVMaximumBitrate);
  gtk_table_attach (GTK_TABLE (table12), labelVBVMaximumBitrate, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelVBVMaximumBitrate), 0, 0.5);

  spinbuttonvbv_max_bitrate_adj = gtk_adjustment_new (0, 0, 99999, 1, 10, 10);
  spinbuttonvbv_max_bitrate = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonvbv_max_bitrate_adj), 1, 0);
  gtk_widget_show (spinbuttonvbv_max_bitrate);
  gtk_table_attach (GTK_TABLE (table12), spinbuttonvbv_max_bitrate, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonvbv_max_bitrate), TRUE);

  spinbuttonvbv_buffer_size_adj = gtk_adjustment_new (0, 0, 99999, 1, 10, 10);
  spinbuttonvbv_buffer_size = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonvbv_buffer_size_adj), 1, 0);
  gtk_widget_show (spinbuttonvbv_buffer_size);
  gtk_table_attach (GTK_TABLE (table12), spinbuttonvbv_buffer_size, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonvbv_buffer_size), TRUE);

  spinbuttonvbv_buffer_init_adj = gtk_adjustment_new (90, 0, 100, 1, 10, 10);
  spinbuttonvbv_buffer_init = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonvbv_buffer_init_adj), 5, 0);
  gtk_widget_show (spinbuttonvbv_buffer_init);
  gtk_table_attach (GTK_TABLE (table12), spinbuttonvbv_buffer_init, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonvbv_buffer_init), TRUE);

  labelVideoBufferVerifier = gtk_label_new (_("<b>Video Buffer Verifier</b>"));
  gtk_widget_show (labelVideoBufferVerifier);
  gtk_frame_set_label_widget (GTK_FRAME (frameVideoBufferVerifier), labelVideoBufferVerifier);
  gtk_label_set_use_markup (GTK_LABEL (labelVideoBufferVerifier), TRUE);

  labelPageRateControl = gtk_label_new (_("Rate Control"));
  gtk_widget_show (labelPageRateControl);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), labelPageRateControl);

  vbox8 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox8);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox8);

  frame10 = gtk_frame_new (NULL);
  gtk_widget_show (frame10);
  gtk_box_pack_start (GTK_BOX (vbox8), frame10, TRUE, TRUE, 0);

  alignment10 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment10);
  gtk_container_add (GTK_CONTAINER (frame10), alignment10);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment10), 0, 0, 12, 0);

  table15 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table15);
  gtk_container_add (GTK_CONTAINER (alignment10), table15);

  spinbuttonBitrateVariance_adj = gtk_adjustment_new (1, 0, 1, 0.10000000149, 10, 10);
  spinbuttonBitrateVariance = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBitrateVariance_adj), 0.10000000149, 1);
  gtk_widget_show (spinbuttonBitrateVariance);
  gtk_table_attach (GTK_TABLE (table15), spinbuttonBitrateVariance, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonBitrateVariance, _("Allowed variance of average bitrate. Lower values mean less variance. Higher values mean more variance."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonBitrateVariance), TRUE);

  labelBitrateVariance = gtk_label_new (_("Bitrate Variance"));
  gtk_widget_show (labelBitrateVariance);
  gtk_table_attach (GTK_TABLE (table15), labelBitrateVariance, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelBitrateVariance), 0, 0.5);

  spinbuttonQuantizerCompression_adj = gtk_adjustment_new (0.600000023842, 0, 1, 0.10000000149, 10, 10);
  spinbuttonQuantizerCompression = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQuantizerCompression_adj), 0.10000000149, 1);
  gtk_widget_show (spinbuttonQuantizerCompression);
  gtk_table_attach (GTK_TABLE (table15), spinbuttonQuantizerCompression, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonQuantizerCompression, _("Allowed variance of average quantizer or quality. Lower values mean less variance. Higher values mean more variance. Note that 0 means constant quality while 1 means constant fluctuation. Recommended 0.6."), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonQuantizerCompression), TRUE);

  spinbuttonTempBlurFrame_adj = gtk_adjustment_new (20, 0, 999, 1, 10, 10);
  spinbuttonTempBlurFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTempBlurFrame_adj), 1, 0);
  gtk_widget_show (spinbuttonTempBlurFrame);
  gtk_table_attach (GTK_TABLE (table15), spinbuttonTempBlurFrame, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTempBlurFrame, _("Reduced fluctuations in Quantizer (before curve compression)"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTempBlurFrame), TRUE);

  spinbuttonTempBlurQuant_adj = gtk_adjustment_new (0.5, 0, 1, 0.5, 10, 10);
  spinbuttonTempBlurQuant = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTempBlurQuant_adj), 0.5, 1);
  gtk_widget_show (spinbuttonTempBlurQuant);
  gtk_table_attach (GTK_TABLE (table15), spinbuttonTempBlurQuant, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonTempBlurQuant, _("Reduce fluctuations in QP (after curve compression)"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTempBlurQuant), TRUE);

  labelQuantizerCompression = gtk_label_new (_("Quantizer Compression"));
  gtk_widget_show (labelQuantizerCompression);
  gtk_table_attach (GTK_TABLE (table15), labelQuantizerCompression, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelQuantizerCompression), 0, 0.5);

  labelTempBlurEstFrame = gtk_label_new (_("Temp. Blur of est Frame Complexity"));
  gtk_widget_show (labelTempBlurEstFrame);
  gtk_table_attach (GTK_TABLE (table15), labelTempBlurEstFrame, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelTempBlurEstFrame), 0, 0.5);

  labelTempBlueQuant = gtk_label_new (_("Temp. Blur of Quant after CC"));
  gtk_widget_show (labelTempBlueQuant);
  gtk_table_attach (GTK_TABLE (table15), labelTempBlueQuant, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelTempBlueQuant), 0, 0.5);

  labelMisc = gtk_label_new (_("<b>Misc</b>"));
  gtk_widget_show (labelMisc);
  gtk_frame_set_label_widget (GTK_FRAME (frame10), labelMisc);
  gtk_label_set_use_markup (GTK_LABEL (labelMisc), TRUE);

  frame11 = gtk_frame_new (NULL);
  gtk_widget_show (frame11);
  gtk_box_pack_start (GTK_BOX (vbox8), frame11, TRUE, TRUE, 0);

  alignment13 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment13);
  gtk_container_add (GTK_CONTAINER (frame11), alignment13);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment13), 0, 0, 12, 0);

  table16 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table16);
  gtk_container_add (GTK_CONTAINER (alignment13), table16);

  spinbutton8_adj = gtk_adjustment_new (1.4, 1, 10, 0.10000000149, 10, 10);
  spinbutton8 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton8_adj), 0.10000000149, 1);
  gtk_widget_show (spinbutton8);
  gtk_table_attach (GTK_TABLE (table16), spinbutton8, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton8, _("Quantization factors used between I- and P- frames"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton8), TRUE);

  spinbutton9_adj = gtk_adjustment_new (1.29999995232, 1, 10, 0.10000000149, 10, 10);
  spinbutton9 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton9_adj), 0.10000000149, 1);
  gtk_widget_show (spinbutton9);
  gtk_table_attach (GTK_TABLE (table16), spinbutton9, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbutton9, _("Quantization used between P- and B- frames"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton9), TRUE);

  spinbuttonChromaQPOffset_adj = gtk_adjustment_new (0, -12, 12, 1, 10, 10);
  spinbuttonChromaQPOffset = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonChromaQPOffset_adj), 1, 0);
  gtk_widget_show (spinbuttonChromaQPOffset);
  gtk_table_attach (GTK_TABLE (table16), spinbuttonChromaQPOffset, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_tooltips_set_tip (tooltips, spinbuttonChromaQPOffset, _("Quantization difference between chroma (color) and luma (brightness)"), NULL);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonChromaQPOffset), TRUE);

  labelFactorbetweenIandP = gtk_label_new (_("Factor between I and P frame Quants"));
  gtk_widget_show (labelFactorbetweenIandP);
  gtk_table_attach (GTK_TABLE (table16), labelFactorbetweenIandP, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelFactorbetweenIandP), 0, 0.5);

  labelFactorbetweenPandB = gtk_label_new (_("Factor between P and B frame Quants"));
  gtk_widget_show (labelFactorbetweenPandB);
  gtk_table_attach (GTK_TABLE (table16), labelFactorbetweenPandB, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelFactorbetweenPandB), 0, 0.5);

  labelChromaQPOffset = gtk_label_new (_("Chroma QP Offset"));
  gtk_widget_show (labelChromaQPOffset);
  gtk_table_attach (GTK_TABLE (table16), labelChromaQPOffset, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (labelChromaQPOffset), 0, 0.5);

  labelQuantizers = gtk_label_new (_("<b>Quantizers</b>"));
  gtk_widget_show (labelQuantizers);
  gtk_frame_set_label_widget (GTK_FRAME (frame11), labelQuantizers);
  gtk_label_set_use_markup (GTK_LABEL (labelQuantizers), TRUE);

  frame9 = gtk_frame_new (NULL);
  gtk_widget_show (frame9);
  gtk_box_pack_start (GTK_BOX (vbox8), frame9, TRUE, TRUE, 0);

  alignment9 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment9);
  gtk_container_add (GTK_CONTAINER (frame9), alignment9);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment9), 0, 0, 12, 0);

  table14 = gtk_table_new (3, 4, FALSE);
  gtk_widget_show (table14);
  gtk_container_add (GTK_CONTAINER (alignment9), table14);

  radiobuttonCustommatrix = gtk_radio_button_new_with_mnemonic (NULL, _("Custom matrix"));
  gtk_widget_show (radiobuttonCustommatrix);
  gtk_table_attach (GTK_TABLE (table14), radiobuttonCustommatrix, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonCustommatrix), radiobuttonCustommatrix_group);
  radiobuttonCustommatrix_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonCustommatrix));

  /* Open custom quanitization matrix file */
  filechooserbuttonOpenCQMFile = gtk_file_chooser_button_new (_("Open CQM file"), GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_show (filechooserbuttonOpenCQMFile);
  gtk_table_attach (GTK_TABLE (table14), filechooserbuttonOpenCQMFile, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  radiobuttonJVTmatrix = gtk_radio_button_new_with_mnemonic (NULL, _("JVT matrix"));
  gtk_widget_show (radiobuttonJVTmatrix);
  gtk_table_attach (GTK_TABLE (table14), radiobuttonJVTmatrix, 0, 4, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonJVTmatrix), radiobuttonCustommatrix_group);
  radiobuttonCustommatrix_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonJVTmatrix));

  radiobuttonFlatmatrix = gtk_radio_button_new_with_mnemonic (NULL, _("Flat matrix"));
  gtk_widget_show (radiobuttonFlatmatrix);
  gtk_table_attach (GTK_TABLE (table14), radiobuttonFlatmatrix, 0, 4, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobuttonFlatmatrix), radiobuttonCustommatrix_group);
  radiobuttonCustommatrix_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobuttonFlatmatrix));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobuttonFlatmatrix), TRUE);

  buttonEditCustomMatrix = gtk_button_new_with_mnemonic (_("Edit Custom Matrix"));
  gtk_widget_show (buttonEditCustomMatrix);
  gtk_table_attach (GTK_TABLE (table14), buttonEditCustomMatrix, 3, 4, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 2, 0);
  gtk_tooltips_set_tip (tooltips, buttonEditCustomMatrix, _("Edit a loaded custom quantization matrix file"), NULL);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_show (vseparator1);
  gtk_table_attach (GTK_TABLE (table14), vseparator1, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  labelFrameQuantizationMatrices = gtk_label_new (_("<b>Quantization Matrices</b>"));
  gtk_widget_show (labelFrameQuantizationMatrices);
  gtk_frame_set_label_widget (GTK_FRAME (frame9), labelFrameQuantizationMatrices);
  gtk_label_set_use_markup (GTK_LABEL (labelFrameQuantizationMatrices), TRUE);

  labelMore = gtk_label_new (_("More"));
  gtk_widget_show (labelMore);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), labelMore);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  buttonResetDaults = gtk_button_new_with_mnemonic (_("Defaults"));
  gtk_widget_show (buttonResetDaults);
  gtk_dialog_add_action_widget (GTK_DIALOG (dialog1), buttonResetDaults, 0);
  GTK_WIDGET_SET_FLAGS (buttonResetDaults, GTK_CAN_DEFAULT);

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
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonRange, "spinbuttonRange");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChromaME, "checkbuttonChromaME");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonMixedRefs, "checkbuttonMixedRefs");
  GLADE_HOOKUP_OBJECT (dialog1, hbox8, "hbox8");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonfastPSkip, "checkbuttonfastPSkip");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDCTDecimate, "checkbuttonDCTDecimate");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttoninterlaced, "checkbuttoninterlaced");
  GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
  GLADE_HOOKUP_OBJECT (dialog1, frameSampleAR, "frameSampleAR");
  GLADE_HOOKUP_OBJECT (dialog1, alignment12, "alignment12");
  GLADE_HOOKUP_OBJECT (dialog1, table35, "table35");
  GLADE_HOOKUP_OBJECT (dialog1, hbox12, "hbox12");
  GLADE_HOOKUP_OBJECT (dialog1, entryAR_Num, "entryAR_Num");
  GLADE_HOOKUP_OBJECT (dialog1, label53, "label53");
  GLADE_HOOKUP_OBJECT (dialog1, entryAR_Den, "entryAR_Den");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonCustomAR, "radiobuttonCustomAR");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonPredefinedAR, "radiobuttonPredefinedAR");
  GLADE_HOOKUP_OBJECT (dialog1, comboboxentry1, "comboboxentry1");
  GLADE_HOOKUP_OBJECT (dialog1, label39, "label39");
  GLADE_HOOKUP_OBJECT (dialog1, frame7, "frame7");
  GLADE_HOOKUP_OBJECT (dialog1, alignment7, "alignment7");
  GLADE_HOOKUP_OBJECT (dialog1, vbox7, "vbox7");
  GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCABAC, "checkbuttonCABAC");
  GLADE_HOOKUP_OBJECT (dialog1, labelTrellis, "labelTrellis");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTrellis, "spinbuttonTrellis");
  GLADE_HOOKUP_OBJECT (dialog1, hbox11, "hbox11");
  GLADE_HOOKUP_OBJECT (dialog1, labelNoiseReduction, "labelNoiseReduction");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonNoiseReduction, "spinbuttonNoiseReduction");
  GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDeblockingFilter, "checkbuttonDeblockingFilter");
  GLADE_HOOKUP_OBJECT (dialog1, table8, "table8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonStrength, "spinbuttonStrength");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonThreshold, "spinbuttonThreshold");
  GLADE_HOOKUP_OBJECT (dialog1, labelThreshold, "labelThreshold");
  GLADE_HOOKUP_OBJECT (dialog1, labelStrength, "labelStrength");
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
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxBFrame, "spinbuttonMaxBFrame");
  GLADE_HOOKUP_OBJECT (dialog1, labelMaxConsecutive, "labelMaxConsecutive");
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
  GLADE_HOOKUP_OBJECT (dialog1, labelKeyframeboost, "labelKeyframeboost");
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
  GLADE_HOOKUP_OBJECT (dialog1, frameVideoBufferVerifier, "frameVideoBufferVerifier");
  GLADE_HOOKUP_OBJECT (dialog1, alignment11, "alignment11");
  GLADE_HOOKUP_OBJECT (dialog1, table12, "table12");
  GLADE_HOOKUP_OBJECT (dialog1, labelVBVBufferSize, "labelVBVBufferSize");
  GLADE_HOOKUP_OBJECT (dialog1, labelVBVInitialbuffer, "labelVBVInitialbuffer");
  GLADE_HOOKUP_OBJECT (dialog1, labelVBVMaximumBitrate, "labelVBVMaximumBitrate");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonvbv_max_bitrate, "spinbuttonvbv_max_bitrate");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonvbv_buffer_size, "spinbuttonvbv_buffer_size");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonvbv_buffer_init, "spinbuttonvbv_buffer_init");
  GLADE_HOOKUP_OBJECT (dialog1, labelVideoBufferVerifier, "labelVideoBufferVerifier");
  GLADE_HOOKUP_OBJECT (dialog1, labelPageRateControl, "labelPageRateControl");
  GLADE_HOOKUP_OBJECT (dialog1, vbox8, "vbox8");
  GLADE_HOOKUP_OBJECT (dialog1, frame10, "frame10");
  GLADE_HOOKUP_OBJECT (dialog1, alignment10, "alignment10");
  GLADE_HOOKUP_OBJECT (dialog1, table15, "table15");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBitrateVariance, "spinbuttonBitrateVariance");
  GLADE_HOOKUP_OBJECT (dialog1, labelBitrateVariance, "labelBitrateVariance");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQuantizerCompression, "spinbuttonQuantizerCompression");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTempBlurFrame, "spinbuttonTempBlurFrame");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTempBlurQuant, "spinbuttonTempBlurQuant");
  GLADE_HOOKUP_OBJECT (dialog1, labelQuantizerCompression, "labelQuantizerCompression");
  GLADE_HOOKUP_OBJECT (dialog1, labelTempBlurEstFrame, "labelTempBlurEstFrame");
  GLADE_HOOKUP_OBJECT (dialog1, labelTempBlueQuant, "labelTempBlueQuant");
  GLADE_HOOKUP_OBJECT (dialog1, labelMisc, "labelMisc");
  GLADE_HOOKUP_OBJECT (dialog1, frame11, "frame11");
  GLADE_HOOKUP_OBJECT (dialog1, alignment13, "alignment13");
  GLADE_HOOKUP_OBJECT (dialog1, table16, "table16");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton8, "spinbutton8");
  GLADE_HOOKUP_OBJECT (dialog1, spinbutton9, "spinbutton9");
  GLADE_HOOKUP_OBJECT (dialog1, spinbuttonChromaQPOffset, "spinbuttonChromaQPOffset");
  GLADE_HOOKUP_OBJECT (dialog1, labelFactorbetweenIandP, "labelFactorbetweenIandP");
  GLADE_HOOKUP_OBJECT (dialog1, labelFactorbetweenPandB, "labelFactorbetweenPandB");
  GLADE_HOOKUP_OBJECT (dialog1, labelChromaQPOffset, "labelChromaQPOffset");
  GLADE_HOOKUP_OBJECT (dialog1, labelQuantizers, "labelQuantizers");
  GLADE_HOOKUP_OBJECT (dialog1, frame9, "frame9");
  GLADE_HOOKUP_OBJECT (dialog1, alignment9, "alignment9");
  GLADE_HOOKUP_OBJECT (dialog1, table14, "table14");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonCustommatrix, "radiobuttonCustommatrix");
  GLADE_HOOKUP_OBJECT (dialog1, filechooserbuttonOpenCQMFile, "filechooserbuttonOpenCQMFile");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonJVTmatrix, "radiobuttonJVTmatrix");
  GLADE_HOOKUP_OBJECT (dialog1, radiobuttonFlatmatrix, "radiobuttonFlatmatrix");
  GLADE_HOOKUP_OBJECT (dialog1, buttonEditCustomMatrix, "buttonEditCustomMatrix");
  GLADE_HOOKUP_OBJECT (dialog1, vseparator1, "vseparator1");
  GLADE_HOOKUP_OBJECT (dialog1, labelFrameQuantizationMatrices, "labelFrameQuantizationMatrices");
  GLADE_HOOKUP_OBJECT (dialog1, labelMore, "labelMore");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (dialog1, buttonResetDaults, "buttonResetDaults");
  GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");
  GLADE_HOOKUP_OBJECT_NO_REF (dialog1, tooltips, "tooltips");

  gtk_widget_grab_default (comboboxDirectMode);
  return dialog1;
}

#endif
