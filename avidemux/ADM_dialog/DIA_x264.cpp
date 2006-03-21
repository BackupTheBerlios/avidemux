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
                                


uint8_t DIA_x264(COMPRES_PARAMS *config);

static GtkWidget       *create_dialog1 (void);  

static COMPRES_PARAMS generic={CodecDummy,"dummy","dummy","dummy",COMPRESS_CQ,4,1500,700,0,0,NULL,0};
static GtkWidget *dialog;

static void updateMode( void );
static int cb_mod(GtkObject * object, gpointer user_data);
static ADM_x264Param *specific;
static void upload(GtkWidget *dialog,ADM_x264Param *param);
static void download(GtkWidget *dialog,ADM_x264Param *param);
/*********************************************/
#define SPIN_SET(x,y)  {gtk_spin_button_set_value(GTK_SPIN_BUTTON(WID(spinbutton##x)),y) ;}
#define SPIN_GET(x,y)  {y=(int32_t)gtk_spin_button_get_value(GTK_SPIN_BUTTON(WID(spinbutton##x))) ;}
uint8_t DIA_x264(COMPRES_PARAMS *config)
{
  
  uint8_t ret=0;
  
        ADM_assert(config->extraSettingsLen==sizeof(ADM_x264Param));
  
        memcpy(&generic,config,sizeof(generic));
        specific=(ADM_x264Param *)config->extraSettings;
       
        dialog=create_dialog1();
        gtk_register_dialog(dialog);
        upload(dialog,specific);
        
        updateMode();
        
        gtk_signal_connect(GTK_OBJECT(WID(comboboxMode)), "changed",
                           GTK_SIGNAL_FUNC(cb_mod), NULL);
        
        if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_OK)
        {
            uint32_t b;
          ret=1;
          
          SPIN_GET(Target,b);
          switch(generic.mode)
          {
            case COMPRESS_CBR:
              generic.bitrate=b*1000;
              break;

            case COMPRESS_2PASS:              
              b=generic.finalsize=b;
              break;

            case COMPRESS_CQ:              
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
        COMBO( 1);                  
      b=generic.bitrate/1000;
      SPIN_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelMode)),"Target bitrate (kb/s):");
      break;

    case COMPRESS_2PASS:
        COMBO(2);                  
      b=generic.finalsize;
      SPIN_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelMode)),"Target FinalSize (MB):");
            
      break;

    case COMPRESS_CQ:
        COMBO(0);                  
      b=generic.qz;
      SPIN_SET(Target,b);     
      gtk_label_set_text(GTK_LABEL(WID(labelMode)),"Quantizer (1-50):");
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
    case 1: generic.mode=COMPRESS_CBR ;break;
    case 0: generic.mode=COMPRESS_CQ ;break;
    case 2: generic.mode=COMPRESS_2PASS ;break;
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
    CHECK(CABAC);\
    CHECK(Trellis);\
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
    SPIN(MaxBFrame);\
    SPIN(Bias);\
    SPIN(Strength);\
    SPIN(Threshold); \
    ENTRY(AR_Num);\
    ENTRY(AR_Den);\
    \
    COMBO(PartitionDecision); \
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
    GtkWidget *table1;
    GtkWidget *label5;
    GtkWidget *labelMode;
    GtkWidget *comboboxMode;
    GtkObject *spinbuttonTarget_adj;
    GtkWidget *spinbuttonTarget;
    GtkWidget *label1;
    GtkWidget *vbox1;
    GtkWidget *frame1;
    GtkWidget *alignment1;
    GtkWidget *table2;
    GtkWidget *label11;
    GtkWidget *label12;
    GtkWidget *label10;
    GtkObject *spinbuttonKeyframeBoost_adj;
    GtkWidget *spinbuttonKeyframeBoost;
    GtkObject *spinbuttonBframeReduction_adj;
    GtkWidget *spinbuttonBframeReduction;
    GtkObject *spinbuttonBitrateVariability_adj;
    GtkWidget *spinbuttonBitrateVariability;
    GtkWidget *label7;
    GtkWidget *frame2;
    GtkWidget *alignment2;
    GtkWidget *table3;
    GtkObject *spinbuttonMinQp_adj;
    GtkWidget *spinbuttonMinQp;
    GtkObject *spinbuttonMaxQp_adj;
    GtkWidget *spinbuttonMaxQp;
    GtkObject *spinbuttonQpStep_adj;
    GtkWidget *spinbuttonQpStep;
    GtkWidget *label13;
    GtkWidget *label14;
    GtkWidget *label15;
    GtkWidget *label8;
    GtkWidget *frame3;
    GtkWidget *alignment3;
    GtkWidget *table4;
    GtkWidget *label16;
    GtkWidget *label18;
    GtkWidget *label17;
    GtkObject *spinbuttonSceneCut_adj;
    GtkWidget *spinbuttonSceneCut;
    GtkObject *spinbuttonMinIdr_adj;
    GtkWidget *spinbuttonMinIdr;
    GtkObject *spinbuttonMaxIdr_adj;
    GtkWidget *spinbuttonMaxIdr;
    GtkWidget *label9;
    GtkWidget *label2;
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
    GtkWidget *label22;
    GtkObject *spinbuttonMaxBFrame_adj;
    GtkWidget *spinbuttonMaxBFrame;
    GtkObject *spinbuttonBias_adj;
    GtkWidget *spinbuttonBias;
    GtkWidget *label21;
    GtkWidget *table6;
    GtkWidget *checkbuttonBasReference;
    GtkWidget *checkbuttonBidirME;
    GtkWidget *checkbuttonAdaptative;
    GtkWidget *checkbuttonWeighted;
    GtkWidget *hbox1;
    GtkWidget *label23;
    GtkWidget *comboboxDirectMode;
    GtkWidget *label20;
    GtkWidget *label3;
    GtkWidget *vbox5;
    GtkWidget *frame6;
    GtkWidget *alignment6;
    GtkWidget *vbox6;
    GtkWidget *table7;
    GtkWidget *label26;
    GtkWidget *label27;
    GtkWidget *comboboxPartitionDecision;
    GtkWidget *comboboxMethod;
    GtkWidget *checkbuttonChromaME;
    GtkWidget *label24;
    GtkWidget *frame7;
    GtkWidget *alignment7;
    GtkWidget *vbox7;
    GtkWidget *hbox2;
    GtkWidget *label28;
    GtkWidget *entryAR_Num;
    GtkWidget *entryAR_Den;
    GtkWidget *hbox3;
    GtkWidget *checkbuttonDeblockingFilter;
    GtkWidget *table8;
    GtkWidget *label29;
    GtkWidget *label30;
    GtkObject *spinbuttonStrength_adj;
    GtkWidget *spinbuttonStrength;
    GtkObject *spinbuttonThreshold_adj;
    GtkWidget *spinbuttonThreshold;
    GtkWidget *hbox4;
    GtkWidget *checkbuttonCABAC;
    GtkWidget *checkbuttonTrellis;
    GtkWidget *label25;
    GtkWidget *label4;
    GtkWidget *dialog_action_area1;
    GtkWidget *cancelbutton1;
    GtkWidget *okbutton1;

    dialog1 = gtk_dialog_new ();
    gtk_window_set_title (GTK_WINDOW (dialog1), _("X264 configuration"));
    gtk_window_set_type_hint (GTK_WINDOW (dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
    gtk_widget_show (dialog_vbox1);

    notebook1 = gtk_notebook_new ();
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

    table1 = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table1);
    gtk_container_add (GTK_CONTAINER (notebook1), table1);

    label5 = gtk_label_new (_("Encoding Mode :"));
    gtk_widget_show (label5);
    gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

    labelMode = gtk_label_new (_("Bitrate (kbps) :"));
    gtk_widget_show (labelMode);
    gtk_table_attach (GTK_TABLE (table1), labelMode, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelMode), 0, 0.5);

    comboboxMode = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxMode);
    gtk_table_attach (GTK_TABLE (table1), comboboxMode, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Constant Quality (CQ)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("Constant Bitrate (CBR)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMode), _("2 Pass mode"));

    spinbuttonTarget_adj = gtk_adjustment_new (100, 0, 1000, 1, 10, 10);
    spinbuttonTarget = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonTarget_adj), 1, 0);
    gtk_widget_show (spinbuttonTarget);
    gtk_table_attach (GTK_TABLE (table1), spinbuttonTarget, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonTarget), TRUE);

    label1 = gtk_label_new (_("Bitrate"));
    gtk_widget_show (label1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox1);

    frame1 = gtk_frame_new (NULL);
    gtk_widget_show (frame1);
    gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_NONE);

    alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment1);
    gtk_container_add (GTK_CONTAINER (frame1), alignment1);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

    table2 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (table2);
    gtk_container_add (GTK_CONTAINER (alignment1), table2);

    label11 = gtk_label_new (_("B-frame reduction (%)"));
    gtk_widget_show (label11);
    gtk_table_attach (GTK_TABLE (table2), label11, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

    label12 = gtk_label_new (_("Bitrate variability (%)"));
    gtk_widget_show (label12);
    gtk_table_attach (GTK_TABLE (table2), label12, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

    label10 = gtk_label_new (_("Keyframe boots (%)"));
    gtk_widget_show (label10);
    gtk_table_attach (GTK_TABLE (table2), label10, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label10), 0, 0.5);

    spinbuttonKeyframeBoost_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonKeyframeBoost = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonKeyframeBoost_adj), 1, 0);
    gtk_widget_show (spinbuttonKeyframeBoost);
    gtk_table_attach (GTK_TABLE (table2), spinbuttonKeyframeBoost, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request (spinbuttonKeyframeBoost, 60, -1);

    spinbuttonBframeReduction_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonBframeReduction = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBframeReduction_adj), 1, 0);
    gtk_widget_show (spinbuttonBframeReduction);
    gtk_table_attach (GTK_TABLE (table2), spinbuttonBframeReduction, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    spinbuttonBitrateVariability_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonBitrateVariability = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBitrateVariability_adj), 1, 0);
    gtk_widget_show (spinbuttonBitrateVariability);
    gtk_table_attach (GTK_TABLE (table2), spinbuttonBitrateVariability, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    label7 = gtk_label_new (_("<b>Bitrate</b>"));
    gtk_widget_show (label7);
    gtk_frame_set_label_widget (GTK_FRAME (frame1), label7);
    gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);

    frame2 = gtk_frame_new (NULL);
    gtk_widget_show (frame2);
    gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame2), GTK_SHADOW_NONE);

    alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment2);
    gtk_container_add (GTK_CONTAINER (frame2), alignment2);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

    table3 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (table3);
    gtk_container_add (GTK_CONTAINER (alignment2), table3);

    spinbuttonMinQp_adj = gtk_adjustment_new (10, 10, 51, 1, 10, 10);
    spinbuttonMinQp = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinQp_adj), 1, 0);
    gtk_widget_show (spinbuttonMinQp);
    gtk_table_attach (GTK_TABLE (table3), spinbuttonMinQp, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request (spinbuttonMinQp, 60, -1);

    spinbuttonMaxQp_adj = gtk_adjustment_new (10, 10, 51, 1, 10, 10);
    spinbuttonMaxQp = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxQp_adj), 1, 0);
    gtk_widget_show (spinbuttonMaxQp);
    gtk_table_attach (GTK_TABLE (table3), spinbuttonMaxQp, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    spinbuttonQpStep_adj = gtk_adjustment_new (4, 0, 10, 1, 10, 10);
    spinbuttonQpStep = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonQpStep_adj), 1, 0);
    gtk_widget_show (spinbuttonQpStep);
    gtk_table_attach (GTK_TABLE (table3), spinbuttonQpStep, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    label13 = gtk_label_new (_("Min Qp"));
    gtk_widget_show (label13);
    gtk_table_attach (GTK_TABLE (table3), label13, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

    label14 = gtk_label_new (_("Max Qp"));
    gtk_widget_show (label14);
    gtk_table_attach (GTK_TABLE (table3), label14, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

    label15 = gtk_label_new (_("Max QP Step"));
    gtk_widget_show (label15);
    gtk_table_attach (GTK_TABLE (table3), label15, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

    label8 = gtk_label_new (_("<b>Quantization Limits</b>"));
    gtk_widget_show (label8);
    gtk_frame_set_label_widget (GTK_FRAME (frame2), label8);
    gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);

    frame3 = gtk_frame_new (NULL);
    gtk_widget_show (frame3);
    gtk_box_pack_start (GTK_BOX (vbox1), frame3, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame3), GTK_SHADOW_NONE);

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
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

    label18 = gtk_label_new (_("Max IDR frame interval"));
    gtk_widget_show (label18);
    gtk_table_attach (GTK_TABLE (table4), label18, 0, 1, 2, 3,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

    label17 = gtk_label_new (_("Min IDR frame interval"));
    gtk_widget_show (label17);
    gtk_table_attach (GTK_TABLE (table4), label17, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

    spinbuttonSceneCut_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonSceneCut = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonSceneCut_adj), 1, 0);
    gtk_widget_show (spinbuttonSceneCut);
    gtk_table_attach (GTK_TABLE (table4), spinbuttonSceneCut, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request (spinbuttonSceneCut, 60, -1);

    spinbuttonMinIdr_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonMinIdr = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMinIdr_adj), 1, 0);
    gtk_widget_show (spinbuttonMinIdr);
    gtk_table_attach (GTK_TABLE (table4), spinbuttonMinIdr, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    spinbuttonMaxIdr_adj = gtk_adjustment_new (1, 0, 1000, 1, 10, 10);
    spinbuttonMaxIdr = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxIdr_adj), 1, 0);
    gtk_widget_show (spinbuttonMaxIdr);
    gtk_table_attach (GTK_TABLE (table4), spinbuttonMaxIdr, 1, 2, 2, 3,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    label9 = gtk_label_new (_("<b>Scene Cut</b>"));
    gtk_widget_show (label9);
    gtk_frame_set_label_widget (GTK_FRAME (frame3), label9);
    gtk_label_set_use_markup (GTK_LABEL (label9), TRUE);

    label2 = gtk_label_new (_("Rate Control"));
    gtk_widget_show (label2);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox2);

    frame4 = gtk_frame_new (NULL);
    gtk_widget_show (frame4);
    gtk_box_pack_start (GTK_BOX (vbox2), frame4, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame4), GTK_SHADOW_NONE);

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

    checkbutton8x8P = gtk_check_button_new_with_mnemonic (_("8x8, 8x16 and 16x8 P-frame search"));
    gtk_widget_show (checkbutton8x8P);
    gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8P, FALSE, FALSE, 0);

    checkbutton8x8B = gtk_check_button_new_with_mnemonic (_("8x8, 8x16 and 16x8 B frame search"));
    gtk_widget_show (checkbutton8x8B);
    gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8B, FALSE, FALSE, 0);

    checkbutton4x4 = gtk_check_button_new_with_mnemonic (_("4x4, 4x8 and 8x4 P frame search"));
    gtk_widget_show (checkbutton4x4);
    gtk_box_pack_start (GTK_BOX (vbox3), checkbutton4x4, FALSE, FALSE, 0);

    checkbutton8x8I = gtk_check_button_new_with_mnemonic (_("8x8 Intra search"));
    gtk_widget_show (checkbutton8x8I);
    gtk_box_pack_start (GTK_BOX (vbox3), checkbutton8x8I, FALSE, FALSE, 0);

    checkbutton4x4I = gtk_check_button_new_with_mnemonic (_("4x4 Intra search"));
    gtk_widget_show (checkbutton4x4I);
    gtk_box_pack_start (GTK_BOX (vbox3), checkbutton4x4I, FALSE, FALSE, 0);

    label19 = gtk_label_new (_("<b>Partitions</b>"));
    gtk_widget_show (label19);
    gtk_frame_set_label_widget (GTK_FRAME (frame4), label19);
    gtk_label_set_use_markup (GTK_LABEL (label19), TRUE);

    frame5 = gtk_frame_new (NULL);
    gtk_widget_show (frame5);
    gtk_box_pack_start (GTK_BOX (vbox2), frame5, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame5), GTK_SHADOW_NONE);

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

    label22 = gtk_label_new (_("Bias"));
    gtk_widget_show (label22);
    gtk_table_attach (GTK_TABLE (table5), label22, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label22), 0, 0.5);

    spinbuttonMaxBFrame_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
    spinbuttonMaxBFrame = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonMaxBFrame_adj), 1, 0);
    gtk_widget_show (spinbuttonMaxBFrame);
    gtk_table_attach (GTK_TABLE (table5), spinbuttonMaxBFrame, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    spinbuttonBias_adj = gtk_adjustment_new (1, -99, 99, 1, 10, 10);
    spinbuttonBias = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonBias_adj), 1, 0);
    gtk_widget_show (spinbuttonBias);
    gtk_table_attach (GTK_TABLE (table5), spinbuttonBias, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    label21 = gtk_label_new (_("Max Consecutive "));
    gtk_widget_show (label21);
    gtk_table_attach (GTK_TABLE (table5), label21, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label21), 0, 0.5);

    table6 = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table6);
    gtk_box_pack_start (GTK_BOX (vbox4), table6, TRUE, TRUE, 0);

    checkbuttonBasReference = gtk_check_button_new_with_mnemonic (_("Use as reference"));
    gtk_widget_show (checkbuttonBasReference);
    gtk_table_attach (GTK_TABLE (table6), checkbuttonBasReference, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    checkbuttonBidirME = gtk_check_button_new_with_mnemonic (_("Bidirectional ME"));
    gtk_widget_show (checkbuttonBidirME);
    gtk_table_attach (GTK_TABLE (table6), checkbuttonBidirME, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    checkbuttonAdaptative = gtk_check_button_new_with_mnemonic (_("Adaptative"));
    gtk_widget_show (checkbuttonAdaptative);
    gtk_table_attach (GTK_TABLE (table6), checkbuttonAdaptative, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    checkbuttonWeighted = gtk_check_button_new_with_mnemonic (_("Weighted biprediction"));
    gtk_widget_show (checkbuttonWeighted);
    gtk_table_attach (GTK_TABLE (table6), checkbuttonWeighted, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox4), hbox1, TRUE, TRUE, 0);

    label23 = gtk_label_new (_("Direct mode : "));
    gtk_widget_show (label23);
    gtk_box_pack_start (GTK_BOX (hbox1), label23, TRUE, FALSE, 0);

    comboboxDirectMode = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxDirectMode);
    gtk_box_pack_start (GTK_BOX (hbox1), comboboxDirectMode, TRUE, TRUE, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Spatial"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxDirectMode), _("Temporal"));

    label20 = gtk_label_new (_("<b>BFrames</b>"));
    gtk_widget_show (label20);
    gtk_frame_set_label_widget (GTK_FRAME (frame5), label20);
    gtk_label_set_use_markup (GTK_LABEL (label20), TRUE);

    label3 = gtk_label_new (_("MB&Frames"));
    gtk_widget_show (label3);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);

    vbox5 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox5);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox5);

    frame6 = gtk_frame_new (NULL);
    gtk_widget_show (frame6);
    gtk_box_pack_start (GTK_BOX (vbox5), frame6, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame6), GTK_SHADOW_NONE);

    alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
    gtk_widget_show (alignment6);
    gtk_container_add (GTK_CONTAINER (frame6), alignment6);
    gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 0);

    vbox6 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox6);
    gtk_container_add (GTK_CONTAINER (alignment6), vbox6);

    table7 = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table7);
    gtk_box_pack_start (GTK_BOX (vbox6), table7, FALSE, TRUE, 0);

    label26 = gtk_label_new (_("Partition decision"));
    gtk_widget_show (label26);
    gtk_table_attach (GTK_TABLE (table7), label26, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label26), 0, 0.5);

    label27 = gtk_label_new (_("Method"));
    gtk_widget_show (label27);
    gtk_table_attach (GTK_TABLE (table7), label27, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (label27), 0, 0.5);

    comboboxPartitionDecision = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxPartitionDecision);
    gtk_table_attach (GTK_TABLE (table7), comboboxPartitionDecision, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("1 (Fastest)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("2"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("4"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("5 (High quality)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("6 (RDO)"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxPartitionDecision), _("6b (RDO on Bframes)"));

    comboboxMethod = gtk_combo_box_new_text ();
    gtk_widget_show (comboboxMethod);
    gtk_table_attach (GTK_TABLE (table7), comboboxMethod, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Diamond search"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Hexagonal search"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Uneven multi hexagon"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxMethod), _("Exhaustive search"));

    checkbuttonChromaME = gtk_check_button_new_with_mnemonic (_("Chroma ME"));
    gtk_widget_show (checkbuttonChromaME);
    gtk_box_pack_start (GTK_BOX (vbox6), checkbuttonChromaME, FALSE, FALSE, 0);

    label24 = gtk_label_new (_("<b>Motion Estimation</b>"));
    gtk_widget_show (label24);
    gtk_frame_set_label_widget (GTK_FRAME (frame6), label24);
    gtk_label_set_use_markup (GTK_LABEL (label24), TRUE);

    frame7 = gtk_frame_new (NULL);
    gtk_widget_show (frame7);
    gtk_box_pack_start (GTK_BOX (vbox5), frame7, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame7), GTK_SHADOW_NONE);

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

    label28 = gtk_label_new (_("Sample A/R "));
    gtk_widget_show (label28);
    gtk_box_pack_start (GTK_BOX (hbox2), label28, FALSE, FALSE, 0);

    entryAR_Num = gtk_entry_new ();
    gtk_widget_show (entryAR_Num);
    gtk_box_pack_start (GTK_BOX (hbox2), entryAR_Num, TRUE, TRUE, 0);

    entryAR_Den = gtk_entry_new ();
    gtk_widget_show (entryAR_Den);
    gtk_box_pack_start (GTK_BOX (hbox2), entryAR_Den, TRUE, TRUE, 0);

    hbox3 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox3);
    gtk_box_pack_start (GTK_BOX (vbox7), hbox3, TRUE, TRUE, 0);

    checkbuttonDeblockingFilter = gtk_check_button_new_with_mnemonic (_("Deblocking Filter  :"));
    gtk_widget_show (checkbuttonDeblockingFilter);
    gtk_box_pack_start (GTK_BOX (hbox3), checkbuttonDeblockingFilter, FALSE, FALSE, 0);

    table8 = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table8);
    gtk_box_pack_start (GTK_BOX (hbox3), table8, TRUE, TRUE, 0);

    label29 = gtk_label_new (_("<i>Strength</i>"));
    gtk_widget_show (label29);
    gtk_table_attach (GTK_TABLE (table8), label29, 0, 1, 0, 1,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label29), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label29), 0, 0.5);

    label30 = gtk_label_new (_("<i>Threshold</i>"));
    gtk_widget_show (label30);
    gtk_table_attach (GTK_TABLE (table8), label30, 0, 1, 1, 2,
                      (GtkAttachOptions) (GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label30), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label30), 0, 0.5);

    spinbuttonStrength_adj = gtk_adjustment_new (0, -5, 6, 1, 10, 10);
    spinbuttonStrength = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonStrength_adj), 1, 0);
    gtk_widget_show (spinbuttonStrength);
    gtk_table_attach (GTK_TABLE (table8), spinbuttonStrength, 1, 2, 0, 1,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);
    gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbuttonStrength), TRUE);

    spinbuttonThreshold_adj = gtk_adjustment_new (1, -5, 6, 1, 10, 10);
    spinbuttonThreshold = gtk_spin_button_new (GTK_ADJUSTMENT (spinbuttonThreshold_adj), 1, 0);
    gtk_widget_show (spinbuttonThreshold);
    gtk_table_attach (GTK_TABLE (table8), spinbuttonThreshold, 1, 2, 1, 2,
                      (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                      (GtkAttachOptions) (0), 0, 0);

    hbox4 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox4);
    gtk_box_pack_start (GTK_BOX (vbox7), hbox4, TRUE, TRUE, 0);

    checkbuttonCABAC = gtk_check_button_new_with_mnemonic (_("CABAC"));
    gtk_widget_show (checkbuttonCABAC);
    gtk_box_pack_start (GTK_BOX (hbox4), checkbuttonCABAC, FALSE, FALSE, 0);

    checkbuttonTrellis = gtk_check_button_new_with_mnemonic (_("Trellis"));
    gtk_widget_show (checkbuttonTrellis);
    gtk_box_pack_start (GTK_BOX (hbox4), checkbuttonTrellis, FALSE, FALSE, 0);

    label25 = gtk_label_new (_("<b>Misc options</b>"));
    gtk_widget_show (label25);
    gtk_frame_set_label_widget (GTK_FRAME (frame7), label25);
    gtk_label_set_use_markup (GTK_LABEL (label25), TRUE);

    label4 = gtk_label_new (_("More"));
    gtk_widget_show (label4);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label4);

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
    GLADE_HOOKUP_OBJECT (dialog1, label5, "label5");
    GLADE_HOOKUP_OBJECT (dialog1, labelMode, "labelMode");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxMode, "comboboxMode");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonTarget, "spinbuttonTarget");
    GLADE_HOOKUP_OBJECT (dialog1, label1, "label1");
    GLADE_HOOKUP_OBJECT (dialog1, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT (dialog1, frame1, "frame1");
    GLADE_HOOKUP_OBJECT (dialog1, alignment1, "alignment1");
    GLADE_HOOKUP_OBJECT (dialog1, table2, "table2");
    GLADE_HOOKUP_OBJECT (dialog1, label11, "label11");
    GLADE_HOOKUP_OBJECT (dialog1, label12, "label12");
    GLADE_HOOKUP_OBJECT (dialog1, label10, "label10");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonKeyframeBoost, "spinbuttonKeyframeBoost");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBframeReduction, "spinbuttonBframeReduction");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBitrateVariability, "spinbuttonBitrateVariability");
    GLADE_HOOKUP_OBJECT (dialog1, label7, "label7");
    GLADE_HOOKUP_OBJECT (dialog1, frame2, "frame2");
    GLADE_HOOKUP_OBJECT (dialog1, alignment2, "alignment2");
    GLADE_HOOKUP_OBJECT (dialog1, table3, "table3");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinQp, "spinbuttonMinQp");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxQp, "spinbuttonMaxQp");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonQpStep, "spinbuttonQpStep");
    GLADE_HOOKUP_OBJECT (dialog1, label13, "label13");
    GLADE_HOOKUP_OBJECT (dialog1, label14, "label14");
    GLADE_HOOKUP_OBJECT (dialog1, label15, "label15");
    GLADE_HOOKUP_OBJECT (dialog1, label8, "label8");
    GLADE_HOOKUP_OBJECT (dialog1, frame3, "frame3");
    GLADE_HOOKUP_OBJECT (dialog1, alignment3, "alignment3");
    GLADE_HOOKUP_OBJECT (dialog1, table4, "table4");
    GLADE_HOOKUP_OBJECT (dialog1, label16, "label16");
    GLADE_HOOKUP_OBJECT (dialog1, label18, "label18");
    GLADE_HOOKUP_OBJECT (dialog1, label17, "label17");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonSceneCut, "spinbuttonSceneCut");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMinIdr, "spinbuttonMinIdr");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxIdr, "spinbuttonMaxIdr");
    GLADE_HOOKUP_OBJECT (dialog1, label9, "label9");
    GLADE_HOOKUP_OBJECT (dialog1, label2, "label2");
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
    GLADE_HOOKUP_OBJECT (dialog1, label22, "label22");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonMaxBFrame, "spinbuttonMaxBFrame");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonBias, "spinbuttonBias");
    GLADE_HOOKUP_OBJECT (dialog1, label21, "label21");
    GLADE_HOOKUP_OBJECT (dialog1, table6, "table6");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBasReference, "checkbuttonBasReference");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonBidirME, "checkbuttonBidirME");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonAdaptative, "checkbuttonAdaptative");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonWeighted, "checkbuttonWeighted");
    GLADE_HOOKUP_OBJECT (dialog1, hbox1, "hbox1");
    GLADE_HOOKUP_OBJECT (dialog1, label23, "label23");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxDirectMode, "comboboxDirectMode");
    GLADE_HOOKUP_OBJECT (dialog1, label20, "label20");
    GLADE_HOOKUP_OBJECT (dialog1, label3, "label3");
    GLADE_HOOKUP_OBJECT (dialog1, vbox5, "vbox5");
    GLADE_HOOKUP_OBJECT (dialog1, frame6, "frame6");
    GLADE_HOOKUP_OBJECT (dialog1, alignment6, "alignment6");
    GLADE_HOOKUP_OBJECT (dialog1, vbox6, "vbox6");
    GLADE_HOOKUP_OBJECT (dialog1, table7, "table7");
    GLADE_HOOKUP_OBJECT (dialog1, label26, "label26");
    GLADE_HOOKUP_OBJECT (dialog1, label27, "label27");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxPartitionDecision, "comboboxPartitionDecision");
    GLADE_HOOKUP_OBJECT (dialog1, comboboxMethod, "comboboxMethod");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonChromaME, "checkbuttonChromaME");
    GLADE_HOOKUP_OBJECT (dialog1, label24, "label24");
    GLADE_HOOKUP_OBJECT (dialog1, frame7, "frame7");
    GLADE_HOOKUP_OBJECT (dialog1, alignment7, "alignment7");
    GLADE_HOOKUP_OBJECT (dialog1, vbox7, "vbox7");
    GLADE_HOOKUP_OBJECT (dialog1, hbox2, "hbox2");
    GLADE_HOOKUP_OBJECT (dialog1, label28, "label28");
    GLADE_HOOKUP_OBJECT (dialog1, entryAR_Num, "entryAR_Num");
    GLADE_HOOKUP_OBJECT (dialog1, entryAR_Den, "entryAR_Den");
    GLADE_HOOKUP_OBJECT (dialog1, hbox3, "hbox3");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonDeblockingFilter, "checkbuttonDeblockingFilter");
    GLADE_HOOKUP_OBJECT (dialog1, table8, "table8");
    GLADE_HOOKUP_OBJECT (dialog1, label29, "label29");
    GLADE_HOOKUP_OBJECT (dialog1, label30, "label30");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonStrength, "spinbuttonStrength");
    GLADE_HOOKUP_OBJECT (dialog1, spinbuttonThreshold, "spinbuttonThreshold");
    GLADE_HOOKUP_OBJECT (dialog1, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonCABAC, "checkbuttonCABAC");
    GLADE_HOOKUP_OBJECT (dialog1, checkbuttonTrellis, "checkbuttonTrellis");
    GLADE_HOOKUP_OBJECT (dialog1, label25, "label25");
    GLADE_HOOKUP_OBJECT (dialog1, label4, "label4");
    GLADE_HOOKUP_OBJECT_NO_REF (dialog1, dialog_action_area1, "dialog_action_area1");
    GLADE_HOOKUP_OBJECT (dialog1, cancelbutton1, "cancelbutton1");
    GLADE_HOOKUP_OBJECT (dialog1, okbutton1, "okbutton1");

    return dialog1;
}



#endif
