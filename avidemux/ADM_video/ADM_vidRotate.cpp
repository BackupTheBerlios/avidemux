/***************************************************************************
                          ADM_vidRotate.cpp  -  description
                             -------------------
    begin                : Sat Jan 8 2003
    copyright            : (C) 2003 by Tracy Harton
    email                : tracy@amphibious.org
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
#include "ADM_video/ADM_vidRotate.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM rotpParam={3,{"width","height","angle"}};


SCRIPT_CREATE(rotate_script,ADMVideoRotate,rotpParam);
BUILD_CREATE(rotate_create,ADMVideoRotate);

char *ADMVideoRotate::printConf( void )
{
  static char buf[50];
 	
  sprintf((char *)buf," Rotate %3.2f degrees", _param->angle);
  return buf;
}

ADMVideoRotate::ADMVideoRotate(AVDMGenericVideoStream *in, CONFcouple *couples)
{
  _in=in;		

  memcpy(&_info,_in->getInfo(),sizeof(_info));  									 	

  _info.encoding=1;

  _uncompressed=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];

  if(couples)
  {
    _param=NEW(ROTATE_PARAM);
	GET(width);
	GET(height);
	GET(angle);
	_info.width=_param->width;
	_info.height=_param->height;

  }
  else
  {
    _param = NEW( ROTATE_PARAM);
    printf("New Angle 0.0\n");
    _param->angle = 0.0;
    _param->width = _info.width;
    _param->height = _info.height;
  }

  printf("New Rotate %ld %ld %f\n", _info.width, _info.height, _param->angle);

  ADM_assert(_uncompressed);    	  	

}



uint8_t	ADMVideoRotate::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(3);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
			CSET(width);
			CSET(height);
			CSET(angle);

			return 1;

}


ADMVideoRotate::~ADMVideoRotate()
{
 	delete [] _uncompressed;
	DELETE(_param);
}
uint8_t ADMVideoRotate::getFrameNumberNoAlloc(uint32_t frame,
                                              uint32_t *len,
                                              uint8_t *data,
                                              uint32_t *flags)
{
  ADM_assert(frame<_info.nb_frames);
								
  // read uncompressed frame
  if(!_in->getFrameNumberNoAlloc(frame, len, _uncompressed, flags)) return 0;

  printf("rotate %ld %f\n", frame, _param->angle);
  printf("%ld,%ld\n", _in->getInfo()->width, _in->getInfo()->height);

  do_rotate(_uncompressed, 
  	_in->getInfo()->width, 
  	_in->getInfo()->height, 
	_param->angle, 
	data, 
	&_info.width,
	&_info.height);

  printf("%ld,%ld\n", _info.width, _info.height);

  *flags=0;
  *len= (_info.width * _info.height) + ((_info.width * _info.height) / 2);

  return 1;
}

void do_rotate(uint8_t *in, uint32_t in_w, uint32_t in_h, float angle, uint8_t *out, uint32_t *out_w, uint32_t *out_h)
{
  uint32_t x, y;
  uint32_t u_offset = (in_w * in_h);
  uint32_t v_offset = u_offset + (in_w/2 * in_h/2);
  uint32_t in_sub_w = in_w/2;
  uint32_t in_sub_h = in_h/2;
  uint32_t out_sub_w, out_sub_h;

  // In general, for 0 <= i < width and 0 <= j < height, the pixel (x + i, y + j) is colored with red value 
  // red value   rgb_buf[(j * rowstride) + (i * 3) + 0], 
  // green value rgb_buf[(j * rowstride) + (i * 3) + 1], 
  // blue value  rgb_buf[(j * rowstride) + (i * 3) + 2].

  if((angle >= 0.0) && (angle < 90.0))
  {
    *out_w = in_w;
    *out_h = in_h;
    out_sub_w = *out_w/2;
    out_sub_h = *out_h/2;

    for(x = 0; x < *out_w; x++)
      for(y = 0; y < *out_h; y++)
         *(out + (*out_w * y) + x) = *(in + (in_w * y) + x);

    for(x = 0; x < out_sub_w; x++)
      for(y = 0; y < out_sub_h; y++)
       {
         *(out + u_offset + (out_sub_w * y) + x) = *(in + u_offset + (in_sub_w * y) + x);
         *(out + v_offset + (out_sub_w * y) + x) = *(in + v_offset + (in_sub_w * y) + x);
       }
  }
  else if((angle >= 90.0) && (angle < 180.0))
  {
    *out_w = in_h;
    *out_h = in_w;
    out_sub_w = *out_w/2;
    out_sub_h = *out_h/2;

    for(x = 0; x < *out_w; x++)
      for(y = 0; y < *out_h; y++)
         *(out + (*out_w * y) + x) = *(in + (in_w * (in_h-x-1)) + y);

    for(x = 0; x < out_sub_w; x++)
      for(y = 0; y < out_sub_h; y++)
       {
         *(out + u_offset + (out_sub_w * y) + x) = *(in + u_offset + (in_sub_w * (in_sub_h-x-1)) + y);
         *(out + v_offset + (out_sub_w * y) + x) = *(in + v_offset + (in_sub_w * (in_sub_h-x-1)) + y);
       }
  }
  else if((angle >= 180.0) && (angle < 270.0))
  {
    *out_w = in_w;
    *out_h = in_h;
    out_sub_w = *out_w/2;
    out_sub_h = *out_h/2;

    for(x = 0; x < *out_w; x++)
      for(y = 0; y < *out_h; y++)
         *(out + (*out_w * y) + x) = *(in + (in_w * (in_h-y-1)) + in_w-x-1);


    for(x = 0; x < out_sub_w; x++)
      for(y = 0; y < out_sub_h; y++)
         {
           *(out + u_offset + (out_sub_w * y) + x) = *(in + u_offset + (in_sub_w * (in_sub_h-y-1)) + in_sub_w-x-1);
           *(out + v_offset + (out_sub_w * y) + x) = *(in + v_offset + (in_sub_w * (in_sub_h-y-1)) + in_sub_w-x-1);
         }
  }
  else if((angle >= 270.0) && (angle < 360.0))
  {
    *out_w = in_h;
    *out_h = in_w;
    out_sub_w = *out_w/2;
    out_sub_h = *out_h/2;

    for(x = 0; x < *out_w; x++)
      for(y = 0; y < *out_h; y++)
        *(out + (*out_w * y) + x) = *(in + (in_w * x) + (in_w-y-1));

    for(x = 0; x < out_sub_w; x++)
      for(y = 0; y < out_sub_h; y++)
      {
         *(out + u_offset + (out_sub_w * y) + x) = *(in + u_offset + (in_sub_w * x) + (in_sub_w-y-1));
         *(out + v_offset + (out_sub_w * y) + x) = *(in + v_offset + (in_sub_w * x) + (in_sub_w-y-1));
      }
  }
}

#endif
