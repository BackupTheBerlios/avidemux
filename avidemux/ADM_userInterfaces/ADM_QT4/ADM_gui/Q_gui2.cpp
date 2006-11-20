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
    
    
    
#include "ui_gui2.h"

 class MainWindow : public QMainWindow
 {
     Q_OBJECT

 public:
     MainWindow();
 public slots:
     void menuSignal(void); //int menuE);
 private slots:
   

 private:
     Ui_MainWindow ui;
 };
 
extern int automation(void );

extern void HandleAction(Action a);

MainWindow::MainWindow()     : QMainWindow()
 {
     ui.setupUi(this);
     connect(  (ui.actionOpen),SIGNAL(clicked()),this,SLOT(menuSignal())  );
     connect(  (ui.actionSave_BMP),SIGNAL(activated()),this,SLOT(menuSignal())  );
     
  // Slots and stuff :  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
 }
 void MainWindow::menuSignal(void) //int menuSignal)
 {
/*    Action action=(Action) menuSignal;
    printf("Action: %d\n");
    HandleAction (action);*/
   printf("Hello!\n");
 }
 
//*********************************************
//***** Hook to core                ***********
//*********************************************
extern int global_argc;
extern char **global_argv;
int UI_Init(int nargc,char **nargv)
{
  global_argc=nargc;
  global_argv=nargv;
  return 0;
}

int UI_RunApp(void)
{

   QApplication a( global_argc, global_argv );
   MainWindow * mw = new MainWindow();
    
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    mw->show();
    return a.exec();
/* if (global_argc >= 2)
    {
     automation();
    }*/
    return 1;
}


#include "Q_gui2.moc"
//EOF
