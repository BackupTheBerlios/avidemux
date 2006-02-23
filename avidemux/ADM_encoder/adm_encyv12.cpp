/***************************************************************************
                          adm_encxvid.cpp  -  description
                             -------------------
                             Encoder for Xvid 1.0x (dev-api4)
    begin                : Sun Jul 14 2002
    copyright            : (C) 2002/2003 by mean
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
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>


#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_codecs/ADM_xvid4.h"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encyv12.h"


#define aprintf printf



 uint8_t     EncoderYV12::getLastQz( void)
 {
       
        return 1; 
 
 
 }
/*_________________________________________________*/
EncoderYV12::EncoderYV12 (void)
{

  _frametogo = 0;
};
EncoderYV12::~EncoderYV12 ()
{

  stop();

};
//--------------------------------
uint8_t         EncoderYV12::configure (AVDMGenericVideoStream * instream)
{
  ADM_assert (instream);
  ADV_Info *info;

  _in=instream;
  info = instream->getInfo ();
  _w = info->width;
  _h = info->height;
  _frametogo=info->nb_frames;
  _vbuffer=new ADMImage(_w,_h);
  ADM_assert(_vbuffer);
  printf ("\n Yv12 Encoder ready , w: %lu h:%lu mode:%d", _w, _h, _state);
  return 1;
}




uint8_t
EncoderYV12::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                     uint32_t * flags,uint32_t *displayFrame)
{
        *displayFrame=frame;
        return encode(frame,len,out,flags);

}
//______________________________
uint8_t
EncoderYV12::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                     uint32_t * flags)
{
  uint32_t l, f,q;

  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }
  l=(_w*_h*3)>>1;
  *len=l;
  *flags=AVI_KEY_FRAME;
  memcpy(out,_vbuffer->data,l);
  return 1;
}

//_______________________________
uint8_t
EncoderYV12::stop (void)
{
  return 1;

}
// EOF





