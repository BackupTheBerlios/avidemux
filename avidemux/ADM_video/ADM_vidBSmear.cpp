/***************************************************************************
                          ADM_vidBSMear.cpp  -  description
                             -------------------
         change part of video into black borders

          Each one ,must be even

          Copy / Paste from crop,almost the same thing


    begin                : Sun Mar 24 2002
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
#include "ADM_video/ADM_vidCommonFilter.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM cropParam={4,{"left","right","top","bottom"}};


SCRIPT_CREATE(bsmear_script,AVDMVideoStreamBSMear,cropParam);

char *AVDMVideoStreamBSMear::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," Black l:%lu  r:%lu  u:%lu x d:%lu",
 				_param->left,
 					_param->right,
 					_param->top,
 					_param->bottom);
        return buf;
}

//_______________________________________________________________

AVDMVideoStreamBSMear::AVDMVideoStreamBSMear(  	AVDMGenericVideoStream *in,CONFcouple *couples)
{


  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		

		if(couples)
		{
			_param=NEW(CROP_PARAMS);
			GET(left);
			GET(right);
			GET(top);
			GET(bottom);
		}	
			else 	
		{	// default parameter	
				_param=NEW(CROP_PARAMS);
				_param->left=_param->top=
						_param->right=_param->bottom=0;
		}										
 	
  _info.encoding=1;

  	  	
}

uint8_t	AVDMVideoStreamBSMear::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(4);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(left);
	CSET(right);
	CSET(top);
	CSET(bottom);
			return 1;

}
AVDMVideoStreamBSMear::~AVDMVideoStreamBSMear()
{
 	DELETE(_param);
 	
}

//
//	Blacken borders, just setting luma to null should be enough
//

uint8_t AVDMVideoStreamBSMear::getFrameNumberNoAlloc(uint32_t frame,
																	uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{

			if(frame>=_info.nb_frames) 
			{
				printf("Filter : out of bound!\n");
				return 0;
			}
	
			ADM_assert(_param);									
								
			// read uncompressed frame directly into follower
			// and blacken there
			
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       			
       		  // blacken top
       		  uint8_t *srcY=data;
       		  uint32_t bytes=_info.width*_param->top;
		  uint32_t page=_info.width*_info.height;
       		
       		  memset(srcY,0x00,bytes);
		  memset(srcY+page,0x80,bytes>>2);
		  memset(srcY+((page*5)>>2),0x80,bytes>>2);
       		  // left & right
       		  uint32_t stride=_info.width;
       		  for(uint32_t y=_info.height;y>0;y--)
       		  {
       		        memset(srcY,0,_param->left);
       		        memset(srcY+stride-_param->right,0,_param->right);       		
       		        srcY+=stride;       		
       		  }
       		
       		  // backen bottom
       		  srcY=data+_info.width*_info.height-1;
       		
       		 bytes=_info.width*_param->bottom;
       	 	 srcY-=bytes;
       		 memset(srcY,0x00,bytes);
		// chroma
		 srcY=data+page+(page>>2)-1;
		 srcY-=bytes>>2;
       		 memset(srcY,0x80,bytes>>2);
		 memset(srcY+((page)>>2),0x80,bytes>>2);
	
       		
       		  	
       		         		       		
      return 1;
}

#endif
