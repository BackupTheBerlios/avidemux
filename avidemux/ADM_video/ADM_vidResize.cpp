/***************************************************************************
                          ADM_vidResize.cpp  -  description
                             -------------------
   Resize a picture in YUV12

   w,h 		size of final picture
   dw,dh  size of black bordered picture


    begin                : Thu Mar 21 2002
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
#include "ADM_video/ADM_resizebis.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"

//BUILD_CREATE(partial_create,ADMVideoPartial);

char *AVDMVideoStreamResize::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," Resize %lu x %lu --> %lu x %lu",
 				_in->getInfo()->width,
 				_in->getInfo()->height,
 				_info.width,
 				_info.height);
        return buf;
}
//_______________________________________________________________
AVDMVideoStreamResize::AVDMVideoStreamResize(
									AVDMGenericVideoStream *in,CONFcouple *couples)
{

  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	//_uncompressed=(uint8_t *)malloc(3*_info.width*_info.height);
	_uncompressed=new uint8_t[3*_info.width*_info.height];

		if(couples)
		{
			 _param=NEW(RESIZE_PARAMS);
			GET(w);
			GET(h);
			GET(algo);
			_info.width=_param->w;
			_info.height=_param->h;

		}
			else
			{
				_param=NEW( RESIZE_PARAMS);
				_param->w=_info.width;
				_param->h = _info.height;
			    _param->algo = 0;
			}
			//_intermediate_buffer=(uint8_t *)malloc(3*_info.width*_in->getInfo()->height);
			_intermediate_buffer=new uint8_t [3*_info.width*_in->getInfo()->height];	

  _info.encoding=1;
  _init=0;
	Vpattern_luma=NULL;
    Vpattern_chroma=NULL;
	Hpattern_luma=NULL;
    Hpattern_chroma=NULL;
}
#if !defined(MPLAYER_RESIZE_PREFFERED) 
AVDMGenericVideoStream *createResizeFromParam(AVDMGenericVideoStream *in,uint32_t x,uint32_t y)
{

	return new AVDMVideoStreamResize(in,x,y);
}
#endif
AVDMVideoStreamResize::AVDMVideoStreamResize(
									AVDMGenericVideoStream *in,uint32_t x,uint32_t y)
{

  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	//_uncompressed=(uint8_t *)malloc(3*_info.width*_info.height);
	_uncompressed=new uint8_t[3*_info.width*_info.height];
	_param=NEW( RESIZE_PARAMS);
	_param->w=x;
	_param->h = y;
	_info.width=_param->w;
	_info.height=_param->h;
	_param->algo = 0;
			//_intermediate_buffer=(uint8_t *)malloc(3*_info.width*_in->getInfo()->height);
			_intermediate_buffer=new uint8_t [3*_info.width*_in->getInfo()->height];

  _info.encoding=1;
  _init=0;
	Vpattern_luma=NULL;
    Vpattern_chroma=NULL;
	Hpattern_luma=NULL;
    Hpattern_chroma=NULL;
}

uint8_t	AVDMVideoStreamResize::getCoupledConf( CONFcouple **couples)
{

			assert(_param);
			*couples=new CONFcouple(3);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(w);
	CSET(h);
	CSET(algo);
			return 1;

}
// ___ destructor_____________
AVDMVideoStreamResize::~AVDMVideoStreamResize()
{
 	delete [] _uncompressed;
	delete [] _intermediate_buffer;
	DELETE(_param);
	_uncompressed=_intermediate_buffer=NULL;
 	endcompute();
}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoStreamResize::getFrameNumberNoAlloc(uint32_t frame,
																		uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
static Image in,out;
	if(frame>=_info.nb_frames) 
	{
		printf("Filter : out of bound!\n");
		return 0;
	}
	
			assert(_param);	
	
   			

			// bypass filter
			//return _in->getFrameNumberNoAlloc(frame, len,data,flags)	;
			
								
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;
       		
       		// do the resize in 3 passes, Y, U then V
       		in.width=_in->getInfo()->width;
       		in.height=_in->getInfo()->height;
       		in.data=_uncompressed;

       		out.width=_info.width;
       		out.height=_info.height;
       		out.data=data;
       		
              if(!_init)
              {
               	_init=1;
               	printf("\n Precomputing with algo :%lu\n",_param->algo);
                	if(_param->algo>2)
                 		{
                      	printf("\n Wrong algorithm selection");
                        assert(0);
                     }
               	 precompute(&out,&in, _param->algo );
              }
       		zoom(&out,&in)         ;
       				       			
       		
       		  *flags=0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       				
      return 1;
}


#endif
