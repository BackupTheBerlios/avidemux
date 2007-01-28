/***************************************************************************
                        3nd gen indexer                                                 
                             
    
    copyright            : (C) 2005 by mean
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
#ifndef DMX_INDEXER_INTERNAL_H
#define DMX_INDEXER_INTERNAL_H

#define MAX_PUSHED 200


typedef struct TimeStamp
{
        uint16_t hh,mm,ss,ff;
}TimeStamp;


typedef struct IndFrame
{
	uint32_t type;
	uint32_t size;
	uint64_t abs;
	uint64_t rel;
	
}IndFrame;

typedef struct runData
{
  
      FILE     *fd;
      uint64_t totalFileSize; 
      uint32_t grabbing;        /* Dont start a new image, still in the old one eg SEQ_START+GOP+1st image */
      uint32_t totalFrame;
      uint32_t nbImage;
      uint32_t nbPushed,nbGop;
      
      uint32_t nbTrack;
      uint32_t seq_found;     /* Sequence start found */
      
      uint32_t lastRefPic;
      
      uint32_t imageW,imageH,imageFPS,imageAR;
      
      
      uint64_t firstPicPTS;
      
      IndFrame *frames;
      
      
      TimeStamp firstStamp,lastStamp; /* Time code hh:mm:ss */
      
      dmx_demuxer *demuxer;
      DIA_progressIndexing *work;
}runData;

/* * For mpeg2 payload * */
uint8_t mainLoopMpeg2(runData *run);
void endLoopMpeg2(runData *run);
#endif
//EOF


