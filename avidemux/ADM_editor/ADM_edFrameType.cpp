/***************************************************************************
                          ADM_edFrameType.cpp  -  description
                             -------------------
  Rederive Frame type if needed

    begin                : Fri Apr 12 2002
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
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "avi_vars.h"
#include "ADM_dialog/DIA_working.h"
#include "ADM_toolkit/toolkit.hxx"


/**
	Rebuild frame type by actually decoding them
	to all videos loaded
	Use hurry_up flag if the codec is able to do it.

*/
uint8_t   ADM_Composer::rebuildFrameType ( void)
{
_VIDEOS *vi;
uint32_t len,flags;
uint32_t frames=0,cur=0;
uint8_t *compBuffer=NULL;
//uint8_t *prepBuffer=NULL;
ADMImage *prepBuffer=NULL;
uint32_t bframe;
aviInfo    info;
						
	if(!_nb_video)
	{
		GUI_Alert("No video loaded");
		return 0;
	}
	if(!isIndexable())
	{
		GUI_Alert("Not indexable (divx5+packed?)");
		return 0;
	}
	compBuffer=new uint8_t[(MAXIMUM_SIZE * MAXIMUM_SIZE * 3)>>1];
	if(!compBuffer)
		{
			GUI_Alert("Cannot allocate memory");
			return 0;
		}
	//prepBuffer=new uint8_t[(MAXIMUM_SIZE * MAXIMUM_SIZE * 3)>>1];
	prepBuffer=new ADMImage(MAXIMUM_SIZE,MAXIMUM_SIZE);
	if(!prepBuffer)
		{
			delete [] compBuffer;
			GUI_Alert("Cannot allocate memory(2)");
			return 0;
		}
	for(uint32_t i=0;i<_nb_video;i++)
	{
		frames+=_videos[i]._nb_video_frames;
	}
	DIA_working *work;
	work=new DIA_working("Rebuilding Frames");


	for(uint32_t vid=0;vid<_nb_video;vid++)
	{
		// set the decoder in fast mode
			vi=&(_videos[vid]);
			vi->_aviheader->getVideoInfo (&info);
			
			bframe=0;
			if(vi->_reorderReady)
			{
				cur+=vi->_nb_video_frames;
			}
			else
			{
				vi->decoder->decodeHeaderOnly();
				for(uint32_t j=0;j<vi->_nb_video_frames;j++)
				{
	  				vi->_aviheader->getFrameNoAlloc (j,
							 compBuffer,
							 &len, &flags);
		    			vi->decoder->uncompress (compBuffer, prepBuffer, len, &flags);
	  				vi->_aviheader->setFlag(j,flags);
					if(flags & AVI_B_FRAME)
						bframe++;

					if(work->update(cur, frames))
	  				{
       						delete work;
						vi->decoder->decodeFull();
            					GUI_Alert("Aborted!");
						delete [] compBuffer;
						delete [] prepBuffer;
             					return 0;
       					}
					cur++;
				}
				vi->decoder->decodeFull();
				// and there is b-frame
				if(bframe)
				{
					vi->_reorderReady=vi->_aviheader->reorder();
				}
			}
	}
	delete work;
	delete [] compBuffer;
	delete  prepBuffer;
	return 1;
}


