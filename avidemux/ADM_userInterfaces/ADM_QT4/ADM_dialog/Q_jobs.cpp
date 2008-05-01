/**
    Jobs dialog
    (c) Mean 2007
*/

#include "Q_jobs.h"
#include "DIA_coreToolkit.h"

static void updateStatus(void);
extern bool parseECMAScript(const char *name);
static const char *StringStatus[]={QT_TR_NOOP("Ready"),QT_TR_NOOP("Succeeded"),QT_TR_NOOP("Failed"),QT_TR_NOOP("Deleted"),QT_TR_NOOP("Running")};

ADM_Job_Descriptor::ADM_Job_Descriptor(void) 
{
	status = STATUS_READY;
	memset(&startDate, 0, sizeof(startDate));
	memset(&endDate, 0, sizeof(startDate));
}

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
     headers << QT_TR_NOOP("Job Name") << QT_TR_NOOP("Status") << QT_TR_NOOP("Start Time") << QT_TR_NOOP("End Time"); 
     
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
      ADM_setText(ADM_GetFileName(_jobsName[i]),0,i,WIDGET(tableWidget));
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
        if(GUI_Confirmation_HIG(QT_TR_NOOP("Sure!"),QT_TR_NOOP("Delete job"),QT_TR_NOOP("Are you sure you want to delete %s job ?"),ADM_GetFileName(_jobsName[sel])))
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
  if(!GUI_Confirmation_HIG(QT_TR_NOOP("Sure!"),QT_TR_NOOP("Delete *all* job"),QT_TR_NOOP("Are you sure you want to delete ALL jobs ?")))
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
    GUI_Info_HIG(ADM_LOG_INFO,QT_TR_NOOP("Already done"),QT_TR_NOOP("This script has already been successfully executed."));
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
