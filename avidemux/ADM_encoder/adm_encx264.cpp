//
// C++ Implementation: X264 encoder
//
// Description: 
//
//
// Author:Mean, fixounet at free dot fr
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#ifdef USE_X264
 
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>
#include "ADM_assert.h"
#include "ADM_library/default.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encx264.h"


#define aprintf printf


/*_________________________________________________*/
EncoderX264::EncoderX264 (X264Config *codecconfig)
{

  _codec = NULL;  
  strcpy (_logname, "");
  _frametogo = 0;
  _pass1Done=0;
  memcpy(&_param,&(codecconfig->generic),sizeof(_param));
  memcpy(&_codecParam,&(codecconfig->specific),sizeof(_codecParam));


};
EncoderX264::~EncoderX264 ()
{

  stop();

};
//--------------------------------
uint8_t         EncoderX264::configure (AVDMGenericVideoStream * instream)
{
  ADM_assert (instream);
  ADV_Info *info;

  info = instream->getInfo ();
  _w = info->width;
  _h = info->height;

  _vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;
  _fps1000=info->fps1000;
    switch (_param.mode)
      {
      
      case COMPRESS_CQ:
                  printf("\n X264 cq mode: %ld",_param.qz);
                  _state = enc_CQ;
                  _codec = new   X264EncoderCQ (_w, _h);
  
                if(!_codec->init(_param.qz,info->fps1000,&_codecParam))
                  {
                          printf("Error initi X264 CQ mode\n");
                          return 0;
                  }
                  break;
      case COMPRESS_CBR:
                  printf("\n X264 cbr mode: %lu",_param.bitrate);
                  _state = enc_CBR;
  
                  _codec = new X264EncoderCBR (_w, _h);
                  if(!_codec->init(_param.bitrate,info->fps1000,&_codecParam))
                  {
                    printf("Error initi X264 CBR mode\n");
                          return 0;
                  }
                  break;
        default:
                  ADM_assert(0);
      }
    

  printf ("\n X264 Encoder ready , w: %lu h:%lu mode:%d", _w, _h, _state);
  return 1;

}



uint8_t    EncoderX264::startPass1 (void)
{
  ADM_assert(0);
}



uint8_t    EncoderX264::isDualPass (void)
{
 
  return 0;

}

uint8_t    EncoderX264::setLogFile (const char *lofile, uint32_t nbframe)
{
  return 1;
  //ADM_assert(0);

}

//______________________________
uint8_t
    EncoderX264::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags)
{
  uint32_t l, f,q;
  //ENC_RESULT enc;

  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
  {
    printf ("\n Error : Cannot read incoming frame !");
    return 0;
  }

      return _codec->encode (_vbuffer, out, len, flags);
 
}

//_______________________________
uint8_t
    EncoderX264::stop (void)
{
  if(_codec)        delete       _codec;
  _codec = NULL;
                

  return 1;

}
uint8_t
    EncoderX264::startPass2 (void)
{
  ADM_assert(0);
}

#endif




