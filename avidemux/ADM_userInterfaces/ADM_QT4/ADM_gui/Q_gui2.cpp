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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "default.h"
#include "ADM_osSupport/ADM_misc.h"

#include "gui_action.hxx"

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


extern int automation(void );
extern void HandleAction(Action a);
static QSlider *slider=NULL;
static int _upd_in_progres=0;
/* Ugly game with macro so that buttons emit their name ...*/

class ADM_Qaction : public QAction 
{
     Q_OBJECT
    
  signals:
        void ADM_clicked( const char *name );
        
   public slots:
        void ADMTriggered(void) 
        {
            const char *name=qPrintable(this->objectName());
            printf("Hello! %s\n",name);
            emit ADM_clicked(name); 
        }
       
        
  public:
        ADM_Qaction(QMainWindow *z) : QAction(z) 
        {
          connect( this,SIGNAL(triggered()),this,SLOT(ADMTriggered()));
        }
        ~ADM_Qaction() {};

};
class ADM_QToolButton : public QToolButton 
{
     Q_OBJECT
    
  signals:
        void ADM_clicked( const char *name );
        
   public slots:
        void ADMTriggered(bool c) 
        {
            const char *name=qPrintable(this->objectName());
            printf("ToolButton! %s\n",name);
            emit ADM_clicked(name); 
        }
       
        
  public:
        ADM_QToolButton(QWidget *z) : QToolButton(z) 
        {
          connect( this,SIGNAL(clicked(bool)),this,SLOT(ADMTriggered(bool)));
        }
        ~ADM_QToolButton() {};

};

#define QAction ADM_Qaction
#define QToolButton ADM_QToolButton

#include "ui_gui2.h"
#undef QAction
#undef QToolButton


    
#define CONNECT(object,zzz) connect( (ui.object),SIGNAL(ADM_clicked(const char *)),this,SLOT(buttonPressed(const char *)));
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
     void buttonPressed(const char *source);
      void sliderMoved(int u) 
        {
          if(!_upd_in_progres)
          {
          //  printf("Slider %d\n",u);
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
     LIST_OF_BUTTONS
#undef PROCESS
 
     // Slider
          slider=ui.horizontalSlider;
          slider->setMinimum(0);
          slider->setMaximum(100);
          connect( slider,SIGNAL(valueChanged(int)),this,SLOT(sliderMoved(int)));
          //connect( slider,SIGNAL(sliderMoved()),this,SLOT(sliderMoved()));
  
 }
 /*
      We receive a button press event
 */
 void MainWindow::buttonPressed(const char *source)
 {
    // Receveid a key press Event, look into table..
    printf("Received Key press <%s>\n",source);
    Action action=searchTranslationTable(source);
    if(action!=ACT_DUMMY)
    {
      HandleAction (action) ;
    }
   
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

int UI_RunApp(void)
{

   QApplication a( global_argc, global_argv );
   MainWindow * mw = new MainWindow();
    
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    mw->show();
    QuiMainWindows=(QWidget*)mw;
    UI_QT4VideoWidget(mw->ui.frame_video);  // Add the widget that will handle video display
   if (global_argc >= 2)
    {
     automation();
    }
    return a.exec();
}

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
/*
    Return % of scale (between 0 and 1)
*/
double 	UI_readScale( void )
{
  double v;
  if(!slider) v=0;
  v= (double)(slider->value());
  printf("GetScale\n");
  return v;
}
void UI_setScale( double val )
{
if(_upd_in_progres) return;
 _upd_in_progres++;
   slider->setValue( (int)val);
 _upd_in_progres--;
   
}
void UI_purge( void )
{

}

//********************************************
#include "Q_gui2.moc"
//EOF
