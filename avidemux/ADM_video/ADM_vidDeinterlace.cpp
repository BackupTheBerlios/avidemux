/***************************************************************************
                          ADM_vidDeinterlace.cpp  -  description
                             -------------------
	Strongly inspired by Donal Graft deinterlacer
 	Could be using some MMX
  	Should be faster than the original due to YV12 colorspace

20-Aug-2002 : Ported also the MMX part

    begin                : Sat Apr 20 2002
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
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include"ADM_video/ADM_vidDeinterlace.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM deintParam={2,{"motion_trigger","blend_trigger"}};


SCRIPT_CREATE(deinterlace_script,ADMVideoDeinterlace,deintParam);
BUILD_CREATE(deinterlace_create,ADMVideoDeinterlace);
//_______________________________________________________________


ADMVideoDeinterlace::~ADMVideoDeinterlace()
{
 	
}
ADMVideoDeinterlace::ADMVideoDeinterlace(  AVDMGenericVideoStream *in,CONFcouple *couples)
		:ADMVideoFields(in,couples)
{


}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t ADMVideoDeinterlace::getFrameNumberNoAlloc(uint32_t frame,
																	uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
//uint8_t *dst,*dstu,*dstv,*srcu,*srcv;
uint32_t uvlen;
			if(frame>=_info.nb_frames) return 0;
			ADM_assert(_uncompressed);					
								
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;         	

       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       			

           // for u & v , no action -> copy it as is
           uvlen=    _info.width*_info.height;
           memcpy(data+uvlen,_uncompressed+uvlen,uvlen>>1);
           // No interleaving detected
           if(!hasMotion())
           	{
               	memcpy(data,_uncompressed,*len);
              }
              else
              {
	              doBlend(data);
             }

      return 1;
}



char *ADMVideoDeinterlace::printConf(void)
{
 		return (char *)"Deinterlace";;
}


#endif

