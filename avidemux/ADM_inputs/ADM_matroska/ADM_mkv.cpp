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

#include "default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "fourcc.h"
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
  if(_curAudio) return _curAudio->getInfo();
  return NULL;
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
  if(_curAudio)
  {
    *audio=_curAudio;
    return 1;
  }
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
  for(int i=0;i<_nbAudioTrack;i++)
  {
    printf("[MKV] Track:%u index:%u fq:%u channels:%u codec:%u extradatalen:%u length:%u nbchunk %u\n"
            ,i,_audioTracks[i].streamIndex ,
            _audioTracks[i].wavHeader.frequency,
            _audioTracks[i].wavHeader.channels,
            _audioTracks[i].wavHeader.encoding,
            _audioTracks[i].extraDataLen,
            _audioTracks[i].length,
            _audioTracks[i].nbPackets);
    
  }
  printf("*********** MKV INFO***********\n");
}
/*
    __________________________________________________________
*/

uint32_t mkvHeader::getNbStream(void) 
{
  return _nbAudioTrack; 
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
  if(_audioTracks)
  {
    for(int i=0;i<_nbAudioTrack;i++)
    { 
      if(_audioTracks[i].extraData) delete []  _audioTracks[i].extraData;
      
    }
    delete _audioTracks;
    _audioTracks=NULL;
  }
  if(_videoExtraData)
  {
    delete [] _videoExtraData;
    _videoExtraData=NULL; 
  }
  if(myName)
  {
    delete [] myName;
    myName=NULL; 
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
  _nbAudioTrack=0;
  _audioTracks=NULL;
  myName=NULL;
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
  av_find_stream_info(CONTEXT);
  
  for(int i = 0; i < CONTEXT->nb_streams; i++) {
    AVCodecContext *enc = CONTEXT->streams[i]->codec;
    switch(enc->codec_type) {
      case CODEC_TYPE_AUDIO:
        _nbAudioTrack++;
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
  if(_nbAudioTrack)
  {
    int cur=0;
    _audioTracks=new mkvAudioTrak[ _nbAudioTrack];
    memset(_audioTracks,0,sizeof(mkvAudioTrak) *_nbAudioTrack);
    for(int i = 0; i < CONTEXT->nb_streams; i++) 
    {
      AVCodecContext *enc = CONTEXT->streams[i]->codec;
      switch(enc->codec_type) 
      {
        case CODEC_TYPE_AUDIO:
          _audioTracks[cur++].streamIndex=i;
          break;
        default:
          break;
      }
    }
    
  }
  
  if(!readVideoInfo())
  {
    printf("[MKV] Failed reading video info\n");
    return 0; 
  }
  myName=ADM_strdup(name);
  prebuildIndex();

  for(int i=0;i<_nbAudioTrack;i++)
  {
    readAudioInfo(i); 
  }
  if(_nbAudioTrack)
  {
    printf("[MKV] Building current audio track\n");
    _isaudiopresent=1;
    _curAudio=new mkvAudio(name,&_audioTracks[0]);
  }
  printf("[MKV] Successfully open\n");
  
  Dump();
  return 1;
}
/*
    __________________________________________________________
*/

 uint8_t  mkvHeader::prebuildIndex(void)
{
  AVPacket pkt1, *pkt = &pkt1;
  int ret;
  int nbVideo=0;
  printf("\n[MKV]Preindexing started, that can take a while...\n");
  while(1)
  {
    ret = av_read_frame(CONTEXT, pkt);
    if (ret < 0) 
    {
      break;
    }
    if(pkt->stream_index==_videoIndex) 
    {
      nbVideo++;
    }
    for(int i=0;i<_nbAudioTrack;i++)
    {
      if( pkt->stream_index==_audioTracks[i].streamIndex)
      {
        _audioTracks[i].nbPackets++;
        _audioTracks[i].length+=pkt->size;
      }
    }
  }
  printf("\n[MKV]Preindexing done\n");
  _videostream.dwLength=_mainaviheader.dwTotalFrames=nbVideo;
  
  rewind();
        
        
  return 1;
}
uint8_t mkvHeader::rewind( void )
{
  int err;
  AVInputFormat *format;
  
  
  format= av_find_input_format("matroska");
  av_close_input_file(CONTEXT);
  _context=NULL;
  err = av_open_input_file((AVFormatContext **)&_context, myName, format, 0, NULL);
  ADM_assert(err>=0);
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
  if(!framenum) rewind();
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
    if(flags)
    {
      if(pkt->flags  &  PKT_FLAG_KEY)
        *flags=AVI_KEY_FRAME;
      else
        *flags=0;
    }
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
  
 
  uint32_t fcc=0;
  // Set codec
  switch(enc->codec_id)
  {
    case CODEC_ID_H264:      fcc=fourCC::get((uint8_t *)"H264");break; 
    case CODEC_ID_MPEG4:     fcc=fourCC::get((uint8_t *)"DIVX");break;
    case CODEC_ID_MSMPEG4V3: fcc=fourCC::get((uint8_t *)"DIV3");break;
    default:
      printf("[MKV] Unknown video fourcc %d\n",enc->codec_id);
      return 0;
  }
  _video_bih.biCompression=_videostream.fccHandler=fcc;
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
  _videostream.dwLength=_mainaviheader.dwTotalFrames=5;
  printf("[MKV] Video info ok\n");
  Dump();
  return 1;
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::readAudioInfo( uint32_t track)
{
  int index=_audioTracks[track].streamIndex;
  WAVHeader *hdr=&(_audioTracks[track].wavHeader);
  AVCodecContext *enc=CONTEXT->streams[index]->codec;
  if(!enc) 
  {
    printf("[MKV]Failed to get audio for track %d (track index %d)\n",track,index);
    return 0;
  }
  // Set codec
  switch(enc->codec_id)
  {
    case CODEC_ID_MPEG4AAC:
    case CODEC_ID_AAC: hdr->encoding=WAV_AAC;break; 
    case CODEC_ID_MP3: hdr->encoding=WAV_MP3;break; 
    case CODEC_ID_AC3: hdr->encoding=WAV_AC3;break; 
    case CODEC_ID_VORBIS: hdr->encoding=WAV_OGG;break; 
    default:
      printf("[MKV] Unknown audio fourcc %d\n",enc->codec_id);
      hdr->encoding=WAV_UNKNOWN;
      return 0;
  }
  // Extradata ?
  if(enc->extradata && enc->extradata_size )
  {
    _audioTracks[track].extraData=new uint8_t[enc->extradata_size];
    _audioTracks[track].extraDataLen= enc->extradata_size;
    memcpy(_audioTracks[track].extraData,enc->extradata,_audioTracks[track].extraDataLen);
  }
  // Channels fq etc...
  hdr->frequency=enc->sample_rate;
  hdr->channels=enc->channels;
  hdr->byterate=(enc->bit_rate>>3);
  if(!hdr->byterate) hdr->byterate=128000>>3;

  
  printf("[MKV] Video audio ok for track %d\n",track);

  return 1;
}

//EOF
