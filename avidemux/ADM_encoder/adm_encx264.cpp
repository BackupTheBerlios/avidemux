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
#include "ADM_toolkit/ADM_cpuCap.h"


#define aprintf printf


/*_________________________________________________*/
EncoderX264::EncoderX264 (COMPRES_PARAMS  *codecconfig)
{

  _codec = NULL;  
  strcpy (_logname, "");
  _frametogo = 0;
  _pass1Done=0;
  memcpy(&_param,codecconfig,sizeof(_param));
  ADM_assert(codecconfig->extraSettingsLen==sizeof(_codecParam));
  memcpy(&_codecParam,(codecconfig->extraSettings),sizeof(_codecParam));
  _codecParam.nbThreads=ADM_useNbThreads();
  _logfile=NULL;

};
EncoderX264::~EncoderX264 ()
{

  stop();

};
uint8_t EncoderX264::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{
uint8_t r=0;
        r=_codec->getExtraData(l,data);
        printf("x264 has %d extra bytes\n",*l);
        return r;

}       
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
     case COMPRESS_2PASS:
               // printf("\n X264 dual size: %lu (%s)\n",_param.finalsize,_logname);
                _state = enc_Pass1;
                _codec = new X264EncoderPass1 (_w, _h);
                _codecParam.logfile=_logfile;
                  //strcpy(encparam.logName,_logname);
                  //printf("Using %s as stat file\n",encparam.logName);
                        
                  break;                  
        default:
                  ADM_assert(0);
      }
    

  printf ("\n X264 Encoder ready , w: %lu h:%lu mode:%d", _w, _h, _state);
  return 1;

}



uint8_t    EncoderX264::startPass1 (void)
{
  ADV_Info *info;
  ADM_assert (_state == enc_Pass1);
  info= _in->getInfo ();
  if(!_codec->init(_param.bitrate,info->fps1000,&(_codecParam)))
  {
    printf("Error initi Xvid4 pass1 mode\n");
    return 0;
  }
  return 1;
}



uint8_t    EncoderX264::isDualPass (void)
{
 
  if ((_state == enc_Pass1) || (_state == enc_Pass2))
  {
    return 1;
  }
  return 0;

}

uint8_t    EncoderX264::setLogFile (const char *lofile, uint32_t nbframe)
{
 // strcpy (_logname, lofile);
  _logfile=ADM_strdup(lofile);
  printf("Enc X264, using %s as logfile\n",_logfile);
  _frametogo = nbframe;
  _totalframe= nbframe;
  return 1;

}
uint8_t
        EncoderX264::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                             uint32_t * flags)
{
    return encode(frame,len,out,flags,NULL);
}
//______________________________
uint8_t
    EncoderX264::encode (uint32_t frame, uint32_t * len, uint8_t * out,
                          uint32_t * flags,uint32_t *displayFrame)
{
  uint32_t l, f,q;
  uint8_t r=0;
  //ENC_RESULT enc;

  if(displayFrame) *displayFrame=frame;
  
  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
  {
    printf ("\n Error : Cannot read incoming frame !");
    return 0;
  }

   //   return _codec->encode (_vbuffer, out, len, flags);
  switch (_state)
  {
   
                        
    case enc_CBR:
    case enc_CQ:
    case enc_Pass1:
    case enc_Pass2:
      r= _codec->encode (_vbuffer, out, len, flags);
      if(displayFrame)
      {
          *displayFrame=_codec->getPTS_FrameNum();
          if(!*displayFrame) *displayFrame=frame;
                        //printf("DTS: %u PTS: %u\n",frame,*displayFrame);
      }
      return r;
      break;
  
    default:
      ADM_assert (0);
  }
  return 1;   
 
 
}

//_______________________________
uint8_t
    EncoderX264::stop (void)
{
  if(_codec)        delete       _codec;
  _codec = NULL;
  if(_logfile) delete [] _logfile;
                

  return 1;

}
uint8_t
    EncoderX264::startPass2 (void)
{
  float br;
  uint32_t finalSize;
  
  ADM_assert (_state == enc_Pass1);
  printf ("\n Starting pass 2 (%dx%d)\n",_w,_h);
  
  
  finalSize=_param.finalsize;
   

   
  br=finalSize*8;
  br*=1024*1024;
  br=br/ _totalframe;                             // bit / frame
  br=br*_fps1000;
  br=br/1000;
   
  finalSize= (int)floor(br);
            
  _state = enc_Pass2;
        // Delete codec and start new one
  if (_codec)
  {
    delete  _codec;
    _codec = NULL;
  }
  printf("\n X264 dual size: %lu MB\n",_param.finalsize);
        
  _codec = new X264EncoderPass2 (_w, _h);
 // strcpy(encparam.logName,_logname);
  //printf("Using %s as stat file, average bitrate %d kbps\n",_logname,finalSize/1000);
  if(!_codec->init((uint32_t )br,_fps1000,&_codecParam))
  {
    printf("Error initializing x264 pass1 mode\n");
    return 0;
  }
  _frametogo=0;
  return 1;
}
uint8_t    EncoderX264::getLastQz(void)
{
    uint8_t q;
    _my_ENC_RESULT_ res;
    ADM_assert(_codec);
    _codec->getResult((void *)&res);
    return res.out_quantizer;
}
#endif




