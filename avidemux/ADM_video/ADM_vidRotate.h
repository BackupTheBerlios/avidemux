/***************************************************************************
                          ADM_vidRotate.h  -  description
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
#ifndef ROTATE
#define ROTATE

#include "ADM_vidRotate_param.h"


class  ADMVideoRotate:public AVDMGenericVideoStream
{

 protected:
  virtual char                  *printConf(void);
  ROTATE_PARAM                  *_param;

 public:
                  ADMVideoRotate(AVDMGenericVideoStream *in, CONFcouple *setup);
  virtual         ~ADMVideoRotate();
  virtual uint8_t configure( AVDMGenericVideoStream *instream) ;
  virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, ADMImage *data,uint32_t *flags);
  virtual uint8_t	getCoupledConf( CONFcouple **couples)				;
 }     ;
#endif
