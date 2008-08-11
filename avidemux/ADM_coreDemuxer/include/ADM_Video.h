/***************************************************************************
                          ADM_Video.h  -  description
                             -------------------
         Describe a video stream with eventually the associated audio stream

    begin                : Thu Apr 18 2002
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

#ifndef __VIDHEADER__
#define __VIDHEADER__
#include "avifmt.h"
#include "avifmt2.h"


#include "ADM_compressedImage.h"
#include "ADM_audioStream.h"
typedef struct 
{
    uint32_t encoding;  // Same as in wavheader
    uint32_t bitrate;   // In kbits
    uint32_t channels;  //
    uint32_t frequency; // In hertz
    int32_t  av_sync;   // in ms
} audioInfo;

typedef struct 
{
 	uint32_t   width,height;
	uint32_t   fps1000;
	uint32_t   nb_frames;
  	uint32_t   fcc;	
    uint32_t   bpp;
} aviInfo;

uint8_t WAV2AudioInfo(WAVHeader *hdr,audioInfo *info);
/**
    \class vidHeader
    \brief Base Class for all demuxers
*/
class vidHeader
{
protected:
          MainAVIHeader 	_mainaviheader;
          uint8_t		    _isvideopresent;
          AVIStreamHeader 	_videostream;
          ADM_BITMAPINFOHEADER  _video_bih;
          uint8_t		    _isaudiopresent;
          AVIStreamHeader 	_audiostream;
          void			   *_audiostreaminfo;
          char			   *_name;
          uint32_t		    _videoExtraLen;
          uint8_t		   *_videoExtraData;

                    
public:
                                vidHeader();
virtual                         ~vidHeader() ;
virtual   void 			Dump(void)=0;
virtual	  uint8_t		getExtraHeaderData(uint32_t *len, uint8_t **data);
// AVI io
virtual 	uint8_t		open(const char *name)=0;
virtual 	uint8_t		close(void)=0;
  //__________________________
  //				 Info
  //__________________________
            uint8_t			        getVideoInfo(aviInfo *info);
            uint32_t			    getWidth( void ) { return _mainaviheader.dwWidth;};
            uint32_t			    getHeight( void ) { return _mainaviheader.dwHeight;};
            uint8_t			        setMyName( const char *name);
            char				    *getMyName( void);
  //__________________________
  //				 Audio
  //__________________________

virtual 	WAVHeader              *getAudioInfo(void ) =0 ;
virtual 	uint8_t                 getAudioStream(ADM_audioStream  **audio)=0;
virtual     uint8_t                 getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **info)=0;
virtual     uint8_t                 changeAudioStream(uint32_t newstream);
virtual     uint32_t                getCurrentAudioStreamNumber(void) { return 0;}
// Frames
  //__________________________
  //				 video
  //__________________________

virtual 	uint8_t                 setFlag(uint32_t frame,uint32_t flags)=0;
virtual 	uint32_t                getFlags(uint32_t frame,uint32_t *flags)=0;			
virtual 	uint8_t                 getFrameSize(uint32_t frame,uint32_t *size)=0;
virtual 	uint8_t                 getFrame(uint32_t framenum,ADMCompressedImage *img)=0;

// New write avi engine
          AVIStreamHeader           *getVideoStreamHeader(void ) { return &_videostream;}
          MainAVIHeader             *getMainHeader(void ) { return &_mainaviheader;}
          ADM_BITMAPINFOHEADER      *getBIH(void ) { return &_video_bih;}
virtual   uint64_t                   getTime(uint32_t frameNum)=0;
virtual   uint64_t                   getVideoDuration(void)=0;

};
#endif
