/***************************************************************************
                          audioprocess.hxx  -  description
                             -------------------
    begin                : Sun Jan 13 2002
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

#ifndef __AudioProcess__
#define __AudioProcess__

#include "ADM_audio/aviaudio.hxx"

#define PROCESS_BUFFER_SIZE 48000*4*4 // should be enougth
#define MINIMUM_BUFFER   8192*4
/*! Base class for chained action. This class is derivated from AVDMGenericAudioStream and has two
    specificities :
        1- All instances are meant to be chained to another instance of AVDMGenericAudioStream
              A->B->C
        2- They store both input & output so that you can ask for a specific number of bytes form them
*/
class AVDMBufferedAudioStream : public  AVDMGenericAudioStream
{
  
        protected:
                AVDMGenericAudioStream *_instream;

                /*! _chunk is the size of an elementary packed, depends on the codec used */
                uint32_t _chunk;

                virtual uint32_t grab(uint8_t *outbuffer)=0;//deprecated
                virtual uint32_t grab(float *outbuffer){return 0;}
                uint32_t _headBuff,_tailBuff;

        public:
                        AVDMBufferedAudioStream(AVDMGenericAudioStream *instream);
                virtual ~AVDMBufferedAudioStream();


                virtual uint32_t read(uint32_t len,uint8_t *buffer)=0;//deprecated
                virtual uint32_t read(uint32_t len,float *buffer)=0;
                virtual uint8_t  goTo(uint32_t newoffset);
                virtual uint8_t  goToTime(uint32_t newoffset);
                virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer) { return read(len,buffer); }//deprecated
                virtual uint32_t readDecompress(uint32_t len,float *buffer) { return read(len,buffer); }
};

#endif
// EOF
