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
//__________________________________________

extern AVDMGenericVideoStream *filterCreateFromTag (VF_FILTERS tag,
						    CONFcouple * conf,
						    AVDMGenericVideoStream *
						    in);

extern const char  *filterGetNameFromTag(VF_FILTERS tag);
typedef enum gui_act
{
  A_BEGIN = 10,
  A_ADD,
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
  A_END
};
//_______________________________________


static VF_FILTERS new_filter (void);

static void on_action (gui_act action);
static void on_action_double_click (GtkButton * button, gpointer user_data);
static void on_action_double_click_1 (GtkButton * button, gpointer user_data);
static void updateFilterList (void);
static void wrapToolButton(GtkWidget * wid, gpointer user_data);
//___________________________________________
extern FILTER_ENTRY allfilters[MAX_FILTER];
extern uint32_t nb_video_filter;
extern FILTER videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;
extern ADM_Composer *video_body;
// display all currently activated filters
//_______________________________________

extern GtkWidget *create_filterMain (void);

static GtkWidget *dialog;
static GtkWidget *dialog2;
static GtkListStore *store;
static GtkTreeViewColumn *column;
static GtkCellRenderer *renderer;

/*
        Open the main filter dialog and call the handlers
        if needed.

*/
int
GUI_handleFilter (void)
{

#ifndef HAVE_ENCODER
  return 0;
}
#else
//VF_FILTERS tag;

  getLastVideoFilter ();	// expand video to full size

  // sanity check
  if (nb_active_filter == 0)
    {
      nb_active_filter = 1;
      videofilters[0].filter =
	new AVDMVideoStreamNull (video_body, frameStart,
				 frameEnd - frameStart);

    }
  dialog = create_filterMain ();
  store=gtk_list_store_new (1, G_TYPE_STRING);
  gtk_tree_view_set_model(GTK_TREE_VIEW(WID(treeview2)),GTK_TREE_MODEL (store));
  gtk_tree_view_columns_autosize(GTK_TREE_VIEW(WID(treeview2)));
  //gtk_widget_set_size_request (WID(treeview2), 400, 400);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("", renderer,
                                                      "markup", (GdkModifierType) 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (WID(treeview2)), column);
  gtk_tree_view_column_set_spacing(column,26);
//#define CALLME(x,y)   gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x),y)

#define CALLME(x,y) gtk_signal_connect(GTK_OBJECT(WID(x)),"clicked",  GTK_SIGNAL_FUNC(wrapToolButton), (void *) y);

  CALLME (toolbuttonAdd, A_ADD);
  CALLME (toolbuttonRemove, A_REMOVE);
  CALLME (toolbuttonProperties, A_CONFIGURE);
  CALLME (toolbuttonUp, A_UP);
  CALLME (toolbuttonDown, A_DOWN);
  CALLME (toolbuttonVCD, A_VCD);
  CALLME (toolbuttonSVCD, A_SVCD);
  CALLME (toolbuttonDVD, A_DVD);
  CALLME (toolbuttonSave, A_SAVE);
  CALLME (toolbuttonOpen, A_LOAD);
  CALLME (toolbuttonPreview, A_PREVIEW);
  CALLME (toolbuttonPartial, A_PARTIAL);
  CALLME (toolbuttonHD1, A_HALFD1);
  CALLME (toolbutton13, A_SCRIPT);

  // Add double click
  gtk_signal_connect (GTK_OBJECT (WID(treeview2)), "row-activated", 
        GTK_SIGNAL_FUNC (on_action_double_click_1),(void *)NULL);

  //      gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(treeview1),A_DOUBLECLICK);

  gtk_widget_set_size_request (WID(treeview2), 400, 400);

  updateFilterList ();
  gtk_register_dialog (dialog);
  gtk_widget_show (dialog);
  
  gui_act ac;
  int run = 1, reply;
  while (run)
    {
      reply = gtk_dialog_run (GTK_DIALOG (dialog));
      //printf("Action : %d\n",reply);
      if (reply > A_BEGIN && reply < A_END)
	{
	  ac = (gui_act) reply;
	  on_action (ac);
	}
      else
	switch (reply)
	  {
          case GTK_RESPONSE_DELETE_EVENT:
	  case GTK_RESPONSE_CLOSE:
	  case GTK_RESPONSE_APPLY:
	  case GTK_RESPONSE_OK:
	  case GTK_RESPONSE_CANCEL:
	    run = 0;
	    break;
	  default:
	    ADM_assert (0);
	  }
    }
  gtk_unregister_dialog (dialog);
  gtk_widget_destroy (dialog);

  return 1;
}

// gtk_dialog_add_action_widget seems buggy for toolbar button
// workaround it...
void wrapToolButton(GtkWidget * wid, gpointer user_data)
{
        gui_act action;
#ifdef ARCH_X86_64
#define TPE long long int
	long long int dummy;
#else	
        int dummy;
#define TPE int
#endif

        dummy=(TPE)user_data;

        action=(gui_act) dummy;
        gtk_dialog_response(GTK_DIALOG(dialog),action);
}
//
// One of the button of the main dialog was pressed
// Retrieve also the associated filter and handle
// the action
//______________________________________________________
void
on_action (gui_act action)
{
  uint32_t action_parameter;
  VF_FILTERS tag = VF_INVALID;

  action_parameter = 0;
  if (nb_active_filter > 1)
    if (getSelectionNumber
	(nb_active_filter - 1, WID(treeview2),
	 store, &action_parameter))
      action_parameter++;

  switch (action)
    {
    case A_SCRIPT:
      GUI_FileSelWrite ("Save as script", filterSaveScript);
      break;
      // Add
    case A_ADD:
      tag = new_filter ();
      // Raise back the old window
      gtk_widget_grab_focus (dialog);
      //
      if (tag != VF_INVALID)
	{
	  CONFcouple *couple;
	  videofilters[nb_active_filter].filter =
	    filterCreateFromTag (tag, NULL,
				 videofilters[nb_active_filter - 1].filter);
	  videofilters[nb_active_filter].filter->
	    configure (videofilters[nb_active_filter - 1].filter);
	  videofilters[nb_active_filter].filter->getCoupledConf (&couple);


	  videofilters[nb_active_filter].tag = tag;
	  videofilters[nb_active_filter].conf = couple;
	  ;
	  nb_active_filter++;
	  updateFilterList ();
	  //gdk_window_raise(dialog->window);
	}
      break;
    case A_VCD:
      setVCD ();
      updateFilterList ();
      break;
    case A_SVCD:
      setSVCD ();
      updateFilterList ();
      break;
    case A_DVD:
      setDVD ();
      updateFilterList ();
      break;
    case A_HALFD1:
      setHalfD1 ();
      updateFilterList ();
      break;
    default:
    case A_DOUBLECLICK:
      printf ("Double clicked..");
    //  break;
    
    case A_CONFIGURE:
//            void *p;
      if (action_parameter)
	{
	  if (videofilters[action_parameter].filter->
	      configure (videofilters[action_parameter - 1].filter))
	    {

	      CONFcouple *couple;
	      videofilters[action_parameter].filter->getCoupledConf (&couple);
	      videofilters[action_parameter].conf = couple;
//                      updateVideoFilters ();
	      getFirstVideoFilter ();
	      updateFilterList ();
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

	  conf = videofilters[action_parameter].conf;
	  if (videofilters[action_parameter].tag == VF_PARTIAL)	// cannot recurse
	    {
	      GUI_Error_HIG ("The filter is already partial", NULL);
	    }
	  else
	    {
	      replace =
		new ADMVideoPartial (videofilters[action_parameter - 1].
				     filter,
				     videofilters[action_parameter].tag,
				     conf);
	      replace->configure (videofilters[action_parameter - 1].filter);
	      delete videofilters[action_parameter].filter;
	      if (conf)
		delete conf;
	      videofilters[action_parameter].filter = replace;
	      replace->getCoupledConf (&conf);
	      videofilters[action_parameter].conf = conf;
	      videofilters[action_parameter].tag = VF_PARTIAL;
//                updateVideoFilters ();
	      getFirstVideoFilter ();
	      updateFilterList ();
	    }
	}
      break;
    case A_UP:
      if (action_parameter > 1)	// Select file
	{
	  // swap action parameter & action parameter -1
	  FILTER tmp;
	  memcpy (&tmp, &videofilters[action_parameter - 1], sizeof (FILTER));
	  memcpy (&videofilters[action_parameter - 1],
		  &videofilters[action_parameter], sizeof (FILTER));
	  memcpy (&videofilters[action_parameter], &tmp, sizeof (FILTER));
	  //updateVideoFilters ();
	  getFirstVideoFilter ();
	  // select action_parameter -1
	  updateFilterList ();

	  setSelectionNumber (nb_active_filter - 1,
			      WID(treeview2),
			      store, action_parameter - 2);

	}
      break;
    case A_DOWN:
      if (((int) action_parameter < (int) (nb_active_filter - 1)) && (action_parameter))	// Select file
	{
	  // swap action parameter & action parameter -1
	  FILTER tmp;
	  memcpy (&tmp, &videofilters[action_parameter + 1], sizeof (FILTER));
	  memcpy (&videofilters[action_parameter + 1],
		  &videofilters[action_parameter], sizeof (FILTER));
	  memcpy (&videofilters[action_parameter], &tmp, sizeof (FILTER));
	  // updateVideoFilters ();
	  getFirstVideoFilter ();
	  updateFilterList ();
	  setSelectionNumber (nb_active_filter - 1,
			      WID(treeview2),
			      store, action_parameter);

	}
      break;

    case A_REMOVE:
//              CONFcouple *couple;
      VF_FILTERS tag;
      AVDMGenericVideoStream *old;
      if (action_parameter > 0)
	{
	  // we store the one we will delete
	  if (videofilters[action_parameter].conf)
	    {
	      delete videofilters[action_parameter].conf;
	      videofilters[action_parameter].conf = NULL;
	    }
	  // recreate derivated filters
	  for (uint32_t i = action_parameter + 1; i < nb_active_filter; i++)
	    {
	      old = videofilters[i - 1].filter;
	      tag = videofilters[i].tag;

	      videofilters[i - 1].filter = filterCreateFromTag (tag,
								videofilters
								[i].conf,
								videofilters[i
									     -
									     2].
								filter);

	      videofilters[i - 1].conf = videofilters[i].conf;
	      videofilters[i - 1].tag = tag;
	      delete old;

	    }
	  delete videofilters[nb_active_filter - 1].filter;
	  videofilters[nb_active_filter - 1].filter = NULL;
	  nb_active_filter--;

	}
      updateFilterList ();
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
	  data = new ADMImage (w, h);	//(uint8_t *) malloc (w * h * 3);
	  ADM_assert (data);
	  in->getFrameNumberNoAlloc (curframe, &l, data, &f);

	  GUI_PreviewInit (w, h);
	  GUI_PreviewRun (data->data);
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
      updateFilterList ();
      break;
    case A_SAVE:
      if (nb_active_filter < 2)
	{
	  GUI_Error_HIG ("Nothing to save", NULL);
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
      printf ("Unknown action :%d, action param %d\n", action,
	      action_parameter);
      ADM_assert (0);
#endif
    }
  //updateFilterList();

}
//
extern GtkWidget *create_dialogList (void);
extern GtkListStore *storeFilterList;

/**
 	Pop-up a list of all available filters
	Return the tag identifying the filter selected

*/
#define NB_TREE 7
VF_FILTERS new_filter (void)
{

//  GtkObject *selected;
  int position = 0;
  uint32_t sel, max = 0;
  VF_FILTERS tag = VF_INVALID;
  GtkWidget             *trees[NB_TREE];
  GtkListStore          *stores[NB_TREE];
  GtkTreeViewColumn     *columns[NB_TREE];
  GtkCellRenderer       *renderers[NB_TREE];
  int                   startFilter[NB_TREE];

  char *str=NULL;

  GtkTreeIter iter;
  uint8_t ret = 0;


  dialog2 = create_dialogList ();
  gtk_register_dialog (dialog2);
  #define LOOK(x){ trees[x-1]=lookup_widget(dialog2,"treeview1"#x);\
              stores[x-1]=gtk_list_store_new (1, G_TYPE_STRING);}
  
  LOOK(1);
  LOOK(2);
  LOOK(3);
  LOOK(4);
  LOOK(5);
  LOOK(6);
  LOOK(7);

  gtk_widget_set_size_request (dialog2, 600, 600);
  int current_tree=-1;
  for (uint32_t i = 0; i < nb_video_filter; i++)
    {
      if (allfilters[i].viewable==1)
	{
                  str = g_strconcat("<span size=\"larger\" weight=\"bold\">", 
                allfilters[i].name, "</span>\n",  
                
                allfilters[i].description, NULL);

                

	  gtk_list_store_append (stores[current_tree], &iter);
	  gtk_list_store_set (stores[current_tree], &iter, 0,str ,-1);
          g_free(str);
	  max++;
	}else 
        {
                current_tree++;
                if(current_tree>=NB_TREE) break;
                startFilter[current_tree]=i+1;
        }
    }
  // Add double click
  

// Attach
  for(int i=0;i<NB_TREE;i++)
   {
        gtk_tree_view_set_model(GTK_TREE_VIEW(trees[i]),GTK_TREE_MODEL (stores[i]));
        gtk_tree_view_columns_autosize(GTK_TREE_VIEW(trees[i]));

        renderers[i] = gtk_cell_renderer_text_new ();
        columns[i] = gtk_tree_view_column_new_with_attributes ("", renderers[i],
                                                      "markup", (GdkModifierType) 0,
                                                      NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (trees[i]), columns[i]);

        gtk_tree_view_column_set_spacing(columns[i],26);
        gtk_signal_connect (GTK_OBJECT (trees[i]),
                      "row-activated",
                      GTK_SIGNAL_FUNC (on_action_double_click),
                      (void *) dialog);
   }
  if (gtk_dialog_run (GTK_DIALOG (dialog2)) == GTK_RESPONSE_OK)
    {
        // 1- identify the current tab/treeview we are in
        int page;

                page=gtk_notebook_get_current_page(GTK_NOTEBOOK(lookup_widget(dialog2,"notebook1")));
        // then get the selection

      if ((ret = getSelectionNumber (max, trees[page], stores[page], &sel)))
	{
                tag = allfilters[sel+startFilter[page]].tag;
	}
    }
  gtk_unregister_dialog (dialog2);
  gtk_widget_destroy (dialog2);

  return tag;
}
void
updateFilterList (void)
{
GtkTreeIter iter;
char *str;
VF_FILTERS fil;
  gtk_list_store_clear (store);
  for (uint32_t i = 1; i < nb_active_filter; i++)
    {
                gtk_list_store_append (store, &iter);
                //str=videofilters[i].filter->printConf ();
                fil=videofilters[i].tag;
                

                str = g_strconcat("<span size=\"larger\" weight=\"bold\">", 
                filterGetNameFromTag(fil), "</span>\n",  
               
                videofilters[i].filter->printConf (), NULL);

                gtk_list_store_set (store, &iter, 0,str,-1);
                g_free(str);
    }

}
void
on_action_double_click (GtkButton * button, gpointer user_data)
{

  gtk_dialog_response (GTK_DIALOG (dialog2), GTK_RESPONSE_OK);

}
void on_action_double_click_1 (GtkButton * button, gpointer user_data)
{

  gtk_dialog_response (GTK_DIALOG (dialog), A_DOUBLECLICK);

}

#endif
