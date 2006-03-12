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

#ifdef USE_XVID_4
#include "xvid.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_codecs/ADM_xvid4.h"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encXvid4.h"


#define aprintf printf



 uint8_t     EncoderXvid4::getLastQz( void)
 {
	myENC_RESULT res;
	_codec->getResult(&res);
	return res.out_quantizer; 
 
 
 }
/*_________________________________________________*/
EncoderXvid4::EncoderXvid4 (COMPRES_PARAMS *codecconfig)
{

  _codec = NULL;  
  strcpy (_logname, "");
  _frametogo = 0;
  _pass1Done=0;
 
 
  memcpy(&_param,codecconfig,sizeof(_param));
  ADM_assert(codecconfig->extraSettingsLen==sizeof(encparam));
  memcpy(&encparam,codecconfig->extraSettings,sizeof(encparam));


};
EncoderXvid4::~EncoderXvid4 ()
{

  stop();

};
//--------------------------------
uint8_t		EncoderXvid4::configure (AVDMGenericVideoStream * instream)
{
  ADM_assert (instream);
  ADV_Info *info;

	//uint32_t flag1,flag2,flag3;

  info = instream->getInfo ();
  _w = info->width;
  _h = info->height;
//  _vbuffer = new uint8_t[_w * _h * 2];
	_vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;
  _fps1000=info->fps1000;
  switch (_param.mode)
    {
     case COMPRESS_SAME:
    			printf("Xvid4 in follow quant mode\n");
			_state=enc_Same;
			_codec = new xvid4EncoderVBRExternal (_w, _h);
      			if(!_codec->init(2,info->fps1000,&encparam))
			{
				printf("Error initi Xvid4 Follow mode\n");
				return 0;
			}
      			break;
    case COMPRESS_CQ:
		printf("\n Xvid4 cq mode: %ld",_param.qz);
      		_state = enc_CQ;
      		_codec = new xvid4EncoderCQ (_w, _h);

      		if(!_codec->init(_param.qz,info->fps1000,&encparam))
		{
			printf("Error initi Xvid4 CQ mode\n");
			return 0;
		}
      		break;
    case COMPRESS_CBR:
		printf("\n Xvid4 cbr mode: %lu",_param.bitrate);
      		_state = enc_CBR;

      		_codec = new xvid4EncoderCBR (_w, _h);
      		if(!_codec->init(_param.bitrate,info->fps1000,&encparam))
		{
			printf("Error initi Xvid4 CQ mode\n");
			return 0;
		}
      		break;
    case COMPRESS_2PASS:
 			printf("\n Xvid dual size: %lu (%s)\n",_param.finalsize,_logname);
 			_state = enc_Pass1;
      			_codec = new xvid4EncoderPass1 (_w, _h);
			strcpy(encparam.logName,_logname);
			printf("Using %s as stat file\n",encparam.logName);
      			
      			break;
    default:
      			ADM_assert (0);
    } 	
  	printf ("\n Xvid4 Encoder ready , w: %lu h:%lu mode:%d", _w, _h, _state);
  	return 1;

}



uint8_t
EncoderXvid4::startPass1 (void)
{
 ADV_Info *info;
  ADM_assert (_state == enc_Pass1);
  info= _in->getInfo ();
  if(!_codec->init(_param.bitrate,info->fps1000,&encparam))
			{
				printf("Error initi Xvid4 pass1 mode\n");
				return 0;
			}
return 1;
}



uint8_t
EncoderXvid4::isDualPass (void)
{
  if ((_state == enc_Pass1) || (_state == enc_Pass2))
    {
      return 1;
    }
  return 0;

}

uint8_t
EncoderXvid4::setLogFile (const char *lofile, uint32_t nbframe)
{
  strcpy (_logname, lofile);
  _frametogo = nbframe;
  _totalframe= nbframe;
  return 1;

}

//______________________________
uint8_t
EncoderXvid4::encode (uint32_t frame, uint32_t * len, uint8_t * out,
		     uint32_t * flags,uint32_t *displayFrame)
{
  uint32_t l, f,q;
  uint8_t r;
  //ENC_RESULT enc;

  ADM_assert (_codec);
  ADM_assert (_in);
  if(displayFrame)
  {
        *displayFrame=frame;
  }
  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }

  switch (_state)
    {
    case enc_Same:
    			*flags=_vbuffer->flags;
    			*flags&=0xffff;
			if(frame<(encparam.bframes+1))
			{
				 *flags=AVI_KEY_FRAME;
				 printf("Forcing keyframe for B frame\n");
			}
			q=_vbuffer->_Qp;
			if(q<2 || q>31)
			{
				printf("Out of bound incoming q:%d\n",q);
				if(q<2) q=2;
				if(q>31) q=31;
			}
			*flags+=(q<<16);
			
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
EncoderXvid4::stop (void)
{
	if(_codec)	  delete       _codec;
 	 _codec = NULL;
  		

  return 1;

}
uint8_t
EncoderXvid4::startPass2 (void)
{
uint32_t 	finalSize;
float 		br;

  ADM_assert (_state == enc_Pass1);
  printf ("\n Starting pass 2 (%dx%d)\n",_w,_h);
  
  
  finalSize=_param.finalsize;
   

   
   	br=finalSize*8;
	br*=1024*1024;
    	br=br/ _totalframe;				// bit / frame
    	br=br*_fps1000;
	br=br/1000;
   
	finalSize= (int)floor(br);
  	    
  	_state = enc_Pass2;
  	// Delete codec and start new one
  	if (_codec)
    	{
      		delete	_codec;
      		_codec = NULL;
    	}
  	printf("\n Xvid dual size: %lu MB\n",_param.finalsize);
 	
      	_codec = new xvid4EncoderPass2 (_w, _h);
	strcpy(encparam.logName,_logname);
	printf("Using %s as stat file, average bitrate %d kbps\n",_logname,finalSize/1000);
      	if(!_codec->init(finalSize,_fps1000,&encparam))
	{
		printf("Error initializing Xvid4 pass1 mode\n");
		return 0;
	}
  	_frametogo=0;
  	return 1;
}

#endif




