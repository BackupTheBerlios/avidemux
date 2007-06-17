/***************************************************************************
    copyright            : (C) 2001 by mean
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
#define CUSTOM_SLIDER
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <ADM_qslider.h>

#include "default.h"
#include "ADM_osSupport/ADM_misc.h"
#include "ADM_codecs/ADM_codec.h"
#include "gui_action.hxx"
#include "ADM_editor/ADM_outputfmt.h"
#include "ADM_toolkit/filesel.h"
#include "prefs.h"

extern int automation(void );
extern void HandleAction(Action a);
extern uint32_t encoderGetNbEncoder (void);
extern const char *encoderGetIndexedName (uint32_t i);
extern uint32_t audioFilterGetNbEncoder(void);
extern const char* audioFilterGetIndexedName(uint32_t i);
extern void checkCrashFile(void);
static void setupMenus(void);
static void GUI_initCustom(void);
#ifdef CUSTOM_SLIDER
static ADM_QSlider *slider=NULL;
#else
static QSlider *slider=NULL;
#endif
static int _upd_in_progres=0;
/* Ugly game with macro so that buttons emit their name ...*/
static char     *customNames[ADM_MAC_CUSTOM_SCRIPT];
static QAction  *customActions[ADM_MAC_CUSTOM_SCRIPT];
static uint32_t ADM_nbCustom=0;


#include "ui_gui2.h"

#define WIDGET(x)  (((MainWindow *)QuiMainWindows)->ui.x)
    
#define CONNECT(object,zzz) connect( (ui.object),SIGNAL(triggered()),this,SLOT(buttonPressed()));
#define CONNECT_TB(object,zzz) connect( (ui.object),SIGNAL(clicked(bool)),this,SLOT(toolButtonPressed(bool)));
#define DECLARE_VAR(object,signal_name) {#object,signal_name},
             
#include "translation_table.h"    
/*
    Declare the table converting widget name to our internal signal           
*/
typedef struct adm_qt4_translation
{
  const char *name;
  Action     action; 
};
const adm_qt4_translation myTranslationTable[]=
{
#define PROCESS DECLARE_VAR
     LIST_OF_OBJECTS
     LIST_OF_BUTTONS
#undef PROCESS
};
static Action searchTranslationTable(const char *name);
#define SIZEOF_MY_TRANSLATION sizeof(myTranslationTable)/sizeof(adm_qt4_translation)
/*
    Declare the class that will be our main window

*/
 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();
     Ui_MainWindow ui;
 public slots:
     void timeChanged(int);
     void buttonPressed(void);
     void custom(void);
     void toolButtonPressed(bool z);
     void comboChanged(int z)
     {
        const char *source=qPrintable(sender()->objectName());

        if(!strcmp(source,"comboBoxVideo"))  
        {
          bool b=FALSE;;
          if(ui.comboBoxVideo->currentIndex())
          {
             b=TRUE;
          }
          ui.pushButtonVideoConf->setEnabled(b);
          ui.pushButtonVideoFilter->setEnabled(b);
          HandleAction (ACT_VideoCodecChanged) ;
        }
        else 
          if(!strcmp(source,"comboBoxAudio"))  
        {
          bool b=FALSE;
          if(ui.comboBoxAudio->currentIndex())
          {
             b=TRUE;
          }
          ui.pushButtonAudioConf->setEnabled(b);
          ui.pushButtonAudioFilter->setEnabled(b);
          HandleAction (ACT_AudioCodecChanged) ;
        }else
        if(!strcmp(source,"comboBoxPreview"))  
        {
          HandleAction (ACT_PreviewChanged) ;
        }
        

        printf("From +: %s\n",source);
     }
      void sliderMoved(int u) 
        {
          if(!_upd_in_progres)
          {
            HandleAction (ACT_Scale) ;
          }
        }
 private slots:
   

 private:
     
 };


MainWindow::MainWindow()     : QMainWindow()
 {
     ui.setupUi(this);
     /*
        Connect our button to buttonPressed
     */
#define PROCESS CONNECT
     LIST_OF_OBJECTS
#undef PROCESS
#define PROCESS CONNECT_TB
     LIST_OF_BUTTONS
#undef PROCESS
     // ComboBox
         
         
         
         //ACT_VideoCodecChanged
         connect( ui.comboBoxVideo,SIGNAL(activated(int)),this,SLOT(comboChanged(int)));
         connect( ui.comboBoxAudio,SIGNAL(activated(int)),this,SLOT(comboChanged(int)));
         connect( ui.comboBoxPreview,SIGNAL(activated(int)),this,SLOT(comboChanged(int)));
     // Slider
          slider=ui.horizontalSlider;
          slider->setMinimum(0);
          slider->setMaximum(1000);
          connect( slider,SIGNAL(valueChanged(int)),this,SLOT(sliderMoved(int)));
          //connect( slider,SIGNAL(sliderMoved()),this,SLOT(sliderMoved()));
  
          // Add a second toolbar
          QToolBar *toolBarMove;
            toolBarMove = new QToolBar(this);
            toolBarMove->setObjectName(QString::fromUtf8("toolBarMove"));
            toolBarMove->setOrientation(Qt::Horizontal);
            this->addToolBar(static_cast<Qt::ToolBarArea>(4), toolBarMove);
          // And actions to it
#define ADD(x) toolBarMove->addAction(ui.x)
            ADD(actionPlay);
            ADD(actionPlay_Stop);
            ADD(actionPrevious_Frame);
            ADD(actionNext_Frame);
            ADD(actionPrevious_intra_frame);
            ADD(actionNext_intra_frame);
            ADD(actionSet_marker_A);
            ADD(actionSet_marker_B);
            ADD(actionPrevious_black_frame);
            ADD(actionNext_blak_frame);
            ADD(actionFirst_Frame);
            ADD(actionLast_Frame);
            ADD(actionGo_to_Marker_A);
            ADD(actionGo_to_Marker_B);
            //** Add shortcuts **//
            QKeySequence seqFilter("CTRL+Alt+F");
            ui.actionFilters->setShortcut(seqFilter);
            // default state
            bool b=0;
          ui.pushButtonVideoConf->setEnabled(b);
          ui.pushButtonVideoFilter->setEnabled(b);
          ui.pushButtonAudioConf->setEnabled(b);
          ui.pushButtonAudioFilter->setEnabled(b);
          /* Time Shift */
  connect(ui.checkBox_TimeShift,SIGNAL(stateChanged(int)),this,SLOT(timeChanged(int)));
  connect(ui.spinBox_TimeValue,SIGNAL(valueChanged(int)),this,SLOT(timeChanged(int)));
          
          
          
          /* Build the custom menu */
          GUI_initCustom();
          for(int i=0;i<ADM_nbCustom;i++)
          {
            customActions[i]=new QAction(customNames[i],NULL);
            ui.menuCustom->addAction(customActions[i]);
            connect(customActions[i], SIGNAL(triggered()), this, SLOT(custom()));
          }
          printf("Menu built\n");
 }
 /**
    \fn     custom
    \brief  Invoked when one of the custom script has been called
 */
void MainWindow::custom(void)
{
    printf("[CUSTOM] Invoked\n");
    QObject *ptr=sender();
    if(!ptr) return;
    for(int i=0;i<ADM_nbCustom;i++)
    {
      if(customActions[i]==ptr)
      {
        printf("[Custom] %u/%u scrips\n",i,ADM_nbCustom);
        HandleAction( (Action)(ACT_CUSTOM_BASE+i));
        return; 
      }
    }
    printf("[Custom] Not found\n");
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
    \fn timeChanged
    \brief Called whenever timeshift is on/off'ed or value changes
*/
void MainWindow::timeChanged(int)
{
   HandleAction (ACT_TimeShift) ;
}
 /*
      We receive a button press event
 */
 void MainWindow::buttonPressed(void)
 {
    // Receveid a key press Event, look into table..
   const char *source=qPrintable(sender()->objectName());

    printf("Button From : %s\n",source);
    Action action=searchTranslationTable(source);
    if(action!=ACT_DUMMY)
    {
      HandleAction (action) ;
    }

 }
  void MainWindow::toolButtonPressed(bool i)
  {
     buttonPressed();
  }
//*********************************************
//***** Hook to core                ***********
//*********************************************
extern int global_argc;
extern char **global_argv;
extern void UI_QT4VideoWidget(QFrame *frame);
int UI_Init(int nargc,char **nargv)
{
  global_argc=nargc;
  global_argv=nargv;
  return 0;
}
QWidget *QuiMainWindows=NULL;
QGraphicsView *drawWindow=NULL;
uint8_t UI_updateRecentMenu( void );

/**
    \fn UI_getCurrentPreview(void)
    \brief Read previewmode from comboxbox 
*/

int    UI_getCurrentPreview(void)
{
  return WIDGET(comboBoxPreview)->currentIndex(); 
}
/**
    \fn UI_setCurrentPreview(int ne)
    \brief Update comboxbox with previewmode
*/

void   UI_setCurrentPreview(int ne)
{
    WIDGET(comboBoxPreview)->setCurrentIndex(ne); 
}


/**
    \fn UI_RunApp(void)
    \brief Main entry point for the GUI application
*/
int UI_RunApp(void)
{
Q_INIT_RESOURCE(avidemux);
Q_INIT_RESOURCE(filter);
   QApplication a( global_argc, global_argv );
   MainWindow * mw = new MainWindow();
    
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    
    mw->show();
    
    QuiMainWindows=(QWidget*)mw;
   
    UI_QT4VideoWidget(mw->ui.frame_video);  // Add the widget that will handle video display
    UI_updateRecentMenu(  );
    setupMenus();
    checkCrashFile();
    
   if (global_argc >= 2)
    {
     automation();
    }
    return a.exec();
}
/**
    \fn searchTranslationTable(const char *name))
    \brief return the action corresponding to a give button. The translation table is in translation_table.h
*/
Action searchTranslationTable(const char *name)
{
  for(int i=0;i< SIZEOF_MY_TRANSLATION;i++)
  {
    if(!strcmp(name, myTranslationTable[i].name))
    {
      return  myTranslationTable[i].action;
    }
  }
  printf("WARNING : Signal not found in translation table : %s\n",name);
  return ACT_DUMMY;
}
/**
    \fn     UI_updateRecentMenu( void )
    \brief  Update the recent submenu with the latest files loaded
*/
uint8_t UI_updateRecentMenu( void )
{
const char **names;
uint32_t nb_item=0;
QAction *actions[4]={WIDGET(actionRecent0),WIDGET(actionRecent1),WIDGET(actionRecent2),WIDGET(actionRecent3)};
        names=prefs->get_lastfiles();
        
        // hide them all
        for(int i=0;i<4;i++) actions[i]->setVisible(0);
        // Redraw..
        for( nb_item=0;nb_item<4;nb_item++)
        {
                if(!names[nb_item]) 
                {
                   return 1;
                }
                else
                {
                    //actions[nb_item]->setVisible(1);
                    // Replace widget ?
                    actions[nb_item]->setText(names[nb_item]);
                    actions[nb_item]->setVisible(1);
                }
                // Update name
        }
        return 1;
}
/** 
  \fn    setupMenus(void)
  \brief Fill in video & audio co
*/
void setupMenus(void)
{
        uint32_t nbVid;
        const char *name;

                nbVid=encoderGetNbEncoder();
                printf("Found %d video encoder\n",nbVid);
                for(uint32_t i=1;i<nbVid;i++)
                {
                        name=encoderGetIndexedName(i);
                        WIDGET(comboBoxVideo)->addItem(name);
                }

        // And A codec
        
        uint32_t nbAud;

                nbAud=audioFilterGetNbEncoder();
                printf("Found %d audio encoder\n",nbAud);		       
                for(uint32_t i=1;i<nbAud;i++)
                {
                        name=audioFilterGetIndexedName(i);
                        WIDGET(comboBoxAudio)->addItem(name);
                }
      /*   Fill in output format window */
        uint32_t nbFormat;

                nbFormat=sizeof(ADM_allOutputFormat)/sizeof(ADM_FORMAT_DESC);
                printf("Found %d Format \n",nbFormat);
                for(uint32_t i=0;i<nbFormat;i++)
                {
                       WIDGET(comboBoxFormat)->addItem(_(ADM_allOutputFormat[i].text));	
                }
        
}
/*
    Return % of scale (between 0 and 1)
*/
double 	UI_readScale( void )
{
  double v;
  if(!slider) v=0;
  v= (double)(slider->value());
  v/=10;
  printf("GetScale\n");
  return v;
}
void UI_setScale( double val )
{
if(_upd_in_progres) return;
 _upd_in_progres++;
   slider->setValue( (int)val*10);
 _upd_in_progres--;
   
}
void UI_purge( void )
{
 QCoreApplication::processEvents ();
}


//*******************************************

/**
    \fn UI_setTitle(char *name)
    \brief Set the main window title, usually name if the file being edited
*/
void UI_setTitle(char *name)
{
      char title[1024];

      strcpy(title,"Avidemux 2 ");
      strncat(title,name,200);

      QuiMainWindows->setWindowTitle( title);
}
/**
    \fn     UI_setFrameType( uint32_t frametype,uint32_t qp)
    \brief  Display frametype (I/P/B) and associated quantizer
*/

void UI_setFrameType( uint32_t frametype,uint32_t qp)
{
char string[100];
char	c='?';
      switch(frametype)
      {
              case AVI_KEY_FRAME: c='I';break;
              case AVI_B_FRAME: c='B';break;
              case 0: c='P';break;
              default:c='?';break;
      
      }
      sprintf(string,_("Frame:%c(%02d)"),c,qp);
      WIDGET(label_8)->setText(string);	

}
/**
    \fn     UI_updateFrameCount(uint32_t curFrame)
    \brief  Display the current displayed frame #
*/

void UI_updateFrameCount(uint32_t curFrame)
{
    char string[30];
        sprintf(string,"%lu",curFrame);
	WIDGET(lineEdit)->setText(string);
    
}
/**
    \fn      UI_setFrameCount(uint32_t curFrame,uint32_t total)
    \brief  Display the current displayed frame # and total frame #
*/

void UI_setFrameCount(uint32_t curFrame,uint32_t total)
{
    char text[80]; 
    if(total) total--; // We display from 0 to X  
    
    UI_updateFrameCount(curFrame);
    
    sprintf(text, "/ %lu ", total);
    WIDGET(label_2)->setText(text);
    
}
/**
    \fn     UI_updateTimeCount(uint32_t curFrame,uint32_t fps)
    \brief  Display the time corresponding to current frame according to fps (fps1000)
*/

void UI_updateTimeCount(uint32_t curFrame,uint32_t fps)
{
 char text[80];   
 uint16_t mm,hh,ss,ms;

      frame2time(curFrame,fps, &hh, &mm, &ss, &ms);
      sprintf(text, "%02d:%02d:%02d.%03d", hh, mm, ss, ms);
      WIDGET(lineEdit_2)->setText(text);

}
/**
    \fn     UI_updateTimeCount(uint32_t curFrame,uint32_t fps)
    \brief  Display the time corresponding to current frame according to fps (fps1000) and total duration
*/

void UI_setTimeCount(uint32_t curFrame,uint32_t total, uint32_t fps)
{
 char text[80];   
 uint16_t mm,hh,ss,ms;

        UI_updateTimeCount(curFrame,fps);
      frame2time(total,fps, &hh, &mm, &ss, &ms);
      sprintf(text, "/%02d:%02d:%02d.%03d", hh, mm, ss, ms);
      WIDGET(label_7)->setText(text);
#ifdef CUSTOM_SLIDER
      slider->setNbFrames(total);
#endif
}
/**
    \fn     UI_setMarkers(uint32_t a, uint32_t b )
    \brief  Display frame # for marker A & B
*/

void UI_setMarkers(uint32_t a, uint32_t b )
{
 char text[80];
      snprintf(text,79,"%lu",a);
      WIDGET(label_9)->setText(text);
      snprintf(text,79,"%lu",b);
      WIDGET(label_10)->setText(text);
      //
#ifdef CUSTOM_SLIDER
      slider->setA(a);
      slider->setB(b);
#endif
}
/**
    \fn     UI_getCurrentVCodec(void)
    \brief  Returns the current selected video code in menu, i.e its number (0 being the first)
*/

int 	UI_getCurrentVCodec(void)
{
  int i=WIDGET(comboBoxVideo)->currentIndex();
  if(i<0) i=0;
  return i; 
}
/**
    \fn     UI_setVideoCodec( int i)
    \brief  Select the video codec which is # x in pulldown menu (starts at zero :copy)
*/

void UI_setVideoCodec( int i)
{
  int b=!!i;
    WIDGET(comboBoxVideo)->setCurrentIndex(i);
    
     WIDGET(pushButtonVideoConf)->setEnabled(b);
     WIDGET(pushButtonVideoFilter)->setEnabled(b);
}
/**
    \fn     UI_getCurrentACodec(void)
    \brief  Returns the current selected audio code in menu, i.e its number (0 being the first)
*/

int 	UI_getCurrentACodec(void)
{
   int i=WIDGET(comboBoxAudio)->currentIndex();
  if(i<0) i=0;
  return i; 
}
/**
    \fn     UI_setAudioCodec( int i)
    \brief  Select the audio codec which is # x in pulldown menu (starts at zero :copy)
*/

void UI_setAudioCodec( int i)
{ int b=!!i;
   WIDGET(comboBoxAudio)->setCurrentIndex(i);
    WIDGET(pushButtonAudioConf)->setEnabled(b);
    WIDGET(pushButtonAudioFilter)->setEnabled(b);
}
/**
    \fn     UI_GetCurrentFormat(void)
    \brief  Returns the current selected output format
*/

ADM_OUT_FORMAT 	UI_GetCurrentFormat( void )
{
    int i=WIDGET(comboBoxFormat)->currentIndex();
  if(i<0) i=0;
  return (ADM_OUT_FORMAT)i; 
}
/**
    \fn     UI_SetCurrentFormat( ADM_OUT_FORMAT fmt )
    \brief  Select  output format
*/
uint8_t 	UI_SetCurrentFormat( ADM_OUT_FORMAT fmt )
{
    WIDGET(comboBoxFormat)->setCurrentIndex((int)fmt);
}
uint8_t UI_sliderResize(uint32_t w)
{
  printf("sliderResize %u\n",w);
  slider->resize(w,w);
  return 1; 
}
/**
      \fn GUI_initCustom
      \brief Initialize custom menu
*/
void GUI_initCustom(void )
{
  char *customdir=ADM_getCustomDir();
  if(!customdir) 
  {
      printf("No custom dir...\n");
      return;
  }
  /* Collect the name */
   if(! buildDirectoryContent(&ADM_nbCustom,customdir, customNames,ADM_MAC_CUSTOM_SCRIPT,".js"))
    {
      printf("Failed to build custom dir content");
      return;
    }
  if(!ADM_nbCustom)
  {
      printf("No custom script\n");
  }
  printf("Found %u custom scripts, adding them\n",ADM_nbCustom);
}
/**
      \fn UI_getTimeShift
      \brief get state (on/off) and value for time Shift
*/
uint8_t UI_getTimeShift(int *onoff,int *value)
{
  if(WIDGET(checkBox_TimeShift)->checkState()==Qt::Checked)
        *onoff=1;
  else
        *onoff=0;
  *value=WIDGET(spinBox_TimeValue)->value();
  return 1;
}
/**
      \fn UI_setTimeShift
      \brief get state (on/off) and value for time Shift
*/

uint8_t UI_setTimeShift(int onoff,int value)
{
  if(onoff)
    WIDGET(checkBox_TimeShift)->setCheckState(Qt::Checked);
  else
    WIDGET(checkBox_TimeShift)->setCheckState(Qt::Unchecked);
  WIDGET(spinBox_TimeValue)->setValue(value);
  return 1;
}

//********************************************
//EOF
