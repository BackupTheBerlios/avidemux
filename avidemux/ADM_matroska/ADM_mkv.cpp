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

WAVHeader *mkvHeader::getAudioInfo(void )
{
  return NULL;
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
  *audio=NULL;
  return 0; 
}
/*
    __________________________________________________________
*/

void mkvHeader::Dump(void)
{
  printf("*********** MKV INFO***********\n");
  printf("[MKV] %d x %d, %d frames\n",
                 _mainaviheader.dwWidth,_mainaviheader.dwHeight,_mainaviheader.dwTotalFrames);
  printf("[MKV] We have %u bytes of extradata for video\n",_videoExtraLen);
  printf("[MKV] %u / %u\n",  _videostream.dwScale,  _videostream.dwRate);
  printf("[MKV] %u frames\n",_videostream.dwLength);
  printf("*********** MKV INFO***********\n");
}
/*
    __________________________________________________________
*/

uint32_t mkvHeader::getNbStream(void) 
{
  return 0; 
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::close(void)
{
  if(_context)
  {
    av_close_input_file(CONTEXT);
    _context=NULL;
  }
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::needDecompress(void)
{
  return 1; 
}
/*
    __________________________________________________________
*/

 mkvHeader::mkvHeader( void ) : vidHeader()
{
  _fd=NULL;
  _audioIndex=_videoIndex=-1;
  _context=NULL;
}
/*
    __________________________________________________________
*/

 mkvHeader::~mkvHeader(  )
{
  close();
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::open(char *name)
{
  int err;
  AVInputFormat *format;
  
  printf("[Matroska] Open\n");
  format= av_find_input_format("matroska");
  if(!format)
  {
    printf("[MKV] input format failed\n");
    return 0;
  }
  err = av_open_input_file((AVFormatContext **)&_context, name, format, 0, NULL);
  if (err < 0) {
    printf("[MKV] Error in open input file %d\n",err);
    return 0;
  }
  for(int i = 0; i < CONTEXT->nb_streams; i++) {
    AVCodecContext *enc = CONTEXT->streams[i]->codec;
    switch(enc->codec_type) {
      case CODEC_TYPE_AUDIO:
        _audioIndex=i;
        printf("[MKV] %d is audio\n",i,CONTEXT->nb_streams);
        break;
      case CODEC_TYPE_VIDEO:
        _videoIndex=i;
        printf("[MKV] %d is video\n",i,CONTEXT->nb_streams);
        break;
      default:
        break;
    }
  }
  if(_videoIndex<0)
  {
    printf("[MKV] No video track found\n");
    return 0; 
  }
  av_find_stream_info(CONTEXT);
  if(!readVideoInfo())
  {
    printf("[MKV] Failed reading video info\n");
    return 0; 
  }
  printf("[MKV] Successfully open\n");
  return 1;
}

/*
    __________________________________________________________
*/

  uint8_t  mkvHeader::setFlag(uint32_t frame,uint32_t flags)
{
  return 0; 
}
/*
    __________________________________________________________
*/

uint32_t mkvHeader::getFlags(uint32_t frame,uint32_t *flags)
{
  *flags=AVI_KEY_FRAME;
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t  mkvHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                     uint32_t *flags)
{
  AVPacket pkt1, *pkt = &pkt1;
  int ret;
  while(1)
  {
    ret = av_read_frame(CONTEXT, pkt);
    if (ret < 0) 
    {
      printf("[MKV] Error reading frame %d\n",framenum);
      return 0;
    }
    if(pkt->stream_index!=_videoIndex) 
    {
      //printf("[MKV] Wrong stream %u %u\n",pkt->stream_index,_videoIndex);
      continue;
    }
    memcpy(ptr,pkt->data,pkt->size);
    *framelen=pkt->size;
    *flags=AVI_KEY_FRAME;
    break;
  }
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t  mkvHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
  uint32_t flags;
  return getFrameNoAlloc(framenum,ptr,framelen,&flags); 
}
/*
    Read info, fill in mainaviheader & video streamheader
    __________________________________________________________
*/
uint8_t mkvHeader::readVideoInfo( void)
{
  AVCodecContext *enc=CONTEXT->streams[_videoIndex]->codec;
  uint32_t nbFrame;
  if(!enc) 
  {
    printf("[MKV]Failed to get video\n");
    return 0;
  }
  _isvideopresent=1;
  memset(&_mainaviheader,0,sizeof(_mainaviheader));
  _mainaviheader.dwWidth=enc->width;
  _mainaviheader.dwHeight=enc->height;
  _video_bih.biWidth=enc->width;
  _video_bih.biHeight=enc->height;
  
 

  // Set codec
  switch(enc->codec_id)
  {
    case CODEC_ID_H264: _videostream.fccHandler=fourCC::get((uint8_t *)"H264");break; 
    case CODEC_ID_MPEG4: _videostream.fccHandler=fourCC::get((uint8_t *)"DIVX");break;
    case CODEC_ID_MSMPEG4V3: _videostream.fccHandler=fourCC::get((uint8_t *)"DIV3");break;
    default:
      printf("[MKV] Unknown video fourcc %d\n",enc->codec_id);
      return 0;
  }
  // Extradata ?
  if(enc->extradata && enc->extradata_size )
  {
    _videoExtraData=new uint8_t[enc->extradata_size];
    _videoExtraLen= enc->extradata_size;
    memcpy(_videoExtraData,enc->extradata,_videoExtraLen);
    
    
  }
  // FrameRate, frame count
  AVRational r=CONTEXT->streams[_videoIndex]->r_frame_rate;
  if(r.num && r.den)
  {
    _videostream.dwScale=r.num;
    _videostream.dwRate=r.den;
  }else
  {
    _videostream.dwScale=1000;
    _videostream.dwRate=25000;
   
  }
  //
  nbFrame=CONTEXT->streams[_videoIndex]->codec_info_nb_frames;
  if(!nbFrame)
  {
    printf("[MKV]No info about nb frame, guessing from duration\n"); 
    printf("[MKV] Duration :%lu\n",CONTEXT->streams[_videoIndex]->duration);
    _videostream.dwLength=_mainaviheader.dwTotalFrames=50;
  }else
  {
    _videostream.dwLength=_mainaviheader.dwTotalFrames=nbFrame; // FIXME 
  }
  
  printf("[MKV] Video info ok\n");
  Dump();
  return 1;
}

//EOF
