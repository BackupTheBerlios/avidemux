/***************************************************************************
    copyright            : (C) 2006 by mean
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
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>
#include <pthread.h>
#define WIN32_CLASH
#include "ADM_library/default.h"

#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_encoder/adm_encoder.h"

#include "ADM_dialog/DIA_encoding.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_toolkit/ADM_threads.h"
#include "ADM_lavformat/ADM_lavformat.h"
#include "ADM_mthread.h"

admMutex accessMutex("accessMutex_MT_muxer");

//*******************************************************
int defaultAudioSlave( muxerMT *context )
{
  uint32_t total_sample=0;
  uint32_t samples,audioLen;
  printf("[AudioThread] Starting\n");
  while(context->audioEncoder->getPacket(context->audioBuffer, &audioLen, &samples) && total_sample<context->audioTargetSample)
  { 
    total_sample+=samples;
    accessMutex.lock();
    if(context->audioAbort)
    {
      context->audioDone=1;
      context->muxer->audioEof();
      accessMutex.unlock();
      return 1;
    }
    accessMutex.unlock();
      
    while(!context->muxer->needAudio()) 
    {
      if(context->audioAbort)
      {
        context->muxer->audioEof();
        context->audioDone=1;
        return 1;
      } 
    };
    if(audioLen) 
    {
      context->muxer->writeAudioPacket(audioLen,context->audioBuffer); 
    }
    accessMutex.lock();
    context->feedAudio+=audioLen;
    accessMutex.unlock();

  }
  accessMutex.lock();
  // Let's say audio is always ok, shall we :)
  context->audioDone=1;
  context->muxer->audioEof();
  accessMutex.unlock();
  printf("[AudioThread] Exiting\n");
  printf("[AudioThread] Target %u, got %u, %f %%\n",context->audioTargetSample,total_sample,
         (float)total_sample/(float)context->audioTargetSample);
  return 1;
}
//*******************************************************
int defaultVideoSlave( muxerMT *context )
{
  printf("[VideoThread] Starting\n");
  for(uint32_t i=0;i<context->nbVideoFrame;i++)
  {

    context->bitstream->cleanup(i);
    if(context->videoAbort)
    {
      context->videoDone=1;
      context->muxer->videoEof();
      return 1;
    }
    if(!context->videoEncoder->encode( i,context->bitstream))
    {
      accessMutex.lock();
      context->videoDone=2;
      context->muxer->videoEof();
      accessMutex.unlock();
  
      return 1;
    }
    if(context->bitstream->len)
      context->muxer->writeVideoPacket(context->bitstream);
      
    accessMutex.lock();
    context->currentVideoFrame=i;
    context->feedVideo+=context->bitstream->len;
    accessMutex.unlock();
          

  }
  accessMutex.lock();
  context->videoDone=1;
  context->muxer->videoEof();
  accessMutex.unlock();

  printf("[VideoThread] Exiting\n");
  return 1;
}
/*
      Slave thread that will put data in the packetQueue
*/
int defaultAudioQueueSlave( audioQueueMT *context )
{
#define QBUFFER 4096*4
  uint32_t total_sample=0;
  uint32_t samples,audioLen;
  PacketQueue *queue=context->packetQueue;
  AVDMGenericAudioStream *audioEncoder=context->audioEncoder;
  
  ADM_assert(queue);
  ADM_assert(audioEncoder);
  
  uint8_t buffer[QBUFFER];
  
  printf("[AudioQueueThread] Starting\n");
  while(audioEncoder->getPacket(buffer, &audioLen, &samples) && total_sample<context->audioTargetSample)
  {
    ADM_assert(samples<= QBUFFER);
    total_sample+=samples;
    //printf("Audio %u\n",samples);
    accessMutex.lock();
    if(context->audioAbort)
    {
      context->audioDone=1;
      queue->Finished();
      printf("[AudioQueueThread] Aborting\n");
      printf("[AudioThread] Target %u, got %u, %f %%\n",context->audioTargetSample,total_sample,
             (float)total_sample/(float)context->audioTargetSample);
      accessMutex.unlock();
      return 1;
    }
    accessMutex.unlock();
    if(audioLen) 
    {
      queue->Push(buffer,audioLen,samples); 
    }
    accessMutex.lock();
    context->feedAudio+=audioLen;
    accessMutex.unlock();
  }
  accessMutex.lock();
  // Let's say audio is always ok, shall we :)
  context->audioDone=1;
  queue->Finished();
  accessMutex.unlock();
  printf("[AudioQueueThread] Exiting\n");
  printf("[AudioThread] Target %u, got %u, %f %%\n",context->audioTargetSample,total_sample,
         (float)total_sample/(float)context->audioTargetSample);
  return 1;
}
//EOF

