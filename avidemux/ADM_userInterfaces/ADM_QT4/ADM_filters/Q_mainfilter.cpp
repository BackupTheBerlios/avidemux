/***************************************************************************
    copyright            : (C) 2007 by mean
    email                : fixounet@free.fr
    
    * We hide some info the the "type"
    * I.e.
    0--1000 : QT4 internal
    2000-3000: Filters
    3000-4000  External Filter
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


#define Ui_Dialog Ui_mainFilterDialog
#include "ui_mainfilter.h"
#undef Ui_Dialog
#include "QStringListModel" 
#include "default.h"
#include "ADM_toolkit/filesel.h"

# include "prefs.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_audio/ADM_audiodef.h"

#include "ADM_assert.h"
#include "GUI_render.h"
#include "ADM_osSupport/ADM_cpuCap.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
/*******************************************************/
#define NB_TREE 7
static int startFilter[NB_TREE];
static int filterSize[NB_TREE];
static int max=0;
/******************************************************/
#define ALL_FILTER_BASE       1000
#define EXTERNAL_FILTER_BASE  2000
#define ACTIVE_FILTER_BASE    3000
/******************************************************/
extern FILTER_ENTRY allfilters[MAX_FILTER];
extern uint32_t nb_video_filter;
extern FILTER videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;
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
     void             buildAvailableFilterList(void);
     void             buildActiveFilterList(void);
     Ui_mainFilterDialog ui;
     
     QListWidget      *allList[NB_TREE];
     
     
     QListWidget      *activeList;
     
     
     
 public slots:
        void add(bool b);
        void up(bool b);
        void down(bool b);
        void remove(bool b);
        void configure(bool b);
        void partial(bool b);
        void activeDoubleClick( QListWidgetItem  *item);
        void allDoubleClick( QListWidgetItem  *item);
 private slots:
 private:
};

/**
        \fn     add( bool b)
        \brief  Retrieve the selected filter and add it to the active filters
*/
void filtermainWindow::add( bool b)
{
  
  /* Get selection if any */
  int tab=ui.tabWidgetSubtitles->currentIndex();
  ADM_assert(tab>=0 && tab < NB_TREE);
  /* Now that we have the tab, get the selection */
   QListWidgetItem *item=allList[tab]->currentItem();
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
  
}
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
            GUI_Error_HIG (_("The filter is already partial"), NULL);
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
#define myBg 0xF0
/**
        \fn     buildAvailableFilterList(void)
        \brief  build the internal datas needed to handle the list. Need to be called only once.
*/
void filtermainWindow::buildAvailableFilterList(void)
{
  int current_tree=-1;
  int current_raw=0;;
  
  max=0;
  QColor colorGrey;
  colorGrey.setRgb(myBg,myBg,myBg);
  QBrush brush(colorGrey);
  QSize sz;
  
  for (uint32_t i = 0; i < nb_video_filter; i++)
    {
      if (allfilters[i].viewable==1)
        {
          QString str; //="<b>";
          str+=allfilters[i].name;
          max++;
          
          QListWidgetItem *item=new QListWidgetItem(str,allList[current_tree],ALL_FILTER_BASE+i);
          item->setToolTip(allfilters[i].description);
          if(i&1) item->setBackground(brush);
          allList[current_tree]->addItem(item);
          current_raw++;
          
        }else 
        {
                current_tree++;
                if(current_tree) filterSize[current_tree-1]=current_raw;
                if(current_tree>=NB_TREE) break;
                allList[current_tree]->clear();
                startFilter[current_tree]=i+1;
                current_raw=0;
                
        }
    }
   
    
    ADM_assert(NB_TREE==7);
  
}
  /**
        \fn     buildActiveFilterList(void)
        \brief  Build and display all active filters (may be empty)
*/
void filtermainWindow::buildActiveFilterList(void)
{
  VF_FILTERS fil;
  activeList->clear();
  QColor colorGrey;
  colorGrey.setRgb(myBg,myBg,myBg);
  QBrush brush(colorGrey);

  for (uint32_t i = 1; i < nb_active_filter; i++)
    {
                QString str;
                fil=videofilters[i].tag;

                 str =filterGetNameFromTag(fil);
                 str+= videofilters[i].filter->printConf ();
                 QListWidgetItem *item=new QListWidgetItem(str,activeList,ACTIVE_FILTER_BASE+i);
                 if(i&1) item->setBackground(brush);
                 activeList->addItem(item);
    }
    

}
  /**
  */
filtermainWindow::filtermainWindow()     : QDialog()
 {
      ui.setupUi(this);
      
#define ASSOCIATE(x,y)   allList[x]=ui.listWidget##y;
      ASSOCIATE(0,Transform);
      ASSOCIATE(1,Interlacing);
      ASSOCIATE(2,Colors);
      ASSOCIATE(3,Noise);
      ASSOCIATE(4,Sharpness);
      ASSOCIATE(5,Subtitles);
      ASSOCIATE(6,Misc);
      
      activeList=ui.listWidgetActive;
      
      buildAvailableFilterList();
      buildActiveFilterList();
      
      
      
    connect(activeList,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(activeDoubleClick(QListWidgetItem *)));
    for(int i=0;i<NB_TREE;i++)
    {
      connect(allList[i],SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(allDoubleClick(QListWidgetItem *)));
    }

    connect((ui.toolButtonConfigure),SIGNAL(clicked(bool)),this,SLOT(configure(bool)));
    connect((ui.toolButtonAdd),SIGNAL(clicked(bool)),this,SLOT(add(bool)));
    connect((ui.pushButtonRemove),SIGNAL(clicked(bool)),this,SLOT(remove(bool)));
    connect((ui.toolButtonUp),SIGNAL(clicked(bool)),this,SLOT(up(bool)));
    connect((ui.toolButtonDown),SIGNAL(clicked(bool)),this,SLOT(down(bool)));
    connect(ui.buttonClose, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(ui.toolButtonPartial, SIGNAL(clicked(bool)), this, SLOT(partial(bool)));
    
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

//EOF

