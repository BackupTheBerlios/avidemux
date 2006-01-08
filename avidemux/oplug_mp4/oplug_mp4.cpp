/***************************************************************************
                          oplug_vcdff.cpp  -  description
                             -------------------
    begin                : Sun Nov 10 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Ouput using FFmpeg mpeg1 encoder
    Much faster than mjpegtools, albeit quality seems inferior
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>


#ifdef USE_FFMPEG
extern "C" {
#include "ADM_lavcodec.h"
};
#include "avi_vars.h"
#include "prototype.h"


#include "ADM_colorspace/colorspace.h"
#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"


#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_encoder/adm_encoder.h"
//#include "ADM_codecs/ADM_divxEncode.h"
//#include "ADM_encoder/adm_encdivx.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"
#include "oplug_mpegFF/oplug_vcdff.h"

#include "ADM_dialog/DIA_encoding.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_lavformat/ADM_lavformat.h"

#include "ADM_encoder/adm_encConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


static uint8_t *_buffer=NULL,*_outbuffer=NULL;
static void  end (void);
extern const char *getStrFromAudioCodec( uint32_t codec);


extern SelectCodecType  current_codec;

static char *twoPass=NULL;
static char *twoFake=NULL;


uint8_t oplug_mp4(const char *name, ADM_OUT_FORMAT type)
{
AVDMGenericVideoStream *_incoming;
FILE 		*file=NULL;
uint8_t		audioBuffer[48000];
uint8_t         *videoBuffer;
uint32_t	audioLen=0;
uint32_t _w,_h,_fps1000,_page,total;	
AVDMGenericAudioStream	*audio;
uint32_t len,flags;
uint32_t size;
ADM_MUXER_TYPE mux;
uint32_t  audio_encoding=0;
uint32_t  real_framenum=0;
uint8_t   ret=0;
uint32_t  sample_target=0;
uint32_t  total_sample=0;


lavMuxer  *muxer=NULL;
aviInfo      info;
uint32_t   width,height;
          _incoming = getFirstVideoFilter (frameStart,frameEnd-frameStart);
          audio=buildRawAudioFilter( video_body->getTime (frameStart),0xffffffff, 0);
          videoBuffer=new uint8_t[_incoming->getInfo()->width*_incoming->getInfo()->height*3];

        
          info.fcc=_incoming->getInfo()->codec;
          info.width=_incoming->getInfo()->width;
          info.height=_incoming->getInfo()->height;
          info.fps1000=_incoming->getInfo()->fps1000;

           muxer= new lavMuxer;
             
           if(!muxer->open(
                name,
                2000000,
                MUXER_MP4,
                &info,
                audio->getInfo()))
                         goto stopit;
//const char *filename, uint32_t inbitrate,ADM_MUXER_TYPE type, aviInfo *info, WAVHeader *audioheader)
           for(int frame=0;frame<_incoming->getInfo()->nb_frames;frame++)
           {
                

           }
           ret=1;
           muxer->close();
stopit:
           delete videoBuffer;
           delete muxer;	

        return ret;
}
	
void end (void)
{

}
	
#endif	
// EOF
