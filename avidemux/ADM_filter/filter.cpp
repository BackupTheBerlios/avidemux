/***************************************************************************
                          filter.cpp  -  description
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include "config.h"
#include "fourcc.h"
#include "avio.hxx"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
#include "avi_vars.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_PREVIEW
#include "ADM_toolkit/ADM_debug.h"

//
void updateVideoFilters( void );
// exported vars
uint32_t nb_video_filter=0;
uint32_t nb_active_filter=0;
FILTER_ENTRY allfilters[MAX_FILTER];
FILTER  videofilters[MAX_FILTER];

static uint32_t lastStart=0, lastNb=0;

extern ADM_Composer *video_body;
extern AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,CONFcouple *conf, AVDMGenericVideoStream *in) ;

//
static   AVDMGenericVideoStream *preview=NULL;
// Ugly should be dynamically allocated
static uint8_t unpackd[MAXIMUM_SIZE*MAXIMUM_SIZE*3];
// dummy constructor used to register the filter
//____________________________________
AVDMVideo_FilterDec::AVDMVideo_FilterDec(char *name,
									AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, void *))
{
    UNUSED_ARG(name);
    UNUSED_ARG(create);
         //registerFilter(name, create);
	 //
}
//
// Delete everything that is pending
void filterCleanUp( void )
{
	for(uint32_t i=0;i<nb_active_filter;i++)
	{
		  	delete videofilters[i].filter;
			if(videofilters[i].conf) delete videofilters[i].conf;
             videofilters[i].conf=NULL;
             videofilters[i].filter=NULL;  		
	}
   nb_active_filter=0;
   preview=NULL;
}
void registerFilter(const char *name,VF_FILTERS tag,uint8_t viewable,
		AVDMGenericVideoStream *(*create) (AVDMGenericVideoStream *in, CONFcouple *))
{
        assert(nb_video_filter<(MAX_FILTER-1));
        allfilters[ nb_video_filter].name=name;
        allfilters[ nb_video_filter].create=create;
        allfilters[ nb_video_filter].tag=tag;
        allfilters[ nb_video_filter].viewable=viewable;
        nb_video_filter ++;

        if(viewable==1)
	{
        	aprintf("\n Registred filter %lu: %s",nb_video_filter,name);
	}


}
/************************** Get the last video filter of the chain ***************************
The reference is 0 in that case !
************************************************************************************************/
AVDMGenericVideoStream *getLastVideoFilter(void)
{
extern ADM_Composer *video_body;
aviInfo info;

	video_body->getVideoInfo(&info);

	lastNb=info.nb_frames;
	lastStart=0;
	aprintf("GetLast : -- LasrNB %lu start %lu \n",lastNb,lastNb);
  // sanity check
  	if(nb_active_filter==0)
  		{
  		 		nb_active_filter=1;
  		 		preview=videofilters[0].filter=  new AVDMVideoStreamNull(video_body,0,info.nb_frames);
				aprintf("--preview filter 0\n");
				return videofilters[  nb_active_filter-1].filter;
  		}
 	updateVideoFilters();
	return videofilters[  nb_active_filter-1].filter;
}

/*
	Return last video filter and rebuild a chain with only the selected frame

*/
AVDMGenericVideoStream *getLastVideoFilter( uint32_t start, uint32_t nb)
{
extern ADM_Composer *video_body;
  // sanity check
  	lastNb=nb;
	lastStart=start;
	aprintf("GetLast full : -- LasrNB %lu start %lu \n",lastNb,lastNb);
  	if(nb_active_filter==0)
  		{
  		 		nb_active_filter=1;
  		 		videofilters[0].filter=  new AVDMVideoStreamNull(video_body,start,nb);
  		}
	updateVideoFilters();
       return videofilters[  nb_active_filter-1].filter;
}

/********* Returns the first video filters starting from frame xx with nb frames ***************/
AVDMGenericVideoStream *getFirstVideoFilter( uint32_t start, uint32_t nbFrame)
{
extern ADM_Composer *video_body;
  // sanity check
  	lastNb=nbFrame;
	lastStart=start;
	aprintf("GetFirst full : -- LasrNB %lu start %lu \n",lastNb,lastNb);
	updateVideoFilters();
       return videofilters[  0].filter;
}
AVDMGenericVideoStream *getFirstVideoFilter( void)
{
extern ADM_Composer *video_body;
  // sanity check
  aviInfo info;

	video_body->getVideoInfo(&info);
	lastNb=info.nb_frames;
	lastStart=0;
	aprintf("GetFirst : -- LasrNB %lu start %lu \n",lastNb,lastNb);
	updateVideoFilters();
       return videofilters[  0].filter;

}

//
//	Parse the list of active filters, delete them et recreate them with (new ?) configuration
//
void updateVideoFilters(void )
{
		if(nb_active_filter!=0)
		{
			delete videofilters[0].filter;
			videofilters[0].filter=NULL;

		}
  		 videofilters[0].filter=  new AVDMVideoStreamNull(video_body,lastStart,lastNb);

  		if(nb_active_filter<=1)
  		{
  		 	nb_active_filter=1;
			preview=videofilters[0].filter;
			aprintf("--preview filter %d\n",nb_active_filter-1);
  		 	return;
  		}
  		// Rebuild other filters
  		    for(uint32_t i=1;i<nb_active_filter;i++)
						{
                   		VF_FILTERS tag;
                    		AVDMGenericVideoStream *old;
                   			old= videofilters[i].filter;
                  			tag=videofilters[i].tag;

                      	videofilters[i].filter=filterCreateFromTag(tag,
                            			videofilters[i].conf,
                               			videofilters[i-1].filter);

                          delete old;
            	}
		preview=videofilters[nb_active_filter-1].filter;
		aprintf("--preview filter %d\n",nb_active_filter-1);
}
//
//	Create a filter from : its tag, its config and an input stream
//

AVDMGenericVideoStream *filterCreateFromTag(VF_FILTERS tag,CONFcouple *couple, AVDMGenericVideoStream *in)
{
	 AVDMGenericVideoStream *filter;

			assert(tag!=VF_INVALID);
			for(unsigned int i=0;i<nb_video_filter;i++)
				{
					if(tag==allfilters[i].tag)
						{
	 						filter=allfilters[i].create( in, couple);
							return filter;
						}
				}
			assert(0);
			return NULL;                      
}
/*____________________________________
	Save and load current set of filters
	
	We save/load the tag that identifies the filter
	and a raw hex dump of its config
		+ comment and name in clear text
	____________________________________ 
*/

/*---------------------------------------*/	 

/* if refresh=1 we use the current full filter list
	else we rebuild a new one */
void 	editorReignitPreview( void )
{
		aprintf("--killing\n");
            GUI_PreviewEnd();

	    	getFirstVideoFilter();
	   
	    preview=videofilters[  nb_active_filter-1].filter;
	    aprintf("--spawning\n");
            GUI_PreviewInit(preview->getInfo()->width,
									preview->getInfo()->height);
}


void editorKillPreview( void )
{
      GUI_PreviewEnd();
}




void editorUpdatePreview(uint32_t framenum)
{
//
//
//
	uint32_t fl,len;	
 	assert(preview);

  if( GUI_StillAlive())
  {
  		aprintf("Preview: Ask for frame %lu\n",framenum);
		preview->getFrameNumberNoAlloc(framenum,&len,unpackd,&fl);
		GUI_PreviewUpdate(unpackd);
  }

//   virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
//          																				uint8_t *data,uint32_t *flags)=0;

}
