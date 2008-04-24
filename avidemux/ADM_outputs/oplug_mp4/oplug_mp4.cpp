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
#include "ADM_default.h"
#include "ADM_threads.h"

#ifdef USE_FFMPEG
extern "C" {
#include "ADM_lavcodec.h"
};
#include "avi_vars.h"
#include "prototype.h"

#include "ADM_colorspace/colorspace.h"
#include "DIA_coreToolkit.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"

#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
//#include "ADM_codecs/ADM_divxEncode.h"
//#include "ADM_encoder/adm_encdivx.h"

#include "ADM_codecs/ADM_ffmpeg.h"
#include "ADM_encoder/adm_encffmpeg.h"
#include "../oplug_mpegFF/oplug_vcdff.h"

#include "ADM_userInterfaces/ADM_commonUI/DIA_encoding.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "../ADM_lavformat.h"

#include "ADM_encoder/adm_encConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_libraries/ADM_mplex/ADM_mthread.h"
#include "ADM_toolkit/ADM_audioQueue.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_MP4
#include "ADM_osSupport/ADM_debug.h"

static uint8_t *_buffer=NULL,*_outbuffer=NULL;
static void  end (void);
extern const char *getStrFromAudioCodec( uint32_t codec);

static char *twoPass=NULL;
static char *twoFake=NULL;

extern AVDMGenericAudioStream *mpt_getAudioStream(void);
uint8_t prepareDualPass(uint32_t bufferSize,uint8_t *buffer,char *TwoPassLogFile,DIA_encoding *encoding_gui,Encoder *_encode,uint32_t total);
uint8_t extractVolHeader(uint8_t *data,uint32_t dataSize,uint32_t *headerSize);
extern void    UI_purge(void );
/*


*/
uint8_t oplug_mp4(const char *name, ADM_OUT_FORMAT type)
{
AVDMGenericVideoStream *_incoming=NULL;
AVDMGenericAudioStream  *audio=NULL;

uint8_t		audioBuffer[48000];
uint8_t         *videoBuffer=NULL;

uint32_t alen;//,flags;
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
uint32_t videoExtraDataSize=0;
uint8_t  *videoExtraData=NULL;
uint8_t *dummy,err;
WAVHeader *audioinfo=NULL;
int prefill=0;
uint32_t displayFrame=0;
ADMBitstream    bitstream(0);
uint32_t        frameWrite=0;
ADM_MUXER_TYPE muxerType=MUXER_MP4;
uint8_t dualPass=0;
uint8_t r=0;
uint32_t skipping=1;
pthread_t     audioThread;
audioQueueMT context;
PacketQueue   *pq;//("MP4 audioQ",50,2*1024*1024);
uint32_t    totalAudioSize=0;
uint32_t sent=0;
const char *containerTitle;
           switch(type)
           {
             case ADM_PSP:muxerType=MUXER_PSP;containerTitle="PSP";break;
             case ADM_MP4:muxerType=MUXER_MP4;containerTitle="MP4";break;
             case ADM_MATROSKA:muxerType=MUXER_MATROSKA;containerTitle="MKV";break;
             default:
                ADM_assert(0);
           }
        // Setup video
        
        if(videoProcessMode())
        {
             _incoming = getLastVideoFilter (frameStart,frameEnd-frameStart);
        }else
        {
             _incoming = getFirstVideoFilter (frameStart,frameEnd-frameStart);
        }

           videoBuffer=new uint8_t[_incoming->getInfo()->width*_incoming->getInfo()->height*3];
                // Set global header encoding, needed for H264
           _encode = getVideoEncoder (_incoming->getInfo()->width,
                        _incoming->getInfo()->height,1);
           total= _incoming->getInfo()->nb_frames;

           encoding_gui=new DIA_encoding(_incoming->getInfo()->fps1000);
           bitstream.bufferSize=_incoming->getInfo()->width*_incoming->getInfo()->height*3;
           if (!_encode)
                {
                  GUI_Error_HIG (QT_TR_NOOP("Cannot initialize the video stream"), NULL);
                        goto  stopit;
                }

                // init compressor
               
                  encoding_gui->setContainer(containerTitle);
               
                encoding_gui->setAudioCodec("None");
                if(!videoProcessMode())
                        encoding_gui->setCodec("Copy");
                else
                        encoding_gui->setCodec(_encode->getDisplayName());
                TwoPassLogFile=new char[strlen(name)+6];
                strcpy(TwoPassLogFile,name);
                strcat(TwoPassLogFile,".stat");
                _encode->setLogFile(TwoPassLogFile,total);

                if (!_encode->configure (_incoming))
                {
                      GUI_Error_HIG (QT_TR_NOOP("Filter init failed"), NULL);
                     goto  stopit;
                };

                dualPass=_encode->isDualPass();
                if(dualPass)
                {
                       
                        if(!prepareDualPass(bitstream.bufferSize,videoBuffer,TwoPassLogFile,encoding_gui,_encode,total))
                                goto stopit;
                }else
                {
                        encoding_gui->setPhasis ("Encoding");
                }
                
                info.width=_incoming->getInfo()->width;
                info.height=_incoming->getInfo()->height;
                info.nb_frames=_incoming->getInfo()->nb_frames;
                info.fps1000=_incoming->getInfo()->fps1000;
                info.fcc=*(uint32_t *)_encode->getCodecName(); //FIXME
                _encode->hasExtraHeaderData( &videoExtraDataSize,&dummy);
                if(videoExtraDataSize)
                {
                        printf("We have extradata for video in copy mode (%d)\n",videoExtraDataSize);
                        videoExtraData=new uint8_t[videoExtraDataSize];
                        memcpy(videoExtraData,dummy,videoExtraDataSize);
                }
        // _________________Setup video (cont) _______________
        // ___________ Read 1st frame _________________
             
             ADM_assert(_encode);
             bitstream.data=videoBuffer;
             
preFilling:
             bitstream.cleanup(0);
             if(!(err=_encode->encode ( prefill, &bitstream)))//&len, videoBuffer, &flags,&displayFrame))
             {
                        printf("MP4:First frame error\n");
                        GUI_Error_HIG (QT_TR_NOOP("Error while encoding"), NULL);
                        goto  stopit;
              }
              sent++;
              if(!bitstream.len)
              {
                prefill++;
                goto preFilling;
              }
              printf("Pass 2 prefill : %u\n",prefill);
              if(!bitstream.flags & AVI_KEY_FRAME)
              {
                GUI_Error_HIG (QT_TR_NOOP("KeyFrame error"),QT_TR_NOOP( "The beginning frame is not a key frame.\nPlease move the A marker."));
                  goto  stopit; 
              }
           //len=bitstream.len;
           // If needed get VOL header
           if(isMpeg4Compatible(info.fcc) && !videoExtraDataSize && bitstream.len)
           {
                // And put them as extradata for esds atom
                uint32_t voslen=0;
               
                if(extractVolHeader(videoBuffer,bitstream.len,&voslen))
                {
                        if(voslen)
                        {
                                videoExtraDataSize=voslen;
                                videoExtraData=new uint8_t[videoExtraDataSize];
                                memcpy(videoExtraData,videoBuffer,videoExtraDataSize);
                        }
                } else  printf("Oops should be settings data for esds\n");
            }

// ____________Setup audio__________________
          if(currentaudiostream)
          {
                audio=mpt_getAudioStream();
                if(!audio)
                {
                        GUI_Error_HIG (QT_TR_NOOP("Cannot initialize the audio stream"), NULL);
                        goto  stopit;
                }
          } 
          if(audio)
          {
                audioinfo=audio->getInfo();
                audio->extraData(&extraDataSize,&extraData);
                if(audioProcessMode())
                        encoding_gui->setAudioCodec(getStrFromAudioCodec(audio->getInfo()->encoding));
                else
                         encoding_gui->setAudioCodec("Copy");

           }else
           {
                encoding_gui->setAudioCodec("None");
           }
// ____________Setup Muxer _____________________
           muxer= new lavMuxer;
           
           if(!muxer->open(
                name,
                2000000, // Muxrate
                muxerType,
                &info,videoExtraDataSize,videoExtraData,
                audioinfo,extraDataSize,extraData))
                         goto stopit;
//_____________ Loop _____________________
          
          encoding_gui->setContainer(containerTitle);
         
          if(!videoProcessMode())
                encoding_gui->setCodec("Copy");
          else
                encoding_gui->setCodec(_encode->getDisplayName());
           //
          UI_purge();
          if(bitstream.len)
          {
            muxer->writeVideoPacket( &bitstream);
            frameWrite++;
          }
//_____________ Start Audio thread _____________________
          if(audio)
          {          
            pq=new PacketQueue("MP4 audioQ",5000,2*1024*1024);
            memset(&context,0,sizeof(context));
            context.audioEncoder=audio;
            context.audioTargetSample=0xFFFF0000; ; //FIXME
            context.packetQueue=pq;
            // start audio thread
            ADM_assert(!pthread_create(&audioThread,NULL,(THRINP)defaultAudioQueueSlave,&context)); 
            ADM_usleep(4000);
          }
//_____________GO !___________________
           for(int frame=1;frame<total;frame++)
           {
               while(muxer->needAudio())
               {
                    if(pq->Pop(audioBuffer,&alen,&sample))
                    {
                     if(alen)
                     {
                        muxer->writeAudioPacket(alen,audioBuffer,sample_got);
                        totalAudioSize+=alen;
                        encoding_gui->setAudioSize(totalAudioSize);
                        sample_got+=sample;
                     }
                    }else break;
               }
               ADM_assert(_encode);
               bitstream.cleanup(frameWrite);
               if(!prefill || frame+prefill<total) 
               {
                  
                  r=_encode->encode ( prefill+frame, &bitstream);
               }
                else
                {
                    r=_encode->encode ( total-1, &bitstream);
                }
               if(!r && frame<total-2)
               {
                        printf("MP4:Frame %u error\n",frame);
                        GUI_Error_HIG (QT_TR_NOOP("Error while encoding"), NULL);
                        goto  stopit;
                }
                if(!bitstream.len && skipping)
                {
                    printf("Frame skipped (xvid ?)\n");
                    continue;
                }
                sent++;
                skipping=0;
            //    printf("Prefill %u FrameWrite :%u Frame %u PtsFrame :%u\n",prefill,frameWrite,frame,bitstream.ptsFrame);
                frameWrite++;
                muxer->writeVideoPacket( &bitstream);
                encoding_gui->setFrame(frame,bitstream.len,bitstream.out_quantizer,total);
               if(!encoding_gui->isAlive())
                {
                    
                    goto stopit;
                }
               
           }
           ret=1;
           
stopit:
    printf("2nd pass, sent %u frames\n",sent);
    // Flush slave Q
    if(audio)
    {
        context.audioAbort=1;
        pq->Abort();
        // Wait for audio slave to be over
        while(!context.audioDone)
        {
          printf("Waiting Audio thread\n");
          ADM_usleep(500000); 
        }
        delete pq;
    }
    //
           if(muxer) muxer->close();
           if(encoding_gui) delete encoding_gui;
           if(TwoPassLogFile) delete [] TwoPassLogFile;
           if(videoBuffer) delete [] videoBuffer;
           if(muxer) delete muxer;
           if(_encode) delete _encode;	
           if(videoExtraData) delete [] videoExtraData;
           // Cleanup
           deleteAudioFilter (audio);
           return ret;
}
uint8_t prepareDualPass(uint32_t bufferSize,uint8_t *buffer,char *TwoPassLogFile,DIA_encoding *encoding_gui,Encoder *_encode,uint32_t total)
{
      uint32_t len, flag;
      FILE *tmp;
      uint8_t reuse=0,r;
      ADMBitstream bitstream(0);
      uint32_t prefill=0;
      uint32_t sent=0;
      
        aprintf("\n** Dual pass encoding**\n");

        if((tmp=fopen(TwoPassLogFile,"rt")))
        {
                fclose(tmp);
                if(GUI_Question(QT_TR_NOOP("\n Reuse the existing log-file ?")))
                {
                        reuse=1;
                }
        }
        
        if(!reuse)
        {
        
                encoding_gui->setPhasis ("1st Pass");
                aprintf("**Pass 1:%lu\n",total);
                _encode->startPass1 ();
                bitstream.data=buffer;
                bitstream.bufferSize=bufferSize;
                
preFilling2:
             bitstream.cleanup(0);
             if(!_encode->encode ( prefill, &bitstream))//&len, videoBuffer, &flags,&displayFrame))
             {
                        printf("MP4:First frame error\n");
                        GUI_Error_HIG (QT_TR_NOOP("Error while encoding"), NULL);
                        return 0;
              }
              sent++;
              if(!bitstream.len)
              {
                prefill++;
                goto preFilling2;
              }

                printf("Pass 1 prefill : %u\n",prefill);
                for (uint32_t cf = 1; cf < total; cf++)
                {
                        if (!encoding_gui->isAlive())
                        {
                                abt:
                                    GUI_Error_HIG (QT_TR_NOOP("Aborting"), NULL);
                                return 0;
                        }
                        bitstream.cleanup(cf);
                        if(!prefill || cf+prefill<total) 
                        {
                            r=_encode->encode ( prefill+cf, &bitstream);
                        }
                          else
                          {
                              r=_encode->encode ( total-1, &bitstream);
                          }
                        if (!r)
                        {
                                printf("\n Encoding of frame %lu failed !\n",cf);
                                return 0;
                        }
                        sent++;
                        encoding_gui->setFrame(cf,bitstream.len,bitstream.out_quantizer,total);
                }
                encoding_gui->reset();
                aprintf("**Pass 1:done\n");
        }// End of reuse

        if(!_encode->startPass2 ())
        {
                printf("Pass2 ignition failed\n");
                return 0;
        }
        printf("First pass : send %u frames\n",sent);
        encoding_gui->setPhasis ("2nd Pass");
        return 1;
}
void end (void)
{

}
uint8_t extractVolHeader(uint8_t *data,uint32_t dataSize,uint32_t *headerSize)
{
    // Search startcode
    uint8_t b;
    uint32_t idx=0;
    uint32_t mw,mh;
    uint32_t time_inc;
    
    *headerSize=0;

    while(dataSize)
    {
        uint32_t startcode=0xffffffff;
        while(dataSize>2)
        {
            startcode=(startcode<<8)+data[idx];
            idx++;
            dataSize--;
            if((startcode&0xffffff)==1) break;
        }
     
            printf("Startcodec:%x\n",data[idx]);
            if(data[idx]==0xB6 && idx>4) // vop start 
            {
                printf("Vop start found at %d\n",idx);
                *headerSize=idx-4;
                return 1;
            }
            
        }
        printf("No vop start found\n");
        return 0;
    
}	
#endif	
// EOF
