/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
//#include "prototype.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_toolkit/filesel.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_filter/vidVCD.h"
//___________________________________________
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
  A_CLOSE,
  A_DOUBLECLICK,
  A_END
};
//___________________________________________
extern AVDMGenericVideoStream *filterCreateFromTag (VF_FILTERS tag,
						    CONFcouple * conf,
						    AVDMGenericVideoStream *
						    in);
extern const char  *filterGetNameFromTag(VF_FILTERS tag);
//___________________________________________
extern FILTER_ENTRY allfilters[MAX_FILTER];
extern uint32_t nb_video_filter;
extern FILTER videofilters[MAX_FILTER];
extern uint32_t nb_active_filter;
extern ADM_Composer *video_body;
//___________________________________________
static gulong row_inserted_id;
static gulong row_deleted_id;

static void on_treeview0_row_deleted(GtkTreeModel *treemodel, GtkTreePath *arg1, gpointer user_data);
static void on_treeview0_row_inserted(GtkTreeModel *treemodel, GtkTreePath *arg1, GtkTreeIter *arg2, gpointer user_data);
static void on_treeview1_size_allocate(GtkWidget *widget, GtkAllocation *allocation, GtkCellRenderer *cell);
static void on_action (gui_act action);
static void on_action_double_click (GtkButton * button, gpointer user_data);
static void on_action_double_click_1 (GtkButton * button, gpointer user_data);
static void updateFilterList (void);
static VF_FILTERS getFilterFromSelection (void);
static void wrapToolButton(GtkWidget * wid, gpointer user_data);
//___________________________________________
#define NB_TREE 9
static  uint32_t max = 0;
static  GtkWidget *trees[NB_TREE];
static  GtkListStore *stores[NB_TREE];
static  GtkTreeViewColumn *columns[NB_TREE];
static  GtkCellRenderer *renderers[NB_TREE];
static  int startFilter[NB_TREE];
//___________________________________________
static GtkWidget *createFilterDialog (void);
extern GtkWidget *create_dialog1 (void);
static GtkWidget *dialog = 0;
//___________________________________________


// Open the main filter dialog and call the handlers
// if needed.
int
GUI_handleVFilter (void)
{

#ifndef HAVE_ENCODER
    return 0;
}
#else
    getLastVideoFilter ();	// expand video to full size

    // sanity check
    if (nb_active_filter == 0)
    {
        nb_active_filter = 1;
        videofilters[0].filter =
        new AVDMVideoStreamNull (video_body, frameStart,
				 frameEnd - frameStart);
    }
        dialog = createFilterDialog();
        GdkWMDecoration decorations=(GdkWMDecoration)0;
        gtk_widget_realize(dialog);
        gdk_window_set_decorations(dialog->window, (GdkWMDecoration)(GDK_DECOR_ALL | GDK_DECOR_MINIMIZE));
        GdkScreen* screen = gdk_screen_get_default();
        gint width = gdk_screen_get_width(screen);
        if(width>=1024)
            gtk_window_set_default_size(GTK_WINDOW(dialog), 900, 600);
        updateFilterList ();
        gtk_register_dialog (dialog);
        //gtk_widget_show (dialog);
        int r;
        while(1)
        {
          r=gtk_dialog_run(GTK_DIALOG(dialog));
          if(r>A_BEGIN && r<A_END)
          {
            on_action((gui_act)r);
          }
          else break;
        };
        gtk_unregister_dialog (dialog);
        gtk_widget_destroy(dialog);
        dialog=NULL;
        
    return 1;
    
}

// gtk_dialog_add_action_widget seems buggy for toolbar button
// workaround it...
void
wrapToolButton(GtkWidget * wid, gpointer user_data)
{
        gui_act action;
#ifdef ARCH_64_BITS
#define TPE long long int
	long long int dummy;
#else
        int dummy;
#define TPE int
#endif

        dummy=(TPE)user_data;

        action=(gui_act) dummy;
        on_action(action);
}
//
// One of the button of the main dialog was pressed
// Retrieve also the associated filter and handle
// the action
//______________________________________________________
void on_action (gui_act action)
{
    uint32_t action_parameter;
    VF_FILTERS tag = VF_INVALID;

    action_parameter = 0;
    if (nb_active_filter > 1)
        if (getSelectionNumber(nb_active_filter - 1,
                                WID(treeview0),
                                stores[0],
                                &action_parameter))
            action_parameter++;

    switch (action)
    {

    case A_ADD:
        tag = getFilterFromSelection();
        if (tag == VF_INVALID) break;
        CONFcouple *coup;
        videofilters[nb_active_filter].filter =
            filterCreateFromTag (tag, NULL, videofilters[nb_active_filter - 1].filter);
        if(!videofilters[nb_active_filter].filter->
                    configure (videofilters[nb_active_filter - 1].filter))
        {
            delete videofilters[nb_active_filter].filter;
            break;
        }
        videofilters[nb_active_filter].filter->getCoupledConf (&coup);
        videofilters[nb_active_filter].tag = tag;
        videofilters[nb_active_filter].conf = coup;
        nb_active_filter++;
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], nb_active_filter-2);
        break;

    case A_VCD:
        setVCD ();
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], nb_active_filter-2);
        break;

    case A_SVCD:
        setSVCD ();
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], nb_active_filter-2);
        break;

    case A_DVD:
        setDVD ();
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], nb_active_filter-2);
        break;

    case A_HALFD1:
        setHalfD1 ();
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], nb_active_filter-2);
        break;

    default:
    case A_DOUBLECLICK:
        printf ("Double clicked..");
    case A_CONFIGURE:
        if(!action_parameter) break;
        if(!videofilters[action_parameter].filter->
            configure (videofilters[action_parameter - 1].filter)) break;
        CONFcouple *couple;
        videofilters[action_parameter].filter->getCoupledConf (&couple);
        videofilters[action_parameter].conf = couple;
        getFirstVideoFilter ();
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], action_parameter-1);
        break;

    case A_PARTIAL:
        if (!action_parameter) break;
        AVDMGenericVideoStream *replace;
        CONFcouple *conf;
        conf = videofilters[action_parameter].conf;
        if (videofilters[action_parameter].tag == VF_PARTIAL)	// cannot recurse
        {
            GUI_Error_HIG (_("The filter is already partial"), NULL);
            break;
	    }
        replace =
		new ADMVideoPartial (videofilters[action_parameter - 1].
				     filter,
				     videofilters[action_parameter].tag,
				     conf);
        if(replace->configure (videofilters[action_parameter - 1].filter))
        {
            delete videofilters[action_parameter].filter;
			if (conf) delete conf;
			videofilters[action_parameter].filter = replace;
			replace->getCoupledConf (&conf);
			videofilters[action_parameter].conf = conf;
			videofilters[action_parameter].tag = VF_PARTIAL;
			getFirstVideoFilter ();
			updateFilterList ();
			setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], action_parameter-1);
        }
        else delete replace;
        break;

    case A_UP:
        if (action_parameter < 2) break;
        // swap action parameter & action parameter -1
        FILTER tmp;
        memcpy (&tmp, &videofilters[action_parameter - 1], sizeof (FILTER));
        memcpy (&videofilters[action_parameter - 1],
            &videofilters[action_parameter], sizeof (FILTER));
        memcpy (&videofilters[action_parameter], &tmp, sizeof (FILTER));
        getFirstVideoFilter ();
        // select action_parameter -1
        updateFilterList ();
        setSelectionNumber (nb_active_filter - 1,
			      WID(treeview0),
			      stores[0], action_parameter - 2);
        break;

    case A_DOWN:
        if (((int) action_parameter < (int) (nb_active_filter - 1)) && (action_parameter))
        {
            // swap action parameter & action parameter -1
            FILTER tmp;
            memcpy (&tmp, &videofilters[action_parameter + 1], sizeof (FILTER));
            memcpy (&videofilters[action_parameter + 1],
                        &videofilters[action_parameter], sizeof (FILTER));
            memcpy (&videofilters[action_parameter], &tmp, sizeof (FILTER));
            getFirstVideoFilter ();
            updateFilterList ();
            setSelectionNumber (nb_active_filter - 1,
			      WID(treeview0),
			      stores[0], action_parameter);
        }
        break;

    case A_REMOVE:
		VF_FILTERS tag;
		AVDMGenericVideoStream *old;
		// we store the one we will delete
		if (action_parameter < 1) break;
		if (videofilters[action_parameter].conf)
		{
			delete videofilters[action_parameter].conf;
			videofilters[action_parameter].conf = NULL;
		}
		// recreate derivated filters
		for (uint32_t i = action_parameter + 1; i < nb_active_filter; i++)
	    {
			delete videofilters[i - 1].filter;
			videofilters[i - 1].filter = filterCreateFromTag(videofilters[i].tag,
															 videofilters[i].conf,
															 videofilters[i - 2].filter);
			videofilters[i - 1].conf = videofilters[i].conf;
			videofilters[i - 1].tag = videofilters[i].tag;
	    }
		delete videofilters[nb_active_filter - 1].filter;
		videofilters[nb_active_filter - 1].filter = NULL;
		nb_active_filter--;
        updateFilterList ();
        if(!setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], action_parameter-1))
            setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], action_parameter-2);
		break;

    case A_DONE:

        break;

    case A_PREVIEW:
        if (!action_parameter) break;
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
        GUI_PreviewShow(w,h,data->data);
        ADM_dealloc (data);
        break;

    case A_LOAD:
#ifdef USE_LIBXML2
        GUI_FileSelRead (_("Load set of filters"), filterLoadXml);
#else
        GUI_FileSelRead (_("Load set of filters"), filterLoad);
#endif
        updateFilterList ();
        setSelectionNumber(nb_active_filter-1, WID(treeview0), stores[0], 0);
        break;
    case A_CLOSE:
        //gtk_widget_destroy(dialog);
      gtk_signal_emit_by_name(GTK_OBJECT(dialog),"delete-event");
        
        break;
    case A_SAVE:
        if (nb_active_filter < 2)
        {
            GUI_Error_HIG (_("Nothing to save"), NULL);
        }
        else
#ifdef USE_LIBXML2
            GUI_FileSelWrite (_("Save set of filters"), filterSaveXml);
#else
            GUI_FileSelWrite (_("Save set of filters"), filterSave);
#endif
        break;
#if 0
    default:
        printf ("Unknown action :%d, action param %d\n", action, action_parameter);
        ADM_assert (0);
#endif
    } //end of switch
}
/*
 	Pop-up a list of all available filters
	Return the tag identifying the filter selected
*/
VF_FILTERS
getFilterFromSelection (void)
{
    uint32_t sel = 0;
	uint8_t ret = 0;
    VF_FILTERS tag = VF_INVALID;
    // 1- identify the current tab/treeview we are in
    int page = gtk_notebook_get_current_page(GTK_NOTEBOOK(lookup_widget(dialog,"notebook1")));
    // then get the selection
    page++;
    if ((ret = getSelectionNumber (max, trees[page], stores[page], &sel)))
	{
        tag = allfilters[sel+startFilter[page]].tag;
	}
    return tag;
}

GtkWidget *
createFilterDialog (void)
{
    dialog = create_dialog1();

    //connect toolbar
#define CALLME_TOOLBAR(x,y) gtk_signal_connect(GTK_OBJECT(WID(x)),"clicked",  GTK_SIGNAL_FUNC(wrapToolButton), (void *) y);
#define CALLME(x,y) gtk_dialog_add_action_widget (GTK_DIALOG (dialog), WID(x), y)

	//CALLME (toolbuttonAdd, A_ADD);
        CALLME (buttonRemove,		A_REMOVE);
        CALLME (buttonProperties,	A_CONFIGURE);
        CALLME (buttonUp, 		A_UP);
        CALLME (buttonDown, 		A_DOWN);
        CALLME (buttonPreview, 		A_PREVIEW);
        CALLME (buttonPartial, 		A_PARTIAL);
        CALLME (buttonAdd, 		A_ADD);
        
        CALLME_TOOLBAR (toolbuttonHalfD1, 	A_HALFD1);
        CALLME_TOOLBAR (toolbuttonScript, 	A_SCRIPT);
        CALLME_TOOLBAR (toolbuttonVCD, 		A_VCD);
        CALLME_TOOLBAR (toolbuttonSVCD, 	A_SVCD);
        CALLME_TOOLBAR (toolbuttonDVD, 		A_DVD);
        CALLME_TOOLBAR (toolbuttonSave, 	A_SAVE);
        CALLME_TOOLBAR (toolbuttonScript, 	A_SAVE);
        CALLME_TOOLBAR (toolbuttonOpen, 	A_LOAD);
        

    //create treeviews
    trees[0]=lookup_widget(dialog,"treeview0");
    stores[0]=gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_POINTER);
    #define LOOK(x) {trees[x]=lookup_widget(dialog,"treeview"#x);\
                    stores[x]=gtk_list_store_new (1, G_TYPE_STRING);}
    LOOK(1);
    LOOK(2);
    LOOK(3);
    LOOK(4);
    LOOK(5);
    LOOK(6);
    LOOK(7);
    LOOK(8);

    //load stores with filter names, get start filter for each page
    char *str=NULL;
    GtkTreeIter iter;
    int current_tree=0;
    for (uint32_t i = 0; i < nb_video_filter; i++)
    {
        if (allfilters[i].viewable==1)
        {
		
		 str = g_strconcat(
				 "<span weight=\"bold\">", allfilters[i].name, "</span>\n",
                 		//	"", allfilters[i].description, "", NULL);
                 			"<span size=\"smaller\">", allfilters[i].description, "</span>", NULL);
				
            gtk_list_store_append (stores[current_tree], &iter);
            gtk_list_store_set (stores[current_tree], &iter, 0, str ,-1);
            g_free(str);
            max++;
        }else
        {
                current_tree++;
                if(current_tree>=NB_TREE) break;
                startFilter[current_tree]=i+1;
        }
    }

    //setup treeviews
    for(int i=0;i<NB_TREE;i++)
    {
        renderers[i] = gtk_cell_renderer_text_new();
   		columns[i] = gtk_tree_view_column_new_with_attributes (
                            "",
                            renderers[i],
                            "markup", (GdkModifierType) 0,
                            NULL);
		gtk_cell_renderer_text_set_fixed_height_from_font
			(GTK_CELL_RENDERER_TEXT(renderers[i]), 3);
        g_object_set(renderers[i], "wrap-width", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW (trees[i]), columns[i]);
        gtk_tree_view_set_model(GTK_TREE_VIEW(trees[i]),GTK_TREE_MODEL (stores[i]));

		// Add double click, 0 is active filter tree
        if(i)
        	g_signal_connect (G_OBJECT(trees[i]),
                      "row-activated",
                      G_CALLBACK(on_action_double_click),
                      (void *) dialog);
  		else
  		{
            gtk_tree_view_set_reorderable(GTK_TREE_VIEW(trees[i]), true);
  			g_signal_connect(G_OBJECT(WID(treeview0)),
  						"row-activated",
        				G_CALLBACK(on_action_double_click_1),
        				(void *)NULL);
   			row_inserted_id=g_signal_connect(G_OBJECT(stores[i]),
  						"row-inserted",
        				G_CALLBACK(on_treeview0_row_inserted),
        				(void *)NULL);
   			row_deleted_id=g_signal_connect(G_OBJECT(stores[i]),
  						"row-deleted",
        				G_CALLBACK(on_treeview0_row_deleted),
        				(void *)NULL);
  		}
        g_signal_connect(G_OBJECT(trees[i]),
                      "size-allocate",
                      G_CALLBACK(on_treeview1_size_allocate),
                      renderers[i]);
    }
    return dialog;
}

void
updateFilterList (void)
{
    g_signal_handler_block(stores[0], row_inserted_id);
    g_signal_handler_block(stores[0], row_deleted_id);
    GtkTreeIter iter;
    char *str;
    VF_FILTERS fil;
    gtk_list_store_clear (stores[0]);
    for (uint32_t i = 1; i < nb_active_filter; i++)
    {
        gtk_list_store_append (stores[0], &iter);
        fil=videofilters[i].tag;
        str = g_strconcat("<span  weight=\"bold\">",
                            filterGetNameFromTag(fil),
                            "</span>\n",
                            "<span size=\"smaller\">", videofilters[i].filter->printConf (), "</span>",  NULL);
        gtk_list_store_set (stores[0], &iter,
                            0, str,
                            1, videofilters[i].tag,
                            2, videofilters[i].conf,
                            -1);
        g_free(str);
    }
    g_signal_handler_unblock(stores[0], row_inserted_id);
    g_signal_handler_unblock(stores[0], row_deleted_id);
}

void
on_action_double_click (GtkButton * button, gpointer user_data)
{
    on_action(A_ADD);
}

void
on_action_double_click_1 (GtkButton * button, gpointer user_data)
{
    on_action(A_DOUBLECLICK);
}

void
on_treeview1_size_allocate(GtkWidget *widget, GtkAllocation *allocation, GtkCellRenderer *cell)
{
    g_object_set(cell,
              "wrap-width", allocation->width-8,
              NULL);
}

void
on_treeview0_row_deleted(GtkTreeModel *treemodel, GtkTreePath *arg1, gpointer user_data)
{
    GtkTreeIter iter;
    VF_FILTERS				tag;
	CONFcouple				*conf = 0;

        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(stores[0]), &iter);
		AVDMGenericVideoStream 	*prevfilter = videofilters[0].filter;
		for (uint32_t i = 1; i < nb_active_filter; i++)
	    {
            gtk_tree_model_get (GTK_TREE_MODEL(stores[0]), &iter,
                            1, &tag,
                            2, &conf,
                            -1);
			delete videofilters[i].filter;
			videofilters[i].filter = filterCreateFromTag(tag,
                                                        conf,
                                                        prevfilter);
			videofilters[i].conf = conf;
			videofilters[i].tag = tag;
			prevfilter = videofilters[i].filter;
            gtk_tree_model_iter_next (GTK_TREE_MODEL(stores[0]), &iter);
	    }
    updateFilterList();
    //on_action(A_REORDERED);
}
void
on_treeview0_row_inserted(GtkTreeModel *treemodel, GtkTreePath *arg1, GtkTreeIter *arg2, gpointer user_data)
{


}

#endif
