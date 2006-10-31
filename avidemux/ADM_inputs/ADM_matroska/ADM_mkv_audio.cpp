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

#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "ADM_library/fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_mkv.h"

extern "C"
{
#include "ADM_lavcodec/avcodec.h"
#include "ADM_lavformat/avformat.h"
};

#define CONTEXT ((AVFormatContext* )_context)

/*
    __________________________________________________________
*/

mkvAudio::~mkvAudio()
{
  printf("[MkvAudio] Destroying track\n");
  if(_context)
  {
    av_close_input_file(CONTEXT);
    _context=NULL;
  }
}
/*
    __________________________________________________________
*/

mkvAudio::mkvAudio(char *name,mkvAudioTrak *track)
{
   printf("[MkvAudio] Creating track\n");
   _trackIndex=track->streamIndex; 
   _wavheader=&(track->wavHeader);
  _extraDataLen=track->extraDataLen;
  _extraData=track->extraData;
  _length=track->length;
  
  // Open it
  AVInputFormat *format;
  
  printf("[Matroska] Open\n");
  format= av_find_input_format("matroska");
  ADM_assert(format)
  ADM_assert(0<= av_open_input_file((AVFormatContext **)&_context, name, format, 0, NULL));
  av_find_stream_info(CONTEXT);
}
/*
    __________________________________________________________
*/

uint32_t            mkvAudio::read(uint32_t len,uint8_t *buffer)
{
  uint32_t lan,samples;
  if(!getPacket(      buffer,    &lan, &samples)) return 0;
  return lan;
}
/*
    __________________________________________________________
*/

uint8_t   mkvAudio::goTo(uint32_t newoffset)
{
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t   mkvAudio::goToTime(uint32_t newoffset)
{
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t   mkvAudio::extraData(uint32_t *l,uint8_t **d)
{
  if(_extraDataLen)
  {
    *l=_extraDataLen;
    *d=_extraData;  
  }
  else
  {
    *l=0;
    *d=NULL; 
  }
  return 1;
}
/*
    __________________________________________________________
*/

uint8_t  mkvAudio::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  
  AVPacket pkt1, *pkt = &pkt1;
  int ret;
  while(1)
  {
    ret = av_read_frame(CONTEXT, pkt);
    if (ret < 0) 
    {
      printf("[MKV] Error reading audio frame %d\n");
      return 0;
    }
    if(pkt->stream_index!=_trackIndex) 
    {
      //printf("[MKV] Wrong stream %u %u\n",pkt->stream_index,_videoIndex);
      continue;
    }
    memcpy(dest,pkt->data,pkt->size);
    *len=pkt->size;
    *samples=1024; // FIXME
    break;
  }
  return 1; 
}
//EOF
