/***************************************************************************
    copyright            : (C) 2007 by mean
    email                : fixounet@free.fr
    
    * We hide some info the the "type"
    * I.e.
    0--1000 : QT4 internal
    2000-3000: Filters
    3000-4000  filterFamilyClick Filter
    8000-9000  Active Filter
    
    
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#define Ui_Dialog Ui_mainFilterDialog
#include "ui_mainfilter.h"
#undef Ui_Dialog
#include "QStringListModel" 
#include "QColor" 
#include "ADM_default.h"
#include "DIA_fileSel.h"

# include "prefs.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/ADM_audiodef.h"

#include "ADM_assert.h"
#include "GUI_render.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
#include "ADM_filter/vidVCD.h"
#include "DIA_factory.h"
/*******************************************************/
#define NB_TREE 8
#define myFg 0xFF
#define myBg 0xF0
static int max=0;
/******************************************************/
#define ALL_FILTER_BASE       1000
#define EXTERNAL_FILTER_BASE  2000
#define ACTIVE_FILTER_BASE    3000
/******************************************************/
extern FILTER videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;
extern std::vector <FILTER_ENTRY> allfilters;

extern const char  *filterGetNameFromTag(VF_FILTERS tag);
extern ADM_Composer *video_body;
extern AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,CONFcouple *conf, AVDMGenericVideoStream *in) ;
/*******************************************************/
class filtermainWindow : public QDialog
{
     Q_OBJECT

 public:
     filtermainWindow();
 //    virtual ~filtermainWindow();
     void             buildActiveFilterList(void);
     Ui_mainFilterDialog ui;
     
     
     
     QListWidget      *availableList;
     QListWidget      *activeList;
     
     
     
 public slots:
        void VCD(bool b);
        void DVD(bool b);
        void SVCD(bool b);
        void halfD1(bool b);
        void add(bool b);
        void up(bool b);
        void down(bool b);
        void remove(bool b);
        void configure(bool b);
        void partial(bool b);
        void activeDoubleClick( QListWidgetItem  *item);
        void allDoubleClick( QListWidgetItem  *item);
	void filterFamilyClick(QListWidgetItem *item);
	void filterFamilyClick(int  item);
 private slots:
 private:
        int startFilter[NB_TREE];
        int filterSize[NB_TREE];
        void setSelected(int sel);
        void displayFamily(uint32_t family);
        void setupFilters(void);
};
/**
        \fn     void setSelected(int sel)
        \brief  Set the sel line as selected in the active filter window
*/
void filtermainWindow::setSelected( int sel)
{
  if(!sel) return;
  activeList->setCurrentRow(sel-1);
}

/**
        \fn     add( bool b)
        \brief  Retrieve the selected filter and add it to the active filters
*/
void filtermainWindow::add( bool b)
{
  /* Now that we have the tab, get the selection */
   QListWidgetItem *item=availableList->currentItem();
   VF_FILTERS tag;
   if(item)
   {
     int itag=item->type();
     if(itag>=ALL_FILTER_BASE && itag < EXTERNAL_FILTER_BASE)
     {
       tag=allfilters[itag-ALL_FILTER_BASE].tag;
       
     }else
     {
        ADM_assert(0); 
     }
      printf("Tag : %d\n",itag);
      
      // Create our filter...
      
       CONFcouple *coup;
        videofilters[nb_active_filter].filter = filterCreateFromTag (tag, NULL, videofilters[nb_active_filter - 1].filter);
        if(!videofilters[nb_active_filter].filter->    configure (videofilters[nb_active_filter - 1].filter))
        {
            delete videofilters[nb_active_filter].filter;
            return;
        }
        videofilters[nb_active_filter].filter->getCoupledConf (&coup);
        videofilters[nb_active_filter].tag = tag;
        videofilters[nb_active_filter].conf = coup;
        nb_active_filter++;
        setSelected(nb_active_filter);
        buildActiveFilterList();
   }

}
/**
        \fn     remove( bool b)
        \brief  Remove selected filters from the active window list
*/
void filtermainWindow::remove( bool b)
{
   /* Get selection if any */
  /* Now that we have the tab, get the selection */
   QListWidgetItem *item=activeList->currentItem();
   if(!item)
   {
      printf("No selection\n");
      return;
   }
    
     int itag=item->type();
     ADM_assert(itag>ACTIVE_FILTER_BASE);
     itag-=ACTIVE_FILTER_BASE;
     /* Filter 0 is the decoder ...*/
      printf("Rank : %d\n",itag); 
     ADM_assert(itag);
     /**/
      if (videofilters[itag].conf)
            {
                    delete videofilters[itag].conf;
                    videofilters[itag].conf = NULL;
            }
        // recreate derivated filters
        for (uint32_t i = itag ; i < nb_active_filter-1; i++)
        {
                    delete videofilters[i ].filter;
                    videofilters[i ].filter = filterCreateFromTag(videofilters[i+1].tag,
                                                    videofilters[i+1].conf,
                                                    videofilters[i - 1].filter);
                    videofilters[i ].conf = videofilters[i+1].conf;
                    videofilters[i ].tag = videofilters[i+1].tag;
        }
        /* Delete last filter which is now at last filter -1 */
            delete videofilters[nb_active_filter - 1].filter;
            videofilters[nb_active_filter - 1].filter = NULL;
            nb_active_filter--;
            buildActiveFilterList ();
            if(nb_active_filter>1)
            {
              if(itag<nb_active_filter-1)
                  setSelected(itag);
              else
                  setSelected(nb_active_filter-1);
            }
  
}
#define MAKE_BUTTON(button,call) \
void filtermainWindow::button( bool b) \
{ \
    call(); \
    getFirstVideoFilter (); \
    buildActiveFilterList ();  \
}
MAKE_BUTTON(DVD,setDVD)
MAKE_BUTTON(VCD,setVCD)
MAKE_BUTTON(SVCD,setSVCD)
MAKE_BUTTON(halfD1,setHalfD1)
/**
        \fn     configure( bool b)
        \brief  Configure the selected active filter
*/
void filtermainWindow::configure( bool b)
{
   /* Get selection if any */
  /* Now that we have the tab, get the selection */
   QListWidgetItem *item=activeList->currentItem();
   if(!item)
   {
      printf("No selection\n");
      return;
   }
    
     int itag=item->type();
     ADM_assert(itag>ACTIVE_FILTER_BASE);
     itag-=ACTIVE_FILTER_BASE;
     /* Filter 0 is the decoder ...*/
      printf("Rank : %d\n",itag); 
     ADM_assert(itag);
     /**/
     
        if(!videofilters[itag].filter->configure (videofilters[itag - 1].filter)) return;
        /* Recreate chain if needed , config has changed */
        CONFcouple *couple;
        videofilters[itag].filter->getCoupledConf (&couple);
        videofilters[itag].conf = couple;
        getFirstVideoFilter ();
        buildActiveFilterList ();
}
/**
        \fn     up( bool b)
        \brief  Move selected filter one place up
*/
void filtermainWindow::up( bool b)
{
   QListWidgetItem *item=activeList->currentItem();
   if(!item)
   {
      printf("No selection\n");
      return;
   }
    
     int itag=item->type();
     ADM_assert(itag>ACTIVE_FILTER_BASE);
     itag-=ACTIVE_FILTER_BASE;
     /* Filter 0 is the decoder ...*/
      printf("Rank : %d\n",itag); 
     ADM_assert(itag);
     
     if (itag < 2) return;
        // swap action parameter & action parameter -1
        FILTER tmp;
        memcpy (&tmp, &videofilters[itag - 1], sizeof (FILTER));
        memcpy (&videofilters[itag - 1],
            &videofilters[itag], sizeof (FILTER));
        memcpy (&videofilters[itag], &tmp, sizeof (FILTER));
        getFirstVideoFilter ();
        buildActiveFilterList ();
        setSelected(itag-1);
}
/**
        \fn     down( bool b)
        \brief  Move selected filter one place down
*/
void filtermainWindow::down( bool b)
{
   QListWidgetItem *item=activeList->currentItem();
   if(!item)
   {
      printf("No selection\n");
      return;
   }
    
     int itag=item->type();
     ADM_assert(itag>ACTIVE_FILTER_BASE);
     itag-=ACTIVE_FILTER_BASE;
     /* Filter 0 is the decoder ...*/
      printf("Rank : %d\n",itag); 
     ADM_assert(itag);
     
    if (((int) itag < (int) (nb_active_filter - 1)) && (itag))
        {
            // swap action parameter & action parameter -1
            FILTER tmp;
            memcpy (&tmp, &videofilters[itag + 1], sizeof (FILTER));
            memcpy (&videofilters[itag + 1],
                        &videofilters[itag], sizeof (FILTER));
            memcpy (&videofilters[itag], &tmp, sizeof (FILTER));
            getFirstVideoFilter ();
            buildActiveFilterList ();
            setSelected(itag+1);
        }
}
/**
        \fn     filtermainWindow::filterFamilyClick( QListWidgetItem  *item)
        \brief  Select family among color etc... 
*/

void filtermainWindow::filterFamilyClick(QListWidgetItem *item)
{
    int family= ui.listFilterCategory->currentRow();
    if(family>=0)
        displayFamily(family);
}
void filtermainWindow::filterFamilyClick(int  m)
{
        if(m>=0)
                displayFamily(m);
}
void filtermainWindow::displayFamily(uint32_t family)
{
  printf("Family :%u\n",family);
  
  availableList->clear();
  QColor colorgrey;
  colorgrey.setRgb(myBg,myBg,myBg);
  QBrush brush(colorgrey);
  QSize sz;
  
  for (uint32_t i = 0; i < filterSize[family]; i++)
    {
      int r=startFilter[family]+i;

      if (allfilters[r].viewable==1)
        {
          QString str; //="<b>";
          str+=allfilters[r].name;
         
          QListWidgetItem *item;
          if(family==NB_TREE-1)
                item=new QListWidgetItem(str,availableList,EXTERNAL_FILTER_BASE+i);
          else
                item=new QListWidgetItem(str,availableList,ALL_FILTER_BASE+r);
          item->setToolTip(allfilters[r].description);
          if(i&1) item->setBackground(brush);
          availableList->addItem(item);
          
        }
     }
   
}

/**
        \fn     filtermainWindow::activeDoubleClick( QListWidgetItem  *item)
        \brief  One of the active window has been double clicked, call configure
*/
void filtermainWindow::activeDoubleClick( QListWidgetItem  *item)
{
  
    configure(0);
}
/**
        \fn     filtermainWindow::activeDoubleClick( QListWidgetItem  *item)
        \brief  One of the active window has been double clicked, call configure
*/
void filtermainWindow::allDoubleClick( QListWidgetItem  *item)
{
  
    add(0);
}
/**
        \fn     filtermainWindow::partial( bool b)
        \brief  Partialize one filter
*/
void filtermainWindow::partial( bool b)
{
  printf("partial\n"); 
   QListWidgetItem *item=activeList->currentItem();
   if(!item)
   {
      printf("No selection\n");
      return;
   }
    
     int itag=item->type();
     ADM_assert(itag>ACTIVE_FILTER_BASE);
     itag-=ACTIVE_FILTER_BASE;
     /* Filter 0 is the decoder ...*/
      printf("Rank : %d\n",itag); 
      ADM_assert(itag);
     
        AVDMGenericVideoStream *replace;
        CONFcouple *conf;
        conf = videofilters[itag].conf;
        if (videofilters[itag].tag == VF_PARTIAL)	// cannot recurse
        {
            GUI_Error_HIG (QT_TR_NOOP("The filter is already partial"), NULL);
            return;
        }
        replace =new ADMVideoPartial (videofilters[itag - 1].
                                      filter,
                                      videofilters[itag].tag,
                                      conf);
        if(replace->configure (videofilters[itag - 1].filter))
        {
            delete videofilters[itag].filter;
            if (conf) delete conf;
            videofilters[itag].filter = replace;
            replace->getCoupledConf (&conf);
            videofilters[itag].conf = conf;
            videofilters[itag].tag = VF_PARTIAL;
            getFirstVideoFilter ();
            buildActiveFilterList ();   
        }
        else delete replace;
}
/**
        \fn setup
        \brief Prepare 
*/
void filtermainWindow::setupFilters(void)
{
  int current_tree=-1;
  int current_raw=0;;
  
  max=0;
  
  for (uint32_t i = 0; i < allfilters.size(); i++)
    {
      if (allfilters[i].viewable==1)
        {
          current_raw++;
          
        }else 
        {
                current_tree++;
                if(current_tree) filterSize[current_tree-1]=current_raw;
                if(current_tree>=NB_TREE) break;
                startFilter[current_tree]=i+1;
                current_raw=0;
                
        }
    }
    ADM_assert(NB_TREE==8);
    startFilter[NB_TREE-1]=2000;
    filterSize[NB_TREE-1]=0;
    for(int i=0;i<NB_TREE;i++)
    {
             printf("%d Start at %d size :%d\n",i,startFilter[i],filterSize[i]);
    }
}

/**
        \fn     buildActiveFilterList(void)
        \brief  Build and display all active filters (may be empty)
*/
void filtermainWindow::buildActiveFilterList(void)
{
  VF_FILTERS fil;
  
  
  activeList->clear();
  QColor colorGrey,colorWhite;
  colorGrey.setRgb(myBg,myBg,myBg);
  colorWhite.setRgb(myFg,myFg,myFg);
  QBrush brush(colorGrey);
  QBrush brushW(colorWhite);

  for (uint32_t i = 1; i < nb_active_filter; i++)
    {
                QString str;
                fil=videofilters[i].tag;

                 str =filterGetNameFromTag(fil);
                 str+= videofilters[i].filter->printConf ();
                 QListWidgetItem *item=new QListWidgetItem(str,activeList,ACTIVE_FILTER_BASE+i);
                 if(i&1) item->setBackground(brush);
                        else item->setBackground(brushW);
                 activeList->addItem(item);
    }
    

}
  /**
  */
filtermainWindow::filtermainWindow()     : QDialog()
 {
        memset( startFilter,0,sizeof(int)*NB_TREE);
        memset( filterSize,0,sizeof(int)*NB_TREE);
 
    ui.setupUi(this);
    setupFilters();  
      
    availableList=ui.listWidgetAvailable;
    activeList=ui.listWidgetActive;
    connect(ui.listFilterCategory,SIGNAL(itemDoubleClicked(QListWidgetItem *)),
                this,SLOT(filterFamilyClick(QListWidgetItem *)));
    connect(ui.listFilterCategory,SIGNAL(itemClicked(QListWidgetItem *)),
                this,SLOT(filterFamilyClick(QListWidgetItem *)));

    connect(activeList,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(activeDoubleClick(QListWidgetItem *)));
    connect(availableList,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(allDoubleClick(QListWidgetItem *)));
    
    connect((ui.toolButtonConfigure),SIGNAL(clicked(bool)),this,SLOT(configure(bool)));
    connect((ui.toolButtonAdd),SIGNAL(clicked(bool)),this,SLOT(add(bool)));
    connect((ui.pushButtonRemove),SIGNAL(clicked(bool)),this,SLOT(remove(bool)));
    connect((ui.toolButtonUp),SIGNAL(clicked(bool)),this,SLOT(up(bool)));
    connect((ui.toolButtonDown),SIGNAL(clicked(bool)),this,SLOT(down(bool)));
    connect((ui.toolButtonPartial),SIGNAL(clicked(bool)),this,SLOT(partial(bool)));
    connect(ui.buttonClose, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui.pushButtonDVD, SIGNAL(clicked(bool)), this, SLOT(DVD(bool)));
    connect(ui.pushButtonVCD, SIGNAL(clicked(bool)), this, SLOT(VCD(bool)));
    connect(ui.pushButtonSVCD, SIGNAL(clicked(bool)), this, SLOT(SVCD(bool)));
    connect(ui.pushButtonHalfDVD, SIGNAL(clicked(bool)), this, SLOT(halfD1(bool)));
   
    displayFamily(0);
    buildActiveFilterList(); 
 }
/*******************************************************/

int GUI_handleVFilter(void);
static void updateFilterList (filtermainWindow *dialog);

/**
      \fn     GUI_handleVFilter(void)
      \brief  Show the main filter window allowing user to add/remove/configure video filters


*/
int GUI_handleVFilter(void)
{
        filtermainWindow dialog;
        if(QDialog::Accepted==dialog.exec())
        {
        }
	return 0;
}
/** 
    \fn partialCb
    \brief Partial callback to configure the swallowed filter
    
*/
static void partialCb(void *cookie);
void partialCb(void *cookie)
{
  void **params=(void **)cookie;
  AVDMGenericVideoStream *son=(AVDMGenericVideoStream *)params[0];
  AVDMGenericVideoStream *previous=(AVDMGenericVideoStream *)params[1];
  son->configure(previous);
}
/** 
    \fn DIA_getPartial
    \brief Partial dialog
    
*/

uint8_t DIA_getPartial(PARTIAL_CONFIG *param,AVDMGenericVideoStream *son,AVDMGenericVideoStream *previous)
{
#define PX(x) &(param->x)
  void *params[2]={son,previous};
         uint32_t fmax=previous->getInfo()->nb_frames;
         if(fmax) fmax--;
         
         diaElemUInteger  start(PX(_start),QT_TR_NOOP("Partial Start Frame:"),0,fmax);
         diaElemUInteger  end(PX(_end),QT_TR_NOOP("Partial End Frame:"),0,fmax);
         diaElemButton    button(QT_TR_NOOP("Configure child"), partialCb,params);
         
         diaElem *tabs[]={&start,&end,&button};
        return diaFactoryRun(QT_TR_NOOP("Partial Video Filter"),3,tabs);
}
//EOF

