/***************************************************************************
                          adm_encffmpeg.cpp  -  description
                             -------------------
    begin                : Tue Sep 10 2002
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
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>



#ifdef USE_FFMPEG
#include "ADM_lavcodec.h"


#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_video/ADM_genvideo.hxx"
//#include "ADM_codecs/ADM_divxEncode.h"
#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encoder.h"
#include "ADM_encoder/adm_encffmpeg.h"



/*_________________________________________________*/
EncoderFFMPEG::EncoderFFMPEG (FF_CODEC_ID id, FFMPEGConfig *config)
{
  _codec = NULL;
  fd = NULL;
  strcpy (_logname, "");
  _frametogo = 0;
//	memset(&_param,0,sizeof(_param));
	memcpy(&_param,&(config->generic),sizeof(_param));
	memcpy(&_settings,&(config->specific),sizeof(_settings));
	_id=id;
};
//---------------huff----------
EncoderFFMPEGHuff::EncoderFFMPEGHuff(FFMPEGConfig *config) :
	EncoderFFMPEG(FF_HUFF,config)
{
	_id=FF_HUFF;
	_frametogo=0;


}
uint8_t EncoderFFMPEGHuff::encode (uint32_t frame, uint32_t * len, uint8_t * out,
			  uint32_t * flags)
{
uint32_t l,f;
  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }

      return _codec->encode (_vbuffer, out, len, flags);
}
 uint8_t EncoderFFMPEGHuff::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{
uint8_t r=0;
	r=_codec->getExtraData(l,data);
	printf("Huff has %d extra bytes\n",*l);
	return r;

}	
uint8_t EncoderFFMPEGHuff::configure (AVDMGenericVideoStream * instream)
{
 ADM_assert (instream);
  ADV_Info *info;



  info = instream->getInfo ();
  _fps=info->fps1000;
  _w = info->width;
  _h = info->height;
 // _vbuffer = new uint8_t[_w * _h * 3];
 _vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;

  _codec = new ffmpegEncoderCQ (_w, _h,_id);
  _codec->init (_param.qz,_fps,0);
  return 1;



}
//-------------------ffv1------------------
EncoderFFMPEGFFV1::EncoderFFMPEGFFV1(FFMPEGConfig *config) :
	EncoderFFMPEG(FF_HUFF,config)
{
	_id=FF_FFV1;
	_frametogo=0;


}
uint8_t EncoderFFMPEGFFV1::encode (uint32_t frame, uint32_t * len, uint8_t * out,
			  uint32_t * flags)
{
uint32_t l,f;
  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }

      return _codec->encode (_vbuffer, out, len, flags);
}
 uint8_t EncoderFFMPEGFFV1::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{
	*l=0;
	*data=NULL;
	return 0;

}	
uint8_t EncoderFFMPEGFFV1::configure (AVDMGenericVideoStream * instream)
{
 ADM_assert (instream);
  ADV_Info *info;



  info = instream->getInfo ();
  _fps=info->fps1000;
  _w = info->width;
  _h = info->height;
//  _vbuffer = new uint8_t[_w * _h * 3];
  _vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;

  _codec = new ffmpegEncoderCQ (_w, _h,_id);
  _codec->init (_param.qz,_fps,0);
  return 1;



}


//--------------end------------
//************************* SNOW **************************
EncodeFFMPEGSNow::EncodeFFMPEGSNow(FFMPEGConfig *config) :
	EncoderFFMPEG(FF_HUFF,config)
{
	_id=FF_SNOW;
	_frametogo=0;


}
uint8_t EncodeFFMPEGSNow::encode (uint32_t frame, uint32_t * len, uint8_t * out,
			  uint32_t * flags)
{
uint32_t l,f;
  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }

      return _codec->encode (_vbuffer, out, len, flags);
}
 uint8_t EncodeFFMPEGSNow::hasExtraHeaderData( uint32_t *l,uint8_t **data)
{
	*l=0;
	*data=NULL;
	return 0;

}	
uint8_t EncodeFFMPEGSNow::configure (AVDMGenericVideoStream * instream)
{
 ADM_assert (instream);
  ADV_Info *info;



  info = instream->getInfo ();
  _fps=info->fps1000;
  _w = info->width;
  _h = info->height;
//  _vbuffer = new uint8_t[_w * _h * 3];
_vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;

  _codec = new ffmpegEncoderCQ (_w, _h,_id);
  _codec->init (_param.qz,_fps,0);
  return 1;



}


//************************* SNOW **************************
// return codec name as seen in avi header
//
const char *EncoderFFMPEG::getCodecName(void )
{
  switch(_id)
  {
  		case FF_HUFF:
					return "HFYU";
					break;
		case FF_MPEG4:
					return "DX50";
					break;
		case FF_H263P:
		case FF_H263:
					return "H263";
					break;
		case FF_FFV1:
					return "FFV1";
					break;
		case FF_SNOW:
					return "SNOW";
					break;
					
		default:
				ADM_assert(0);

	}
	return NULL;
}
//--------------------------------
uint8_t
EncoderFFMPEG::configure (AVDMGenericVideoStream * instream)
{

  ADM_assert (instream);
  ADV_Info *info;

	uint32_t flag1,flag2,flag3;
	flag1=flag2=flag3=0;

  info = instream->getInfo ();
  _fps=info->fps1000;
  _w = info->width;
  _h = info->height;
 // _vbuffer = new uint8_t[_w * _h * 3];
 _vbuffer=new ADMImage(_w,_h);
  ADM_assert (_vbuffer);
  _in = instream;


/*   COMPRES_PARAMS par;

  memcpy (&par, &_param, sizeof (par));
	if(! getFFCompressParams(&par.mode,&par.qz,
		      &par.bitrate,&par.finalsize,&flag1, &flag2,&flag3))
    {
      //delete par;
      return 0;
    }
    memcpy (&_param, &par, sizeof (par));
*/    
  switch (_param.mode)
    {
    case COMPRESS_SAME:
    				printf("FFmpeg in follow quant mode\n");
				_state=enc_Same;
				_codec=  new ffmpegEncoderVBRExternal (_w, _h,_id); 
				_codec->setConfig(&_settings);
      				_codec->init (2,_fps,0);
      				break;
				
    case COMPRESS_CQ:
				printf("ffmpeg cq mode: %ld\n",_param.qz);
      				_state = enc_CQ;
     				_codec = new ffmpegEncoderCQ (_w, _h,_id);
     				_codec->setConfig(&_settings);
      				_codec->init (_param.qz,_fps,0);
      				break;
    case COMPRESS_CBR:
				printf("ffmpeg cbr mode: %ld\n",_param.bitrate);
      				_state = enc_CBR;
     				_codec = new ffmpegEncoderCBR (_w, _h,_id);
				_codec->setConfig(&_settings);
     				_codec->init (_param.bitrate,_fps,flag1);
      break;
    case COMPRESS_2PASS:
               ffmpegEncoderCQ *cdec;
				
				printf("\n ffmpeg dual size: %lu",_param.finalsize);
  			    	_state = enc_Pass1;
    			 	cdec = new ffmpegEncoderCQ (_w, _h,_id); // Pass1
			 	cdec->setConfig(&_settings);
          // 1+  VBR stats required
          // no stats
             // force internal
				cdec->setLogFile(_logname);             
        			cdec->init (2,_fps,1);
             		
				_codec=cdec;
				if(flag1)
					_internal=0;
				else
					_internal=1;
   				   break;

      break;
    default:
      ADM_assert (0);

    }
  _in = instream;
  printf ("\n ffmpeg Encoder , w: %lu h:%lu mode:%d", _w, _h, _state);
  return 1;

}



uint8_t
EncoderFFMPEG::startPass1 (void)
{
		 ADM_assert (_state == enc_Pass1);
  _frametogo = 0;
  printf ("\n Starting pass 1\n");
  printf (" Creating logfile :%s\n", _logname);
/*  fd = fopen (_logname, "wt");
  if (!fd)
    {
      printf ("\n cannot create logfile !\n");
      return 0;
    }
*/    
  return 1;
}


uint8_t
EncoderFFMPEG::isDualPass (void)
{
    if ((_state == enc_Pass1) || (_state == enc_Pass2))
    {
      return 1;
    }
  return 0;
}

uint8_t
EncoderFFMPEG::setLogFile (const char *lofile, uint32_t nbframe)
{
   strcpy (_logname, lofile);
  _frametogo = nbframe;
  return 1;

}
uint8_t EncoderFFMPEG::getLastQz( void) 
{
  myENC_RESULT enc;

	_codec->getResult((void *)&enc);   
	return enc.out_quantizer;

}
//______________________________
uint8_t
EncoderFFMPEG::encode (uint32_t frame, uint32_t * len, uint8_t * out,
		     uint32_t * flags)
{
  uint32_t l, f;
  //ENC_RESULT enc;

  ADM_assert (_codec);
  ADM_assert (_in);

  if (!_in->getFrameNumberNoAlloc (frame, &l, _vbuffer, &f))
    {
      printf ("\n Error : Cannot read incoming frame !");
      return 0;
    }

  switch (_state)
    {
    case enc_CBR:
    case enc_CQ:
      return _codec->encode (_vbuffer, out, len, flags);
      break;
      case enc_Same:
      		{
			uint32_t inq;
      			if(*flags& AVI_KEY_FRAME) *flags=1;
				else		  *flags=0;
			inq=_vbuffer->_Qp;
			
			inq>>=1; 	// ?
			
      			*flags+=(inq<<16);
			
			if(!_codec->encode(   _vbuffer,out,len,flags))
			{
				printf("\n codec error on 1st pass !");
				return 0;
			}
                        myENC_RESULT enc;
			// Grab result
        		_codec->getResult((void *)&enc);       
			
			//printf("Inq:%lu outq:%lu\n",inq,enc.out_quantizer);                       
			_frametogo++;
			return 1;
			break;
		}
  case enc_Pass1:

					//		ADM_assert(fd);
			if(!_codec->encode(   _vbuffer,out,len,flags))
			{
				printf("\n codec error on 1st pass !");
				return 0;
			}
                        myENC_RESULT enc;
			// Grab result
        		_codec->getResult((void *)&enc);                                
			_frametogo++;
			return 1;
			break;
    case enc_Pass2:
                             // Encode it !
			*flags=	0; // ugly but help to keep interface
               		if(!_codec->encode(_vbuffer,		out,len,flags)) return 0;
                             // Update bits
                        _codec->getResult(&enc);
                        old_bits=enc.total_bits;
			return 1;
                              break;

          default:
      ADM_assert (0);
    }
  return 0;
}

//_______________________________
uint8_t
EncoderFFMPEG::stop (void)
{
	if(_codec)	  delete       _codec;
  _codec = NULL;
  return 1;

}

uint8_t
EncoderFFMPEG::startPass2 (void)
{

	 ADM_assert (_state = enc_Pass1);
  printf ("\n Starting pass 2\n");


  printf ("\n VBR paramaters computed\n");
  _state = enc_Pass2;
  old_bits = 0;
  // Delete codec and start new one
  if (_codec)
    {
      delete
	_codec;
      _codec = NULL;
    }
	_codec=  new ffmpegEncoderVBR (_w, _h,_internal ,_id); //0 -> external 1 -> internal
	_codec->setConfig(&_settings);
  printf ("\n ready to encode in 2pass : %s\n",_logname);

  // compute average bitrate
	double db,ti;
	uint32_t vbr=0;
		db= _param.finalsize;
		db=db*1024.*1024.*8.;
	 	// now deb is in Bits
	  
	  // compute duration
	  ti=frameEnd-frameStart+1;
	  ti*=1000;
	  ti/=_fps;  // nb sec
	  db=db/ti;
  
  vbr=(uint32_t )floor(db);
  
   _codec->setLogFile(_logname);  
   _codec->init (vbr,_fps);
 
  printf("\n ** Total size     : %lu MBytes \n",_param.finalsize);
//  printf(" ** Total frame    : %lu  \n",_totalframe);   
  printf(" (using avg bitrate of %lu kbps",vbr/1000);
  return 1;
}
//-----------------------ffmpegEncoderHuff

#endif

