/***************************************************************************
                          gui_filter.cpp  -  description
                             -------------------
    begin                : Wed Mar 27 2002
    copyright            : (C) 2002 by mean
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
#include <string.h>


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <time.h>
#include <sys/time.h>

#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/toolkit_gtk.h"



#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_toolkit/filesel.h"

#include "prototype.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
#include "ADM_gui2/support.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "ADM_filter/vidVCD.h"
//_______________________________________


static VF_FILTERS new_filter (void);
//static void  on_configure (GtkButton * button, gpointer user_data);
static void on_action (GtkButton * button, gpointer user_data);
static void  on_action_double_click (GtkButton * button, gpointer user_data);
static void updateFilterList( void );
//__________________________________________

extern AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,CONFcouple *conf, AVDMGenericVideoStream *in) ;


typedef enum gui_act
{
  A_ADD = 1,
  A_CONFIGURE,
  A_UP,
  A_DOWN,
  A_REMOVE,
  A_DONE,
  A_VCD,
  A_SVCD,  
  A_DVD,
  A_HALFD1,
  A_LOAD,
  A_SAVE,
  A_PREVIEW,
  A_PARTIAL,
  A_SCRIPT,
  A_DOUBLECLICK,
  
};

//___________________________________________
extern FILTER_ENTRY allfilters[MAX_FILTER];
extern uint32_t nb_video_filter;
extern FILTER videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;
extern ADM_Composer *video_body;
// display all currently activated filters
//_______________________________________

extern GtkWidget		*create_filterMain (void);
extern GtkListStore 		*storeMainFilter;
static  GtkWidget *dialog;
static  GtkWidget *dialog2;
int GUI_handleFilter (void)
{

#ifndef HAVE_ENCODER
  return 0;
}
#else
//VF_FILTERS tag;

getLastVideoFilter(); // expand video to full size

  // sanity check
  if (nb_active_filter == 0)
    {
      nb_active_filter = 1;
      videofilters[0].filter = new AVDMVideoStreamNull (video_body,frameStart,frameEnd-frameStart);

    }


      dialog = create_filterMain ();
      gtk_register_dialog(dialog);
      #define CALLME(widget,action) gtk_signal_connect (GTK_OBJECT (lookup_widget(dialog,#widget)),"clicked", \
			GTK_SIGNAL_FUNC (on_action), (void *)action);

 	 gtk_widget_set_size_request (lookup_widget(dialog,"treeview1"), 400, 400);

	CALLME(buttonAdd		,A_ADD);
	CALLME(buttonRemove	,A_REMOVE);
	CALLME(buttonConfigure	,A_CONFIGURE);
	CALLME(buttonUp		,A_UP);
	CALLME(buttonDown		,A_DOWN);
	CALLME(buttonVCD		,A_VCD);
	CALLME(buttonSVCD	,A_SVCD);
	CALLME(buttonDVD		,A_DVD);
	CALLME(buttonSave		,A_SAVE);
	CALLME(buttonLoad		,A_LOAD);
	CALLME(buttonPreview	,A_PREVIEW);
	CALLME(buttonPartial	,A_PARTIAL);
	CALLME(buttonHaldD1	,A_HALFD1);
	CALLME(buttonScript	,A_SCRIPT);
	// Add double click
	gtk_signal_connect (GTK_OBJECT (lookup_widget(dialog,"treeview1")),
				"row-activated", 
				GTK_SIGNAL_FUNC (on_action), 
				(void *)A_DOUBLECLICK);

	updateFilterList();


     gtk_dialog_run(GTK_DIALOG(dialog));
     gtk_unregister_dialog(dialog);
     gtk_widget_destroy(dialog);

  return 1;
  }

 // configure button was pushed
//
// Add buton clicked, pop up new window and update list
//______________________________________________________
void  on_action (GtkButton * button, gpointer user_data)
{
int dummy;
gui_act action;
uint32_t action_parameter;
VF_FILTERS tag=VF_INVALID;


UNUSED_ARG(button);


dummy=(int)user_data;
 action=(gui_act)dummy;

	action_parameter=0;
	if(nb_active_filter>1)
 		if( getSelectionNumber(nb_active_filter-1,lookup_widget(dialog,"treeview1")  , storeMainFilter,&action_parameter))
			action_parameter++;

  switch (action)
	    {
	    case A_SCRIPT:
	    		GUI_FileSelWrite ("Save as script", filterSaveScript);
	    		break;
	    case A_DOUBLECLICK:
	    		printf("Double clicked..");
			break;
	      // Add
	    case A_ADD:
	      tag = new_filter ();
	      // Raise back the old window
	      gtk_widget_grab_focus(dialog);
	      //
	      if (tag != VF_INVALID)
		{
		CONFcouple *couple;
		videofilters[nb_active_filter].filter=filterCreateFromTag( tag,NULL,videofilters[nb_active_filter - 1].filter) ;
		videofilters[nb_active_filter].filter->configure (videofilters[nb_active_filter - 1].filter);
		videofilters[nb_active_filter].filter->getCoupledConf(&couple);


		  videofilters[nb_active_filter].tag = tag;
		  videofilters[nb_active_filter].conf = couple;
		  ;
		  nb_active_filter++;
		  updateFilterList();
		   //gdk_window_raise(dialog->window);
		}
	      break;
	    case A_VCD:
	      setVCD ();
	      	updateFilterList();
	      break;
	    case A_SVCD:
	      setSVCD ();
	      	updateFilterList();
	      break;
	    case A_DVD:
	      setDVD ();
	      	updateFilterList();
	      break;
 	    case A_HALFD1:
	      	setHalfD1 ();
	      	updateFilterList();
	      break;
	    default:
	    case A_CONFIGURE:
//	      void *p;
	      if (action_parameter)
		{
		    if(videofilters[action_parameter].filter->
		    configure (videofilters[action_parameter - 1].filter))
		    {

		    	CONFcouple *couple;
			videofilters[action_parameter].filter->getCoupledConf(&couple);
		      	videofilters[action_parameter].conf = couple;
//		      	updateVideoFilters ();
			getFirstVideoFilter();
			updateFilterList();
		    }
		    // Raise back dialog
		    //gtk_window_set_focus(GTK_WINDOW(dialog));
		    //gdk_window_raise(dialog->window);
		}
	      break;
	    case A_PARTIAL:

	      if (action_parameter)
		{
		  AVDMGenericVideoStream *replace;
		  CONFcouple *conf;

		  conf=videofilters[action_parameter].conf;
                  if(videofilters[action_parameter].tag==VF_PARTIAL) // cannot recurse
                  {
                        GUI_Alert("Cannot partial-ize a partial filter !");
                  }
                  else
                  {
		      replace =new ADMVideoPartial (	videofilters[action_parameter - 1].filter,
										 videofilters[action_parameter].tag,
					 					conf);
		      replace->configure (videofilters[action_parameter - 1].filter);
		      delete videofilters[action_parameter].filter;
		      if(conf) delete conf;
		      videofilters[action_parameter].filter = replace;
		      replace->getCoupledConf(&conf);
		      videofilters[action_parameter].conf = conf;
		      videofilters[action_parameter].tag = VF_PARTIAL;
//		  updateVideoFilters ();
			getFirstVideoFilter();
			updateFilterList();
                  }
		}
	      break;
	    case A_UP:
	      if (action_parameter > 1)	// Select file
		{
		  // swap action parameter & action parameter -1
		  FILTER tmp;
		  memcpy (&tmp, &videofilters[action_parameter - 1],
			  sizeof (FILTER));
		  memcpy (&videofilters[action_parameter - 1],
			  &videofilters[action_parameter], sizeof (FILTER));
		  memcpy (&videofilters[action_parameter], &tmp,
			  sizeof (FILTER));
		  //updateVideoFilters ();
		  getFirstVideoFilter();
		  // select action_parameter -1
		  updateFilterList();

		  setSelectionNumber(nb_active_filter-1, lookup_widget(dialog,"treeview1")  ,
		 		 storeMainFilter,action_parameter-2);

		}
	      break;
	    case A_DOWN:
	      if (((int) action_parameter < (int) (nb_active_filter - 1))
	          &&(action_parameter)
	      		)	// Select file
		{
		  // swap action parameter & action parameter -1
		  FILTER tmp;
		  memcpy (&tmp, &videofilters[action_parameter + 1],
			  sizeof (FILTER));
		  memcpy (&videofilters[action_parameter + 1],
			  &videofilters[action_parameter], sizeof (FILTER));
		  memcpy (&videofilters[action_parameter], &tmp,
			  sizeof (FILTER));
		 // updateVideoFilters ();
		 getFirstVideoFilter();
		updateFilterList();
		  setSelectionNumber(nb_active_filter-1, lookup_widget(dialog,"treeview1")  ,
		 		 storeMainFilter,action_parameter);

		}
	      break;

	    case A_REMOVE:
//	        CONFcouple *couple;
		VF_FILTERS tag;
		AVDMGenericVideoStream *old;
	      if (action_parameter > 0)
		{
		 // we store the one we will delete
		 if(videofilters[action_parameter].conf)
		 {
			delete videofilters[action_parameter].conf;
			videofilters[action_parameter].conf=NULL;
		 }
		  // recreate derivated filters
		  for (uint32_t i = action_parameter + 1;   i < nb_active_filter; i++)
		    {
		      old = videofilters[i - 1].filter;
		      tag = videofilters[i].tag;

		      videofilters[i - 1].filter = filterCreateFromTag (tag,
									videofilters [i].conf,
									videofilters [i - 2].filter);

		      videofilters[i - 1].conf = videofilters[i].conf;
		      videofilters[i - 1].tag = tag;
		      delete old;

		    }
		  delete videofilters[nb_active_filter - 1].filter;
		  videofilters[nb_active_filter - 1].filter=NULL;
		  nb_active_filter--;

		}
	      updateFilterList();
	      break;
	    case A_DONE:

	      break;
	    case A_PREVIEW:

	      if (action_parameter > 0)
		{
		  AVDMGenericVideoStream *in;
		  uint32_t w, h, l, f;
		  ADMImage *data;
		  extern uint32_t curframe;
		  in = videofilters[action_parameter].filter;
		  w = in->getInfo ()->width;
		  h = in->getInfo ()->height;
		  data = new ADMImage(w,h); //(uint8_t *) malloc (w * h * 3);
		  ADM_assert (data);
		  in->getFrameNumberNoAlloc (curframe, &l, data, &f);
		  
		  GUI_PreviewInit (w, h);
		  GUI_PreviewRun(data->data);
		  GUI_PreviewEnd ();
		  ADM_dealloc (data);
		}

	      break;
	    case A_LOAD:
#ifdef USE_LIBXML2
	      GUI_FileSelRead ("Load set of filters", filterLoadXml);
#else
      		GUI_FileSelRead ("Load set of filters", filterLoad);
#endif
		updateFilterList();
	      break;
	    case A_SAVE:
	      if (nb_active_filter < 2)
		{
		  GUI_Alert ("Nothing to save");
		}
	      else
#ifdef USE_LIBXML2
		GUI_FileSelWrite ("Save set of filters", filterSaveXml);
#else
		GUI_FileSelWrite ("Save set of filters", filterSave);

#endif
	      break;
#if 0
	    default:
	    	printf("Unknown action :%d, action param %d\n",action,action_parameter);
	      ADM_assert (0);
#endif	      
	    }
	//updateFilterList();

}
//
extern GtkWidget	*create_dialogList (void);
extern GtkListStore 	*storeFilterList;

/**
 	Pop-up a list of all available filters
	Return the tag identifying the filter selected

*/
VF_FILTERS new_filter (void)
{
  
//  GtkObject *selected;
  int position=0;
  uint32_t sel,max=0;
  VF_FILTERS tag=VF_INVALID;
GtkWidget  *tree;
GtkTreeIter   iter;
uint8_t ret=0;


  dialog2 = create_dialogList ();
	gtk_register_dialog(dialog2);
  tree=lookup_widget(dialog2,"treeview2");
  gtk_widget_set_size_request (tree, 400, 400);
  for (uint32_t i = 0; i < nb_video_filter; i++)
    {
     if(allfilters[i].viewable)
     {
   		gtk_list_store_append (storeFilterList, &iter);
     		gtk_list_store_set (storeFilterList, &iter,0,allfilters[i].name,-1);
		max++;
     }
    }
        // Add double click
	gtk_signal_connect (GTK_OBJECT (lookup_widget(dialog2,"treeview2")),
				"row-activated", 
				GTK_SIGNAL_FUNC (on_action_double_click), 
				(void *)dialog);
	if(gtk_dialog_run(GTK_DIALOG(dialog2))==GTK_RESPONSE_OK)
	{
			if(( ret=getSelectionNumber(max,tree  , storeFilterList,&sel)))
			{
					uint32_t  idx=0;
					for(unsigned int i=0;i<nb_video_filter;i++)
					{
						if((sel==idx)&&(allfilters[i].viewable==1))
						{
							printf("\n position : %d tag :%d idx:%lu\n",position,tag,idx);
							tag=allfilters[i].tag;
							i=nb_video_filter;
						}
						else
     							if(allfilters[i].viewable) idx++;
					}
			}
	}
	gtk_unregister_dialog(dialog2);
	gtk_widget_destroy(dialog2);

	return tag;
}
void updateFilterList( void )
{
GtkTreeIter   iter;
  gtk_list_store_clear(storeMainFilter);
  for (uint32_t i = 1; i < nb_active_filter; i++)
    {
    	gtk_list_store_append (storeMainFilter, &iter);
     	gtk_list_store_set (storeMainFilter, &iter,0,videofilters[i].filter->printConf (),-1);

    }

}
void  on_action_double_click (GtkButton * button, gpointer user_data)
{
	
    		gtk_dialog_response(GTK_DIALOG(dialog2), GTK_RESPONSE_OK);
    		
}

#endif
