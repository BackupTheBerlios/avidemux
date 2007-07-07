/**
    Jobs dialog
    (c) Mean 2007
*/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "ui_jobs.h"


#include "default.h"
#include "avidemutils.h"
#include "avi_vars.h"
#include "ADM_osSupport/ADM_misc.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_assert.h"

static void             updateStatus(void);
extern bool parseECMAScript(const char *name);
static const char *StringStatus[]={_("Ready"),_("Succeeded"),_("Failed"),_("Deleted"),_("Running")};


typedef enum 
{
        STATUS_READY=0,
        STATUS_SUCCEED,
        STATUS_FAILED,
        STATUS_DELETED,
        STATUS_RUNNING
}JOB_STATUS;


class ADM_Job_Descriptor
{
  public:
  JOB_STATUS  status;
  ADM_date    startDate;
  ADM_date    endDate;
  ADM_Job_Descriptor(void) 
  {
    status=STATUS_READY;
    memset(&startDate,0,sizeof(startDate));
    memset(&endDate,0,sizeof(startDate));
  }
  
};

typedef enum 
{
        COMMAND_DELETE_ALL=1,
        COMMAND_DELETE=2,
        COMMAND_RUN_ALL=3,
        COMMAND_RUN=4
};





class jobsWindow : public QDialog
 {
     Q_OBJECT
 protected : 
        uint32_t        _nbJobs;
        char            **_jobsName;
        ADM_Job_Descriptor  *desc;
        uint8_t             updateRows(void);
 public:
                    jobsWindow(uint32_t n,char **j);
                    ~jobsWindow();
     Ui_Jobs        ui;
 public slots:
      //void gather(void);
      int RunOne(bool b);
      int RunAll(bool b);
      int DeleteOne(bool b);
      int DeleteAll(bool b);
 private slots:
   

 private:
     
 };
 /**
          \fn jobsWindow
 */
jobsWindow::jobsWindow(uint32_t n,char **j)     : QDialog()
 {
     ui.setupUi(this);
     _nbJobs=n;
     _jobsName=j;
     desc=new ADM_Job_Descriptor[n];
     // Setup display
#define WIDGET(x) ui.x
     WIDGET(tableWidget)->setRowCount(_nbJobs);
     WIDGET(tableWidget)->setColumnCount(4);

     // Set headers
      QStringList headers;
     headers << _("Job Name") << _("Status") << _("Start Time") << _("End Time"); 
     
     WIDGET(tableWidget)->setVerticalHeaderLabels(headers);
     updateRows();
    
#define CNX(x) connect( ui.pushButton##x,SIGNAL(clicked(bool)),this,SLOT(x(bool)))
           //connect( ui.pushButtonRunOne,SIGNAL(buttonPressed(const char *)),this,SLOT(runOne(const char *)));
      CNX(RunOne);
      CNX(RunAll);
      CNX(DeleteAll);
      CNX(DeleteOne);
 }
 /**
    \fn ~jobsWindow
 */
 jobsWindow::~jobsWindow()
 {
   
        // Now delete the "deleted" jobs
        for(int i=0;i<_nbJobs;i++)
        {
                if(desc[i].status==STATUS_DELETED)
                {
                        unlink(_jobsName[i]);
                }

        }
        delete [] desc;
        desc=NULL;

 }
 /*
    There is maybe a huge mem leak here
 */
static void ADM_setText(const char *txt,uint32_t col, uint32_t row,QTableWidget *w)
{
        QString str(txt);
        QTableWidgetItem *newItem = new QTableWidgetItem(str);//GetFileName(_jobsName[i]));
        w->setItem(row, col, newItem);
  
}
 /**
      \fn updateRaw
      \brief update display for raw x
 */
uint8_t jobsWindow::updateRows(void)
{
   WIDGET(tableWidget)->clear();
   ADM_Job_Descriptor *j;
   char str[20];
   for(int i=0;i<_nbJobs;i++)
   {
      j=&(desc[i]);
      ADM_setText(GetFileName(_jobsName[i]),0,i,WIDGET(tableWidget));
      ADM_setText(StringStatus[j->status],1,i,WIDGET(tableWidget));
      
      sprintf(str,"%02u:%02u:%02u",j->startDate.hours,j->startDate.minutes,j->startDate.seconds);
      ADM_setText(str,2,i,WIDGET(tableWidget));
      
      sprintf(str,"%02u:%02u:%02u",j->endDate.hours,j->endDate.minutes,j->endDate.seconds);
      ADM_setText(str,3,i,WIDGET(tableWidget));

   }
   
      return 1;
}

                                                                 
                                                                 
/**
      \fn deleteOne
      \brief delete one job
*/
int jobsWindow::DeleteOne(bool b)
{
  int sel=WIDGET(tableWidget)->currentRow();
        if(sel<=0 || sel>=_nbJobs) return 0;
        if(GUI_Confirmation_HIG(_("Sure!"),_("Delete job"),_("Are you sure you want to delete %s job ?"),GetFileName(_jobsName[sel])))
        {
                desc[sel].status=STATUS_DELETED;
        }
        updateRows();
        return 0;
}         
/**
      \fn deleteAll
      \brief delete all job
*/
int jobsWindow::DeleteAll(bool b)
{
  if(!GUI_Confirmation_HIG(_("Sure!"),_("Delete *all* job"),_("Are you sure you want to delete ALL jobs ?")))
  {
          return 0;
  }
  for(int sel=0;sel<_nbJobs;sel++)
  {
        desc[sel].status=STATUS_DELETED;
   
  }
  updateRows();
  return 0;
}                                                                 
                                                        
/**
      \fn runOne
      \brief Run one job
*/
int jobsWindow::RunOne(bool b)
{
  int sel=WIDGET(tableWidget)->currentRow();
  printf("Selected %d\n",sel);
  if(sel<0) return 0;
  if(sel>=_nbJobs) return 0;
  
  if(desc[sel].status==STATUS_SUCCEED) 
  {
    GUI_Info_HIG(ADM_LOG_INFO,_("Already done"),_("This script has already been successfully executed."));
    return 0;
  }

  desc[sel].status=STATUS_RUNNING;
  updateRows();
  GUI_Quiet();
  TLK_getDate(&(desc[sel].startDate));
  if(parseECMAScript(_jobsName[sel])) desc[sel].status=STATUS_SUCCEED;
  else desc[sel].status=STATUS_FAILED;
  TLK_getDate(&(desc[sel].endDate));
  updateRows();
  GUI_Verbose();
  return 0;
}
/**
      \fn RunAll
      \brief Run all jobs
*/
int jobsWindow::RunAll(bool b)
{
  for(int sel=0;sel<_nbJobs;sel++)
  {
    if(desc[sel].status==STATUS_SUCCEED) continue;
    desc[sel].status=STATUS_RUNNING;
    updateRows();
    GUI_Quiet();
    TLK_getDate(&(desc[sel].startDate));
    if(parseECMAScript(_jobsName[sel])) desc[sel].status=STATUS_SUCCEED;
    else desc[sel].status=STATUS_FAILED;
    TLK_getDate(&(desc[sel].endDate));
    updateRows();
    GUI_Verbose();
  }
  return 0;
}

/**
    \fn     DIA_job
    \brief  
*/
uint8_t  DIA_job(uint32_t nb, char **name)
{
  uint8_t r=0;
  jobsWindow jobswindow(nb,name) ;
     
     if(jobswindow.exec()==QDialog::Accepted)
     {
       r=1;
     }
     return r;
}

//EOF