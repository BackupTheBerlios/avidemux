/***************************************************************************
                          op_savesmart.cpp  -  description
                             -------------------
    begin                : Mon May 6 2002
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
#include <math.h>
#include <unistd.h>
#include "ADM_assert.h" 

#include <time.h>
#include <sys/time.h>
#include "config.h"
#ifdef USE_FFMPEG
extern "C" {
	#include "ADM_lavcodec.h"
	};
#endif

#include "fourcc.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_codecs/ADM_divxEncode.h"
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "oplug_avi/op_aviwrite.hxx"
#include "oplug_avi/op_avisave.h"
#include "oplug_avi/op_savesmart.hxx"
#ifdef USE_FFMPEG		
 
#include "ADM_codecs/ADM_ffmpeg.h"		
#endif
#ifdef USE_DIVX

#endif
GenericAviSaveSmart::GenericAviSaveSmart(uint32_t qf) : GenericAviSave()
{
	_cqReenc=qf;
	ADM_assert(qf>=2 && qf<32);
}
uint8_t	GenericAviSaveSmart::setupVideo (char *name)
{

int value=4;;
	 

		printf("\n Q: %u",_cqReenc);
  // init save avi
  memcpy(&_bih,video_body->getBIH (),sizeof(_bih));
  memcpy(&_videostreamheader,video_body->getVideoStreamHeader (),sizeof( _videostreamheader));
  memcpy(&_mainaviheader,video_body->getMainHeader (),sizeof(_mainaviheader));
 
   uint8_t *extraData;
   uint32_t extraLen;
  _lastIPFrameSent=0xfffffff;
   video_body->getExtraHeaderData(&extraLen,&extraData);

  if (!writter->saveBegin (name,
			   &_mainaviheader,
			   frameEnd - frameStart + 1, 
			   &_videostreamheader,
			   &_bih,
			   extraData,extraLen,
			   audio_filter,
			   NULL
		))
    {

      return 0;
    }
  compEngaged = 0;
  encoderReady = 0;
  _encoder = NULL;
  aImage=new ADMImage(_mainaviheader.dwWidth,_mainaviheader.dwHeight);
  _incoming = getFirstVideoFilter (frameStart,frameEnd-frameStart);
  encoding_gui->setFps(_incoming->getInfo()->fps1000);
  encoding_gui->setPhasis("Smart Copy");
  //
  return 1;
  //---------------------
}

//
//      Just to keep gcc happy....
//
GenericAviSaveSmart::~GenericAviSaveSmart ()
{
  if (encoderReady && _encoder)
    {
      _encoder->stopEncoder ();
    }
  if (_encoder)
    delete      _encoder;
 if(aImage)
 {
 	delete aImage;
	aImage=NULL;
 }
}

// copy mode
// Basically ask a video frame and send it to writter
uint8_t
GenericAviSaveSmart::writeVideoChunk (uint32_t frame)
{
  uint32_t    len;
  uint8_t     ret1, seq;
//static char str[200];

  if (compEngaged)		// we were re-encoding
    {
      video_body->getFlags (frameStart + frame, &_videoFlag);
      if (_videoFlag & AVI_KEY_FRAME)
	{
	  // It is a kf, go back to copy mode
	  compEngaged = 0;
	  stopEncoder ();	// Tobias F
	  delete	    _encoder;
	  _encoder = NULL;
	  goto cpmod;
	}
    prmod:
      printf ("\n %lu encoding", frame);
      // Else encode it ....
      //1-Read it
      ret1 = video_body->getUncompressedFrame (frameStart + frame, aImage);
      if (!ret1)
	return 0;
      // 2-encode it
      if (!_encoder->encode (aImage, vbuffer, &len, &_videoFlag))
	return 0;
      // 3-write it
      return writter->saveVideoFrame (len, _videoFlag, vbuffer);


    }

cpmod:
  // Do we have to re-encode ?
  ret1 = video_body->getFrameNoAlloc (frameStart + frame, vbuffer, &len,
				      &_videoFlag, &seq);

  if (!ret1)
    return 0;

  // if it is neither a keyframe and there is a flow cut.....

  if (!(_videoFlag & AVI_KEY_FRAME) && !seq)
    {
      compEngaged = 1;
      // Reinit encoder
      initEncoder (_cqReenc);
      goto prmod;
    }
  // else just write it...
  printf ("\n %lu copying", frame);
    if(muxSize)
      	{
				 		// we overshot the limit and it is a key frame
				   	// start a new chunk
				  	if(handleMuxSize() && (_videoFlag & AVI_KEY_FRAME))
				   	{		
					 	uint8_t *extraData;
						uint32_t extraLen;

   							video_body->getExtraHeaderData(&extraLen,&extraData);
					   
							if(!reigniteChunk(extraLen,extraData)) return 0;
						
						}
				 }
  return writter->saveVideoFrame (len, _videoFlag, vbuffer);

}
 
 //
 //
uint8_t
GenericAviSaveSmart::initEncoder (uint32_t qz)
{
  aviInfo
    info;
  video_body->getVideoInfo (&info);
  ADM_assert (0 == encoderReady);
  encoderReady = 1;
  uint8_t ret=0;
  FFcodecSetting myConfig=
	 {
	 ME_EPZS,//	ME
	 0, // 		GMC	
	 1,	// 4MV
	 0,//		_QPEL;	 
	 0,//		_TREILLIS_QUANT
	 2,//		qmin;
	 31,//		qmax;
	 3,//		max_qdiff;
	 0,//		max_b_frames;
	 0, //		mpeg_quant;
	 1, //
	 -2, // 		luma_elim_threshold;
	 1,//
	 -5, 		// chroma_elim_threshold;
	 0.05,		//lumi_masking;
	 1,		// is lumi
	 0.01,		//dark_masking; 
	 1,		// is dark
 	 0.5,		// qcompress amount of qscale change between easy & hard scenes (0.0-1.0
    	 0.5,		// qblur;    amount of qscale smoothing over time (0.0-1.0) 
	0,		// min bitrate in kB/S
	0,		// max bitrate
	0, 		// default matrix
	0, 		// no gop size
	NULL,
	NULL,
	0,		// interlaced
	0,		// WLA: bottom-field-first
	0,		// wide screen
	2,		// mb eval = distortion
	8000,		// vratetol 8Meg
	0,		// is temporal
	0.0,		// temporal masking
	0,		// is spatial
	0.0,		// spatial masking
	0,		// NAQ
	0		// DUMMY 
 	} ;


  if(  isMpeg4Compatible(info.fcc) )
  	{
/*	
#ifdef USE_DIVX		 
		 	 _encoder = new divxEncoderCQ (info.width, info.height);
	   
#else			
*/
// 	uint8_t				setConfig(FFcodecSetting *set);	
			ffmpegEncoderCQ *tmp;		
			tmp = new ffmpegEncoderCQ (info.width, info.height,FF_MPEG4);					
			
			tmp->setConfig(&myConfig);
			printf("\n init qz %ld\n",qz);
	    		ret= tmp->init (qz,25000);
			_encoder=tmp;
/*			
#endif		  		  
*/
		
#warning 25 fps hardcoded

		 }
		 else
		 {
#ifdef USE_FFMPEG			 
			 if(isMSMpeg4Compatible(info.fcc) )
			 {
				 ffmpegEncoderCQ *tmp;
				  tmp = new ffmpegEncoderCQ (info.width, info.height,FF_MSMP4V3);
				  tmp->setConfig(&myConfig);
			    	  ret= tmp->init (qz,25000);
			    	  _encoder=tmp;
				}
				else
					{
				       ADM_assert(0);
					}			
			}
#else
			ADM_assert(0);
			}			
#endif

		return ret;
}

uint8_t
GenericAviSaveSmart::stopEncoder (void)
{
  ADM_assert (1 == encoderReady);
  encoderReady = 0;
  return (_encoder->stopEncoder ());
}

#endif
