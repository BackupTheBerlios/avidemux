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
#include <assert.h>
#include <assert.h>

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


char *AVDMVideoStreamBSMear::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," Black l:%lu  r:%lu  u:%lu x d:%lu",
 				_param->cropx,
 					_param->cropx2,
 					_param->cropy,
 					_param->cropy2);
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
			GET(cropx);
			GET(cropx2);
			GET(cropy);
			GET(cropy2);
		}	
			else 	
		{	// default parameter	
				_param=NEW(CROP_PARAMS);
				_param->cropx=_param->cropy=
						_param->cropx2=_param->cropy2=0;
		}										
 	
  _info.encoding=1;

  	  	
}

uint8_t	AVDMVideoStreamBSMear::getCoupledConf( CONFcouple **couples)
{

			assert(_param);
			*couples=new CONFcouple(4);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(cropx);
	CSET(cropx2);
	CSET(cropy);
	CSET(cropy2);
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

			assert(frame<_info.nb_frames);
			assert(_param);									
								
			// read uncompressed frame directly into follower
			// and blacken there
			
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       			
       		  // blacken top
       		  uint8_t *srcY=data;
       		  uint32_t bytes=_info.width*_param->cropy;
		  uint32_t page=_info.width*_info.height;
       		
       		  memset(srcY,0x00,bytes);
		  memset(srcY+page,0x80,bytes>>2);
		  memset(srcY+((page*5)>>2),0x80,bytes>>2);
       		  // left & right
       		  uint32_t stride=_info.width;
       		  for(uint32_t y=_info.height;y>0;y--)
       		  {
       		        memset(srcY,0,_param->cropx);
       		        memset(srcY+stride-_param->cropx2,0,_param->cropx2);       		
       		        srcY+=stride;       		
       		  }
       		
       		  // backen bottom
       		  srcY=data+_info.width*_info.height-1;
       		
       		 bytes=_info.width*_param->cropy2;
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
