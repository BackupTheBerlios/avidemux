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

typedef struct ROTATE_PARAM
{
  uint32_t width;
  uint32_t height;
  float angle;
};

void do_rotate(uint8_t *in, uint32_t in_w, uint32_t in_h, float angle, uint8_t *out, uint32_t *out_w, uint32_t *out_h);


class  ADMVideoRotate:public AVDMGenericVideoStream
{

 protected:

  virtual char 			*printConf(void);

  ROTATE_PARAM                  *_param;

 public:
 		

  ADMVideoRotate(AVDMGenericVideoStream *in, CONFcouple *setup);
  virtual ~ADMVideoRotate();
  virtual uint8_t configure( AVDMGenericVideoStream *instream) ;
  virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len, uint8_t *data,uint32_t *flags);
  virtual uint8_t	getCoupledConf( CONFcouple **couples)				;
 }     ;
#endif
