/***************************************************************************
                          keyframes.cpp  -  description
                             -------------------
Rebuild keyframe in index

    begin                : Sat Feb 2 2002
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
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include <config.h>


#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_GUI
#include "ADM_toolkit/ADM_debug.h"
/**
  	Rebuild the frame flags
		(keyframe & B-frames)

*/
void A_rebuildKeyFrame(void)
{

	video_body->rebuildFrameType();
#if 0
uint8_t unpacked[720*576*3];

aviInfo info;
uint32_t flagz; //,len;
DIA_working *work;
uint32_t nbBframe=0;

	work=new DIA_working("Rebuilding Frames");
	video_body->getVideoInfo(&info);
	for(uint32_t i=0;i< info.nb_frames;i++)
 	{
		if(work->update(i, info.nb_frames))
  			{
       				delete work;
            			GUI_Alert("Aborted!");
             			return;
       			}

    		video_body-> getUncompressedFrame(i,(uint8_t*)unpacked,&flagz)    ;
		if(flagz& AVI_KEY_FRAME)
		{
			aprintf("\n Frame %lu was a I frame\n");
		}
		if(flagz& AVI_B_FRAME)
		{
			aprintf("\n Frame %lu was a B frame\n");
			nbBframe++;
		}
		else
		{
			aprintf("\n Frame %lu was a P frame\n");
		}
                video_body->setFlag(i,flagz);
    }

    delete work;
    if(nbBframe)
    {
    	printf("\n Reordering...\n");
    	video_body->reorder();
    }
    GUI_Alert("Done!");
    return;
#endif
};
