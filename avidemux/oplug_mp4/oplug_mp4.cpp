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

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MP4
#include "ADM_toolkit/ADM_debug.h"

static uint8_t *_buffer=NULL,*_outbuffer=NULL;
static void  end (void);
extern const char *getStrFromAudioCodec( uint32_t codec);


extern SelectCodecType  current_codec;

static char *twoPass=NULL;
static char *twoFake=NULL;

extern AVDMGenericAudioStream *mpt_getAudioStream(void);
uint8_t prepareDualPass(uint8_t *buffer,char *TwoPassLogFile,DIA_encoding *encoding_gui,Encoder *_encode,uint32_t total);

uint8_t oplug_mp4(const char *name, ADM_OUT_FORMAT type)
{
AVDMGenericVideoStream *_incoming;
AVDMGenericAudioStream  *audio;

uint8_t		audioBuffer[48000];
uint8_t         *videoBuffer=NULL;

uint32_t len,flags;
uint32_t size;

uint8_t   ret=0;

uint32_t  sample_got=0,sample;
uint32_t  extraDataSize=0;
uint8_t   *extraData=NULL;
lavMuxer  *muxer=NULL;
aviInfo      info;
uint32_t   width,height;
DIA_encoding *encoding_gui=NULL;
Encoder         *_encode=NULL;
char            *TwoPassLogFile=NULL;
uint32_t total=0;

          _incoming = getFirstVideoFilter (frameStart,frameEnd-frameStart);
          
          videoBuffer=new uint8_t[_incoming->getInfo()->width*_incoming->getInfo()->height*3];

       
        // _________________Setup video_______________
        if(!videoProcessMode())
        {
                total=frameEnd-frameStart;
                video_body->getVideoInfo(&info);
        }else
        {
                _incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
                _encode = getVideoEncoder (_incoming->getInfo()->width,_incoming->getInfo()->height);
                total= _incoming->getInfo()->nb_frames;
                if (!_encode)
                {
                        GUI_Error_HIG ("Cannot initialize the video stream", NULL);
                        goto  stopit;
                }

                // init compressor
                TwoPassLogFile=new char[strlen(name)+6];
                strcpy(TwoPassLogFile,name);
                strcat(TwoPassLogFile,".stat");
                _encode->setLogFile(TwoPassLogFile,total);
                  if (!_encode->configure (_incoming))
                {
                        GUI_Error_HIG ("Filter init failed", NULL);
                        goto  stopit;
                };
                
                if (_encode->isDualPass ())
                {
                        
                        if(!prepareDualPass(videoBuffer,TwoPassLogFile,encoding_gui,_encode,total))
                                goto stopit;
                }
                _incoming=  NULL;
        }
           

// ____________Setup audio__________________
          audio=mpt_getAudioStream();
          if(!audio)
          {
                GUI_Error_HIG ("Cannot initialize the audio stream", NULL);
                                goto  stopit;
          }
// ____________Setup Muxer _____________________
           muxer= new lavMuxer;
           audio->extraData(&extraDataSize,&extraData);  
           if(!muxer->open(
                name,
                2000000, // Muxrate
                MUXER_MP4,
                &info,
                audio->getInfo(),extraDataSize,extraData))
                         goto stopit;
         
           //_____________ Loop _____________________
          encoding_gui=new DIA_encoding(info.fps1000);
          encoding_gui->setContainer("MP4");

          encoding_gui->setAudioCodec("None");
          if(!videoProcessMode())
                encoding_gui->setCodec("Copy");
          else
                encoding_gui->setCodec(_encode->getDisplayName());
           //
           for(int frame=0;frame<total;frame++)
           {
                 if(!videoProcessMode())
                 {
                        video_body->getFrameNoAlloc (frameStart + frame, videoBuffer, &len,
                                      &flags);
                 }else
                {
                        ADM_assert(_encode);
                         if(!_encode->encode ( frame, &len, videoBuffer, &flags))
                         {
                                GUI_Error_HIG ("Error while encoding", NULL);
                                goto  stopit;
                         }
                }
               muxer->writeVideoPacket( len,flags,videoBuffer,frame,frame);

               encoding_gui->setFrame(frame,total);
               encoding_gui->feedFrame(len);

               while(muxer->needAudio())
               {
                     if(!audio->getPacket(audioBuffer,&len,&sample)) break;
                     muxer->writeAudioPacket(len,audioBuffer,sample_got);
                     encoding_gui->feedAudioFrame(len);
                     sample_got+=sample;
               }
           }
           ret=1;
           muxer->close();
stopit:
           if(encoding_gui) delete encoding_gui;
           if(TwoPassLogFile) delete [] TwoPassLogFile;
           if(videoBuffer) delete [] videoBuffer;
           if(muxer) delete muxer;
           if(_encode) delete _encode;	
           // Cleanup
           deleteAudioFilter ();
           return ret;
}
uint8_t prepareDualPass(uint8_t *buffer,char *TwoPassLogFile,DIA_encoding *encoding_gui,Encoder *_encode,uint32_t total)
{
      uint32_t len, flag;
      FILE *tmp;
      uint8_t reuse=0;

        aprintf("\n** Dual pass encoding**\n");

        if((tmp=fopen(TwoPassLogFile,"rt")))
        {
                fclose(tmp);
                if(GUI_Question("\n Reuse the existing log-file ?"))
                {
                        reuse=1;
                }
        }
        
        if(!reuse)
        {
        
                encoding_gui->setPhasis ("1st Pass");
                aprintf("**Pass 1:%lu\n",total);
                _encode->startPass1 ();
                for (uint32_t cf = 0; cf < total; cf++)
                {
                        encoding_gui->setFrame (cf, total);
                        if (encoding_gui->isAlive())
                        {
                                abt:
                                GUI_Error_HIG ("Aborting", NULL);
                                return 0;
                        }
                        if (!_encode->encode (cf, &len, buffer, &flag))
                        {
                                printf("\n Encoding of frame %lu failed !\n",cf);
                                return 0;
                        }
                        encoding_gui->setQuant(_encode->getLastQz());
                        encoding_gui->feedFrame(len);                
                }
                encoding_gui->reset();
                aprintf("**Pass 1:done\n");
        }// End of reuse

        if(!_encode->startPass2 ())
        {
                printf("Pass2 ignition failed\n");
                return 0;
        }
        return 1;
}
void end (void)
{

}
	
#endif	
// EOF
