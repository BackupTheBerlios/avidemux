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

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ENCODER
#include "ADM_toolkit/ADM_debug.h"

extern "C" {
#include "ADM_encoder/xvid_vbr.h"
};
#include "adm_encffmatrix.h"

#define FIRST_PASS_QUANTIZER 2


extern "C" {
#include "ADM_encoder/xvid_vbr.h"
};

static vbr_control_t vbrstate;

/*_________________________________________________*/
EncoderFFMPEGMpeg1::EncoderFFMPEGMpeg1 (FF_CODEC_ID id, FFMPEGConfig *config)
: EncoderFFMPEG (id,config)

{
  _frametogo = 0;
 _pass1Done=0;
 _lastQz=0;
 _lastBitrate=0;
 _totalframe=0;
 memcpy(&_param,&(config->generic),sizeof(_param));
 memcpy(&_settings,&(config->specific),sizeof(_settings));
 _use_xvid_ratecontrol=0;
 
};

uint8_t EncoderFFMPEGMpeg1::encode (uint32_t frame, uint32_t * len, uint8_t * out,
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
	switch(_state)
	{
		case enc_CQ:
		case enc_CBR:
      				return _codec->encode (_vbuffer, out, len, flags);
				break;
		case enc_Pass1:
					// collect result
					if(!_codec->encode(   _vbuffer,out,len,flags))
					{
						printf("\n codec error on 1st pass !");	
						return 0;
					}
					if(_use_xvid_ratecontrol)
					{
						if(!*len)
						{
							printf("Skipping delay\n");
							return 1;
						}
						 myENC_RESULT enc;
                                		_codec->getResult((void *)&enc);
						updateStats(*len);
						
					}
                                        _frametogo++;
					return 1;
					break;
		case enc_Pass2:
					if(!_use_xvid_ratecontrol)
					{
        					*flags=0;
                      				if(!_codec->encode(_vbuffer,out,len,flags)) 
							return 0;	
						return 1;
					}

					uint16_t nq;
					uint8_t   nf;

          				nq=vbrGetQuant(&vbrstate);
					nf=vbrGetIntra(&vbrstate);
					// Encode it !
					#define MPEG1_MIN_Q 2
					#define MPEG1_MAX_Q 20

					if(nq<MPEG1_MIN_Q)
					{
							nq=MPEG1_MIN_Q;
					}
					if(nq>MPEG1_MAX_Q)
					{
							nq=MPEG1_MAX_Q;
					}
			     		//printf("asked :%d ",nq);
					*flags=	(nq<<16)+nf; // ugly but help to keep interface
                                	if(!_codec->encode(_vbuffer,		out,len,flags)) return 0;
//#define ALLOW_PADDING

					if(!*len)
					{
						printf("Skipping delay\n");
						return 1;
					}
#ifdef ALLOW_PADDING					
					if(vbrstate.underflow_warning)
					{
						uint32_t padding;
							padding=(vbrstate.underflow_warning*1000)/_fps;

						printf(" underflow detected ...adding : %lu bytes at frame %lu\n",
									padding,frame);
						memset(_vbuffer+*len,0,padding);
						// we give the size before padding to Xvid 2 pass engine
						// as else it will make it believe we got it all wrong
						// since the bitrate is low and it happens seldom it is safe
						// It may double up the effectice padding but
						// with such low value it should be harmless
						updateStats(*len);
						*len+=padding;

					}
					else
#endif
					{
                       				updateStats(*len);
					}
                             		
					return 1;

					break;
		default:
				ADM_assert(0);
				break;
		}
		ADM_assert(0);
		return 0;

}
EncoderFFMPEGMpeg1::~EncoderFFMPEGMpeg1()
{ stop();};


uint8_t EncoderFFMPEGMpeg1::getLastQz( void) 
{
  myENC_RESULT enc;

	_codec->getResult((void *)&enc);   
	return enc.out_quantizer;

}

//--------------------------------
uint8_t	EncoderFFMPEGMpeg1::configure (AVDMGenericVideoStream * instream)
{

  ADM_assert (instream);
  ADV_Info *info;
  fd=NULL;

	uint32_t flag1,flag2,flag3;
	flag1=flag2=flag3=0;

	  info = instream->getInfo ();
  	_fps=info->fps1000;
  	_w = info->width;
  	_h = info->height;
  	//_vbuffer = new uint8_t[_w * _h * 3];
	_vbuffer=new ADMImage(_w,_h);
  	ADM_assert (_vbuffer);
  	_in = instream;
	_use_xvid_ratecontrol=_settings.use_xvid_ratecontrol;

  switch (_param.mode)
    {
    case COMPRESS_CQ:
				printf("ffmpeg mpeg1 cq mode: %ld\n",_param.qz);
      				_state = enc_CQ;
				setMatrix(); //_settings.user_matrix,_settings.gop_size);
     				_codec = new ffmpegEncoderCQ (_w, _h,_id);
     				_codec->setConfig(&_settings);
      				_codec->init (_param.qz,_fps,0);
      				break;
    case COMPRESS_CBR:
				printf("ffmpeg mpeg1 cbr mode: %ld\n",_param.bitrate);
      				_state = enc_CBR;
				setMatrix();
     				_codec = new ffmpegEncoderCBR (_w, _h,_id);				
				_codec->setConfig(&_settings);			
				flag1=1;
     				_codec->init (_param.bitrate,_fps,flag1);

      				break;
    case COMPRESS_2PASS:
				{
				ffmpegEncoderCQ *cdec=NULL;
						if(_use_xvid_ratecontrol)
						{
							if(0>  vbrSetDefaults(&vbrstate)) 
								return 0;
	       						vbrstate.fps=info->fps1000/1000.;
							_state = enc_Pass1;
							printf("\n ffmpeg dual size: %lu",_param.finalsize);
							setMatrix();
							cdec = new ffmpegEncoderCQ (_w, _h,_id); // Pass1
							
							FFcodecSetting tmp;
							memcpy(&tmp,&_settings,sizeof(_settings));
							tmp.maxBitrate=tmp.minBitrate=tmp.bufferSize=0;
			    				cdec->setConfig(&tmp);
							{
							char dummy[strlen(_logname)+10];
							strcpy(dummy,_logname);
							strcat(dummy,".fake");
							
							cdec->setLogFile(dummy);
							}
							cdec->init (FIRST_PASS_QUANTIZER,_fps,1);
							_codec=cdec;
									
						}
						else
						{
							_state = enc_Pass1;
							printf("\n ffmpeg dual size: %lu",_param.finalsize);
							setMatrix();
							cdec = new ffmpegEncoderCQ (_w, _h,_id); // Pass1
			    				FFcodecSetting tmp;
							memcpy(&tmp,&_settings,sizeof(_settings));
							tmp.maxBitrate=tmp.minBitrate=tmp.bufferSize=0;
			    				cdec->setConfig(&tmp);
							cdec->setLogFile(_logname);
							cdec->init (FIRST_PASS_QUANTIZER,_fps,1);
							_codec=cdec;
						}
				}
   				 break;
    default:
      				ADM_assert (0);
				break;

    }
  _in = instream;
  printf ("\n ffmpeg Encoder , w: %lu h:%lu mode:%d", _w, _h, _state);
  return 1;

}



uint8_t EncoderFFMPEGMpeg1::startPass1 (void)
{
 ADM_assert (_state == enc_Pass1);
  _frametogo = 0;
  printf ("\n Starting pass 1\n");
  printf (" Creating logfile :%s\n", _logname);
  _pass1Done=1;  
  if(_use_xvid_ratecontrol)
  {
  	printf("Using Xvid 2 pass rate control (%s)\n",_logname);
	vbrstate.mode=VBR_MODE_2PASS_1;
	vbrstate.desired_size=_param.finalsize*1024*1024;
	vbrstate.debug=0;
	vbrstate.filename=_logname; //XvidInternal2pass_statfile;	
 	if(0>   vbrInit(&vbrstate))             
		return 0;
	
  }
  return 1;
}


uint8_t EncoderFFMPEGMpeg1::isDualPass (void)
{
    if ((_state == enc_Pass1) || (_state == enc_Pass2))
    {
      return 1;
    }
  return 0;
}

uint8_t EncoderFFMPEGMpeg1::setLogFile (const char *lofile, uint32_t nbframe)
{
   strcpy (_logname, lofile);
  _frametogo = nbframe;
   _totalframe= nbframe;
  return 1;

}

//_______________________________
uint8_t EncoderFFMPEGMpeg1::stop (void)
{
	printf("Stopping encoder\n");
	if(_codec)	  delete       _codec;
	_codec = NULL;
	 if(_use_xvid_ratecontrol)
  	{
		if(  _state == enc_Pass1 || _state ==enc_Pass2)
		{
			if(_state== enc_Pass1 && _pass1Done)
					vbrFinish(&vbrstate);
			_state=enc_Invalid;
			
		}
	}

	return 1;
}
//_______________________________
uint8_t EncoderFFMPEGMpeg1::startPass2 (void)
{
uint32_t vbr;
uint32_t avg_bitrate;
 ADM_assert (_state == enc_Pass1);
  printf ("\n-------* Starting pass 2*-------------\n");


   float db,ti;

   	db= _param.finalsize;
	db=db*1024.*1024.*8.;
	// now deb is in Bits
	db=db/_totalframe;				// bit / frame
    	db=db*_fps; 
	db/=1000.;
	  
	avg_bitrate=(uint32_t)floor(db);
	  
	printf("\n ** Total size     : %lu MBytes \n",_param.finalsize);
	printf(" ** Total frame    : %lu  \n",_totalframe);
	printf(" ** Average bitrate: %d \n",avg_bitrate/1000);

	
	

  	printf ("\n VBR parameters computed\n");
  	_state = enc_Pass2;
  	old_bits = 0;
  	// Delete codec and start new one
  	if (_codec)
    	{
      		delete _codec;
      		_codec = NULL;
    	}

  if(!_use_xvid_ratecontrol)
  {
  
  	
  
  	_codec=  new ffmpegEncoderVBR (_w, _h,0 ,_id); //0 -> external 1 -> internal
	
	_codec->setConfig(&_settings);
	  	
  	setMatrix();
   	_codec->setLogFile(_logname);
   	//_codec->setLogFile("/tmp/dummylog.txt");
   	_codec->init (avg_bitrate,_fps);  
  	printf ("\n FF:ready to encode in 2pass (%s)\n",_logname);
  	_frametogo=0;
  	return 1;
  }
  // If we use Xvid...
	if(vbrstate.mode==VBR_MODE_2PASS_1)
		vbrFinish(&vbrstate);// ???
	
  	memset(&vbrstate,0,sizeof(vbrstate));
	if(0>  vbrSetDefaults(&vbrstate)) 
			return 0;
	vbrstate.fps=_fps/1000.;
  	vbrstate.desired_bitrate= avg_bitrate;
	vbrstate.vbv_buffer_size=_settings.bufferSize*9*1024;
  	vbrstate.debug=0;
 	vbrstate.filename=_logname;//XvidInternal2pass_statfile;
	vbrstate.mode=VBR_MODE_2PASS_2;
	vbrstate.maxAllowedBitrate=_settings.maxBitrate;
	//
	vbrstate.curve_compression_high=25, // curve_compression_high;
	vbrstate.curve_compression_low=10, // curve_compression_low;
	vbrstate.bitrate_payback_delay=240;
	vbrstate.use_alt_curve=1; // use_alt_curve;
	vbrstate.alt_curve_type=2; //VBR_ALT_CURVE_LINEAR, //int alt_curve_type;
	vbrstate.alt_curve_low_dist=90; // alt_curve_low_dist;
	vbrstate.alt_curve_high_dist=500; // alt_curve_high_dist;
	vbrstate.alt_curve_min_rel_qual=50; // alt_curve_min_rel_qual;
	
	
	//
	
  	if(vbrInit(&vbrstate)<0)
	{
		printf("*** Error in Xvid ratecontrol ****\n**************\n****************\n*********\n");
	
	}
 	_codec= new  ffmpegEncoderVBRExternal (_w, _h,_id); //0 -> external 1 -> internal (_w, _h);
	
  	FFcodecSetting tmp;
	memcpy(&tmp,&_settings,sizeof(_settings));
	tmp.maxBitrate=tmp.minBitrate=tmp.bufferSize=0;
	_codec->setConfig(&tmp);
	
	setMatrix();
	_codec->setLogFile("/tmp/dummylog.txt");
  	_codec->init (_param.qz,_fps);
  	printf ("\n XV:ready to encode in 2pass(%s)\n",_logname);
  	_frametogo=0;
	return 1;


}

uint8_t  EncoderFFMPEGMpeg1::updateStats (uint32_t len)
{

	 myENC_RESULT enc;

      	 _codec->getResult(&enc);
	//printf("Resuld :%d\n",enc.is_key_frame);
	vbrUpdate(&vbrstate,
					enc.out_quantizer,
					enc.is_key_frame,
					0,
					len,
					0,0,0);

			return 1;
}

//
//	Allow the user to set a custom matrix
//     The size parameter is not used
//
// *intra_matrix;
//	uint16_t *inter_matrix;
uint8_t  EncoderFFMPEGMpeg1::setMatrix( void)
{
		switch(_settings.user_matrix)
					{
						case ADM_MATRIX_DEFAULT:
								break;
						case ADM_MATRIX_TMP:
								printf("\n using custom matrix : Tmpg\n");
								_settings.intra_matrix=		tmpgenc_intra;
								_settings.inter_matrix=		tmpgenc_inter;
								break;
						case ADM_MATRIX_ANIME:
								printf("\n using custom matrix : anim\n");
								_settings.intra_matrix=		anime_intra;
								_settings.inter_matrix=		anime_inter;

								break;
						case ADM_MATRIX_KVCD:
								printf("\n using custom matrix : kvcd\n");
								_settings.intra_matrix=		kvcd_intra;
								_settings.inter_matrix=		kvcd_inter;
								break;
					}
		return 1;
}

#endif



