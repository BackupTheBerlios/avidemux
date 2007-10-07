#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include "default.h"
#include "prefs.h"
#include "default.h"

#include <QDialog>
#include <QMessageBox>

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h" 

static int beQuiet=0;
extern QWidget *QuiMainWindows;

//****************************************************************************************************
void            GUI_Alert(const char *alertstring)
{
    QMessageBox::StandardButton reply;
              reply = QMessageBox::critical(QuiMainWindows, "Alert",
                                        alertstring,
                                        QMessageBox::Ok );
}
//****************************************************************************************************
void            GUI_Info(const char *alertstring)
{
    QMessageBox::StandardButton reply;
              reply = QMessageBox::information(QuiMainWindows, "Info",
                                        alertstring,
                                        QMessageBox::Ok );
}

//****************************************************************************************************
void            GUI_Info_HIG(const ADM_LOG_LEVEL level,const char *primary, const char *secondary_format, ...)
{
  uint32_t msglvl=2;
  char *string;
  char alertstring[1024];
  char alertstring2[1024];

        prefs->get(MESSAGE_LEVEL,&msglvl);

        if(msglvl<level)
        {
                printf("Info : %s \n",primary);
                return;
        }

        if(! secondary_format)
        {
            snprintf(alertstring,1024,"<big><b>%s</b></big>",primary);
         }else
         {
            va_list ap;
            va_start(ap, secondary_format);
            vsnprintf(alertstring2,1023,secondary_format, ap);
            snprintf(alertstring,1024,"<big><b>%s</b></big><br><br>%s",primary,alertstring2);
            va_end(ap);
         }
        QMessageBox::StandardButton reply;
          reply = QMessageBox::information(QuiMainWindows, "Info",
                                    alertstring,
                                    QMessageBox::Ok );

}
//****************************************************************************************************
void            GUI_Error_HIG(const char *primary, const char *secondary_format, ...)
{
  uint32_t msglvl=2;
  char *string;
  char alertstring[1024];
  char alertstring2[1024];
  
        prefs->get(MESSAGE_LEVEL,&msglvl);

        if(msglvl==ADM_LOG_NONE) 
        {
                printf("Error :%s\n",primary);
                return;
        }

        if(! secondary_format)
        {
            snprintf(alertstring,1024,"<big><b>%s</b></big>",primary);
         }else
         {
            va_list ap;
            va_start(ap, secondary_format);
            vsnprintf(alertstring2,1023,secondary_format, ap);
            snprintf(alertstring,1024,"<big><b>%s</b></big><br><br>%s",primary,alertstring2);
            va_end(ap);
         }
          QMessageBox::StandardButton reply;
          reply = QMessageBox::critical(QuiMainWindows, "Info",
                                    alertstring,
                                    QMessageBox::Ok );
}
//****************************************************************************************************
int             GUI_Confirmation_HIG(const char *button_confirm, const char *primary, const char *secondary_format, ...)
{
uint32_t msglvl=2;
char alertstring[1024];
char alertstring2[1024];
        if (beQuiet)
              {
                      printf("Info: %s\n", primary);
                      return 0;
              }
        
        if (!secondary_format)
        {
              snprintf(alertstring,1024,"<big><b>%s</b></big>",primary);
        }
        else
        {	
              va_list ap;
              va_start(ap, secondary_format);
              vsnprintf(alertstring2,1023,secondary_format, ap);
              snprintf(alertstring,1024,"<big><b>%s</b></big><br><br>%s",primary,alertstring2);
              va_end(ap);
        }
          QMessageBox::StandardButton reply;
          reply = QMessageBox::question(QuiMainWindows, "Confirmation",
                                    alertstring,
                                    QMessageBox::Yes | QMessageBox::No );
          if(reply==QMessageBox::Yes) return 1;
        return 0; 
}
//****************************************************************************************************
int             GUI_YesNo(const char *primary, const char *secondary_format, ...)
{
uint32_t msglvl=2;
char alertstring[1024];
char alertstring2[1024];
        if (beQuiet)
              {
                      printf("Info: %s\n", primary);
                      return 0;
              }
        
        if (!secondary_format)
        {
              snprintf(alertstring,1024,"<big><b>%s</b></big>",primary);
        }
        else
        {	
              va_list ap;
              va_start(ap, secondary_format);
              vsnprintf(alertstring2,1023,secondary_format, ap);
              snprintf(alertstring,1024,"<big><b>%s</b></big><br><br>%s",primary,alertstring2);
              va_end(ap);
        }
          QMessageBox::StandardButton reply;
          reply = QMessageBox::question(QuiMainWindows, "Confirmation",
                                    alertstring,
                                    QMessageBox::Yes | QMessageBox::No );
          if(reply==QMessageBox::Yes) return 1;
        return 0; 
}
//****************************************************************************************************
int             GUI_Question(const char *alertstring)
{
 QMessageBox::StandardButton reply;
          if (beQuiet)
              {
                      printf("Question: %s\n", alertstring);
                      return 0;
              }
          reply = QMessageBox::question(QuiMainWindows, "Question",
                                    alertstring,
                                    QMessageBox::Yes | QMessageBox::No );
          if(reply==QMessageBox::Yes) return 1;
          return 0;
}
/**
    \fn GUI_Alternate(char *title,char *choice1,char *choice2)
    \brief Popup a dialog box name title with 2 buttons (choice1/choice2)
    \return 0 if first is selected, 1 if second
*/
int      GUI_Alternate(char *title,char *choice1,char *choice2)
{
int reply;
QMessageBox *box;
    if (beQuiet)
              {
                      printf("Alternate<%s>: %s or %s\n", title,choice1,choice2);
                      return 0;
              }

char alertstring[1024];

        box=new QMessageBox(QuiMainWindows);
        box->setWindowTitle("Question ?");
        box->addButton(choice1,QMessageBox::YesRole);
        box->addButton(choice2,QMessageBox::NoRole);
         if (title)
        {
              snprintf(alertstring,1024,"%s",title);
        }else
        {
            snprintf(alertstring,1024,"Question");
        }
        box->setText(alertstring);
        box->setIcon(QMessageBox::Question);
        reply = box->exec();
        delete box;
        printf("Reply:%d\n",reply);
        if(reply==0) return 1;
        return 0;

}
//****************************************************************************************************
uint8_t  GUI_getDoubleValue(double *valye, float min, float max, const char *title)
{
  
  
  return 0; 
}
//****************************************************************************************************
uint8_t		isQuiet(void)
{
    return beQuiet;
}
//****************************************************************************************************
void            GUI_Verbose(void)
{
    beQuiet=0;
}
//****************************************************************************************************
void            GUI_Quiet(void)
{
  beQuiet=1;
}
//****************************************************************************************************
//EOF
