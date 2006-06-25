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
#include <stdio.h>
#include <stdlib.h>

#ifndef __VIDHEADER__
#define __VIDHEADER__
#include "avifmt.h"
#include "avifmt2.h"

#include "ADM_audio/aviaudio.hxx"

typedef struct audioInfo
{
    uint32_t encoding;  // Same as in wavheader
    uint32_t bitrate;   // In kbits
    uint32_t channels;  //
    uint32_t frequency; // In hertz
    int32_t  av_sync;   // in ms
};

typedef struct aviInfo
{
 	uint32_t   width,height;
	uint32_t   fps1000;
	uint32_t   nb_frames;
  	uint32_t   fcc;	
}  ;

uint8_t WAV2AudioInfo(WAVHeader *hdr,audioInfo *info);

class vidHeader
{
protected:
		MainAVIHeader 		_mainaviheader;
  		uint8_t			_isvideopresent;
    		AVIStreamHeader 	_videostream;
	  	BITMAPINFOHEADER  	_video_bih;
  		uint8_t			_isaudiopresent;
		AVIStreamHeader 	_audiostream;
	  	void			*_audiostreaminfo;
		char			*_name;
		uint32_t		_videoExtraLen;
		uint8_t			*_videoExtraData;
                	  
public:
//  static int checkFourCC(uint8_t *in, uint8_t *fourcc);
         vidHeader();
virtual   ~vidHeader() ;
virtual   void 				Dump(void)=0;
virtual   uint32_t 			getNbStream(void)=0;
virtual   uint8_t 			needDecompress(void)=0;
virtual	  uint8_t			getExtraHeaderData(uint32_t *len, uint8_t **data);
// AVI io
virtual 	uint8_t			open(char *name)=0;
virtual 	uint8_t			close(void)=0;
virtual	uint8_t			reorder( void ) { return 0;} // by default we don"t do frame re-ordering
virtual	uint8_t			isReordered( void ) { return 0;} // by default we don"t do frame re-ordering
  //__________________________
  //				 Info
  //__________________________
  uint8_t					getVideoInfo(aviInfo *info);
  uint32_t				getWidth( void ) { return _mainaviheader.dwWidth;};
  uint32_t				getHeight( void ) { return _mainaviheader.dwHeight;};
  uint8_t					setMyName( char *name);
  char						*getMyName( void);
  //__________________________
  //				 Audio
  //__________________________

virtual 	WAVHeader              *getAudioInfo(void ) =0 ;
virtual 	uint8_t			getAudioStream(AVDMGenericAudioStream **audio)=0;
virtual         uint8_t                 getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **info);
virtual         uint8_t                 changeAudioStream(uint32_t newstream);
virtual         uint32_t                getCurrentAudioStreamNumber(void) { return 0;}
// Frames
  //__________________________
  //				 video
  //__________________________

virtual 	uint8_t  setFlag(uint32_t frame,uint32_t flags)=0;
virtual 	uint8_t  getFrameSize(uint32_t frame,uint32_t *size);
virtual 	uint32_t getFlags(uint32_t frame,uint32_t *flags)=0;			
virtual 	uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
												uint32_t *flags)=0;
virtual 	uint8_t  	getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)=0	;
virtual 	uint8_t  	getRaw(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)													
						{
							 return getFrameNoAlloc(framenum,ptr,framelen)	;
						}
virtual 	uint8_t   getRawStart(uint8_t *ptr,uint32_t *len);
uint32_t 						getTime(uint32_t frame);


// index and stuff

// New write avi engine
 	 AVIStreamHeader *getVideoStreamHeader(void ) { return &_videostream;}
 	 MainAVIHeader 		*getMainHeader(void ) { return &_mainaviheader;}
 	 BITMAPINFOHEADER 		*getBIH(void ) { return &_video_bih;}
	     	 		
};
#endif
